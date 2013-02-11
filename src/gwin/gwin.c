/*
    ChibiOS/GFX - Copyright (C) 2012
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
 * @file    src/gwin/gwin.c
 * @brief   GWIN sub-system code.
 *
 * @defgroup Window Window
 * @ingroup GWIN
 *
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GWIN || defined(__DOXYGEN__)

#include "gwin/internal.h"

// Internal routine for use by GWIN components only
// Initialise a window creating it dynamicly if required.
GHandle _gwinInit(GWindowObject *gw, coord_t x, coord_t y, coord_t width, coord_t height, size_t size) {
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
	if (!gw) {
		if (!(gw = (GWindowObject *)chHeapAlloc(NULL, size)))
			return 0;
		gw->flags = GWIN_FLG_DYNAMIC;
	} else
		gw->flags = 0;
	
	// Initialise all basic fields (except the type)
	gw->x = x;
	gw->y = y;
	gw->width = width;
	gw->height = height;
	gw->color = White;
	gw->bgcolor = Black;
#if GDISP_NEED_TEXT
	gw->font = 0;
#endif
	return (GHandle)gw;
}

/**
 * @brief   Create a basic window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gw		The window structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @note				The default drawing color gets set to White and the background drawing color to Black.
 * @note				No default font is set so make sure to set one before drawing any text.
 * @note				The dimensions and position may be changed to fit on the real screen.
 * @note				The window is not automatically cleared on creation. You must do that by calling gwinClear() (possibly after changing your background color)
 *
 * @api
 */
GHandle gwinCreateWindow(GWindowObject *gw, coord_t x, coord_t y, coord_t width, coord_t height) {
	if (!(gw = (GWindowObject *)_gwinInit((GWindowObject *)gw, x, y, width, height, sizeof(GWindowObject))))
		return 0;
	gw->type = GW_WINDOW;
	return (GHandle)gw;
}

/**
 * @brief   Destroy a window (of any type). Releases any dynamicly allocated memory.
 *
 * @param[in] gh		The window handle
 *
 * @api
 */
void gwinDestroyWindow(GHandle gh) {
	// Clean up any type specific dynamic memory allocations
	switch(gh->type) {
#if GWIN_NEED_BUTTON
	case GW_BUTTON:
		if ((gh->flags & GBTN_FLG_ALLOCTXT)) {
			gh->flags &= ~GBTN_FLG_ALLOCTXT;		// To be sure, to be sure
			chHeapFree((void *)((GButtonObject *)gh)->txt);
		}
		break;
#endif
	default:
		break;
	}

	// Clean up the structure
	if (gh->flags & GWIN_FLG_DYNAMIC) {
		gh->flags = 0;							// To be sure, to be sure
		chHeapFree((void *)gh);
	}
}

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Set the current font for this window.
 *
 * @param[in] gh		The window handle
 * @param[in] font		The font to use for text functions
 *
 * @api
 */
void gwinSetFont(GHandle gh, font_t font) {
	gh->font = font;
#if GWIN_NEED_CONSOLE
	if (font && gh->type == GW_CONSOLE) {
		((GConsoleObject *)gh)->fy = gdispGetFontMetric(font, fontHeight);
		((GConsoleObject *)gh)->fp = gdispGetFontMetric(font, fontCharPadding);
	}
#endif
}
#endif

/**
 * @brief   Clear the window
 * @note	Uses the current background color to clear the window
 *
 * @param[in] gh		The window handle
 *
 * @api
 */
void gwinClear(GHandle gh) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArea(gh->x, gh->y, gh->width, gh->height, gh->bgcolor);

	#if GWIN_NEED_CONSOLE
		if (gh->type == GW_CONSOLE) {
			((GConsoleObject *)gh)->cx = 0;
			((GConsoleObject *)gh)->cy = 0;
		}
	#endif
}

/**
 * @brief   Set a pixel in the window
 * @note	Uses the current foreground color to set the pixel
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The coordinates of the pixel
 *
 * @api
 */
void gwinDrawPixel(GHandle gh, coord_t x, coord_t y) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawPixel(gh->x+x, gh->y+y, gh->color);
}

/**
 * @brief   Draw a line in the window
 * @note	Uses the current foreground color to draw the line
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x0,y0		The start position
 * @param[in] x1,y1 	The end position
 *
 * @api
 */
void gwinDrawLine(GHandle gh, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawLine(gh->x+x0, gh->y+y0, gh->x+x1, gh->y+y1, gh->color);
}

/**
 * @brief   Draw a box in the window
 * @note	Uses the current foreground color to draw the box
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The start position
 * @param[in] cx,cy		The size of the box (outside dimensions)
 *
 * @api
 */
void gwinDrawBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawBox(gh->x+x, gh->y+y, cx, cy, gh->color);
}

/**
 * @brief   Fill an rectangular area in the window
 * @note	Uses the current foreground color to fill the box
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The start position
 * @param[in] cx,cy		The size of the box (outside dimensions)
 *
 * @api
 */
void gwinFillArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArea(gh->x+x, gh->y+y, cx, cy, gh->color);
}

/**
 * @brief   Fill an area in the window using the supplied bitmap.
 * @details The bitmap is in the pixel format specified by the low level driver
 * @note	If GDISP_NEED_ASYNC is defined then the buffer must be static
 * 			or at least retained until this call has finished the blit. You can
 * 			tell when all graphics drawing is finished by @p gdispIsBusy() going FALSE.
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x, y		The start filled area
 * @param[in] cx, cy	The width and height to be filled
 * @param[in] srcx, srcy	The bitmap position to start the fill from
 * @param[in] srccx		The width of a line in the bitmap.
 * @param[in] buffer	The pixels to use to fill the area.
 *
 * @api
 */
void gwinBlitArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispBlitAreaEx(gh->x+x, gh->y+y, cx, cy, srcx, srcy, srccx, buffer);
}

#if GDISP_NEED_CIRCLE || defined(__DOXYGEN__)
/**
 * @brief   Draw a circle in the window.
 * @note	Uses the current foreground color to draw the circle
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x, y		The center of the circle
 * @param[in] radius	The radius of the circle
 *
 * @api
 */
void gwinDrawCircle(GHandle gh, coord_t x, coord_t y, coord_t radius) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawCircle(gh->x+x, gh->y+y, radius, gh->color);
}
#endif

#if GDISP_NEED_CIRCLE || defined(__DOXYGEN__)
/**
 * @brief   Draw a filled circle in the window.
 * @note	Uses the current foreground color to draw the filled circle
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x, y		The center of the circle
 * @param[in] radius	The radius of the circle
 *
 * @api
 */
void gwinFillCircle(GHandle gh, coord_t x, coord_t y, coord_t radius) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillCircle(gh->x+x, gh->y+y, radius, gh->color);
}
#endif

#if GDISP_NEED_ELLIPSE || defined(__DOXYGEN__)
/**
 * @brief   Draw an ellipse.
 * @note	Uses the current foreground color to draw the ellipse
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The center of the ellipse
 * @param[in] a,b		The dimensions of the ellipse
 *
 * @api
 */
void gwinDrawEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawEllipse(gh->x+x, gh->y+y, a, b, gh->color);
}
#endif

#if GDISP_NEED_ELLIPSE || defined(__DOXYGEN__)
/**
 * @brief   Draw an filled ellipse.
 * @note	Uses the current foreground color to draw the filled ellipse
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The center of the ellipse
 * @param[in] a,b		The dimensions of the ellipse
 *
 * @api
 */
void gwinFillEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillEllipse(gh->x+x, gh->y+y, a, b, gh->color);
}
#endif

#if GDISP_NEED_ARC || defined(__DOXYGEN__)
/*
 * @brief	Draw an arc in the window.
 * @note	Uses the current foreground color to draw the arc
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The center point
 * @param[in] radius	The radius of the arc
 * @param[in] start		The start angle (0 to 360)
 * @param[in] end		The end angle (0 to 360)
 *
 * @api
 */
void gwinDrawArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawArc(gh->x+x, gh->y+y, radius, startangle, endangle, gh->color);
}
#endif

#if GDISP_NEED_ARC || defined(__DOXYGEN__)
/*
 * @brief	Draw a filled arc in the window.
 * @note	Uses the current foreground color to draw the filled arc
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The center point
 * @param[in] radius	The radius of the arc
 * @param[in] start		The start angle (0 to 360)
 * @param[in] end		The end angle (0 to 360)
 *
 * @api
 */
void gwinFillArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArc(gh->x+x, gh->y+y, radius, startangle, endangle, gh->color);
}
#endif

#if GDISP_NEED_PIXELREAD || defined(__DOXYGEN__)
/**
 * @brief   Get the color of a pixel in the window.
 * @return  The color of the pixel.
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The position in the window
 *
 * @api
 */
color_t gwinGetPixelColor(GHandle gh, coord_t x, coord_t y) {
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	return gdispGetPixelColor(gh->x+x, gh->y+y);
}
#endif

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Draw a text character at the specified position in the window.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The position for the text
 * @param[in] c			The character to draw
 *
 * @api
 */
void gwinDrawChar(GHandle gh, coord_t x, coord_t y, char c) {
	if (!gh->font) return;
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawChar(gh->x+x, gh->y+y, c, gh->font, gh->color);
}
#endif

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Draw a text character with a filled background at the specified position in the window.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The position for the text
 * @param[in] c			The character to draw
 *
 * @api
 */
void gwinFillChar(GHandle gh, coord_t x, coord_t y, char c) {
	if (!gh->font) return;
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillChar(gh->x+x, gh->y+y, c, gh->font, gh->color, gh->bgcolor);
}
#endif

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Draw a text string in the window
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The position for the text
 * @param[in] str		The string to draw
 *
 * @api
 */
void gwinDrawString(GHandle gh, coord_t x, coord_t y, const char *str) {
	if (!gh->font) return;
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawString(gh->x+x, gh->y+y, str, gh->font, gh->color);
}
#endif

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Draw a text string with a filled background in the window
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The position for the text
 * @param[in] str		The string to draw
 *
 * @api
 */
void gwinFillString(GHandle gh, coord_t x, coord_t y, const char *str) {
	if (!gh->font) return;
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillString(gh->x+x, gh->y+y, str, gh->font, gh->color, gh->bgcolor);
}
#endif

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Draw a text string verticly centered within the specified box.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character.
 * @note    The specified box does not need to align with the window box
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
 * @param[in] cx,cy		The width and height of the box
 * @param[in] str		The string to draw
 * @param[in] justify	Justify the text left, center or right within the box
 *
 * @api
 */
void gwinDrawStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify) {
	if (!gh->font) return;
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispDrawStringBox(gh->x+x, gh->y+y, cx, cy, str, gh->font, gh->color, justify);
}
#endif

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Draw a text string verticly centered within the specified filled box.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 * @note    The entire box is filled. Note this box does not need to align with the window box
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gh		The window handle
 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
 * @param[in] cx,cy		The width and height of the box
 * @param[in] str		The string to draw
 * @param[in] justify	Justify the text left, center or right within the box
 *
 * @api
 */
void gwinFillStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify) {
	if (!gh->font) return;
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillStringBox(gh->x+x, gh->y+y, cx, cy, str, gh->font, gh->color, gh->bgcolor, justify);
}
#endif

#endif /* GFX_USE_GWIN */
/** @} */

