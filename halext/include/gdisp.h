/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*
   Concepts and parts of this file have been contributed by:
		Joel Bodenmann aka Tectu	-> Maintainer
		Andrew Hannam aka inmarket	-> Framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/**
 * @file    gdisp.h
 * @brief   GDISP Graphic Driver macros and structures.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_H
#define _GDISP_H

#if HAL_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   Some basic colors
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

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    GDISP more complex functionality to be compiled
 * @{
 */
/**
 * @brief   Should all operations be clipped to the screen and colors validated.
 * @details	Defaults to TRUE.
 * @note    If this is FALSE, any operations that extend beyond the
 *          edge of the screen will have undefined results. Any
 *			out-of-range colors will produce undefined results.
 * @note	If defined then all low level and high level driver routines
 *			must check the validity of inputs and do something sensible
 *			if they are out of range. It doesn't have to be efficient,
 *			just valid.
 */
#ifndef GDISP_NEED_VALIDATION
	#define GDISP_NEED_VALIDATION	TRUE
#endif

/**
 * @brief   Are circle functions needed.
 * @details	Defaults to TRUE
 */
#ifndef GDISP_NEED_CIRCLE
	#define GDISP_NEED_CIRCLE		TRUE
#endif

/**
 * @brief   Are ellipse functions needed.
 * @details	Defaults to TRUE
 */
#ifndef GDISP_NEED_ELLIPSE
	#define GDISP_NEED_ELLIPSE		TRUE
#endif

/**
 * @brief   Are text functions needed.
 * @details	Defaults to TRUE
 */
#ifndef GDISP_NEED_TEXT
	#define GDISP_NEED_TEXT			TRUE
#endif

/**
 * @brief   Is scrolling needed.
 * @details	Defaults to FALSE
 */
#ifndef GDISP_NEED_SCROLL
	#define GDISP_NEED_SCROLL		FALSE
#endif

/**
 * @brief   Is the capability to read pixels back needed.
 * @details	Defaults to FALSE
 */
#ifndef GDISP_NEED_PIXELREAD
	#define GDISP_NEED_PIXELREAD	FALSE
#endif

/**
 * @brief   Do the drawing functions need to be thread-safe.
 * @details	Defaults to FALSE
 * @note	Turning this on adds two context switches per transaction
 *			so it can significantly slow graphics drawing.
 */
#ifndef GDISP_NEED_MULTITHREAD
	#define GDISP_NEED_MULTITHREAD	FALSE
#endif
/** @} */

/* Include the low level driver configuration information */
#include "gdisp_lld_config.h"

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if GDISP_NEED_SCROLL && !GDISP_HARDWARE_SCROLL
	#error "GDISP: Hardware scrolling is wanted but not supported."
#endif

#if GDISP_NEED_PIXELREAD && !GDISP_HARDWARE_PIXELREAD
	#error "GDISP: Pixel read-back is wanted but not supported."
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/* Define the basic Macro's for the various pixel formats */

#if defined(GDISP_PIXELFORMAT_RGB565) || defined(__DOXYGEN__)
	/**
	 * @brief   The color of a pixel.
	 */
	typedef uint16_t color_t;
	/**
	 * @brief   Convert a number (of any type) to a color_t.
	 * @details Masks any invalid bits in the color
	 */
	#define COLOR(c)			((color_t)(c))
	/**
	 * @brief   Does the color_t type contain invalid bits that need masking.
	 */
	#define MASKCOLOR			FALSE
	/**
	 * @brief   Convert red, green, blue (each 0 to 255) into a color value.
	 */
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF8)<<8) | (((g) & 0xFC)<<3) | (((b) & 0xF8)>>3)))
	/**
	 * @brief   Convert a 6 digit HTML code (hex) into a color value.
	 */
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF80000)>>8) | (((h) & 0x00FC00)>>5) | (((h) & 0x0000F8)>>3)))
	/**
	 * @brief   Extract the red component (0 to 255) of a color value.
	 */
	#define RED_OF(c)			(((c) & 0xF800)>>8)
	/**
	 * @brief   Extract the green component (0 to 255) of a color value.
	 */
	#define GREEN_OF(c)			(((c)&0x007E)>>3)
	/**
	 * @brief   Extract the blue component (0 to 255) of a color value.
	 */
	#define BLUE_OF(c)			(((c)&0x001F)<<3)

