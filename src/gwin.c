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
 * @file    src/gwin.c
 * @brief   GWIN Driver code.
 *
 * @addtogroup GWIN
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gwin.h"

#ifndef _GWIN_C
#define _GWIN_C

#if GFX_USE_GWIN || defined(__DOXYGEN__)

#include <string.h>

#define GWIN_CONSOLE_USE_CLEAR_LINES			TRUE
#define GWIN_CONSOLE_USE_FILLED_CHARS			FALSE

#define GWIN_FLG_DYNAMIC				0x0001
#define GWIN_FIRST_CONTROL_FLAG			0x0002
#define GBTN_FLG_ALLOCTXT				(GWIN_FIRST_CONTROL_FLAG<<0)

// Initialise a window creating it dynamicly if required.
static GHandle gwinInit(GWindowObject *gw, coord_t x, coord_t y, coord_t width, coord_t height, size_t size) {
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
		if (!(gw = (GWindowObject *)malloc(size)))
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
	if (!(gw = (GWindowObject *)gwinInit((GWindowObject *)gw, x, y, width, height, sizeof(GWindowObject))))
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
			free((char *)((GButtonObject *)gh)->txt);
		}
		break;
#endif
	default:
		break;
	}

	// Clean up the structure
	if (gh->flags & GWIN_FLG_DYNAMIC) {
		gh->flags = 0;							// To be sure, to be sure
		free(gh);
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
	#if GDISP_SET_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillArea(gh->x, gh->y, gh->width, gh->height, gh->bgcolor);
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
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
	#if GDISP_SET_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	gdispFillStringBox(gh->x+x, gh->y+y, cx, cy, str, gh->font, gh->color, gh->bgcolor, justify);
}
#endif

/*------------------------------------------------------------------------------------------------------------------------*/

#if GWIN_NEED_CONSOLE || defined(__DOXYGEN__)

/*
 * Stream interface implementation. The interface is write only
 */

#define Stream2GWindow(ip)		((GHandle)(((char *)(ip)) - (size_t)(&(((GConsoleObject *)0)->stream))))

static size_t GWinStreamWrite(void *ip, const uint8_t *bp, size_t n) { gwinPutCharArray(Stream2GWindow(ip), (const char *)bp, n); return RDY_OK; }
static size_t GWinStreamRead(void *ip, uint8_t *bp, size_t n) {	(void)ip; (void)bp; (void)n; return 0; }
static msg_t GWinStreamPut(void *ip, uint8_t b) { gwinPutChar(Stream2GWindow(ip), (char)b); return RDY_OK; }
static msg_t GWinStreamGet(void *ip) {(void)ip; return RDY_OK; }
static msg_t GWinStreamPutTimed(void *ip, uint8_t b, systime_t time) { (void)time; gwinPutChar(Stream2GWindow(ip), (char)b); return RDY_OK; }
static msg_t GWinStreamGetTimed(void *ip, systime_t timeout) { (void)ip; (void)timeout; return RDY_OK; }
static size_t GWinStreamWriteTimed(void *ip, const uint8_t *bp, size_t n, systime_t time) { (void)time; gwinPutCharArray(Stream2GWindow(ip), (const char *)bp, n); return RDY_OK; }
static size_t GWinStreamReadTimed(void *ip, uint8_t *bp, size_t n, systime_t time) { (void)ip; (void)bp; (void)n; (void)time; return 0; }

struct GConsoleWindowVMT_t {
	_base_asynchronous_channel_methods
};

static const struct GConsoleWindowVMT_t GWindowConsoleVMT = {
	GWinStreamWrite,
	GWinStreamRead,
	GWinStreamPut,
	GWinStreamGet,
	GWinStreamPutTimed,
	GWinStreamGetTimed,
	GWinStreamWriteTimed,
	GWinStreamReadTimed
};

/**
 * @brief   Create a console window.
 * @details	A console window allows text to be written using chprintf() (and the console functions defined here).
 * @brief	Text in a console window supports newlines and will wrap text as required.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gc		The GConsoleObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @param[in] font		The font to use
 * @note				The console is not automatically cleared on creation. You must do that by calling gwinClear() (possibly after changing your background color)
 * @note				If the dispay does not support scrolling, the window will be cleared when the bottom line is reached.
 * @note				The default drawing color gets set to White and the background drawing color to Black.
 * @note				The dimensions and position may be changed to fit on the real screen.
 *
 * @api
 */
GHandle gwinCreateConsole(GConsoleObject *gc, coord_t x, coord_t y, coord_t width, coord_t height, font_t font) {
	if (!(gc = (GConsoleObject *)gwinInit((GWindowObject *)gc, x, y, width, height, sizeof(GConsoleObject))))
		return 0;
	gc->gwin.type = GW_CONSOLE;
	gwinSetFont(&gc->gwin, font);
	gc->stream.vmt = &GWindowConsoleVMT;
	gc->cx = 0;
	gc->cy = 0;
	return (GHandle)gc;
}

