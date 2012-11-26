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
 * @file    gwin.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @addtogroup GWIN
 * @{
 */
#ifndef _GWIN_H
#define _GWIN_H

#ifndef GFX_USE_GWIN
	#define GFX_USE_GWIN FALSE
#endif

#if GFX_USE_GWIN || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    GWIN more complex functionality to be compiled
 * @{
 */
/** @} */

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

#if !defined(GFX_USE_GDISP) || !GFX_USE_GDISP
	#error "GWIN: GFX_USE_GDISP must also be defined"
#endif
#include "gdisp.h"

#if !GDISP_NEED_CLIP
	#warning "GWIN: Drawing can occur outside the defined window as GDISP_NEED_CLIP is FALSE"
#endif

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
GHandle gwinCreateWindow(GWindowObject *gw, coord_t x, coord_t y, coord_t width, coord_t height);
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
#if GDISP_NEED_TEXT
void gwinSetFont(GHandle gh, font_t font);
#endif

/* Drawing Functions */
void gwinClear(GHandle gh);
void gwinDrawPixel(GHandle gh, coord_t x, coord_t y);
void gwinDrawLine(GHandle gh, coord_t x0, coord_t y0, coord_t x1, coord_t y1);
void gwinDrawBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy);
void gwinFillArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy);
void gwinBlitArea(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);

/* Circle Functions */
#if GDISP_NEED_CIRCLE
	void gwinDrawCircle(GHandle gh, coord_t x, coord_t y, coord_t radius);
	void gwinFillCircle(GHandle gh, coord_t x, coord_t y, coord_t radius);
#endif

/* Ellipse Functions */
#if GDISP_NEED_ELLIPSE
	void gwinDrawEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b);
	void gwinFillEllipse(GHandle gh, coord_t x, coord_t y, coord_t a, coord_t b);
#endif

/* Arc Functions */
#if GDISP_NEED_ARC
	void gwinDrawArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);
	void gwinFillArc(GHandle gh, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);
#endif

/* Read a pixel Function */
#if GDISP_NEED_PIXELREAD
	color_t gwinGetPixelColor(GHandle gh, coord_t x, coord_t y);
#endif

/* Extra Text Functions */
#if GDISP_NEED_TEXT
	void gwinDrawChar(GHandle gh, coord_t x, coord_t y, char c);
	void gwinFillChar(GHandle gh, coord_t x, coord_t y, char c);
	void gwinDrawString(GHandle gh, coord_t x, coord_t y, const char *str);
	void gwinFillString(GHandle gh, coord_t x, coord_t y, const char *str);
	void gwinDrawStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify);
	void gwinFillStringBox(GHandle gh, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify);
#endif

#ifdef __cplusplus
}
#endif

/* Include extra window types */
#include "gwin/gwin_console.h"
#include "gwin/gwin_button.h"

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_H */
/** @} */
