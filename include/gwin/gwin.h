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
 * @file    include/gwin/gwin.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Window Window
 * @ingroup GWIN
 *
 * @details	GWIN provides a basic window manager which allows it to easily
 *			create and destroy different windows on runtime. Each window
 *			will have it's own properties such as colors, brushes as well as
 *			it's own drawing origin.
 *			Moving the windows around is not supported yet.
 *
 * @pre		GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_H
#define _GWIN_H

#include "gfx.h"

#if GFX_USE_GWIN || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef uint16_t	GWindowType;
#define GW_WINDOW				0x0000
#define GW_FIRST_USER_WINDOW	0x8000

// A basic window
typedef struct GWindowObject_t {
	GWindowType	type;				// What type of window is this
	uint16_t	flags;				// Internal flags
	coord_t		x, y;				// Screen relative position
	coord_t		width, height;		// Dimensions of this window
	color_t		color, bgcolor;		// Current drawing colors
#if GDISP_NEED_TEXT
	font_t		font;				// Current font
#endif
} GWindowObject, * GHandle;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* Base Functions */

/**
 * @brief   Create a basic window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gw		The window structure to initialize. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen coordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @note				The default drawing color gets set to White and the background drawing color to Black.
 * @note				No default font is set so make sure to set one before drawing any text.
 * @note				The dimensions and position may be changed to fit on the real screen.
 * @note				The window is not automatically cleared on creation. You must do that by calling gwinClear() (possibly after changing your background color)
 *
 * @api
 */
GHandle gwinCreateWindow(GWindowObject *gw, coord_t x, coord_t y, coord_t width, coord_t height);

/**
 * @brief   Destroy a window (of any type). Releases any dynamically allocated memory.
 *
 * @param[in] gh		The window handle
 *
 * @api
 */
void gwinDestroyWindow(GHandle gh);

/**
 * @brief	Get the X coordinate of the window
 * @details	Returns the X coordinate of the origin of the window.
 *			The coordinate is relative to the physical screen zero point.
 *
 * @param[in] gh	The window
 */
#define gwinGetScreenX(gh)			((gh)->x)

/**
 * @brief	Get the Y coordinate of the window
 * @details	Returns the Y coordinate of the origin of the window.
 *			The coordinate is relative to the physical screen zero point.
 *
 * @param[in] gh	The window
 */
#define gwinGetScreenY(gh)			((gh)->y)

/**
 * @brief	Get the width of the window
 *
 * @param[in] gh	The window
 */
#define gwinGetWidth(gh)			((gh)->width)

/**
 * @brief	Get the height of the window
 *
 * @param[in] gh	The window
 */
#define gwinGetHeight(gh)			((gh)->height)

/**
 * @brief	Set foreground color
 * @details Set the color which will be used to draw
 *
 * @param[in] gh	The window
 * @param[in] clr	The color to be set
 */
#define gwinSetColor(gh, clr)		(gh)->color = (clr)

/**
 * @brief	Set background color
 * @details	Set the color which will be used as background
 * @note	gwinClear() must be called to set the background color
 *
 * @param[in] gh	The window
 * @param[in] bgclr	The background color
 */
#define gwinSetBgColor(gh, bgclr)	(gh)->bgcolor = (bgclr)

/* Set up for text */

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Set the current font for this window.
	 *
	 * @param[in] gh		The window handle
	 * @param[in] font		The font to use for text functions
	 *
	 * @api
	 */
	void gwinSetFont(GHandle gh, font_t font);
#endif

/* Drawing Functions */

/**
 * @brief   Draw the window
 * @note	Redraws the Window if the GWIN object has a draw routine
 *
 * @param[in] gh		The window handle
 *
 * @api
 */
void gwinDraw(GHandle gh);

/**
 * @brief   Clear the window
 * @note	Uses the current background color to clear the window
 *
 * @param[in] gh		The window handle
 *
 * @api
 */
