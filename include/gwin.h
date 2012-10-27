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

#if !defined(GFX_USE_GDISP)
	#error "GWIN: GFX_USE_GDISP must also be defined"
#endif
#include "gdisp.h"

#if !GDISP_NEED_CLIP
	#warning "GWIN: Drawing can occur outside the defined window as GDISP_NEED_CLIP is FALSE"
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p GWindow virtual methods table.
 */
struct GWindowVMT {
	_base_asynchronous_channel_methods
};

struct GWindowText {
	const struct GWindowVMT *vmt;
	_base_asynchronous_channel_data
	font_t		font;			// Current font
	uint8_t		fy;				// Current font height
	uint8_t		fp;				// Current font inter-character spacing
	coord_t		cx,cy;			// Cursor position
};

typedef struct GWindow_t {
#if GDISP_NEED_TEXT
	struct GWindowText	txt;
#endif
	coord_t x, y;				// Screen relative position
	coord_t	width, height;		// Dimensions of this window
	color_t color, bgcolor;		// Current drawing colors
} GWindow;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* Base Functions */
bool_t gwinInit(GWindow *gw, coord_t x, coord_t y, coord_t width, coord_t height);

/* Status Functions */
#define gwinGetScreenX(gw)			((gw)->x)
#define gwinGetScreenY(gw)			((gw)->y)
#define gwinGetWidth(gw)			((gw)->width)
#define gwinGetHeight(gw)			((gw)->height)

/* Set up for drawing */
#define gwinSetColor(gw, clr)		(gw)->color = (clr)
#define gwinSetBgColor(gw, bgclr)	(gw)->bgcolor = (bgclr)

/* Set up for text */
#if GDISP_NEED_TEXT
void gwinSetFont(GWindow *gw, font_t font);
#define gwinGetStream(gw)	((BaseSequentialStream *)gw)
#endif

/* Drawing Functions */
void gwinClear(GWindow *gw);
void gwinDrawPixel(GWindow *gw, coord_t x, coord_t y);
void gwinDrawLine(GWindow *gw, coord_t x0, coord_t y0, coord_t x1, coord_t y1);
void gwinDrawBox(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy);
void gwinFillArea(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy);
void gwinBlitArea(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);

/* Circle Functions */
#if GDISP_NEED_CIRCLE
void gwinDrawCircle(GWindow *gw, coord_t x, coord_t y, coord_t radius);
void gwinFillCircle(GWindow *gw, coord_t x, coord_t y, coord_t radius);
#endif

/* Ellipse Functions */
#if GDISP_NEED_ELLIPSE
void gwinDrawEllipse(GWindow *gw, coord_t x, coord_t y, coord_t a, coord_t b);
void gwinFillEllipse(GWindow *gw, coord_t x, coord_t y, coord_t a, coord_t b);
#endif

/* Arc Functions */
#if GDISP_NEED_ARC
void gwinDrawArc(GWindow *gw, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);
void gwinFillArc(GWindow *gw, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle);
#endif

/* Read a pixel Function */
#if GDISP_NEED_PIXELREAD
color_t gwinGetPixelColor(GWindow *gw, coord_t x, coord_t y);
#endif

/* Scrolling Function - clears the area scrolled out */
#if GDISP_NEED_SCROLL
void gwinVerticalScroll(GWindow *gw, int lines);
#endif

/* Extra Text Functions */
#if GDISP_NEED_TEXT
void gwinDrawChar(GWindow *gw, coord_t x, coord_t y, char c);
void gwinFillChar(GWindow *gw, coord_t x, coord_t y, char c);
void gwinDrawString(GWindow *gw, coord_t x, coord_t y, const char *str);
void gwinFillString(GWindow *gw, coord_t x, coord_t y, const char *str);
void gwinBoxString(GWindow *gw, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, justify_t justify);
void gwinPutChar(GWindow *gw, char c);
void gwinPutString(GWindow *gw, const char *str);
void gwinPutCharArray(GWindow *gw, const char *str, size_t n);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_H */
/** @} */
