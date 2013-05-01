/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    src/gdisp/gdisp.c
 * @brief   GDISP Driver code.
 *
 * @addtogroup GDISP
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GDISP

#ifdef GDISP_NEED_TEXT
	#include "gdisp/fonts.h"
#endif

/* Include the low level driver information */
#include "gdisp/lld/gdisp_lld.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if GDISP_NEED_MULTITHREAD
	#if !CH_USE_MUTEXES
		#error "GDISP: CH_USE_MUTEXES must be defined in chconf.h because GDISP_NEED_MULTITHREAD is defined"
	#endif
#endif

#if GDISP_NEED_ASYNC
	#if !CH_USE_MAILBOXES || !CH_USE_MUTEXES || !CH_USE_SEMAPHORES
		#error "GDISP: CH_USE_MAILBOXES, CH_USE_SEMAPHORES and CH_USE_MUTEXES must be defined in chconf.h because GDISP_NEED_ASYNC is defined"
	#endif
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

#if GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC
	static Mutex			gdispMutex;
#endif

#if GDISP_NEED_ASYNC
	#define GDISP_THREAD_STACK_SIZE	512		/* Just a number - not yet a reflection of actual use */
	#define GDISP_QUEUE_SIZE		8		/* We only allow a short queue */

	static Thread *			lldThread;
	static Mailbox			gdispMailbox;
	static msg_t 			gdispMailboxQueue[GDISP_QUEUE_SIZE];
	static Semaphore		gdispMsgsSem;
	static Mutex			gdispMsgsMutex;
	static gdisp_lld_msg_t	gdispMsgs[GDISP_QUEUE_SIZE];
	static WORKING_AREA(waGDISPThread, GDISP_THREAD_STACK_SIZE);
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if GDISP_NEED_ASYNC
	static msg_t GDISPThreadHandler(void *arg) {
		(void)arg;
		gdisp_lld_msg_t	*pmsg;

		#if CH_USE_REGISTRY
			chRegSetThreadName("GDISPAsyncAPI");
		#endif

		while(1) {
			/* Wait for msg with work to do. */
			chMBFetch(&gdispMailbox, (msg_t *)&pmsg, TIME_INFINITE);

			/* OK - we need to obtain the mutex in case a synchronous operation is occurring */
			chMtxLock(&gdispMutex);
			gdisp_lld_msg_dispatch(pmsg);
			chMtxUnlock();

			/* Mark the message as free */
			pmsg->action = GDISP_LLD_MSG_NOP;
			chSemSignal(&gdispMsgsSem);
		}
		return 0;
	}

	static gdisp_lld_msg_t *gdispAllocMsg(gdisp_msgaction_t action) {
		gdisp_lld_msg_t	*p;

		while(1) {		/* To be sure, to be sure */

			/* Wait for a slot */
			chSemWait(&gdispMsgsSem);

			/* Find the slot */
			chMtxLock(&gdispMsgsMutex);
			for(p=gdispMsgs; p < &gdispMsgs[GDISP_QUEUE_SIZE]; p++) {
				if (p->action == GDISP_LLD_MSG_NOP) {
					/* Allocate it */
					p->action = action;
					chMtxUnlock();
					return p;
				}
			}
			chMtxUnlock();

			/* Oops - none found, try again */
			chSemSignal(&gdispMsgsSem);
		}
	}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

#if GDISP_NEED_MULTITHREAD
	bool_t gdispInit(void) {
		bool_t	res;

		/* Initialise Mutex */
		chMtxInit(&gdispMutex);

		/* Initialise driver */
		chMtxLock(&gdispMutex);
		res = gdisp_lld_init();
		chMtxUnlock();

		return res;
	}
#elif GDISP_NEED_ASYNC
	bool_t gdispInit(void) {
		bool_t		res;
		unsigned	i;

		/* Mark all the Messages as free */
		for(i=0; i < GDISP_QUEUE_SIZE; i++)
			gdispMsgs[i].action = GDISP_LLD_MSG_NOP;

		/* Initialise our Mailbox, Mutex's and Counting Semaphore.
		 * 	A Mutex is required as well as the Mailbox and Thread because some calls have to be synchronous.
		 *	Synchronous calls get handled by the calling thread, asynchronous by our worker thread.
		 */
		chMBInit(&gdispMailbox, gdispMailboxQueue, sizeof(gdispMailboxQueue)/sizeof(gdispMailboxQueue[0]));
		chMtxInit(&gdispMutex);
		chMtxInit(&gdispMsgsMutex);
		chSemInit(&gdispMsgsSem, GDISP_QUEUE_SIZE);

		lldThread = chThdCreateStatic(waGDISPThread, sizeof(waGDISPThread), NORMALPRIO, GDISPThreadHandler, NULL);

		/* Initialise driver - synchronous */
		chMtxLock(&gdispMutex);
		res = gdisp_lld_init();
		chMtxUnlock();

		return res;
	}