/**
 * @brief   Get a stream from a console window suitable for use with chprintf().
 * @return	The stream handle or NULL if this is not a console window.
 *
 * @param[in] gh	The window handle (must be a console window)
 *
 * @api
 */
BaseSequentialStream *gwinGetConsoleStream(GHandle gh) {
	if (gh->type != GW_CONSOLE)
		return 0;
	return (BaseSequentialStream *)&(((GConsoleObject *)(gh))->stream);
}

/**
 * @brief   Put a character at the cursor position in the window.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] c		The character to draw
 *
 * @api
 */
void gwinPutChar(GHandle gh, char c) {
	uint8_t			width;
	#define gcw		((GConsoleObject *)gh)

	if (gh->type != GW_CONSOLE || !gh->font) return;

	#if GDISP_SET_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	
	if (c == '\n') {
		gcw->cx = 0;
		gcw->cy += gcw->fy;
		// We use lazy scrolling here and only scroll when the next char arrives
	} else if (c == '\r') {
		// gcw->cx = 0;
	} else {
		width = gdispGetCharWidth(c, gh->font) + gcw->fp;
		if (gcw->cx + width >= gh->width) {
			gcw->cx = 0;
			gcw->cy += gcw->fy;
		}

		if (gcw->cy + gcw->fy > gh->height) {
#if GDISP_NEED_SCROLL
			/* scroll the console */
			gdispVerticalScroll(gh->x, gh->y, gh->width, gh->height, gcw->fy, gh->bgcolor);
			/* reset the cursor to the start of the last line */
			gcw->cx = 0;
			gcw->cy = (((coord_t)(gh->height/gcw->fy))-1)*gcw->fy;
#else
			/* clear the console */
			gdispFillArea(gh->x, gh->y, gh->width, gh->height, gh->bgcolor);
			/* reset the cursor to the top of the window */
			gcw->cx = 0;
			gcw->cy = 0;
#endif
		}

#if GWIN_CONSOLE_USE_CLEAR_LINES
		/* clear to the end of the line */
		if (gcw->cx == 0)
			gdispFillArea(gh->x, gh->y + gcw->cy, gh->width, gcw->fy, gh->bgcolor);
#endif
#if GWIN_CONSOLE_USE_FILLED_CHARS
		gdispFillChar(gh->x + gcw->cx, gh->y + gcw->cy, c, gh->font, gh->color, gh->bgcolor);
#else
		gdispDrawChar(gh->x + gcw->cx, gh->y + gcw->cy, c, gh->font, gh->color);
#endif

		/* update cursor */
		gcw->cx += width;
	}
	#undef gcw
}

/**
 * @brief   Put a string at the cursor position in the window. It will wrap lines as required.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] str	The string to draw
 *
 * @api
 */
void gwinPutString(GHandle gh, const char *str) {
	while(*str)
		gwinPutChar(gh, *str++);
}

/**
 * @brief   Put the character array at the cursor position in the window. It will wrap lines as required.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] str	The string to draw
 * @param[in] n		The number of characters to draw
 *
 * @api
 */
void gwinPutCharArray(GHandle gh, const char *str, size_t n) {
	while(n--)
		gwinPutChar(gh, *str++);
}
#endif

/*------------------------------------------------------------------------------------------------------------------------*/

#if GWIN_NEED_BUTTON || defined(__DOXYGEN__)

static const GButtonStyle GButtonDefaultStyle = {
	GBTN_3D,
	HTML2COLOR(0x404040),		// color_up_edge;
	HTML2COLOR(0xE0E0E0),		// color_up_fill;
	HTML2COLOR(0x000000),		// color_up_txt;
	HTML2COLOR(0x404040),		// color_dn_edge;
	HTML2COLOR(0x808080),		// color_dn_fill;
	HTML2COLOR(0x404040),		// color_dn_txt;
	};

/**
 * @brief   Create a button window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gb		The GConsoleWindow structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @param[in] font		The font to use
 * @param[in] type		The type of button
 * @note				The drawing color gets set to White and the background drawing color to Black.
 * @note				The dimensions and position may be changed to fit on the real screen.
 * @note				The button is not automatically drawn. Call gwinButtonDraw() after changing the button style or setting the text.
 *
 * @api
 */
GHandle gwinCreateButton(GButtonObject *gb, coord_t x, coord_t y, coord_t width, coord_t height, font_t font, GButtonType type) {
	if (!(gb = (GButtonObject *)gwinInit((GWindowObject *)gb, x, y, width, height, sizeof(GButtonObject))))
		return 0;
	gb->gwin.type = GW_BUTTON;
	gwinSetFont(&gb->gwin, font);
	gwinSetButtonStyle(&gb->gwin, &GButtonDefaultStyle);
	gb->type = type;
	gb->state = GBTN_UP;
	gb->txt = "";
	gb->callback = 0;
	gb->inputsrc = 0;
	return (GHandle)gb;
}

/**
 * @brief   Set the style of a button.
 * @details	The button style is defined by its shape and colours.
 *
 * @param[in] gh		The window handle (must be a button window)
 * @param[in] style		The button style to set.
 * @note				The button is not automatically redrawn. Call gwinButtonDraw() after changing the button style
 *
 * @api
 */
