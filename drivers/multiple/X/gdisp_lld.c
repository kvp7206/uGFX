/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/multiple/X/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for X.
 */

#include "gfx.h"

#if GFX_USE_GDISP

/**
 * Our color model - Default or 24 bit only.
 *
 * At present we don't define this as we don't need to.
 * It may however be useful later if we implement bitblits.
 * As this may be dead code we don't include it in gdisp/options.h
 */
#ifndef GDISP_FORCE_24BIT	
	#define GDISP_FORCE_24BIT	FALSE
#endif

#if GINPUT_NEED_MOUSE
	/* Include mouse support code */
	#include "ginput/lld/mouse.h"
#endif

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		480
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		640
#endif

Display			*dis;
int				scr;
Window			win;
Pixmap			pix;
XEvent			evt;
GC 				gc;
Colormap		cmap;
XVisualInfo		vis;
int				depth;
#if GINPUT_NEED_MOUSE
	coord_t			mousex, mousey;
	uint16_t		mousebuttons;
#endif

static void ProcessEvent(void) {
	XColor	col;

	switch(evt.type) {
	case Expose:
		XCopyArea(dis, pix, win, gc,
			evt.xexpose.x, evt.xexpose.y,
			evt.xexpose.width, evt.xexpose.height,   
			evt.xexpose.x, evt.xexpose.y);
		break;
#if GINPUT_NEED_MOUSE
	case ButtonPress:
		mousex = evt.xbutton.x;
		mousey = evt.xbutton.y;
		switch(evt.xbutton.button){
		case 1:	mousebuttons |= GINPUT_MOUSE_BTN_LEFT;		break;
		case 2:	mousebuttons |= GINPUT_MOUSE_BTN_MIDDLE;	break;
		case 3:	mousebuttons |= GINPUT_MOUSE_BTN_RIGHT;		break;
		case 4:	mousebuttons |= GINPUT_MOUSE_BTN_4;			break;
		}
		#if GINPUT_MOUSE_POLL_PERIOD == TIME_INFINITE
			ginputMouseWakeup();
		#endif
		break;
	case ButtonRelease:
		mousex = evt.xbutton.x;
		mousey = evt.xbutton.y;
		switch(evt.xbutton.button){
		case 1:	mousebuttons &= ~GINPUT_MOUSE_BTN_LEFT;		break;
		case 2:	mousebuttons &= ~GINPUT_MOUSE_BTN_MIDDLE;	break;
		case 3:	mousebuttons &= ~GINPUT_MOUSE_BTN_RIGHT;	break;
		case 4:	mousebuttons &= ~GINPUT_MOUSE_BTN_4;		break;
		}
		#if GINPUT_MOUSE_POLL_PERIOD == TIME_INFINITE
			ginputMouseWakeup();
		#endif
		break;
	case MotionNotify:
		mousex = evt.xmotion.x;
		mousey = evt.xmotion.y;
		#if GINPUT_MOUSE_POLL_PERIOD == TIME_INFINITE
			ginputMouseWakeup();
		#endif
		break;
#endif
	}
}

/* this is the X11 thread which keeps track of all events */
static DECLARE_THREAD_STACK(waXThread, 1024);
static DECLARE_THREAD_FUNCTION(ThreadX, arg) {
	(void)arg;

	while(1) {
		gfxSleepMilliseconds(100);
		while(XPending(dis)) {
			XNextEvent(dis, &evt);
			ProcessEvent();
		}
	}
	return 0;
}
 
static int FatalXIOError(Display *d) {
	(void) d;

	/* The window has closed */
	fprintf(stderr, "GFX Window closed!\n");
	exit(0);
}

