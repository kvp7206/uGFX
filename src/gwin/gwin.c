/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

#include "gfx.h"

#if GFX_USE_GWIN

#include "gwin/class_gwin.h"

// Needed if there is no window manager
#define MIN_WIN_WIDTH	1
#define MIN_WIN_HEIGHT	1

/*-----------------------------------------------
 * Data
 *-----------------------------------------------*/

static const gwinVMT basegwinVMT = {
		"GWIN",					// The classname
		0,						// The destroy routine
		0,						// The redraw routine
		0,						// The after-clear routine
};

static color_t	defaultFgColor = White;
static color_t	defaultBgColor = Black;
#if GDISP_NEED_TEXT
	static font_t	defaultFont;
#endif
#if GWIN_NEED_WINDOWMANAGER
	gfxQueueASync			_GWINList;
	extern GWindowManager	GNullWindowManager;
	static GWindowManager *	cwm;
#endif

/*-----------------------------------------------
 * Helper Routines
 *-----------------------------------------------*/

#if !GWIN_NEED_WINDOWMANAGER
	static void _gwm_vis(GHandle gh) {
		if (gh->vmt->Redraw) {
			#if GDISP_NEED_CLIP
				gdispSetClip(gh->x, gh->y, gh->width, gh->height);
			#endif
			gh->vmt->Redraw(gh);
		} else
			gwinClear(gh);
	}
	static void _gwm_redim(GHandle gh, coord_t x, coord_t y, coord_t w, coord_t h) {
		if (x < 0) { w += x; x = 0; }
		if (y < 0) { h += y; y = 0; }
		if (x > gdispGetWidth()-MIN_WIN_WIDTH)		x = gdispGetWidth()-MIN_WIN_WIDTH;
		if (y > gdispGetHeight()-MIN_WIN_HEIGHT)	y = gdispGetHeight()-MIN_WIN_HEIGHT;
		if (w < MIN_WIN_WIDTH) { w = MIN_WIN_WIDTH; }
		if (h < MIN_WIN_HEIGHT) { h = MIN_WIN_HEIGHT; }
		if (x+w > gdispGetWidth()) w = gdispGetWidth() - x;
		if (y+h > gdispGetHeight()) h = gdispGetHeight() - y;
		gh->x = x; gh->y = y;
		gh->width = w; gh->height = h;
	}
#endif

/*-----------------------------------------------
 * Class Routines
 *-----------------------------------------------*/

void _gwinInit(void) {
	#if GWIN_NEED_WIDGET
		extern void _gwidgetInit(void);

		_gwidgetInit();
	#endif
	#if GWIN_NEED_WINDOWMANAGER
		gfxQueueASyncInit(&_GWINList);
		cwm = &GNullWindowManager;
		cwm->vmt->Init();
	#endif
}

// Internal routine for use by GWIN components only
// Initialise a window creating it dynamically if required.
GHandle _gwindowCreate(GWindowObject *pgw, coord_t x, coord_t y, coord_t width, coord_t height, size_t size, const gwinVMT *vmt, uint16_t flags) {
	// Allocate the structure if necessary
	if (!pgw) {
		if (!(pgw = (GWindowObject *)gfxAlloc(size)))
			return 0;
		pgw->flags = flags|GWIN_FLG_DYNAMIC;
	} else
		pgw->flags = flags;
	
	// Initialise all basic fields
	pgw->vmt = vmt;
	pgw->color = defaultFgColor;
	pgw->bgcolor = defaultBgColor;
	#if GDISP_NEED_TEXT
		pgw->font = defaultFont;
	#endif

	#if GWIN_NEED_WINDOWMANAGER
		if (!cwm->vmt->Add(pgw, x, y, width, height)) {
			if ((pgw->flags & GWIN_FLG_DYNAMIC))
				gfxFree(pgw);
			return 0;
		}
	#else
		_gwm_redim(pgw, x, y, width, height);
		if ((pgw->flags & GWIN_FLG_VISIBLE))
			_gwm_vis(pgw);
	#endif

	return (GHandle)pgw;
}

/*-----------------------------------------------
 * Routines that affect all windows
 *-----------------------------------------------*/

#if GWIN_NEED_WINDOWMANAGER
	void gwinSetWindowManager(struct GWindowManager *gwm) {
		if (!gwm)
			gwm = &GNullWindowManager;
		if (cwm != gwm) {
			cwm->vmt->DeInit();
			cwm = gwm;
			cwm->vmt->Init();
		}
	}
#endif

void gwinSetDefaultColor(color_t clr) {
	defaultFgColor = clr;
}

void gwinSetDefaultBgColor(color_t bgclr) {
	defaultBgColor = bgclr;
}

#if GDISP_NEED_TEXT
	void gwinSetDefaultFont(font_t font) {
		defaultFont = font;
	}
#endif

/*-----------------------------------------------
 * The GWindow Routines
 *-----------------------------------------------*/

GHandle gwinCreateWindow(GWindowObject *pgw, coord_t x, coord_t y, coord_t width, coord_t height) {
	return _gwindowCreate(pgw, x, y, width, height, sizeof(GWindowObject), &basegwinVMT, GWIN_FLG_VISIBLE);
}

void gwinDestroy(GHandle gh) {
	// Remove from the window manager
	#if GWIN_NEED_WINDOWMANAGER
		cwm->vmt->Delete(gh);
	#endif

	// Class destroy routine
	if (gh->vmt->Destroy)
		gh->vmt->Destroy(gh);

	// Clean up the structure
	if (gh->flags & GWIN_FLG_DYNAMIC)
		gfxFree((void *)gh);

	gh->flags = 0;							// To be sure, to be sure
}

const char *gwinGetClassName(GHandle gh) {
	return gh->vmt->classname;
}

void gwinSetVisible(GHandle gh, bool_t visible) {
	if (visible) {
		if (!(gh->flags & GWIN_FLG_VISIBLE)) {
			gh->flags |= GWIN_FLG_VISIBLE;
			#if GWIN_NEED_WINDOWMANAGER
				cwm->vmt->Visible(gh);
			#else
				_gwm_vis(gh);
			#endif
		}
	} else {
		if ((gh->flags & GWIN_FLG_VISIBLE)) {
			gh->flags &= ~GWIN_FLG_VISIBLE;
			#if GWIN_NEED_WINDOWMANAGER
				cwm->vmt->Visible(gh);
			#endif
		}
	}
}

bool_t gwinGetVisible(GHandle gh) {
	return (gh->flags & GWIN_FLG_VISIBLE) ? TRUE : FALSE;
}

void gwinMove(GHandle gh, coord_t x, coord_t y) {
	#if GWIN_NEED_WINDOWMANAGER
		cwm->vmt->Redim(gh, x, y, gh->width, gh->height);
	#else
		_gwm_redim(gh, x, y, gh->width, gh->height);
	#endif
}

void gwinResize(GHandle gh, coord_t width, coord_t height) {
	#if GWIN_NEED_WINDOWMANAGER
		cwm->vmt->Redim(gh, gh->x, gh->y, width, height);
	#else
		_gwm_redim(gh, gh->x, gh->y, width, height);
	#endif
}

void gwinSetMinMax(GHandle gh, GWindowMinMax minmax) {
	#if GWIN_NEED_WINDOWMANAGER
		cwm->vmt->MinMax(gh, minmax);
	#else
		(void) gh;
		(void) minmax;
	#endif
}

void gwinRaise(GHandle gh) {
	#if GWIN_NEED_WINDOWMANAGER
		cwm->vmt->Raise(gh);
	#else
		if ((gh->flags & GWIN_FLG_VISIBLE)) {
			if (gh->vmt->Redraw) {
				#if GDISP_NEED_CLIP
					gdispSetClip(gh->x, gh->y, gh->width, gh->height);
				#endif
				gh->vmt->Redraw(gh);
			}
		}
	#endif
}

GWindowMinMax gwinGetMinMax(GHandle gh) {
	if (gh->flags & GWIN_FLG_MINIMIZED)
		return GWIN_MINIMIZE;
	if (gh->flags & GWIN_FLG_MAXIMIZED)
		return GWIN_MAXIMIZE;
	return GWIN_NORMAL;
}

#if GDISP_NEED_TEXT
	void gwinSetFont(GHandle gh, font_t font) {
		gh->font = font;
	}
#endif

void gwinClear(GHandle gh) {
	if (!((gh->flags & GWIN_FLG_VISIBLE)))
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArea(gh->x, gh->y, gh->width, gh->height, gh->bgcolor);
	if (gh->vmt->AfterClear)
		gh->vmt->AfterClear(gh);
}