void gwinSetButtonStyle(GHandle gh, const GButtonStyle *style) {
	#define gbw		((GButtonObject *)gh)
	if (gh->type != GW_BUTTON)
		return;
	
	gbw->style.shape = style->shape;
	gbw->style.color_up_edge = style->color_up_edge;
	gbw->style.color_up_fill = style->color_up_fill;
	gbw->style.color_dn_edge = style->color_dn_edge;
	gbw->style.color_dn_fill = style->color_dn_fill;
	gbw->style.color_up_txt = style->color_up_txt;
	gbw->style.color_dn_txt = style->color_dn_txt;
	#undef gbw
}

/**
 * @brief   Set the text of a button.
 *
 * @param[in] gh		The window handle (must be a button window)
 * @param[in] txt		The button text to set. This must be a constant string unless useAlloc is set.
 * @param[in] useAlloc	If TRUE the string specified will be copied into dynamically allocated memory.
 * @note				The button is not automatically redrawn. Call gwinButtonDraw() after changing the button text.
 *
 * @api
 */
void gwinSetButtonText(GHandle gh, const char *txt, bool_t useAlloc) {
	#define gbw		((GButtonObject *)gh)
	if (gh->type != GW_BUTTON)
		return;

	// Dispose of the old string
	if ((gh->flags & GBTN_FLG_ALLOCTXT)) {
		gh->flags &= ~GBTN_FLG_ALLOCTXT;
		if (gbw->txt) {
			free((char *)gbw->txt);
			gbw->txt = "";
		}
	}
	// Alloc the new text if required
	if (txt && useAlloc) {
		char *str;
		
		if ((str = (char *)malloc(strlen(txt)+1))) {
			gh->flags |= GBTN_FLG_ALLOCTXT;
			strcpy(str, txt);
		}
		txt = (const char *)str;
	}
	
	gbw->txt = txt ? txt : "";
	#undef gbw
}

/**
 * @brief   Redraw the button.
 *
 * @param[in] gh		The window handle (must be a button window)
 *
 * @api
 */
void gwinButtonDraw(GHandle gh) {
	color_t			cedge;
	color_t			cfill;
	color_t			ctxt;
	const char *	txt;
	#define gbw		((GButtonObject *)gh)
	#define RND_CNR_SIZE	5
	
	if (gh->type != GW_BUTTON)
		return;

	#if GDISP_SET_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	
	// Get the text (safely)
	txt = gh->font && gbw->txt ? gbw->txt : "";
	
	// Determine the colors to use
	switch(gbw->state) {
	case GBTN_DOWN:
		cedge = gbw->style.color_dn_edge;
		cfill = gbw->style.color_dn_fill;
		ctxt = gbw->style.color_dn_txt;
		break;
	case GBTN_UP:	default:
		cedge = gbw->style.color_up_edge;
		cfill = gbw->style.color_up_fill;
		ctxt = gbw->style.color_up_txt;
		break;
	}
	
	// Draw according to the shape specified.
	switch(gbw->style.shape) {
#if GDISP_NEED_ARC
	case GBTN_ROUNDED:
		if (gh->width >= 2*RND_CNR_SIZE+10) {
			gdispFillRoundedBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, RND_CNR_SIZE-1, cfill);
			gdispDrawStringBox(gh->x+1, gh->y+RND_CNR_SIZE, gh->width-2, gh->height-(2*RND_CNR_SIZE), txt, gh->font, ctxt, justifyCenter);
			gdispDrawRoundedBox(gh->x, gh->y, gh->width, gh->height, RND_CNR_SIZE, cedge);
			break;
		}
		/* Fall Through */
#endif
	case GBTN_SQUARE:
		gdispFillStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, ctxt, cfill, justifyCenter);
		gdispDrawBox(gh->x, gh->y, gh->width, gh->height, cedge);
		break;
#if GDISP_NEED_ELLIPSE
	case GBTN_ELLIPSE:
		gdispFillEllipse(gh->x+1, gh->y+1, gh->width/2-1, gh->height/2-1, cfill);
		gdispDrawStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, ctxt, justifyCenter);
		gdispDrawEllipse(gh->x, gh->y, gh->width/2, gh->height/2, cedge);
		break;
#endif
	case GBTN_3D:	default:
		gdispFillStringBox(gh->x, gh->y, gh->width-1, gh->height-1, txt, gh->font, ctxt, cfill, justifyCenter);
		gdispDrawLine(gh->x+gh->width-1, gh->y, gh->x+gh->width-1, gh->y+gh->height-1, cedge);
		gdispDrawLine(gh->x, gh->y+gh->height-1, gh->x+gh->width-2, gh->y+gh->height-1, cedge);
		break;
	}
	#undef gbw
}

//void gwinSetButtonCallback(GHandle gh, ????);
//void gwinSetButtonInput(GHandle gh, ????);
#endif

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_C */
/** @} */