#endif

#if GDISP_NEED_MULTITHREAD
	bool_t gdispIsBusy(void) {
		return FALSE;
	}
#elif GDISP_NEED_ASYNC
	bool_t gdispIsBusy(void) {
		return chMBGetUsedCountI(&gdispMailbox) != FALSE;
	}
#endif

#if GDISP_NEED_MULTITHREAD
	void gdispClear(color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_clear(color);
		chMtxUnlock();
	}
#elif GDISP_NEED_ASYNC
	void gdispClear(color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_CLEAR);
		p->clear.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if GDISP_NEED_MULTITHREAD
	void gdispDrawPixel(coord_t x, coord_t y, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_draw_pixel(x, y, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_ASYNC
	void gdispDrawPixel(coord_t x, coord_t y, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWPIXEL);
		p->drawpixel.x = x;
		p->drawpixel.y = y;
		p->drawpixel.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif
	
#if GDISP_NEED_MULTITHREAD
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_draw_line(x0, y0, x1, y1, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_ASYNC
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWLINE);
		p->drawline.x0 = x0;
		p->drawline.y0 = y0;
		p->drawline.x1 = x1;
		p->drawline.y1 = y1;
		p->drawline.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if GDISP_NEED_MULTITHREAD
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_fill_area(x, y, cx, cy, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_ASYNC
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLAREA);
		p->fillarea.x = x;
		p->fillarea.y = y;
		p->fillarea.cx = cx;
		p->fillarea.cy = cy;
		p->fillarea.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif
	
#if GDISP_NEED_MULTITHREAD
	void gdispBlitAreaEx(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		chMtxLock(&gdispMutex);
		gdisp_lld_blit_area_ex(x, y, cx, cy, srcx, srcy, srccx, buffer);
		chMtxUnlock();
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
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif
	
#if (GDISP_NEED_CLIP && GDISP_NEED_MULTITHREAD)
	void gdispSetClip(coord_t x, coord_t y, coord_t cx, coord_t cy) {
		chMtxLock(&gdispMutex);
		gdisp_lld_set_clip(x, y, cx, cy);
		chMtxUnlock();
	}
#elif GDISP_NEED_CLIP && GDISP_NEED_ASYNC
	void gdispSetClip(coord_t x, coord_t y, coord_t cx, coord_t cy) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_SETCLIP);
		p->setclip.x = x;
		p->setclip.y = y;
		p->setclip.cx = cx;
		p->setclip.cy = cy;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if (GDISP_NEED_CIRCLE && GDISP_NEED_MULTITHREAD)
	void gdispDrawCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_draw_circle(x, y, radius, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_CIRCLE && GDISP_NEED_ASYNC
	void gdispDrawCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWCIRCLE);
		p->drawcircle.x = x;
		p->drawcircle.y = y;
		p->drawcircle.radius = radius;
		p->drawcircle.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif
	
#if (GDISP_NEED_CIRCLE && GDISP_NEED_MULTITHREAD)
	void gdispFillCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_fill_circle(x, y, radius, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_CIRCLE && GDISP_NEED_ASYNC
	void gdispFillCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLCIRCLE);
		p->fillcircle.x = x;
		p->fillcircle.y = y;
		p->fillcircle.radius = radius;
		p->fillcircle.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_NEED_MULTITHREAD)
	void gdispDrawEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_draw_ellipse(x, y, a, b, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_ELLIPSE && GDISP_NEED_ASYNC
	void gdispDrawEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWELLIPSE);
		p->drawellipse.x = x;
		p->drawellipse.y = y;
		p->drawellipse.a = a;
		p->drawellipse.b = b;
		p->drawellipse.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif
	
#if (GDISP_NEED_ELLIPSE && GDISP_NEED_MULTITHREAD)
	void gdispFillEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_fill_ellipse(x, y, a, b, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_ELLIPSE && GDISP_NEED_ASYNC
	void gdispFillEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLELLIPSE);
		p->fillellipse.x = x;
		p->fillellipse.y = y;
		p->fillellipse.a = a;
		p->fillellipse.b = b;
		p->fillellipse.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if (GDISP_NEED_ARC && GDISP_NEED_MULTITHREAD)
	void gdispDrawArc(coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_draw_arc(x, y, radius, start, end, color);
		chMtxUnlock();
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
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if (GDISP_NEED_ARC && GDISP_NEED_MULTITHREAD)
	void gdispFillArc(coord_t x, coord_t y, coord_t radius, coord_t start, coord_t end, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_fill_arc(x, y, radius, start, end, color);
		chMtxUnlock();
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
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
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

#if (GDISP_NEED_TEXT && GDISP_NEED_MULTITHREAD)
	void gdispDrawChar(coord_t x, coord_t y, char c, font_t font, color_t color) {
		chMtxLock(&gdispMutex);
		gdisp_lld_draw_char(x, y, c, font, color);
		chMtxUnlock();
	}
#elif GDISP_NEED_TEXT && GDISP_NEED_ASYNC
	void gdispDrawChar(coord_t x, coord_t y, char c, font_t font, color_t color) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_DRAWCHAR);
		p->drawchar.x = x;
		p->drawchar.y = y;
		p->drawchar.c = c;
		p->drawchar.font = font;
		p->drawchar.color = color;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_NEED_MULTITHREAD)
	void gdispFillChar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		chMtxLock(&gdispMutex);
		gdisp_lld_fill_char(x, y, c, font, color, bgcolor);
		chMtxUnlock();
	}
