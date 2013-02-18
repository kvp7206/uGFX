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
 * @file    include/gdisp/gdisp.h
 * @brief   GDISP Graphic Driver subsystem header file.
 *
 * @addtogroup GDISP
 *
 * @details		The GDISP module provides high level abstraction to interface pixel oriented graphic displays.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in gfxconf.h
 *
 * @{
 */

#ifndef _GDISP_H
#define _GDISP_H

#include "gfx.h"

/* This type definition is defined here as it gets used in other gfx sub-systems even
 * if GFX_USE_GDISP is FALSE.
 */

/**
 * @brief   The type for a coordinate or length on the screen.
 */
typedef int16_t	coord_t;

#if GFX_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

/**
 * @name   Some basic colors
 * @{
 */
#define White			HTML2COLOR(0xFFFFFF)
#define Black			HTML2COLOR(0x000000)
#define Gray			HTML2COLOR(0x808080)
#define Grey			Gray
#define Blue			HTML2COLOR(0x0000FF)
#define Red				HTML2COLOR(0xFF0000)
#define Fuchsia			HTML2COLOR(0xFF00FF)
#define Magenta			Fuchsia
#define Green			HTML2COLOR(0x008000)
#define Yellow			HTML2COLOR(0xFFFF00)
#define Aqua			HTML2COLOR(0x00FFFF)
#define Cyan			Aqua
#define Lime			HTML2COLOR(0x00FF00)
#define Maroon			HTML2COLOR(0x800000)
#define Navy			HTML2COLOR(0x000080)
#define Olive			HTML2COLOR(0x808000)
#define Purple			HTML2COLOR(0x800080)
#define Silver			HTML2COLOR(0xC0C0C0)
#define Teal			HTML2COLOR(0x008080)
#define Orange			HTML2COLOR(0xFFA500)
#define Pink			HTML2COLOR(0xFFC0CB)
#define SkyBlue			HTML2COLOR(0x87CEEB)
/** @} */

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

/* Include the low level driver information */
#include "gdisp/lld/gdisp_lld.h"

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief   Type for the text justification.
 */
typedef enum justify {justifyLeft, justifyCenter, justifyRight} justify_t;
/**
 * @brief   Type for the font metric.
 */
typedef enum fontmetric {fontHeight, fontDescendersHeight, fontLineSpacing, fontCharPadding, fontMinWidth, fontMaxWidth} fontmetric_t;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if (GDISP_NEED_TEXT && GDISP_OLD_FONT_DEFINITIONS) || defined(__DOXYGEN__)
	#if GDISP_INCLUDE_FONT_SMALL
		extern const struct font fontSmall;
		extern const struct font fontSmallDouble;
		extern const struct font fontSmallNarrow;
	#endif
	#if GDISP_INCLUDE_FONT_LARGER
		extern const struct font fontLarger;
		extern const struct font fontLargerDouble;
		extern const struct font fontLargerNarrow;
	#endif
	#if GDISP_INCLUDE_FONT_UI1
		extern const struct font fontUI1;
		extern const struct font fontUI1Double;
		extern const struct font fontUI1Narrow;
	#endif
	#if GDISP_INCLUDE_FONT_UI2
		extern const struct font fontUI2;
		extern const struct font fontUI2Double;
		extern const struct font fontUI2Narrow;
	#endif
	#if GDISP_INCLUDE_FONT_LARGENUMBERS
		extern const struct font fontLargeNumbers;
		extern const struct font fontLargeNumbersDouble;
		extern const struct font fontLargeNumbersNarrow;
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC
	/* These routines can be hardware accelerated
	 *	- Do not add a routine here unless it has also been added to the hardware acceleration layer
	 */

	/* Base Functions */
	bool_t gdispInit(void);
	bool_t gdispIsBusy(void);

	/* Drawing Functions */
	void gdispClear(color_t color);
	void gdispDrawPixel(coord_t x, coord_t y, color_t color);
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
	void gdispBlitAreaEx(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);

	/* Clipping Functions */
	#if GDISP_NEED_CLIP
	void gdispSetClip(coord_t x, coord_t y, coord_t cx, coord_t cy);
	#endif

	/* Circle Functions */
	#if GDISP_NEED_CIRCLE
	void gdispDrawCircle(coord_t x, coord_t y, coord_t radius, color_t color);
	void gdispFillCircle(coord_t x, coord_t y, coord_t radius, color_t color);
	#endif
	
	/* Ellipse Functions */
	#if GDISP_NEED_ELLIPSE
	void gdispDrawEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	void gdispFillEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	#endif

	/* Arc Functions */
	#if GDISP_NEED_ARC
	void gdispDrawArc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	void gdispFillArc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	#endif

	/* Basic Text Rendering Functions */
	#if GDISP_NEED_TEXT
	void gdispDrawChar(coord_t x, coord_t y, char c, font_t font, color_t color);
	void gdispFillChar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor);
	#endif
	
	/* Read a pixel Function */
	#if GDISP_NEED_PIXELREAD
	color_t gdispGetPixelColor(coord_t x, coord_t y);
	#endif

	/* Scrolling Function - clears the area scrolled out */
	#if GDISP_NEED_SCROLL
	void gdispVerticalScroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
	#endif

	/* Set driver specific control */
	#if GDISP_NEED_CONTROL
	void gdispControl(unsigned what, void *value);
	#endif

	/* Query driver specific data */
	void *gdispQuery(unsigned what);

