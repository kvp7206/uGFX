/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/gdisp.c
 * @brief   GDISP Driver code.
 *
 * @addtogroup GDISP
 * @{
 */
#include "gfx.h"

#if GFX_USE_GDISP

/* Include the low level driver information */
#include "gdisp/lld/gdisp_lld.h"

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

#if GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC
	static gfxMutex			gdispMutex;
#endif

#if GDISP_NEED_ASYNC
	#define GDISP_THREAD_STACK_SIZE	256		/* Just a number - not yet a reflection of actual use */
	#define GDISP_QUEUE_SIZE		8		/* We only allow a short queue */

	static gfxQueue 		gdispQueue;
	static gfxMutex			gdispMsgsMutex;
	static gfxSem			gdispMsgsSem;
	static gdisp_lld_msg_t	gdispMsgs[GDISP_QUEUE_SIZE];
	static 					DECLARE_THREAD_STACK(waGDISPThread, GDISP_THREAD_STACK_SIZE);
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if GDISP_NEED_ASYNC
	static DECLARE_THREAD_FUNCTION(GDISPThreadHandler, arg) {
		(void)arg;
		gdisp_lld_msg_t	*pmsg;

		while(1) {
			/* Wait for msg with work to do. */
			pmsg = (gdisp_lld_msg_t *)gfxQueueGet(&gdispQueue, TIME_INFINITE);

			/* OK - we need to obtain the mutex in case a synchronous operation is occurring */
			gfxMutexEnter(&gdispMutex);

			gdisp_lld_msg_dispatch(pmsg);

			/* Mark the message as free */
			pmsg->action = GDISP_LLD_MSG_NOP;

			gfxMutexExit(&gdispMutex);
		}
		return 0;
	}

	static gdisp_lld_msg_t *gdispAllocMsg(gdisp_msgaction_t action) {
		gdisp_lld_msg_t	*p;

		while(1) {		/* To be sure, to be sure */

			/* Wait for a slot */
			gfxSemWait(&gdispMsgsSem, TIME_INFINITE);

			/* Find the slot */
			gfxMutexEnter(&gdispMsgsMutex);
			for(p=gdispMsgs; p < &gdispMsgs[GDISP_QUEUE_SIZE]; p++) {
				if (p->action == GDISP_LLD_MSG_NOP) {
					/* Allocate it */
					p->action = action;
					gfxMutexExit(&gdispMsgsMutex);
					return p;
				}
			}
			gfxMutexExit(&gdispMsgsMutex);

			/* Oops - none found, try again */
			gfxSemSignal(&gdispMsgsSem);
		}
	}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* Our module initialiser */
#if GDISP_NEED_MULTITHREAD
	void _gdispInit(void) {
		/* Initialise Mutex */
		gfxMutexInit(&gdispMutex);

		/* Initialise driver */
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_init();
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ASYNC
	void _gdispInit(void) {
		unsigned		i;
		gfxThreadHandle	hth;

		/* Mark all the Messages as free */
		for(i=0; i < GDISP_QUEUE_SIZE; i++)
			gdispMsgs[i].action = GDISP_LLD_MSG_NOP;

		/* Initialise our Queue, Mutex's and Counting Semaphore.
		 * 	A Mutex is required as well as the Queue and Thread because some calls have to be synchronous.
		 *	Synchronous calls get handled by the calling thread, asynchronous by our worker thread.
		 */
		gfxQueueInit(&gdispQueue);
		gfxMutexInit(&gdispMutex);
		gfxMutexInit(&gdispMsgsMutex);
		gfxSemInit(&gdispMsgsSem, GDISP_QUEUE_SIZE, GDISP_QUEUE_SIZE);

		hth = gfxThreadCreate(waGDISPThread, sizeof(waGDISPThread), NORMAL_PRIORITY, GDISPThreadHandler, NULL);
		if (hth) gfxThreadClose(hth);

		/* Initialise driver - synchronous */
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_init();
		gfxMutexExit(&gdispMutex);
	}
#else
	void _gdispInit(void) {
		gdisp_lld_init();
	}
#endif

#if GDISP_NEED_MULTITHREAD
	bool_t gdispIsBusy(void) {
		return FALSE;
	}
#elif GDISP_NEED_ASYNC
	bool_t gdispIsBusy(void) {
		return !gfxQueueIsEmpty(&gdispQueue);
	}
#endif

#if GDISP_NEED_MULTITHREAD
	void gdispClear(color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_clear(color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ASYNC
	void gdispClear(color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_CLEAR);
		p->clear.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if GDISP_NEED_MULTITHREAD
	void gdispDrawPixel(coord_t x, coord_t y, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_draw_pixel(x, y, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ASYNC
	void gdispDrawPixel(coord_t x, coord_t y, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWPIXEL);
		p->drawpixel.x = x;
		p->drawpixel.y = y;
		p->drawpixel.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif
	
#if GDISP_NEED_MULTITHREAD
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_draw_line(x0, y0, x1, y1, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ASYNC
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWLINE);
		p->drawline.x0 = x0;
		p->drawline.y0 = y0;
		p->drawline.x1 = x1;
		p->drawline.y1 = y1;
		p->drawline.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if GDISP_NEED_MULTITHREAD
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_fill_area(x, y, cx, cy, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ASYNC
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLAREA);
		p->fillarea.x = x;
		p->fillarea.y = y;
		p->fillarea.cx = cx;
		p->fillarea.cy = cy;
		p->fillarea.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif
	
#if GDISP_NEED_MULTITHREAD
	void gdispBlitAreaEx(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_blit_area_ex(x, y, cx, cy, srcx, srcy, srccx, buffer);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ASYNC
	void gdispBlitAreaEx(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_BLITAREA);
		p->blitarea.x = x;
		p->blitarea.y = y;
		p->blitarea.cx = cx;
		p->blitarea.cy = cy;
		p->blitarea.srcx = srcx;
		p->blitarea.srcy = srcy;
		p->blitarea.srccx = srccx;
		p->blitarea.buffer = buffer;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif
	
#if (GDISP_NEED_CLIP && GDISP_NEED_MULTITHREAD)
	void gdispSetClip(coord_t x, coord_t y, coord_t cx, coord_t cy) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_set_clip(x, y, cx, cy);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_CLIP && GDISP_NEED_ASYNC
	void gdispSetClip(coord_t x, coord_t y, coord_t cx, coord_t cy) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_SETCLIP);
		p->setclip.x = x;
		p->setclip.y = y;
		p->setclip.cx = cx;
		p->setclip.cy = cy;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if (GDISP_NEED_CIRCLE && GDISP_NEED_MULTITHREAD)
	void gdispDrawCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_draw_circle(x, y, radius, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_CIRCLE && GDISP_NEED_ASYNC
	void gdispDrawCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWCIRCLE);
		p->drawcircle.x = x;
		p->drawcircle.y = y;
		p->drawcircle.radius = radius;
		p->drawcircle.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif
	
#if (GDISP_NEED_CIRCLE && GDISP_NEED_MULTITHREAD)
	void gdispFillCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_fill_circle(x, y, radius, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_CIRCLE && GDISP_NEED_ASYNC
	void gdispFillCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLCIRCLE);
		p->fillcircle.x = x;
		p->fillcircle.y = y;
		p->fillcircle.radius = radius;
		p->fillcircle.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_NEED_MULTITHREAD)
	void gdispDrawEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_draw_ellipse(x, y, a, b, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ELLIPSE && GDISP_NEED_ASYNC
	void gdispDrawEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWELLIPSE);
		p->drawellipse.x = x;
		p->drawellipse.y = y;
		p->drawellipse.a = a;
		p->drawellipse.b = b;
		p->drawellipse.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif
	
#if (GDISP_NEED_ELLIPSE && GDISP_NEED_MULTITHREAD)
	void gdispFillEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_fill_ellipse(x, y, a, b, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ELLIPSE && GDISP_NEED_ASYNC
	void gdispFillEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLELLIPSE);
		p->fillellipse.x = x;
		p->fillellipse.y = y;
		p->fillellipse.a = a;
		p->fillellipse.b = b;
		p->fillellipse.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if (GDISP_NEED_ARC && GDISP_NEED_MULTITHREAD)
	void gdispDrawArc(coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_draw_arc(x, y, radius, start, end, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ARC && GDISP_NEED_ASYNC
	void gdispDrawArc(coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWARC);
		p->drawarc.x = x;
		p->drawarc.y = y;
		p->drawarc.radius = radius;
		p->drawarc.start = start;
		p->drawarc.end = end;
		p->drawarc.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if (GDISP_NEED_ARC && GDISP_NEED_MULTITHREAD)
	void gdispFillArc(coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_fill_arc(x, y, radius, start, end, color);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_ARC && GDISP_NEED_ASYNC
	void gdispFillArc(coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLARC);
		p->fillarc.x = x;
		p->fillarc.y = y;
		p->fillarc.radius = radius;
		p->fillarc.start = start;
		p->fillarc.end = end;
		p->fillarc.color = color;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if GDISP_NEED_ARC
void gdispDrawRoundedBox(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color) {
	if (2*radius > cx || 2*radius > cy) {
		gdispDrawBox(x, y, cx, cy, color);
		return;
	}
	gdispDrawArc(x+radius, y+radius, radius, 90, 180, color);
	gdispDrawLine(x+radius+1, y, x+cx-2-radius, y, color);
	gdispDrawArc(x+cx-1-radius, y+radius, radius, 0, 90, color);
	gdispDrawLine(x+cx-1, y+radius+1, x+cx-1, y+cy-2-radius, color);
	gdispDrawArc(x+cx-1-radius, y+cy-1-radius, radius, 270, 360, color);
	gdispDrawLine(x+radius+1, y+cy-1, x+cx-2-radius, y+cy-1, color);
	gdispDrawArc(x+radius, y+cy-1-radius, radius, 180, 270, color);
	gdispDrawLine(x, y+radius+1, x, y+cy-2-radius, color);
}
#endif

#if GDISP_NEED_ARC
void gdispFillRoundedBox(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color) {
	coord_t radius2;

	radius2 = radius*2;
	if (radius2 > cx || radius2 > cy) {
		gdispFillArea(x, y, cx, cy, color);
		return;
	}
	gdispFillArc(x+radius, y+radius, radius, 90, 180, color);
	gdispFillArea(x+radius+1, y, cx-radius2, radius, color);
	gdispFillArc(x+cx-1-radius, y+radius, radius, 0, 90, color);
	gdispFillArc(x+cx-1-radius, y+cy-1-radius, radius, 270, 360, color);
	gdispFillArea(x+radius+1, y+cy-radius, cx-radius2, radius, color);
	gdispFillArc(x+radius, y+cy-1-radius, radius, 180, 270, color);
	gdispFillArea(x, y+radius, cx, cy-radius2, color);
}
#endif

#if (GDISP_NEED_PIXELREAD && (GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC))
	color_t gdispGetPixelColor(coord_t x, coord_t y) {
		color_t		c;

		/* Always synchronous as it must return a value */
		gfxMutexEnter(&gdispMutex);
		c = gdisp_lld_get_pixel_color(x, y);
		gfxMutexExit(&gdispMutex);

		return c;
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_NEED_MULTITHREAD)
	void gdispVerticalScroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_vertical_scroll(x, y, cx, cy, lines, bgcolor);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_SCROLL && GDISP_NEED_ASYNC
	void gdispVerticalScroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_VERTICALSCROLL);
		p->verticalscroll.x = x;
		p->verticalscroll.y = y;
		p->verticalscroll.cx = cx;
		p->verticalscroll.cy = cy;
		p->verticalscroll.lines = lines;
		p->verticalscroll.bgcolor = bgcolor;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_NEED_MULTITHREAD)
	void gdispControl(unsigned what, void *value) {
		gfxMutexEnter(&gdispMutex);
		gdisp_lld_control(what, value);
		gfxMutexExit(&gdispMutex);
	}
#elif GDISP_NEED_CONTROL && GDISP_NEED_ASYNC
	void gdispControl(unsigned what, void *value) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_CONTROL);
		p->control.what = what;
		p->control.value = value;
		gfxQueuePut(&gdispQueue, &p->qi, TIME_IMMEDIATE);
	}
#endif

#if (GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC) && GDISP_NEED_QUERY
	void *gdispQuery(unsigned what) {
		void *res;

		gfxMutexEnter(&gdispMutex);
		res = gdisp_lld_query(what);
		gfxMutexExit(&gdispMutex);
		return res;
	}
#endif

/*===========================================================================*/
/* High Level Driver Routines.                                               */
/*===========================================================================*/

void gdispDrawBox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
	/* No mutex required as we only call high level functions which have their own mutex */
	coord_t	x1, y1;

	x1 = x+cx-1;
	y1 = y+cy-1;

	if (cx > 2) {
		if (cy >= 1) {
			gdispDrawLine(x, y, x1, y, color);
			if (cy >= 2) {
				gdispDrawLine(x, y1, x1, y1, color);
				if (cy > 2) {
					gdispDrawLine(x, y+1, x, y1-1, color);
					gdispDrawLine(x1, y+1, x1, y1-1, color);
				}
			}
		}
	} else if (cx == 2) {
		gdispDrawLine(x, y, x, y1, color);
		gdispDrawLine(x1, y, x1, y1, color);
	} else if (cx == 1) {
		gdispDrawLine(x, y, x, y1, color);
	}
}

#if GDISP_NEED_CONVEX_POLYGON
	void gdispDrawPoly(coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color) {
		const point	*epnt, *p;

		epnt = &pntarray[cnt-1];
		for(p = pntarray; p < epnt; p++)
			gdispDrawLine(tx+p->x, ty+p->y, tx+p[1].x, ty+p[1].y, color);
		gdispDrawLine(tx+p->x, ty+p->y, tx+pntarray->x, ty+pntarray->y, color);
	}

	void gdispFillConvexPoly(coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color) {
		const point	*lpnt, *rpnt, *epnts;
		fixed		lx, rx, lk, rk;
		coord_t		y, ymax, lxc, rxc;

		epnts = &pntarray[cnt-1];

		/* Find a top point */
		rpnt = pntarray;
		for(lpnt=pntarray+1; lpnt <= epnts; lpnt++) {
			if (lpnt->y < rpnt->y)
				rpnt = lpnt;
		}
		lx = rx = FIXED(rpnt->x);
		y = rpnt->y;

		/* Work out the slopes of the two attached line segs */
		lpnt = rpnt <= pntarray ? epnts : rpnt-1;
		while (lpnt->y == y) {
			lx = FIXED(lpnt->x);
			lpnt = lpnt <= pntarray ? epnts : lpnt-1;
			if (!cnt--) return;
		}
		rpnt = rpnt >= epnts ? pntarray : rpnt+1;
		while (rpnt->y == y) {
			rx = rpnt->x<<16;
			rpnt = rpnt >= epnts ? pntarray : rpnt+1;
			if (!cnt--) return;
		}
		lk = (FIXED(lpnt->x) - lx) / (lpnt->y - y);
		rk = (FIXED(rpnt->x) - rx) / (rpnt->y - y);

		while(1) {
			/* Determine our boundary */
			ymax = rpnt->y < lpnt->y ? rpnt->y : lpnt->y;

			/* Scan down the line segments until we hit a boundary */
			for(; y < ymax; y++) {
				lxc = NONFIXED(lx);
				rxc = NONFIXED(rx);
				/*
				 * Doesn't print the right hand point in order to allow polygon joining.
				 * Also ensures that we draw from left to right with the minimum number
				 * of pixels.
				 */
				if (lxc < rxc) {
					if (rxc - lxc == 1)
						gdispDrawPixel(tx+lxc, ty+y, color);
					else
						gdispDrawLine(tx+lxc, ty+y, tx+rxc-1, ty+y, color);
				} else if (lxc > rxc) {
					if (lxc - rxc == 1)
						gdispDrawPixel(tx+rxc, ty+y, color);
					else
						gdispDrawLine(tx+rxc, ty+y, tx+lxc-1, ty+y, color);
				}

				lx += lk;
				rx += rk;
			}

			if (!cnt--) return;

			/* Replace the appropriate point */
			if (ymax == lpnt->y) {
				lpnt = lpnt <= pntarray ? epnts : lpnt-1;
				while (lpnt->y == y) {
					lx = FIXED(lpnt->x);
					lpnt = lpnt <= pntarray ? epnts : lpnt-1;
					if (!cnt--) return;
				}
				lk = (FIXED(lpnt->x) - lx) / (lpnt->y - y);
			} else {
				rpnt = rpnt >= epnts ? pntarray : rpnt+1;
				while (rpnt->y == y) {
					rx = FIXED(rpnt->x);
					rpnt = rpnt >= epnts ? pntarray : rpnt+1;
					if (!cnt--) return;
				}
				rk = (FIXED(rpnt->x) - rx) / (rpnt->y - y);
			}
		}
	}
#endif

#if GDISP_NEED_TEXT
	#include "mcufont.h"

	#if GDISP_NEED_ANTIALIAS && GDISP_NEED_PIXELREAD
		static void text_draw_char_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
			if (alpha == 255) {
				if (count == 1)
					gdispDrawPixel(x, y, ((color_t *)state)[0]);
				else
					gdispFillArea(x, y, count, 1, ((color_t *)state)[0]);
			} else {
				while (count--) {
					gdispDrawPixel(x, y, gdispBlendColor(((color_t *)state)[0], gdispGetPixelColor(x, y), alpha));
					x++;
				}
			}
		}
	#else
		static void text_draw_char_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
			if (alpha > 0x80) {			// A best approximation when using anti-aliased fonts but we can't actually draw them anti-aliased
				if (count == 1)
					gdispDrawPixel(x, y, ((color_t *)state)[0]);
				else
					gdispFillArea(x, y, count, 1, ((color_t *)state)[0]);
			}
		}
	#endif

	void gdispDrawChar(coord_t x, coord_t y, uint16_t c, font_t font, color_t color) {
		/* No mutex required as we only call high level functions which have their own mutex */
		mf_render_character(font, x, y, c, text_draw_char_callback, &color);
	}

	#if GDISP_NEED_ANTIALIAS
		static void text_fill_char_callback(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
			if (alpha == 255) {
				if (count == 1)
					gdispDrawPixel(x, y, ((color_t *)state)[0]);
				else
					gdispFillArea(x, y, count, 1, ((color_t *)state)[0]);
			} else {
				while (count--) {
					gdispDrawPixel(x, y, gdispBlendColor(((color_t *)state)[0], ((color_t *)state)[1], alpha));
					x++;
				}
			}
		}
	#else
		#define text_fill_char_callback	text_draw_char_callback
	#endif

	void gdispFillChar(coord_t x, coord_t y, uint16_t c, font_t font, color_t color, color_t bgcolor) {
		/* No mutex required as we only call high level functions which have their own mutex */
		color_t		state[2];

		state[0] = color;
		state[1] = bgcolor;

		gdispFillArea(x, y, mf_character_width(font, c) + font->baseline_x, font->height, bgcolor);
		mf_render_character(font, x, y, c, text_fill_char_callback, state);
	}

	typedef struct
	{
		font_t font;
		color_t color;
		coord_t	x, y;
		coord_t	cx, cy;
	} gdispDrawString_state_t;

	/* Callback to render characters. */
	static uint8_t gdispDrawString_callback(int16_t x, int16_t y, mf_char character, void *state)
	{
		gdispDrawString_state_t *s = state;
		uint8_t w;
		
		w = mf_character_width(s->font, character);
		if (x >= s->x && x+w < s->x + s->cx && y >= s->y && y+s->font->height <= s->y + s->cy)
			mf_render_character(s->font, x, y, character, text_draw_char_callback, &s->color);
		return w;
	}

	void gdispDrawString(coord_t x, coord_t y, const char *str, font_t font, color_t color) {
		/* No mutex required as we only call high level functions which have their own mutex */
		gdispDrawString_state_t state;
		
		state.font = font;
		state.color = color;
		state.x = x;
		state.y = y;
		state.cx = GDISP.Width - x;
		state.cy = GDISP.Height - y;
		
		x += font->baseline_x;
		mf_render_aligned(font, x, y, MF_ALIGN_LEFT, str, 0, gdispDrawString_callback, &state);
	}

	typedef struct
	{
		font_t font;
		color_t color[2];
		coord_t	x, y;
		coord_t	cx, cy;
	} gdispFillString_state_t;

	/* Callback to render characters. */
	static uint8_t gdispFillString_callback(int16_t x, int16_t y, mf_char character, void *state)
	{
		gdispFillString_state_t *s = state;
		uint8_t w;

		w = mf_character_width(s->font, character);
		if (x >= s->x && x+w < s->x + s->cx && y >= s->y && y+s->font->height <= s->y + s->cy)
			mf_render_character(s->font, x, y, character, text_fill_char_callback, s->color);
		return w;
	}

	void gdispFillString(coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor) {
		/* No mutex required as we only call high level functions which have their own mutex */
		gdispFillString_state_t state;
		
		state.font = font;
		state.color[0] = color;
		state.color[1] = bgcolor;
		state.x = x;
		state.y = y;
		state.cx = mf_get_string_width(font, str, 0, 0);
		state.cy = font->height;
		
		gdispFillArea(x, y, state.cx, state.cy, bgcolor);
		mf_render_aligned(font, x+font->baseline_x, y, MF_ALIGN_LEFT, str, 0, gdispFillString_callback, &state);
	}

	void gdispDrawStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, justify_t justify) {
		/* No mutex required as we only call high level functions which have their own mutex */
		gdispDrawString_state_t state;
		
		state.font = font;
		state.color = color;
		state.x = x;
		state.y = y;
		state.cx = cx;
		state.cy = cy;
		
		/* Select the anchor position */
		switch(justify) {
		case justifyCenter:
			x += (cx + 1) / 2;
			break;
		case justifyRight:
			x += cx;
			break;
		default:	// justifyLeft
			x += font->baseline_x;
			break;
		}
		y += (cy+1 - font->height)/2;

		mf_render_aligned(font, x, y, justify, str, 0, gdispDrawString_callback, &state);
	}

	void gdispFillStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgcolor, justify_t justify) {
		/* No mutex required as we only call high level functions which have their own mutex */
		gdispFillString_state_t state;

		state.font = font;
		state.color[0] = color;
		state.color[1] = bgcolor;
		state.x = x;
		state.y = y;
		state.cx = cx;
		state.cy = cy;

		gdispFillArea(x, y, cx, cy, bgcolor);
		
		/* Select the anchor position */
		switch(justify) {
		case justifyCenter:
			x += (cx + 1) / 2;
			break;
		case justifyRight:
			x += cx;
			break;
		default:	// justifyLeft
			x += font->baseline_x;
			break;
		}
		y += (cy+1 - font->height)/2;
		
		/* Render */
		mf_render_aligned(font, x, y, justify, str, 0, gdispFillString_callback, &state);
	}

	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric) {
		/* No mutex required as we only read static data */
		switch(metric) {
		case fontHeight:			return font->height;
		case fontDescendersHeight:	return font->height - font->baseline_y;
		case fontLineSpacing:		return font->line_height;
		case fontCharPadding:		return 0;
		case fontMinWidth:			return font->min_x_advance;
		case fontMaxWidth:			return font->max_x_advance;
		}
		return 0;
	}

	coord_t gdispGetCharWidth(char c, font_t font) {
		/* No mutex required as we only read static data */
		return mf_character_width(font, c);
	}

	coord_t gdispGetStringWidth(const char* str, font_t font) {
		/* No mutex required as we only read static data */
		return mf_get_string_width(font, str, 0, 0);
	}
