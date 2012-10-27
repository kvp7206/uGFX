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
 * @file    gwin.c
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

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if GDISP_NEED_TEXT
/*
 * Stream interface implementation. The interface is write only
 */
static size_t GWinStreamWrite(void *ip, const uint8_t *bp, size_t n) { gwinPutCharArray((GWindow *)ip, (const char *)bp, n); return RDY_OK; }
static size_t GWinStreamRead(void *ip, uint8_t *bp, size_t n) {	(void)ip; (void)bp; (void)n; return 0; }
static msg_t GWinStreamPut(void *ip, uint8_t b) { gwinPutChar((GWindow *)ip, (char)b); return RDY_OK; }
static msg_t GWinStreamGet(void *ip) {(void)ip; return RDY_OK; }
static msg_t GWinStreamPutTimed(void *ip, uint8_t b, systime_t time) { (void)time; gwinPutChar((GWindow *)ip, (char)b); return RDY_OK; }
static msg_t GWinStreamGetTimed(void *ip, systime_t timeout) { (void)ip; (void)timeout; return RDY_OK; }
static size_t GWinStreamWriteTimed(void *ip, const uint8_t *bp, size_t n, systime_t time) { (void)time; gwinPutCharArray((GWindow *)ip, (const char *)bp, n); return RDY_OK; }
static size_t GWinStreamReadTimed(void *ip, uint8_t *bp, size_t n, systime_t time) { (void)ip; (void)bp; (void)n; (void)time; return 0; }

static const struct GWindowVMT vmt = {
	GWinStreamWrite,
	GWinStreamRead,
	GWinStreamPut,
	GWinStreamGet,
	GWinStreamPutTimed,
	GWinStreamGetTimed,
	GWinStreamWriteTimed,
	GWinStreamReadTimed
};
#endif

/*===========================================================================*/
/* High Level Driver Routines.                                               */
/*===========================================================================*/

/**
 * @brief   Initialise a window.
 * @return  FALSE if there is no resultant drawing area, otherwise TRUE.
 *
 * @param[in] gw      The window to initialise
 * @param[in] x,y     The screen co-ordinates for the bottom left corner of the window
 * @param[in] width   The width of the window
 * @param[in] height  The height of the window
 * @note			  The drawing color gets set to White and the background drawing color to Black.
 * @note			  No default font is set so make sure to set one before drawing any text.
 * @note			  The dimensions and position may be changed to fit on the real screen.
 *
 * @api
 */
bool_t gwinInit(GWindow *gw, coord_t x, coord_t y, coord_t width, coord_t height) {
	coord_t	w, h;

	w = gdispGetWidth();
	h = gdispGetHeight();
	if (x < 0) { width += x; x = 0; }
	if (y < 0) { height += y; y = 0; }
	if (x >= w || y >= h) return FALSE;
	if (x+width > w) width = w - x;
	if (y+height > h) height = h - y;
	gw->x = x;
	gw->y = y;
	gw->width = width;
	gw->height = height;
	gw->color = White;
	gw->bgcolor = Black;
#if GDISP_NEED_TEXT
	gw->txt.font = 0;
	gw->txt.fy = 0;
	gw->txt.fp = 0;
	gw->txt.cx = 0;
	gw->txt.cy = 0;
	gw->txt.vmt = &vmt;
#endif
	return TRUE;
}

#if GDISP_NEED_TEXT
/**
 * @brief   Set the current font for this window.
 * @pre		The window must be initialised.
 *
 * @param[in] gw      The window
 * @param[in] font    The font to use for text functions
 *
 * @api
 */
void gwinSetFont(GWindow *gw, font_t font) {
	gw->txt.font = font;
	gw->txt.fy = gdispGetFontMetric(font, fontHeight);
	gw->txt.fp = gdispGetFontMetric(font, fontCharPadding);
}
#endif

/**
 * @brief   Clear the window
 * @pre		The window must be initialised.
 * @note	Uses the current background color to clear the window
 *
 * @param[in] gw      The window
 *
 * @api
 */
void gwinClear(GWindow *gw) {
	gdispFillArea(gw->x, gw->y, gw->width, gw->height, gw->bgcolor);
}

/**
 * @brief   Set a pixel in the window
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to set the pixel
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gw      The window
 *
 * @api
 */
void gwinDrawPixel(GWindow *gw, coord_t x, coord_t y) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawPixel(gw->x+x, gw->y+y, gw->color);
}

/**
 * @brief   Draw a line in the window
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the line
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gw      The window
 * @param[in] x0,y0		The start position
 * @param[in] x1,y1 	The end position
 *
 * @api
 */
void gwinDrawLine(GWindow *gw, coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawLine(gw->x+x0, gw->y+y0, gw->x+x1, gw->y+y1, gw->color);
}

/**
 * @brief   Draw a box in the window
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the box
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gw      The window
 * @param[in] x,y     The start position
 * @param[in] cx,cy   The size of the box (outside dimensions)
 *
 * @api
 */
