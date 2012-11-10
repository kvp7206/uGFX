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
	/**
	 * @brief   Should console functions be included.
	 * @details	Defaults to TRUE
	 */
	#ifndef GWIN_NEED_CONSOLE
		#define GWIN_NEED_CONSOLE	TRUE
	#endif
	/**
	 * @brief   Should button functions be included.
	 * @details	Defaults to FALSE for now as implementation is not complete
	 */
	#ifndef GWIN_NEED_BUTTON
		#define GWIN_NEED_BUTTON	FALSE
	#endif

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

#if GWIN_NEED_CONSOLE && !GDISP_NEED_TEXT
	#error "GWIN: Text support (GDISP_NEED_TEXT) is required if GWIN_NEED_CONSOLE is defined."
#endif

#if GWIN_NEED_BUTTON && !GDISP_NEED_TEXT
	#error "GWIN: Text support (GDISP_NEED_TEXT) is required if GWIN_NEED_BUTTON is defined."
#endif

#if GWIN_NEED_BUTTON
	#warning "GWIN: Button support is not complete yet"
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef enum GWindowType_e {
	GW_WINDOW, GW_CONSOLE, GW_BUTTON
	} GWindowType;

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

#if GWIN_NEED_CONSOLE
	// A console window. Supports wrapped text writing and a cursor.
	typedef struct GConsoleObject_t {
		GWindowObject		gwin;
		
		struct GConsoleWindowStream_t {
			const struct GConsoleWindowVMT_t *vmt;
			_base_asynchronous_channel_data
			} stream;
		
		coord_t		cx,cy;			// Cursor position
		uint8_t		fy;				// Current font height
		uint8_t		fp;				// Current font inter-character spacing
	} GConsoleObject;
#endif

#if GWIN_NEED_BUTTON
	typedef enum GButtonShape_e {
		GBTN_3D, GBTN_SQUARE, GBTN_ROUNDED, GBTN_ELLIPSE
	} GButtonShape;

	typedef struct GButtonStyle_t {
		GButtonShape		shape;
		color_t				color_up_edge;
		color_t				color_up_fill;
		color_t				color_up_txt;
		color_t				color_dn_edge;
		color_t				color_dn_fill;
		color_t				color_dn_txt;
	} GButtonStyle;
	
	typedef enum GButtonType_e {
		GBTN_NORMAL, GBTN_TOGGLE
	} GButtonType;
	
	typedef enum GButtonState_e {
		GBTN_UP, GBTN_DOWN
	} GButtonState;
	
	// A button window
	typedef struct GButtonObject_t {
		GWindowObject		gwin;

		GButtonStyle		style;
		GButtonState		state;
		GButtonType			type;
		const char *		txt;
		void *				callback;		// To be fixed
		void *				inputsrc;		// To be fixed
	} GButtonObject;
#endif

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* Base Functions */
GHandle gwinCreateWindow(GWindowObject *gw, coord_t x, coord_t y, coord_t width, coord_t height);
void gwinDestroyWindow(GHandle gh);

/* Status Functions */
#define gwinGetScreenX(gh)			((gh)->x)
#define gwinGetScreenY(gh)			((gh)->y)
#define gwinGetWidth(gh)			((gh)->width)
#define gwinGetHeight(gh)			((gh)->height)

/* Set up for drawing */
#define gwinSetColor(gh, clr)		(gh)->color = (clr)
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

#if GWIN_NEED_CONSOLE
GHandle gwinCreateConsole(GConsoleObject *gc, coord_t x, coord_t y, coord_t width, coord_t height, font_t font);
BaseSequentialStream *gwinGetConsoleStream(GHandle gh);
void gwinPutChar(GHandle gh, char c);
void gwinPutString(GHandle gh, const char *str);
void gwinPutCharArray(GHandle gh, const char *str, size_t n);
#endif

#if GWIN_NEED_BUTTON
GHandle gwinCreateButton(GButtonObject *gb, coord_t x, coord_t y, coord_t width, coord_t height, font_t font, GButtonType type);
void gwinSetButtonStyle(GHandle gh, const GButtonStyle *style);
void gwinSetButtonText(GHandle gh, const char *txt, bool_t useAlloc);
void gwinButtonDraw(GHandle gh);
#define gwinGetButtonState(gh)		(((GButtonObject *)(gh))->state)
//void gwinSetButtonCallback(GHandle gh, ????);
//void gwinSetButtonInput(GHandle gh, ????);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_H */
/** @} */