bool_t gdisp_lld_init(void)
{
	XSizeHints				*pSH;
	XSetWindowAttributes	xa;
	XTextProperty			WindowTitle;
	char *					WindowTitleText;
	gfxThreadHandle			hth;

	#if GFX_USE_OS_LINUX || GFX_USE_OS_OSX
		XInitThreads();
	#endif

	dis = XOpenDisplay(NULL);
	scr = DefaultScreen(dis);

	#if GDISP_FORCE_24BIT	
		if (!XMatchVisualInfo(dis, scr, 24, TrueColor, &vis)) {
			fprintf(stderr, "Your display has no TrueColor mode\n");
			XCloseDisplay(dis);
			return FALSE;
		}
		cmap = XCreateColormap(dis, RootWindow(dis, scr),
				vis.visual, AllocNone);
	#else
		vis.visual = CopyFromParent;
		vis.depth = DefaultDepth(dis, scr);
		cmap = DefaultColormap(dis, scr);
	#endif
	fprintf(stderr, "Running GFX Window in %d bit color\n", vis.depth);

	xa.colormap = cmap;
	xa.border_pixel = 0xFFFFFF;
	xa.background_pixel = 0x000000;
	
	win = XCreateWindow(dis, RootWindow(dis, scr), 16, 16,
			GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT,
			0, vis.depth, InputOutput, vis.visual,
			CWBackPixel|CWColormap|CWBorderPixel, &xa);
	XSync(dis, TRUE);
	
	WindowTitleText = "GFX";
	XStringListToTextProperty(&WindowTitleText, 1, &WindowTitle);
	XSetWMName(dis, win, &WindowTitle);
	XSetWMIconName(dis, win, &WindowTitle);
	XSync(dis, TRUE);
			
	pSH = XAllocSizeHints();
	pSH->flags = PSize | PMinSize | PMaxSize;
	pSH->min_width = pSH->max_width = pSH->base_width = GDISP_SCREEN_WIDTH;
	pSH->min_height = pSH->max_height = pSH->base_height = GDISP_SCREEN_HEIGHT;
	XSetWMNormalHints(dis, win, pSH);
	XFree(pSH);
	XSync(dis, TRUE);
	
	pix = XCreatePixmap(dis, win, 
				GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT, vis.depth);
	XSync(dis, TRUE);

	gc = XCreateGC(dis, win, 0, 0);
	XSetBackground(dis, gc, BlackPixel(dis, scr));
	XSync(dis, TRUE);

	XSelectInput(dis, win, StructureNotifyMask);
	XMapWindow(dis, win);
	do { XNextEvent(dis, &evt); } while (evt.type != MapNotify);

	/* start the X11 thread */
	XSetIOErrorHandler(FatalXIOError);
	XSelectInput(dis, win,
		ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	if (!(hth = gfxThreadCreate(waXThread, sizeof(waXThread), HIGH_PRIORITY, ThreadX, 0))) {
		fprintf(stderr, "Cannot start X Thread\n");
		XCloseDisplay(dis);
		exit(0);
	}
	#if GFX_USE_OS_LINUX || GFX_USE_OS_OSX
		pthread_detach(hth);
	#endif
	gfxThreadClose(hth);
	
    /* Initialise the GDISP structure to match */
    GDISP.Orientation = GDISP_ROTATE_0;
    GDISP.Powermode = powerOn;
    GDISP.Backlight = 100;
    GDISP.Contrast = 50;
    GDISP.Width = GDISP_SCREEN_WIDTH;
    GDISP.Height = GDISP_SCREEN_HEIGHT;
    #if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
            GDISP.clipx0 = 0;
            GDISP.clipy0 = 0;
            GDISP.clipx1 = GDISP.Width;
            GDISP.clipy1 = GDISP.Height;
    #endif
    return TRUE;
}

void gdisp_lld_draw_pixel(coord_t x, coord_t y, color_t color)
{
	XColor	col;

   #if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
        // Clip pre orientation change
        if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
    #endif

	col.red = RED_OF(color) << 8;
	col.green = GREEN_OF(color) << 8;
	col.blue = BLUE_OF(color) << 8;
	XAllocColor(dis, cmap, &col);
	XSetForeground(dis, gc, col.pixel);
	XDrawPoint(dis, pix, gc, (int)x, (int)y );
	XDrawPoint(dis, win, gc, (int)x, (int)y );
	XFlush(dis);
}

void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
	XColor	col;
	
    #if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
        // Clip pre orientation change
        if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
        if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
        if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
        if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
        if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
    #endif

	col.red = RED_OF(color) << 8;
	col.green = GREEN_OF(color) << 8;
	col.blue = BLUE_OF(color) << 8;
	XAllocColor(dis, cmap, &col);
	XSetForeground(dis, gc, col.pixel);
	XFillRectangle(dis, pix, gc, x, y, cx, cy);
	XFillRectangle(dis, win, gc, x, y, cx, cy);
	XFlush(dis);
}

// Start of Bitblit code
//XImage			bitmap;
//pixel_t			*bits;
//	bits = malloc(vis.depth * GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT);
//	bitmap = XCreateImage(dis, vis, vis.depth, ZPixmap,
//				0, bits, GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT,
//				0, 0);

#if GINPUT_NEED_MOUSE

	void ginput_lld_mouse_init(void) {}

	void ginput_lld_mouse_get_reading(MouseReading *pt) {
		pt->x = mousex;
		pt->y = mousey;
		pt->z = (mousebuttons & GINPUT_MOUSE_BTN_LEFT) ? 100 : 0;
		pt->buttons = mousebuttons;
	}

#endif /* GINPUT_NEED_MOUSE */

#endif /* GFX_USE_GDISP */
/** @} */