#elif defined(GDISP_PIXELFORMAT_RGB888)
	typedef uint32_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0xFFFFFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xFF)<<16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))
	#define HTML2COLOR(h)		((color_t)(h))
	#define RED_OF(c)			(((c) & 0xFF0000)>>16)
	#define GREEN_OF(c)			(((c)&0x00FF00)>>8)
	#define BLUE_OF(c)			((c)&0x0000FF)

#elif defined(GDISP_PIXELFORMAT_RGB444)
	typedef uint16_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0x0FFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF0)<<4) | ((g) & 0xF0) | (((b) & 0xF0)>>4)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF00000)>>12) | (((h) & 0x00F000)>>8) | (((h) & 0x0000F0)>>4)))
	#define RED_OF(c)			(((c) & 0x0F00)>>4)
	#define GREEN_OF(c)			((c)&0x00F0)
	#define BLUE_OF(c)			(((c)&0x000F)<<4)

#elif defined(GDISP_PIXELFORMAT_RGB332)
	typedef uint8_t color_t;
	#define COLOR(c)			((color_t)(c))
	#define MASKCOLOR			FALSE
	#define RGB2COLOR(r,g,b)	((color_t)(((r) & 0xE0) | (((g) & 0xE0)>>3) | (((b) & 0xC0)>>6)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xE00000)>>16) | (((h) & 0x00E000)>>11) | (((h) & 0x0000C0)>>6)))
	#define RED_OF(c)			((c) & 0xE0)
	#define GREEN_OF(c)			(((c)&0x1C)<<3)
	#define BLUE_OF(c)			(((c)&0x03)<<6)

#elif defined(GDISP_PIXELFORMAT_RGB666)
	typedef uint32_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0x03FFFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xFC)<<10) | (((g) & 0xFC)<<4) | (((b) & 0xFC)>>2)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xFC0000)>>6) | (((h) & 0x00FC00)>>4) | (((h) & 0x0000FC)>>2)))
	#define RED_OF(c)			(((c) & 0x03F000)>>12)
	#define GREEN_OF(c)			(((c)&0x00FC00)>>8)
	#define BLUE_OF(c)			(((c)&0x00003F)<<2)

#elif !defined(GDISP_PIXELFORMAT_CUSTOM)
	#error "GDISP: No supported pixel format has been specified."
#endif

/* Verify information for packed pixels and define a non-packed pixel macro */
#if !GDISP_PACKED_PIXELS
	#define gdispPackPixels(buf,cx,x,y,c)	{ ((color_t *)(buf))[(y)*(cx)+(x)] = (c); }
#elif !GDISP_HARDWARE_BITFILLS
	#error "GDISP: packed pixel formats are only supported for hardware accelerated drivers."
#elif !defined(GDISP_PIXELFORMAT_RGB888) \
		&& !defined(GDISP_PIXELFORMAT_RGB444) \
		&& !defined(GDISP_PIXELFORMAT_RGB666) \
		&& !defined(GDISP_PIXELFORMAT_CUSTOM)
	#error "GDISP: A packed pixel format has been specified for an unsupported pixel format."
#endif

/**
 * @brief   The type for a coordinate or length on the screen.
 */
typedef uint16_t	coord_t;
/**
 * @brief   The type of a pixel.
 */
typedef color_t		pixel_t;
/**
 * @brief   The type of a font.
 */
typedef const struct font *font_t;
/**
 * @brief   Type of a structure representing a GDISP driver.
 */
typedef struct GDISPDriver GDISPDriver;
/**
 * @brief   Type for the screen orientation.
 */