#elif GDISP_NEED_TEXT && GDISP_NEED_ASYNC
	void gdispFillChar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_FILLCHAR);
		p->fillchar.x = x;
		p->fillchar.y = y;
		p->fillchar.c = c;
		p->fillchar.font = font;
		p->fillchar.color = color;
		p->fillchar.bgcolor = bgcolor;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif
	
#if (GDISP_NEED_PIXELREAD && (GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC))
	color_t gdispGetPixelColor(coord_t x, coord_t y) {
		color_t		c;

		/* Always synchronous as it must return a value */
		chMtxLock(&gdispMutex);
		c = gdisp_lld_get_pixel_color(x, y);
		chMtxUnlock();

		return c;
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_NEED_MULTITHREAD)
	void gdispVerticalScroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		chMtxLock(&gdispMutex);
		gdisp_lld_vertical_scroll(x, y, cx, cy, lines, bgcolor);
		chMtxUnlock();
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
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_NEED_MULTITHREAD)
	void gdispControl(unsigned what, void *value) {
		chMtxLock(&gdispMutex);
		gdisp_lld_control(what, value);
		chMtxUnlock();
	}
#elif GDISP_NEED_CONTROL && GDISP_NEED_ASYNC
	void gdispControl(unsigned what, void *value) {
		gdisp_lld_msg_t *p = gdispAllocMsg(GDISP_LLD_MSG_CONTROL);
		p->control.what = what;
		p->control.value = value;
		chMBPost(&gdispMailbox, (msg_t)p, TIME_INFINITE);
		chThdSleepMilliseconds(100);
	}
#endif

#if (GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC) && GDISP_NEED_QUERY
	void *gdispQuery(unsigned what) {
		void *res;

		chMtxLock(&gdispMutex);
		res = gdisp_lld_query(what);
		chMtxUnlock();
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
		fpcoord_t	lx, rx, lk, rk;
		coord_t		y, ymax, lxc, rxc;

		epnts = &pntarray[cnt-1];

		/* Find a top point */
		rpnt = pntarray;
		for(lpnt=pntarray+1; lpnt <= epnts; lpnt++) {
			if (lpnt->y < rpnt->y)
				rpnt = lpnt;
		}
		lx = rx = rpnt->x<<16;
		y = rpnt->y;

		/* Work out the slopes of the two attached line segs */
		lpnt = rpnt <= pntarray ? epnts : rpnt-1;
		while (lpnt->y == y) {
			lx = lpnt->x<<16;
			lpnt = lpnt <= pntarray ? epnts : lpnt-1;
			if (!cnt--) return;
		}
		rpnt = rpnt >= epnts ? pntarray : rpnt+1;
		while (rpnt->y == y) {
			rx = rpnt->x<<16;
			rpnt = rpnt >= epnts ? pntarray : rpnt+1;
			if (!cnt--) return;
		}
		lk = (((fpcoord_t)(lpnt->x)<<16) - lx) / (lpnt->y - y);
		rk = (((fpcoord_t)(rpnt->x)<<16) - rx) / (rpnt->y - y);

		while(1) {
			/* Determine our boundary */
			ymax = rpnt->y < lpnt->y ? rpnt->y : lpnt->y;

			/* Scan down the line segments until we hit a boundary */
			for(; y < ymax; y++) {
				lxc = lx>>16;
				rxc = rx>>16;
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
					lx = lpnt->x<<16;
					lpnt = lpnt <= pntarray ? epnts : lpnt-1;
					if (!cnt--) return;
				}
				lk = (((fpcoord_t)(lpnt->x)<<16) - lx) / (lpnt->y - y);
			} else {
				rpnt = rpnt >= epnts ? pntarray : rpnt+1;
				while (rpnt->y == y) {
					rx = rpnt->x<<16;
					rpnt = rpnt >= epnts ? pntarray : rpnt+1;
					if (!cnt--) return;
				}
				rk = (((fpcoord_t)(rpnt->x)<<16) - rx) / (rpnt->y - y);
			}
		}
	}