void gwinDrawBox(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawBox(gw->x+x, gw->y+y, cx, cy, gw->color);
}

/**
 * @brief   Fill an rectangular area in the window
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to fill the box
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] gw      The window
 * @param[in] x,y     The start position
 * @param[in] cx,cy   The size of the box (outside dimensions)
 *
 * @api
 */
void gwinFillArea(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispFillArea(gw->x+x, gw->y+y, cx, cy, gw->color);
}

/**
 * @brief   Fill an area in the window using the supplied bitmap.
 * @details The bitmap is in the pixel format specified by the low level driver
 * @pre		The window must be initialised.
 * @note	If GDISP_NEED_ASYNC is defined then the buffer must be static
 * 			or at least retained until this call has finished the blit. You can
 * 			tell when all graphics drawing is finished by @p gdispIsBusy() going FALSE.
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x, y     The start filled area
 * @param[in] cx, cy   The width and height to be filled
 * @param[in] srcx, srcy   The bitmap position to start the fill from
 * @param[in] srccx    The width of a line in the bitmap.
 * @param[in] buffer   The pixels to use to fill the area.
 *
 * @api
 */
void gwinBlitArea(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispBlitAreaEx(gw->x+x, gw->y+y, cx, cy, srcx, srcy, srccx, buffer);
}

#if GDISP_NEED_CIRCLE
/**
 * @brief   Draw a circle in the window.
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the circle
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x, y    The center of the circle
 * @param[in] radius  The radius of the circle
 *
 * @api
 */
void gwinDrawCircle(GWindow *gw, coord_t x, coord_t y, coord_t radius) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawCircle(gw->x+x, gw->y+y, radius, gw->color);
}
#endif

#if GDISP_NEED_CIRCLE
/**
 * @brief   Draw a filled circle in the window.
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the filled circle
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x, y    The center of the circle
 * @param[in] radius  The radius of the circle
 *
 * @api
 */
void gwinFillCircle(GWindow *gw, coord_t x, coord_t y, coord_t radius) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispFillCircle(gw->x+x, gw->y+y, radius, gw->color);
}
#endif

#if GDISP_NEED_ELLIPSE
/**
 * @brief   Draw an ellipse.
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the ellipse
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The center of the ellipse
 * @param[in] a,b     The dimensions of the ellipse
 *
 * @api
 */
void gwinDrawEllipse(GWindow *gw, coord_t x, coord_t y, coord_t a, coord_t b) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawEllipse(gw->x+x, gw->y+y, a, b, gw->color);
}
#endif

#if GDISP_NEED_ELLIPSE
/**
 * @brief   Draw an filled ellipse.
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the filled ellipse
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The center of the ellipse
 * @param[in] a,b     The dimensions of the ellipse
 *
 * @api
 */
void gwinFillEllipse(GWindow *gw, coord_t x, coord_t y, coord_t a, coord_t b) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispFillEllipse(gw->x+x, gw->y+y, a, b, gw->color);
}
#endif

#if GDISP_NEED_ARC
/*
 * @brief	Draw an arc in the window.
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the arc
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y		The center point
 * @param[in] radius	The radius of the arc
 * @param[in] start		The start angle (0 to 360)
 * @param[in] end		The end angle (0 to 360)
 *
 * @api
 */
void gwinDrawArc(GWindow *gw, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawArc(gw->x+x, gw->y+y, radius, startangle, endangle, gw->color);
}
#endif

#if GDISP_NEED_ARC
/*
 * @brief	Draw a filled arc in the window.
 * @pre		The window must be initialised.
 * @note	Uses the current foreground color to draw the filled arc
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y		The center point
 * @param[in] radius	The radius of the arc
 * @param[in] start		The start angle (0 to 360)
 * @param[in] end		The end angle (0 to 360)
 *
 * @api
 */
void gwinFillArc(GWindow *gw, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispFillArc(gw->x+x, gw->y+y, radius, startangle, endangle, gw->color);
}
#endif

#if GDISP_NEED_PIXELREAD
/**
 * @brief   Get the color of a pixel in the window.
 * @pre		The window must be initialised.
 * @return  The color of the pixel.
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The position in the window
 *
 * @api
 */
color_t gwinGetPixelColor(GWindow *gw, coord_t x, coord_t y) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	return gdispGetPixelColor(gw->x+x, gw->y+y);
}
#endif

#if GDISP_NEED_SCROLL
/**
 * @brief   Scroll vertically a section of the window.
 * @pre		The window must be initialised.
 * @note    If lines is >= cy, it is equivelent to a area fill with the current background drawing color.
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
 *
 * @api
 */
void gwinVerticalScroll(GWindow *gw, int lines) {
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispVerticalScroll(gw->x, gw->y, gw->width, gw->height, lines, gw->bgcolor);
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Draw a text character at the specified position in the window.
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The position for the text
 * @param[in] c       The character to draw
 *
 * @api
 */
void gwinDrawChar(GWindow *gw, coord_t x, coord_t y, char c) {
	if (!gw->txt.font) return;
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawChar(gw->x+x, gw->y+y, c, gw->txt.font, gw->color);
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Draw a text character with a filled background at the specified position in the window.
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The position for the text
 * @param[in] c       The character to draw
 *
 * @api
 */
void gwinFillChar(GWindow *gw, coord_t x, coord_t y, char c) {
	if (!gw->txt.font) return;
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispFillChar(gw->x+x, gw->y+y, c, gw->txt.font, gw->color, gw->bgcolor);
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Draw a text string in the window
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The position for the text
 * @param[in] str     The string to draw
 *
 * @api
 */
void gwinDrawString(GWindow *gw, coord_t x, coord_t y, const char *str) {
	if (!gw->txt.font) return;
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispDrawString(gw->x+x, gw->y+y, str, gw->txt.font, gw->color);
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Draw a text string with a filled background in the window
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The position for the text
 * @param[in] str     The string to draw
 *
 * @api
 */
void gwinFillString(GWindow *gw, coord_t x, coord_t y, const char *str) {
	if (!gw->txt.font) return;
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispFillString(gw->x+x, gw->y+y, str, gw->txt.font, gw->color, gw->bgcolor);
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Draw a text string verticly centered within the specified box.
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 * @note    The entire box is filled. Note this box does not need to align with the window box
 * @note	May leave GDISP clipping to this window's dimensions
 *
 * @param[in] x,y     The position for the text (need to define top-right or base-line - check code)
 * @param[in] str     The string to draw
 * @param[in] justify Justify the text left, center or right within the box
 *
 * @api
 */
void gwinBoxString(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify) {
	if (!gw->txt.font) return;
	#if GDISP_SET_CLIP
		gdispSetClip(gw->x, gw->y+y, gw->width, gw->height);
	#endif
	gdispFillStringBox(gw->x+x, gw->y+y, cx, cy, str, gw->txt.font, gw->color, gw->bgcolor, justify);
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Put a character at the cursor position in the window.
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 *
 * @param[in] c     The character to draw
 *
 * @api
 */
void gwinPutChar(GWindow *gw, char c) {
	uint8_t width;

	if (!gw->txt.font) return;

	if (c == '\n') {
		/* clear the text at the end of the line */
		if (gw->txt.cx < gw->width)
			gdispFillArea(gw->x + gw->txt.cx, gw->y + gw->txt.cy, gw->width - gw->txt.cx, gw->txt.fy, gw->bgcolor);
		gw->txt.cx = 0;
		gw->txt.cy += gw->txt.fy;
	} else if (c == '\r') {
		// gw->cx = 0;
	} else {
		width = gdispGetCharWidth(c, gw->txt.font) + gw->txt.fp;
		if (gw->txt.cx + width >= gw->width) {
			/* clear the text at the end of the line */
			if (gw->txt.cy + gw->txt.fy <= gw->height)
				gdispFillArea(gw->x + gw->txt.cx, gw->y + gw->txt.cy, gw->width - (gw->txt.cx + width), gw->txt.fy, gw->bgcolor);
			gw->txt.cx = 0;
			gw->txt.cy += gw->txt.fy;
		}

		if (gw->txt.cy + gw->txt.fy > gw->height) {
#if GDISP_NEED_SCROLL
			/* scroll the console */
			gdispVerticalScroll(gw->x, gw->y, gw->width, gw->height, gw->txt.fy, gw->bgcolor);
			/* reset the cursor to the start of the last line */
			gw->txt.cx = 0;
			gw->txt.cy = (((int16_t)(gw->height/gw->txt.fy))-1)*gw->txt.fy;
#else
			/* clear the console */
			gdispFillArea(gw->x, gw->y, gw->width, gw->height, gw->bgcolor);
			/* reset the cursor to the top of the window */
			gw->txt.cx = 0;
			gw->txt.cy = 0;
#endif
		}

		gdispDrawChar(gw->x + gw->txt.cx, gw->y + gw->txt.cy, c, gw->txt.font, gw->color);

		/* update cursor */
		gw->txt.cx += width;
	}
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Put a string at the cursor position in the window. It will wrap lines as required.
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] str     The string to draw
 *
 * @api
 */
void gwinPutString(GWindow *gw, const char *str) {
	while(*str)
		gwinPutChar(gw, *str++);
}
#endif

#if GDISP_NEED_TEXT
/**
 * @brief   Put the character array at the cursor position in the window. It will wrap lines as required.
 * @pre		The window must be initialised.
 * @pre		The font must have been set.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] str     The string to draw
 * @param[in] n       The number of characters to draw
 *
 * @api
 */
void gwinPutCharArray(GWindow *gw, const char *str, size_t n) {
	while(n--)
		gwinPutChar(gw, *str++);
}
#endif

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_C */
/** @} */