typedef enum orientation {portrait, landscape, portraitInv, landscapeInv} gdisp_orientation_t;
/**
 * @brief   Type for the available power modes for the screen.
 */
typedef enum powermode {powerOff, powerSleep, powerOn} gdisp_powermode_t;
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

/* Include the low level driver information */
#include "gdisp_lld.h"

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
/**
 * @brief   Predefined fonts.
 */
extern const struct font fontSmall;
extern const struct font fontLarger;
extern const struct font fontUI1;
extern const struct font fontUI2;
extern const struct font fontLargeNumbers;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if GDISP_NEED_MULTITHREAD

	/* Base Functions */
	void gdispInit(GDISPDriver *gdisp);
	void gdispSetPowerMode(gdisp_powermode_t powerMode);
	void gdispSetOrientation(gdisp_orientation_t newOrientation);

	/* Drawing Functions */
	void gdispClear(color_t color);
	void gdispDrawPixel(coord_t x, coord_t y, color_t color);
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
	void gdispDrawBox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
	void gdispBlitArea(coord_t x, coord_t y, coord_t cx, coord_t cy, pixel_t *buffer);

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

#else

	/* The same as above but use the low level driver directly if no multi-thread support is needed */
	#define gdispInit(gdisp)									gdisp_lld_init()
	#define gdispSetPowerMode(powerMode)						gdisp_lld_setpowermode(powerMode)
	#define gdispSetOrientation(newOrientation)					gdisp_lld_setorientation(newOrientation)
	#define gdispClear(color)									gdisp_lld_clear(color)
	#define gdispDrawPixel(x, y, color)							gdisp_lld_drawpixel(x, y, color)
	#define gdispDrawLine(x0, y0, x1, y1, color)				gdisp_lld_drawline(x0, y0, x1, y1, color)
	#define gdispDrawBox(x, y, cx, cy, color)					gdisp_lld_drawbox(x, y, cx, cy, color)
	#define gdispFillArea(x, y, cx, cy, color)					gdisp_lld_fillarea(x, y, cx, cy, color)
	#define gdispBlitArea(x, y, cx, cy, buffer)					gdisp_lld_blitarea(x, y, cx, cy, buffer)
	#define gdispDrawCircle(x, y, radius, color)				gdisp_lld_drawcircle(x, y, radius, color)
	#define gdispFillCircle(x, y, radius, color)				gdisp_lld_fillcircle(x, y, radius, color)
	#define gdispDrawEllipse(x, y, a, b, color)					gdisp_lld_drawellipse(x, y, a, b, color)
	#define gdispFillEllipse(x, y, a, b, color)					gdisp_lld_fillellipse(x, y, a, b, color)
	#define gdispDrawChar(x, y, c, font, color)					gdisp_lld_drawchar(x, y, c, font, color)
	#define gdispFillChar(x, y, c, font, color, bgcolor)		gdisp_lld_fillchar(x, y, c, font, color, bgcolor)
	#define gdispGetPixelColor(x, y)							gdisp_lld_getpixelcolor(x, y)
	#define gdispVerticalScroll(x, y, cx, cy, lines, bgcolor)	gdisp_lld_verticalscroll(x, y, cx, cy, lines, bgcolor)

#endif

/* Extra Text Functions */
#if GDISP_NEED_TEXT
	void gdispDrawString(coord_t x, coord_t y, const char *str, font_t font, color_t color);
	void gdispFillString(coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor);
	void gdispFillStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgColor, justify_t justify);
	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric);
	coord_t gdispGetCharWidth(char c, font_t font);
	coord_t gdispGetStringWidth(const char* str, font_t font);
#endif

/* Support routine for packed pixel formats */
#ifndef gdispPackPixels
	void gdispPackPixels(pixel_t *buf, coord_t cx, coord_t x, coord_t y, color_t color);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_GDISP */

#endif /* _GDISP_H */
/** @} */