#endif

	#if GDISP_NEED_TEXT
	void gdispDrawString(coord_t x, coord_t y, const char *str, font_t font, color_t color) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w, p;
		char		c;
		int			first;
		
		if (!str) return;
		
		first = 1;
		p = font->charPadding * font->xscale;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c) * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first)
					x += p;
				else
					first = 0;
			}
			
			/* Print the character */
			gdispDrawChar(x, y, c, font, color);
			x += w;
		}
	}
#endif
	
#if GDISP_NEED_TEXT
	void gdispFillString(coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w, h, p;
		char		c;
		int			first;
		
		if (!str) return;
		
		first = 1;
		h = font->height * font->yscale;
		p = font->charPadding * font->xscale;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c) * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first) {
					gdispFillArea(x, y, p, h, bgcolor);
					x += p;
				} else
					first = 0;
			}

			/* Print the character */
			gdispFillChar(x, y, c, font, color, bgcolor);
			x += w;
		}
	}
#endif
	
#if GDISP_NEED_TEXT
	void gdispDrawStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, justify_t justify) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w, h, p, ypos, xpos;
		char		c;
		int			first;
		const char *rstr;
		
		if (!str) str = "";

		h = font->height * font->yscale;
		p = font->charPadding * font->xscale;

		/* Oops - font too large for the area */
		if (h > cy) return;

		/* See if we need to fill above the font */
		ypos = (cy - h + 1)/2;
		if (ypos > 0) {
			y += ypos;
			cy -= ypos;
		}
		
		/* See if we need to fill below the font */
		ypos = cy - h;
		if (ypos > 0)
			cy -= ypos;
		
		/* get the start of the printable string and the xpos */
		switch(justify) {
		case justifyCenter:
			/* Get the length of the entire string */
			w = gdispGetStringWidth(str, font);
			if (w <= cx)
				xpos = x + (cx - w)/2;
			else {
				/* Calculate how much of the string we need to get rid of */
				ypos = (w - cx)/2;
				xpos = 0;
				first = 1;
				while(*str) {
					/* Get the next printable character */
					c = *str++;
					w = _getCharWidth(font, c) * font->xscale;
					if (!w) continue;
					
					/* Handle inter-character padding */
					if (p) {
						if (!first) {
							xpos += p;
							if (xpos > ypos) break;
						} else
							first = 0;
					}

					/* Print the character */
					xpos += w;
					if (xpos > ypos) break;
				}
				xpos = ypos - xpos + x;
			}
			break;
		case justifyRight:
			/* Find the end of the string */
			for(rstr = str; *str; str++);
			xpos = x+cx - 2;
			first = 1;
			for(str--; str >= rstr; str--) {
				/* Get the next printable character */
				c = *str;
				w = _getCharWidth(font, c) * font->xscale;
				if (!w) continue;
				
				/* Handle inter-character padding */
				if (p) {
					if (!first) {
						if (xpos - p < x) break;
						xpos -= p;
					} else
						first = 0;
				}

				/* Print the character */
				if (xpos - w < x) break;
				xpos -= w;
			}
			str++;
			break;
		case justifyLeft:
			/* Fall through */
		default:
			xpos = x+1;
			break;
		}
		
		/* Print characters until we run out of room */
		first = 1;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c) * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first) {
					if (xpos + p > x+cx) break;
					xpos += p;
				} else
					first = 0;
			}

			/* Print the character */
			if (xpos + w > x+cx) break;
			gdispDrawChar(xpos, y, c, font, color);
			xpos += w;
		}
	}