#endif

color_t gdispBlendColor(color_t fg, color_t bg, uint8_t alpha)
{
	uint16_t fg_ratio = alpha + 1;
	uint16_t bg_ratio = 256 - alpha;
	uint16_t r, g, b;
	
	r = RED_OF(fg) * fg_ratio;
	g = GREEN_OF(fg) * fg_ratio;
	b = BLUE_OF(fg) * fg_ratio;
	
	r += RED_OF(bg) * bg_ratio;
	g += GREEN_OF(bg) * bg_ratio;
	b += BLUE_OF(bg) * bg_ratio;
	
	r /= 256;
	g /= 256;
	b /= 256;
	
	return RGB2COLOR(r, g, b);
}

#if (!defined(gdispPackPixels) && !defined(GDISP_PIXELFORMAT_CUSTOM))
	void gdispPackPixels(pixel_t *buf, coord_t cx, coord_t x, coord_t y, color_t color) {
		/* No mutex required as we only read static data */
		#if defined(GDISP_PIXELFORMAT_RGB888)
			#error "GDISP: Packed pixels not supported yet"
		#elif defined(GDISP_PIXELFORMAT_RGB444)
			#error "GDISP: Packed pixels not supported yet"
		#elif defined(GDISP_PIXELFORMAT_RGB666)
			#error "GDISP: Packed pixels not supported yet"
		#elif
			#error "GDISP: Unsupported packed pixel format"
		#endif
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */
