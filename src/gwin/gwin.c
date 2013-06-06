/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

#include "gfx.h"

#if GFX_USE_GWIN

#include "gwin/class_gwin.h"

static const gwinVMT basegwinVMT = {
		"GWIN",					// The classname
		0,						// The destroy routine
		0,						// The after-clear routine
};

static font_t	defaultFont;
static color_t	defaultFgColor = White;
static color_t	defaultBgColor = Black;

// Internal routine for use by GWIN components only
// Initialise a window creating it dynamicly if required.
GHandle _gwinInit(GWindowObject *pgw, coord_t x, coord_t y, coord_t width, coord_t height, size_t size, const gwinVMT *vmt) {
	coord_t	w, h;

	// Check the window size against the screen size
	w = gdispGetWidth();
	h = gdispGetHeight();
	if (x < 0) { width += x; x = 0; }
	if (y < 0) { height += y; y = 0; }
	if (x >= w || y >= h) return 0;
	if (x+width > w) width = w - x;
	if (y+height > h) height = h - y;
	
	// Allocate the structure if necessary
	if (!pgw) {
		if (!(pgw = (GWindowObject *)gfxAlloc(size)))
			return 0;
		pgw->flags = GWIN_FLG_DYNAMIC;
	} else
		pgw->flags = 0;
	
	// Initialise all basic fields
	pgw->vmt = vmt;
	pgw->x = x;
	pgw->y = y;
	pgw->width = width;
	pgw->height = height;
	pgw->color = defaultFgColor;
	pgw->bgcolor = defaultBgColor;
	#if GDISP_NEED_TEXT
		pgw->font = defaultFont;
	#endif
	return (GHandle)pgw;
}

GHandle gwinCreateWindow(GWindowObject *pgw, coord_t x, coord_t y, coord_t width, coord_t height) {
	return _gwinInit(pgw, x, y, width, height, sizeof(GWindowObject), &basegwinVMT);
}

void gwinDestroy(GHandle gh) {
	if (gh->vmt->Destroy)
		gh->vmt->Destroy(gh);

	// Clean up the structure
	if (gh->flags & GWIN_FLG_DYNAMIC) {
		gh->flags = 0;							// To be sure, to be sure
		gfxFree((void *)gh);
	}
}

const char *gwinGetClassName(GHandle gh) {
	return gh->vmt->classname;
}

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

	void gwinSetFont(GHandle gh, font_t font) {
		gh->font = font;
	}
#endif

void gwinClear(GHandle gh) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArea(gh->x, gh->y, gh->width, gh->height, gh->bgcolor);
	if (gh->vmt->AfterClear)
		gh->vmt->AfterClear(gh);
}

void gwinDrawPixel(GHandle gh, coord_t x, coord_t y) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawPixel(gh->x+x, gh->y+y, gh->color);
}

void gwinDrawLine(GHandle gh, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawLine(gh->x+x0, gh->y+y0, gh->x+x1, gh->y+y1, gh->color);
}

void gwinDrawBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawBox(gh->x+x, gh->y+y, cx, cy, gh->color);
}

void gwinFillArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArea(gh->x+x, gh->y+y, cx, cy, gh->color);
}

void gwinBlitArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispBlitAreaEx(gh->x+x, gh->y+y, cx, cy, srcx, srcy, srccx, buffer);
}

#if GDISP_NEED_CIRCLE
	void gwinDrawCircle(GHandle gh, coord_t x, coord_t y, coord_t radius) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawCircle(gh->x+x, gh->y+y, radius, gh->color);
	}

	void gwinFillCircle(GHandle gh, coord_t x, coord_t y, coord_t radius) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillCircle(gh->x+x, gh->y+y, radius, gh->color);
	}
#endif

#if GDISP_NEED_ELLIPSE
	void gwinDrawEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawEllipse(gh->x+x, gh->y+y, a, b, gh->color);
	}

	void gwinFillEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillEllipse(gh->x+x, gh->y+y, a, b, gh->color);
	}
#endif

#if GDISP_NEED_ARC
	void gwinDrawArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawArc(gh->x+x, gh->y+y, radius, startangle, endangle, gh->color);
	}

	void gwinFillArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillArc(gh->x+x, gh->y+y, radius, startangle, endangle, gh->color);
	}
#endif

#if GDISP_NEED_PIXELREAD
	color_t gwinGetPixelColor(GHandle gh, coord_t x, coord_t y) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		return gdispGetPixelColor(gh->x+x, gh->y+y);
	}
#endif

#if GDISP_NEED_TEXT
	void gwinDrawChar(GHandle gh, coord_t x, coord_t y, char c) {
		if (!gh->font) return;
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawChar(gh->x+x, gh->y+y, c, gh->font, gh->color);
	}

	void gwinFillChar(GHandle gh, coord_t x, coord_t y, char c) {
		if (!gh->font) return;
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillChar(gh->x+x, gh->y+y, c, gh->font, gh->color, gh->bgcolor);
	}

	void gwinDrawString(GHandle gh, coord_t x, coord_t y, const char *str) {
		if (!gh->font) return;
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawString(gh->x+x, gh->y+y, str, gh->font, gh->color);
	}

	void gwinFillString(GHandle gh, coord_t x, coord_t y, const char *str) {
		if (!gh->font) return;
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillString(gh->x+x, gh->y+y, str, gh->font, gh->color, gh->bgcolor);
	}

	void gwinDrawStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify) {
		if (!gh->font) return;
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawStringBox(gh->x+x, gh->y+y, cx, cy, str, gh->font, gh->color, justify);
	}

	void gwinFillStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify) {
		if (!gh->font) return;
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillStringBox(gh->x+x, gh->y+y, cx, cy, str, gh->font, gh->color, gh->bgcolor, justify);
	}
#endif

#if GDISP_NEED_CONVEX_POLYGON
	void gwinDrawPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispDrawPoly(tx+gh->x, ty+gh->y, pntarray, cnt, gh->color);
	}

	void gwinFillConvexPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		gdispFillConvexPoly(tx+gh->x, ty+gh->y, pntarray, cnt, gh->color);
	}
#endif

#if GDISP_NEED_IMAGE
	gdispImageError gwinImageDraw(GHandle gh, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy) {
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		return gdispImageDraw(img, gh->x+x, gh->y+y, cx, cy, sx, sy);
	}
#endif

#endif /* GFX_USE_GWIN */
/** @} */