#else

	/* The same as above but use the low level driver directly if no multi-thread support is needed */
	#define gdispInit(gdisp)									gdisp_lld_init()
	#define gdispIsBusy()										FALSE
	#define gdispClear(color)									gdisp_lld_clear(color)
	#define gdispDrawPixel(x, y, color)							gdisp_lld_draw_pixel(x, y, color)
	#define gdispDrawLine(x0, y0, x1, y1, color)				gdisp_lld_draw_line(x0, y0, x1, y1, color)
	#define gdispFillArea(x, y, cx, cy, color)					gdisp_lld_fill_area(x, y, cx, cy, color)
	#define gdispBlitAreaEx(x, y, cx, cy, sx, sy, scx, buf)		gdisp_lld_blit_area_ex(x, y, cx, cy, sx, sy, scx, buf)
	#define gdispSetClip(x, y, cx, cy)							gdisp_lld_set_clip(x, y, cx, cy)
	#define gdispDrawCircle(x, y, radius, color)				gdisp_lld_draw_circle(x, y, radius, color)
	#define gdispFillCircle(x, y, radius, color)				gdisp_lld_fill_circle(x, y, radius, color)
	#define gdispDrawArc(x, y, radius, sangle, eangle, color)	gdisp_lld_draw_arc(x, y, radius, sangle, eangle, color)
	#define gdispFillArc(x, y, radius, sangle, eangle, color)	gdisp_lld_fill_arc(x, y, radius, sangle, eangle, color)
	#define gdispDrawEllipse(x, y, a, b, color)					gdisp_lld_draw_ellipse(x, y, a, b, color)
	#define gdispFillEllipse(x, y, a, b, color)					gdisp_lld_fill_ellipse(x, y, a, b, color)
	#define gdispDrawChar(x, y, c, font, color)					gdisp_lld_draw_char(x, y, c, font, color)
	#define gdispFillChar(x, y, c, font, color, bgcolor)		gdisp_lld_fill_char(x, y, c, font, color, bgcolor)
	#define gdispGetPixelColor(x, y)							gdisp_lld_get_pixel_color(x, y)
	#define gdispVerticalScroll(x, y, cx, cy, lines, bgcolor)	gdisp_lld_vertical_scroll(x, y, cx, cy, lines, bgcolor)
	#define gdispControl(what, value)							gdisp_lld_control(what, value)
	#define gdispQuery(what)									gdisp_lld_query(what)

#endif

/* These routines are not hardware accelerated
 *	- Do not add a hardware accelerated routines here.
 */

/* Extra drawing functions */
void gdispDrawBox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);

/* Extra Text Functions */
#if GDISP_NEED_TEXT
	void gdispDrawString(coord_t x, coord_t y, const char *str, font_t font, color_t color);
	void gdispFillString(coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor);
	void gdispDrawStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, justify_t justify);
	void gdispFillStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgColor, justify_t justify);
	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric);
	coord_t gdispGetCharWidth(char c, font_t font);
	coord_t gdispGetStringWidth(const char* str, font_t font);
	font_t gdispOpenFont(const char *name);
	void gdispCloseFont(font_t font);
	const char *gdispGetFontName(font_t font);
#endif

/* Extra Arc Functions */
#if GDISP_NEED_ARC
	void gdispDrawRoundedBox(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color);
	void gdispFillRoundedBox(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color);
#endif

/* Support routine for packed pixel formats */
#ifndef gdispPackPixels
	void gdispPackPixels(const pixel_t *buf, coord_t cx, coord_t x, coord_t y, color_t color);
#endif

/* 
 * Macro definitions
 */

/* Now obsolete functions */
#define gdispBlitArea(x, y, cx, cy, buffer)		gdispBlitAreaEx(x, y, cx, cy, 0, 0, cx, buffer)

/* Macro definitions for common gets and sets */
#define gdispSetPowerMode(powerMode)			gdispControl(GDISP_CONTROL_POWER, (void *)(unsigned)(powerMode))
#define gdispSetOrientation(newOrientation)		gdispControl(GDISP_CONTROL_ORIENTATION, (void *)(unsigned)(newOrientation))
#define gdispSetBacklight(percent)				gdispControl(GDISP_CONTROL_BACKLIGHT, (void *)(unsigned)(percent))
#define gdispSetContrast(percent)				gdispControl(GDISP_CONTROL_CONTRAST, (void *)(unsigned)(percent))

#define gdispGetWidth()							((coord_t)(unsigned)gdispQuery(GDISP_QUERY_WIDTH))
#define gdispGetHeight()						((coord_t)(unsigned)gdispQuery(GDISP_QUERY_HEIGHT))
#define gdispGetPowerMode()						((gdisp_powermode_t)(unsigned)gdispQuery(GDISP_QUERY_POWER))
#define gdispGetOrientation()					((gdisp_orientation_t)(unsigned)gdispQuery(GDISP_QUERY_ORIENTATION))
#define gdispGetBacklight()						((coord_t)(unsigned)gdispQuery(GDISP_QUERY_BACKLIGHT))
#define gdispGetContrast()						((coord_t)(unsigned)gdispQuery(GDISP_QUERY_CONTRAST))

/* More interesting macro's */
#define gdispUnsetClip()						gdispSetClip(0,0,gdispGetWidth(),gdispGetHeight())


#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GDISP */

#endif /* _GDISP_H */
/** @} */

