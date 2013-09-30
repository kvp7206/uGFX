/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

// Used by the NULL window manager
#define MIN_WIN_WIDTH	3
#define MIN_WIN_HEIGHT	3

/*-----------------------------------------------
 * The default window manager (GNullWindowManager)
 *-----------------------------------------------*/

#if GFX_USE_GWIN && GWIN_NEED_WINDOWMANAGER

#include "gwin/class_gwin.h"

/*-----------------------------------------------
 * Data
 *-----------------------------------------------*/

static void WM_Init(void);
static void WM_DeInit(void);
static bool_t WM_Add(GHandle gh, const GWindowInit *pInit);
static void WM_Delete(GHandle gh);
static void WM_Visible(GHandle gh);
static void WM_Redim(GHandle gh, coord_t x, coord_t y, coord_t w, coord_t h);
static void WM_Raise(GHandle gh);
static void WM_MinMax(GHandle gh, GWindowMinMax minmax);

static const gwmVMT GNullWindowManagerVMT = {
	WM_Init,
	WM_DeInit,
	WM_Add,
	WM_Delete,
	WM_Visible,
	WM_Redim,
	WM_Raise,
	WM_MinMax,
};

static const GWindowManager	GNullWindowManager = {
	&GNullWindowManagerVMT,
};

gfxQueueASync			_GWINList;
GWindowManager *		_GWINwm;

/*-----------------------------------------------
 * Window Routines
 *-----------------------------------------------*/

void _gwmInit(void) {
	gfxQueueASyncInit(&_GWINList);
	_GWINwm = (GWindowManager *)&GNullWindowManager;
	_GWINwm->vmt->Init();
}

void gwinSetWindowManager(struct GWindowManager *gwm) {
	if (!gwm)
		gwm = (GWindowManager *)&GNullWindowManager;
	if (_GWINwm != gwm) {
		_GWINwm->vmt->DeInit();
		_GWINwm = gwm;
		_GWINwm->vmt->Init();
	}
}

void gwinSetMinMax(GHandle gh, GWindowMinMax minmax) {
	_GWINwm->vmt->MinMax(gh, minmax);
}

void gwinRaise(GHandle gh) {
	_GWINwm->vmt->Raise(gh);
}

GWindowMinMax gwinGetMinMax(GHandle gh) {
	if (gh->flags & GWIN_FLG_MINIMIZED)
		return GWIN_MINIMIZE;
	if (gh->flags & GWIN_FLG_MAXIMIZED)
		return GWIN_MAXIMIZE;
	return GWIN_NORMAL;
}

/*-----------------------------------------------
 * Window Manager Routines
 *-----------------------------------------------*/

static void WM_Init(void) {
	// We don't need to do anything here.
	// A full window manager would move the windows around, add borders etc

	// clear the screen
	// cycle through the windows already defined displaying them
	// or cut all the window areas out of the screen and clear the remainder
}

static void WM_DeInit(void) {
	// We don't need to do anything here.
	// A full window manager would remove any borders etc
}

static bool_t WM_Add(GHandle gh, const GWindowInit *pInit) {
	// Note the window will not be marked as visible yet

	// Put it on the queue
	gfxQueueASyncPut(&_GWINList, &gh->wmq);

	// Make sure the size is valid
	WM_Redim(gh, pInit->x, pInit->y, pInit->width, pInit->height);
	return TRUE;
}

static void WM_Delete(GHandle gh) {
	// Make the window invisible and clear the area underneath
	if ((gh->flags & GWIN_FLG_VISIBLE)) {
		gh->flags &= ~GWIN_FLG_VISIBLE;
		gdispFillArea(gh->x, gh->y, gh->width, gh->height, gwinGetDefaultBgColor());
	}

	// Remove it from the queue
	gfxQueueASyncRemove(&_GWINList, &gh->wmq);
}

static void WM_Visible(GHandle gh) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	if ((gh->flags & GWIN_FLG_VISIBLE)) {
		if (gh->vmt->Redraw)
			gh->vmt->Redraw(gh);
		else
			gdispFillArea(gh->x, gh->y, gh->width, gh->height, gh->bgcolor);
		// A real window manager would also redraw the borders here
	} else
		gdispFillArea(gh->x, gh->y, gh->width, gh->height, gwinGetDefaultBgColor());
}

static void WM_Redim(GHandle gh, coord_t x, coord_t y, coord_t w, coord_t h) {
	// This is the simplest way of doing it - just clip the the screen
	// If it won't fit on the screen move it around until it does.
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if (x > gdispGetWidth()-MIN_WIN_WIDTH)		x = gdispGetWidth()-MIN_WIN_WIDTH;
	if (y > gdispGetHeight()-MIN_WIN_HEIGHT)	y = gdispGetHeight()-MIN_WIN_HEIGHT;
	if (w < MIN_WIN_WIDTH) { w = MIN_WIN_WIDTH; }
	if (h < MIN_WIN_HEIGHT) { h = MIN_WIN_HEIGHT; }
	if (x+w > gdispGetWidth()) w = gdispGetWidth() - x;
	if (y+h > gdispGetHeight()) h = gdispGetHeight() - y;

	// If there has been no resize just exit
	if (gh->x == x && gh->y == y && gh->width == w && gh->height == h)
		return;

	// Clear the old area
	if ((gh->flags & GWIN_FLG_VISIBLE))
		gdispFillArea(gh->x, gh->y, gh->width, gh->height, gwinGetDefaultBgColor());

	// Set the new size
	gh->x = x; gh->y = y;
	gh->width = w; gh->height = h;

	// Redraw the window (if possible)
	if ((gh->flags & GWIN_FLG_VISIBLE)) {
		if (gh->vmt->Redraw) {
			#if GDISP_NEED_CLIP
				gdispSetClip(gh->x, gh->y, gh->width, gh->height);
			#endif
			gh->vmt->Redraw(gh);
		}
	}
}

static void WM_MinMax(GHandle gh, GWindowMinMax minmax) {
	(void)gh; (void) minmax;
	// We don't support minimising, maximising or restoring
}

static void WM_Raise(GHandle gh) {
	// Take it off the list and then put it back on top
	// The order of the list then reflects the z-order.
	gfxQueueASyncRemove(&_GWINList, &gh->wmq);
	gfxQueueASyncPut(&_GWINList, &gh->wmq);

	// Redraw the window
	if ((gh->flags & GWIN_FLG_VISIBLE)) {
		if (gh->vmt->Redraw) {
			#if GDISP_NEED_CLIP
				gdispSetClip(gh->x, gh->y, gh->width, gh->height);
			#endif
			gh->vmt->Redraw(gh);
		}
	}
}

#endif /* GFX_USE_GWIN && GWIN_NEED_WINDOWMANAGER */
/** @} */