void gwinClear(GHandle gh);

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
void gwinDrawPixel(GHandle gh, coord_t x, coord_t y);

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
void gwinDrawLine(GHandle gh, coord_t x0, coord_t y0, coord_t x1, coord_t y1);

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
void gwinDrawBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy);

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
void gwinFillArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy);

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
void gwinBlitArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);

/* Circle Functions */

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
	void gwinDrawCircle(GHandle gh, coord_t x, coord_t y, coord_t radius);

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
	void gwinFillCircle(GHandle gh, coord_t x, coord_t y, coord_t radius);
#endif

/* Ellipse Functions */

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
	void gwinDrawEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b);

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
	void gwinFillEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b);
#endif

/* Arc Functions */

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
	void gwinDrawArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);

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
	void gwinFillArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);
#endif

/* Read a pixel Function */

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
	color_t gwinGetPixelColor(GHandle gh, coord_t x, coord_t y);
#endif

/* Extra Text Functions */

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
	void gwinDrawChar(GHandle gh, coord_t x, coord_t y, char c);

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
	void gwinFillChar(GHandle gh, coord_t x, coord_t y, char c);

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
	void gwinDrawString(GHandle gh, coord_t x, coord_t y, const char *str);

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
	void gwinFillString(GHandle gh, coord_t x, coord_t y, const char *str);

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
	void gwinDrawStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify);

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
	void gwinFillStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify);
#endif

#if GDISP_NEED_CONVEX_POLYGON || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an enclosed polygon (convex, non-convex or complex).
	 *
	 * @note	Uses the current foreground color.
	 *
	 * @param[in] gh		The window handle
	 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
	 * @param[in] pntarray	An array of points
	 * @param[in] cnt		The number of points in the array
	 *
	 * @api
	 */
	void gwinDrawPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt);

	/**
	 * @brief   Fill a convex polygon
	 * @details Doesn't handle non-convex or complex polygons.
	 *
	 * @note	Uses the current foreground color.
	 *
	 * @param[in] gh		The window handle
	 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
	 * @param[in] pntarray	An array of points
	 * @param[in] cnt		The number of points in the array
	 *
	 * @note	Convex polygons are those that have no internal angles. That is;
	 * 			you can draw a line from any point on the polygon to any other point
	 * 			on the polygon without it going outside the polygon. In our case we generalise
	 * 			this a little by saying that an infinite horizontal line (at any y value) will cross
	 * 			no more than two edges on the polygon. Some non-convex polygons do fit this criteria
	 * 			and can therefore be drawn.
	 * @note	This routine is designed to be very efficient with even simple display hardware.
	 *
	 * @api
	 */
	void gwinFillConvexPoly(GHandle gh, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt);
#endif

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	/**
	 * @brief	Draw the image
	 * @return	GDISP_IMAGE_ERR_OK (0) on success or an error code.
	 *
	 * @param[in] gh		The window handle
	 * @param[in] img   	The image structure
	 * @param[in] x,y		The window location to draw the image
	 * @param[in] cx,cy		The area on the screen to draw
	 * @param[in] sx,sy		The image position to start drawing at
	 *
	 * @pre		gdispImageOpen() must have returned successfully.
	 *
	 * @note	If sx,sy + cx,cy is outside the image boundaries the area outside the image
	 * 			is simply not drawn.
	 * @note	If @p gdispImageCache() has been called first for this frame, this routine will draw using a
	 * 			fast blit from the cached frame. If not, it reads the input and decodes it as it
	 * 			is drawing. This may be significantly slower than if the image has been cached (but
	 * 			uses a lot less RAM)
	 */
	gdispImageError gwinImageDraw(GHandle gh, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
#endif

#ifdef __cplusplus
}
#endif

/* Include extra window types */
#include "gwin/console.h"
#include "gwin/button.h"
#include "gwin/graph.h"
#include "gwin/slider.h"

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_H */
/** @} */