void gwinDrawPixel(GHandle gh, coord_t x, coord_t y) {
	if (!((gh->flags & GWIN_FLG_VISIBLE)))
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawPixel(gh->x+x, gh->y+y, gh->color);
}

void gwinDrawLine(GHandle gh, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	if (!((gh->flags & GWIN_FLG_VISIBLE)))
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawLine(gh->x+x0, gh->y+y0, gh->x+x1, gh->y+y1, gh->color);
}

void gwinDrawBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	if (!((gh->flags & GWIN_FLG_VISIBLE)))
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawBox(gh->x+x, gh->y+y, cx, cy, gh->color);
}

void gwinFillArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	if (!((gh->flags & GWIN_FLG_VISIBLE)))
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArea(gh->x+x, gh->y+y, cx, cy, gh->color);
}

void gwinBlitArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
	if (!((gh->flags & GWIN_FLG_VISIBLE)))
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispBlitAreaEx(gh->x+x, gh->y+y, cx, cy, srcx, srcy, srccx, buffer);
}

#if GDISP_NEED_CIRCLE
	void gwinDrawCircle(GHandle gh, coord_t x, coord_t y, coord_t radius) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawCircle(gh->x+x, gh->y+y, radius, gh->color);
	}

	void gwinFillCircle(GHandle gh, coord_t x, coord_t y, coord_t radius) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillCircle(gh->x+x, gh->y+y, radius, gh->color);
	}
#endif

#if GDISP_NEED_ELLIPSE
	void gwinDrawEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawEllipse(gh->x+x, gh->y+y, a, b, gh->color);
	}

	void gwinFillEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillEllipse(gh->x+x, gh->y+y, a, b, gh->color);
	}
#endif

#if GDISP_NEED_ARC
	void gwinDrawArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawArc(gh->x+x, gh->y+y, radius, startangle, endangle, gh->color);
	}

	void gwinFillArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillArc(gh->x+x, gh->y+y, radius, startangle, endangle, gh->color);
	}
#endif

#if GDISP_NEED_PIXELREAD
	color_t gwinGetPixelColor(GHandle gh, coord_t x, coord_t y) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		return gdispGetPixelColor(gh->x+x, gh->y+y);
	}
#endif

#if GDISP_NEED_TEXT
	void gwinDrawChar(GHandle gh, coord_t x, coord_t y, char c) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)) || !gh->font)
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawChar(gh->x+x, gh->y+y, c, gh->font, gh->color);
	}

	void gwinFillChar(GHandle gh, coord_t x, coord_t y, char c) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)) || !gh->font)
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillChar(gh->x+x, gh->y+y, c, gh->font, gh->color, gh->bgcolor);
	}

	void gwinDrawString(GHandle gh, coord_t x, coord_t y, const char *str) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)) || !gh->font)
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawString(gh->x+x, gh->y+y, str, gh->font, gh->color);
	}

	void gwinFillString(GHandle gh, coord_t x, coord_t y, const char *str) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)) || !gh->font)
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillString(gh->x+x, gh->y+y, str, gh->font, gh->color, gh->bgcolor);
	}

	void gwinDrawStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)) || !gh->font)
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawStringBox(gh->x+x, gh->y+y, cx, cy, str, gh->font, gh->color, justify);
	}

	void gwinFillStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)) || !gh->font)
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillStringBox(gh->x+x, gh->y+y, cx, cy, str, gh->font, gh->color, gh->bgcolor, justify);
	}
#endif

#if GDISP_NEED_CONVEX_POLYGON
	void gwinDrawPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawPoly(tx+gh->x, ty+gh->y, pntarray, cnt, gh->color);
	}

	void gwinFillConvexPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillConvexPoly(tx+gh->x, ty+gh->y, pntarray, cnt, gh->color);
	}
#endif

#if GDISP_NEED_IMAGE
	gdispImageError gwinImageDraw(GHandle gh, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy) {
		if (!((gh->flags & GWIN_FLG_VISIBLE)))
			return GDISP_IMAGE_ERR_OK;

		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		return gdispImageDraw(img, gh->x+x, gh->y+y, cx, cy, sx, sy);
	}
#endif

#endif /* GFX_USE_GWIN */
/** @} */