#endif
	
#if GDISP_NEED_TEXT
	void gdispFillStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgcolor, justify_t justify) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w, h, p, ypos, xpos;
		char		c;
		int			first;
		const char *rstr;
		
		if (!str) str = "";

		h = font->height * font->yscale;
		p = font->charPadding * font->xscale;

		/* Oops - font too large for the area */
		if (h > cy) return;

		/* See if we need to fill above the font */
		ypos = (cy - h + 1)/2;
		if (ypos > 0) {
			gdispFillArea(x, y, cx, ypos, bgcolor);
			y += ypos;
			cy -= ypos;
		}
		
		/* See if we need to fill below the font */
		ypos = cy - h;
		if (ypos > 0) {
			gdispFillArea(x, y+cy-ypos, cx, ypos, bgcolor);
			cy -= ypos;
		}
		
		/* get the start of the printable string and the xpos */
		switch(justify) {
		case justifyCenter:
			/* Get the length of the entire string */
			w = gdispGetStringWidth(str, font);
			if (w <= cx)
				xpos = x + (cx - w)/2;
			else {
				/* Calculate how much of the string we need to get rid of */
				ypos = (w - cx)/2;
				xpos = 0;
				first = 1;
				while(*str) {
					/* Get the next printable character */
					c = *str++;
					w = _getCharWidth(font, c) * font->xscale;
					if (!w) continue;
					
					/* Handle inter-character padding */
					if (p) {
						if (!first) {
							xpos += p;
							if (xpos > ypos) break;
						} else
							first = 0;
					}

					/* Print the character */
					xpos += w;
					if (xpos > ypos) break;
				}
				xpos = ypos - xpos + x;
			}
			break;
		case justifyRight:
			/* Find the end of the string */
			for(rstr = str; *str; str++);
			xpos = x+cx - 2;
			first = 1;
			for(str--; str >= rstr; str--) {
				/* Get the next printable character */
				c = *str;
				w = _getCharWidth(font, c) * font->xscale;
				if (!w) continue;
				
				/* Handle inter-character padding */
				if (p) {
					if (!first) {
						if (xpos - p < x) break;
						xpos -= p;
					} else
						first = 0;
				}

				/* Print the character */
				if (xpos - w < x) break;
				xpos -= w;
			}
			str++;
			break;
		case justifyLeft:
			/* Fall through */
		default:
			xpos = x+1;
			break;
		}
		
		/* Fill any space to the left */
		if (x < xpos)
			gdispFillArea(x, y, xpos-x, cy, bgcolor);
		
		/* Print characters until we run out of room */
		first = 1;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c) * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first) {
					if (xpos + p > x+cx) break;
					gdispFillArea(xpos, y, p, cy, bgcolor);
					xpos += p;
				} else
					first = 0;
			}

			/* Print the character */
			if (xpos + w > x+cx) break;
			gdispFillChar(xpos, y, c, font, color, bgcolor);
			xpos += w;
		}
		
		/* Fill any space to the right */
		if (xpos < x+cx)
			gdispFillArea(xpos, y, x+cx-xpos, cy, bgcolor);
	}
#endif
	
#if GDISP_NEED_TEXT
	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric) {
		/* No mutex required as we only read static data */
		switch(metric) {
		case fontHeight:			return font->height * font->yscale;
		case fontDescendersHeight:	return font->descenderHeight * font->yscale;
		case fontLineSpacing:		return font->lineSpacing * font->yscale;
		case fontCharPadding:		return font->charPadding * font->xscale;
		case fontMinWidth:			return font->minWidth * font->xscale;
		case fontMaxWidth:			return font->maxWidth * font->xscale;
		}
		return 0;
	}
#endif
	
#if GDISP_NEED_TEXT
	coord_t gdispGetCharWidth(char c, font_t font) {
		/* No mutex required as we only read static data */
		return _getCharWidth(font, c) * font->xscale;
	}
#endif
	
#if GDISP_NEED_TEXT
	coord_t gdispGetStringWidth(const char* str, font_t font) {
		/* No mutex required as we only read static data */
		coord_t		w, p, x;
		char		c;
		int			first;
		
		first = 1;
		x = 0;
		p = font->charPadding * font->xscale;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c)  * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first)
					x += p;
				else
					first = 0;
			}
			
			/* Add the character width */
			x += w;
		}
		return x;
	}
#endif

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
