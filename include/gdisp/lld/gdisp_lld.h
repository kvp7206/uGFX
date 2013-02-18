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
 * @file    include/gdisp/lld/gdisp_lld.h
 * @brief   GDISP Graphic Driver subsystem low level driver header.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_H
#define _GDISP_LLD_H

#if GFX_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Low level driver configuration needs					                     */
/*===========================================================================*/

/*===========================================================================*/
/* Include the low level driver configuration information                    */
/*===========================================================================*/

#include "gdisp_lld_config.h"

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

/**
 * @brief   Driver Control Constants
 * @details	Unsupported control codes are ignored.
 * @note	The value parameter should always be typecast to (void *).
 * @note	There are some predefined and some specific to the low level driver.
 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
 * 											that only supports off/on anything other
 * 											than zero is on.
 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
 * 											this value.
 */
#define GDISP_CONTROL_POWER			0
#define GDISP_CONTROL_ORIENTATION	1
#define GDISP_CONTROL_BACKLIGHT		2
#define GDISP_CONTROL_CONTRAST		3
#define GDISP_CONTROL_LLD			1000

/**
 * @brief   Driver Query Constants
 * @details	Unsupported query codes return (void *)-1.
 * @note	There are some predefined and some specific to the low level driver.
 * @note	The result should be typecast the required type.
 * @note	GDISP_QUERY_WIDTH			- Gets the width of the screen
 * 			GDISP_QUERY_HEIGHT			- Gets the height of the screen
 * 			GDISP_QUERY_POWER			- Get the current powermode
 * 			GDISP_QUERY_ORIENTATION		- Get the current orientation
 * 			GDISP_QUERY_BACKLIGHT		- Get the backlight state (0 to 100)
 * 			GDISP_QUERY_CONTRAST		- Get the contrast.
 * 			GDISP_QUERY_LLD				- Low level driver control constants start at
 * 											this value.
 */
#define GDISP_QUERY_WIDTH			0
#define GDISP_QUERY_HEIGHT			1
#define GDISP_QUERY_POWER			2
#define GDISP_QUERY_ORIENTATION		3
#define GDISP_QUERY_BACKLIGHT		4
#define GDISP_QUERY_CONTRAST		5
#define GDISP_QUERY_LLD				1000

/**
 * @brief   Driver Pixel Format Constants
 */
#define GDISP_PIXELFORMAT_RGB565	565
#define GDISP_PIXELFORMAT_RGB888	888
#define GDISP_PIXELFORMAT_RGB444	444
#define GDISP_PIXELFORMAT_RGB332	332
#define GDISP_PIXELFORMAT_RGB666	666
#define GDISP_PIXELFORMAT_CUSTOM	99999
#define GDISP_PIXELFORMAT_ERROR		88888

/*===========================================================================*/
/* Error checks.                                                             */
/*===========================================================================*/

/**
 * @name    GDISP hardware accelerated support
 * @{
 */
	/**
	 * @brief   Hardware accelerated line drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_LINES
		#define GDISP_HARDWARE_LINES			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated screen clears.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_CLEARS
		#define GDISP_HARDWARE_CLEARS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated rectangular fills.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_FILLS
		#define GDISP_HARDWARE_FILLS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated fills from an image.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_BITFILLS
		#define GDISP_HARDWARE_BITFILLS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_CIRCLES
		#define GDISP_HARDWARE_CIRCLES			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated filled circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_CIRCLEFILLS
		#define GDISP_HARDWARE_CIRCLEFILLS		FALSE
	#endif

	/**
	 * @brief   Hardware accelerated ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ELLIPSES
		#define GDISP_HARDWARE_ELLIPSES			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated filled ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ELLIPSEFILLS
		#define GDISP_HARDWARE_ELLIPSEFILLS		FALSE
	#endif

	/**
	 * @brief   Hardware accelerated arc's.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ARCS
		#define GDISP_HARDWARE_ARCS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated filled arcs.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ARCFILLS
		#define GDISP_HARDWARE_ARCFILLS		FALSE
	#endif

	/**
	 * @brief   Hardware accelerated text drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_TEXT
		#define GDISP_HARDWARE_TEXT				FALSE
	#endif

	/**
	 * @brief   Hardware accelerated text drawing with a filled background.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_TEXTFILLS
		#define GDISP_HARDWARE_TEXTFILLS		FALSE
	#endif

	/**
	 * @brief   Hardware accelerated scrolling.
	 * @details If set to @p FALSE there is no support for scrolling.
	 */
	#ifndef GDISP_HARDWARE_SCROLL
		#define GDISP_HARDWARE_SCROLL			FALSE
	#endif

	/**
	 * @brief   Reading back of pixel values.
	 * @details If set to @p FALSE there is no support for pixel read-back.
	 */
	#ifndef GDISP_HARDWARE_PIXELREAD
		#define GDISP_HARDWARE_PIXELREAD		FALSE
	#endif

	/**
	 * @brief   The driver supports one or more control commands.
	 * @details If set to @p FALSE there is no support for control commands.
	 */
	#ifndef GDISP_HARDWARE_CONTROL
		#define GDISP_HARDWARE_CONTROL			FALSE
	#endif

	/**
	 * @brief   The driver supports a non-standard query.
	 * @details If set to @p FALSE there is no support for non-standard queries.
	 */
	#ifndef GDISP_HARDWARE_QUERY
		#define GDISP_HARDWARE_QUERY			FALSE
	#endif

	/**
	 * @brief   The driver supports a clipping in hardware.
	 * @details If set to @p FALSE there is no support for non-standard queries.
	 */
	#ifndef GDISP_HARDWARE_CLIP
		#define GDISP_HARDWARE_CLIP			FALSE
	#endif
/** @} */

/**
 * @name    GDISP software algorithm choices
 * @{
 */
	/**
	 * @brief   For filled text drawing, use a background fill and then draw
	 *			the text instead of using a blit or direct pixel drawing.
	 * @details If set to @p TRUE background fill and then text draw is used.
	 * @note    This is ignored if hardware accelerated text is supported.
	 */
	#ifndef GDISP_SOFTWARE_TEXTFILLDRAW
		#define GDISP_SOFTWARE_TEXTFILLDRAW		FALSE
	#endif

	/**
	 * @brief   For filled text drawing, when using a bitmap blit
	 *			use a column by column buffer rather than a full character
	 *			buffer to save memory at a small performance cost.
	 * @details If set to @p TRUE background fill one character column at a time.
	 * @note    This is ignored if software text using blit is not being used.
	 */
	#ifndef GDISP_SOFTWARE_TEXTBLITCOLUMN
		#define GDISP_SOFTWARE_TEXTBLITCOLUMN	FALSE
	#endif
/** @} */

/**
 * @name    GDISP pixel format choices
 * @{
 */
	/**
	 * @brief   The native pixel format for this device
	 * @note	Should be set to one of the following:
	 *				GDISP_PIXELFORMAT_RGB565
	 *				GDISP_PIXELFORMAT_RGB888
	 *				GDISP_PIXELFORMAT_RGB444
	 *				GDISP_PIXELFORMAT_RGB332
	 *				GDISP_PIXELFORMAT_RGB666
	 *				GDISP_PIXELFORMAT_CUSTOM
	 * @note	If you set GDISP_PIXELFORMAT_CUSTOM you need to also define
	 *				color_t, RGB2COLOR(r,g,b), HTML2COLOR(h),
	 *              RED_OF(c), GREEN_OF(c), BLUE_OF(c),
	 *              COLOR(c) and MASKCOLOR.
	 */
	#ifndef GDISP_PIXELFORMAT
		#define GDISP_PIXELFORMAT	GDISP_PIXELFORMAT_ERROR
	#endif

	/**
	 * @brief   Do pixels require packing for a blit
	 * @note	Is only valid for a pixel format that doesn't fill it's datatype. ie formats:
	 *				GDISP_PIXELFORMAT_RGB888
	 *				GDISP_PIXELFORMAT_RGB444
	 *				GDISP_PIXELFORMAT_RGB666
	 *				GDISP_PIXELFORMAT_CUSTOM
	 * @note	If you use GDISP_PIXELFORMAT_CUSTOM and packed bit fills
	 *				you need to also define @p gdispPackPixels(buf,cx,x,y,c)
	 * @note	If you are using GDISP_HARDWARE_BITFILLS = FALSE then the pixel
	 *				format must not be a packed format as the software blit does
	 *				not support packed pixels
	 * @note	Very few cases should actually require packed pixels as the low
	 *				level driver can also pack on the fly as it is sending it
	 *				to the graphics device.
	 */
	#ifndef GDISP_PACKED_PIXELS
		#define GDISP_PACKED_PIXELS			FALSE
	#endif

	/**
	 * @brief   Do lines of pixels require packing for a blit
	 * @note	Ignored if GDISP_PACKED_PIXELS is FALSE
	 */
	#ifndef GDISP_PACKED_LINES
		#define GDISP_PACKED_LINES			FALSE
	#endif
/** @} */

/*===========================================================================*/
/* Define the macro's for the various pixel formats */
/*===========================================================================*/

#if defined(__DOXYGEN__)
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

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB565
	typedef uint16_t color_t;
	#define COLOR(c)			((color_t)(c))
	#define MASKCOLOR			FALSE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF8)<<8) | (((g) & 0xFC)<<3) | (((b) & 0xF8)>>3)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF80000)>>8) | (((h) & 0x00FC00)>>5) | (((h) & 0x0000F8)>>3)))
	#define RED_OF(c)			(((c) & 0xF800)>>8)
	#define GREEN_OF(c)			(((c)&0x07E0)>>3)
	#define BLUE_OF(c)			(((c)&0x001F)<<3)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB888
	typedef uint32_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0xFFFFFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xFF)<<16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))
	#define HTML2COLOR(h)		((color_t)(h))
	#define RED_OF(c)			(((c) & 0xFF0000)>>16)
	#define GREEN_OF(c)			(((c)&0x00FF00)>>8)
	#define BLUE_OF(c)			((c)&0x0000FF)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB444
	typedef uint16_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0x0FFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF0)<<4) | ((g) & 0xF0) | (((b) & 0xF0)>>4)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF00000)>>12) | (((h) & 0x00F000)>>8) | (((h) & 0x0000F0)>>4)))
	#define RED_OF(c)			(((c) & 0x0F00)>>4)
	#define GREEN_OF(c)			((c)&0x00F0)
	#define BLUE_OF(c)			(((c)&0x000F)<<4)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB332
	typedef uint8_t color_t;
	#define COLOR(c)			((color_t)(c))
	#define MASKCOLOR			FALSE
	#define RGB2COLOR(r,g,b)	((color_t)(((r) & 0xE0) | (((g) & 0xE0)>>3) | (((b) & 0xC0)>>6)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xE00000)>>16) | (((h) & 0x00E000)>>11) | (((h) & 0x0000C0)>>6)))
	#define RED_OF(c)			((c) & 0xE0)
	#define GREEN_OF(c)			(((c)&0x1C)<<3)
	#define BLUE_OF(c)			(((c)&0x03)<<6)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB666
	typedef uint32_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0x03FFFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xFC)<<10) | (((g) & 0xFC)<<4) | (((b) & 0xFC)>>2)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xFC0000)>>6) | (((h) & 0x00FC00)>>4) | (((h) & 0x0000FC)>>2)))
	#define RED_OF(c)			(((c) & 0x03F000)>>12)
	#define GREEN_OF(c)			(((c)&0x00FC00)>>8)
	#define BLUE_OF(c)			(((c)&0x00003F)<<2)

#elif GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_CUSTOM
	#error "GDISP: No supported pixel format has been specified."
#endif

/* Verify information for packed pixels and define a non-packed pixel macro */
#if !GDISP_PACKED_PIXELS
	#define gdispPackPixels(buf,cx,x,y,c)	{ ((color_t *)(buf))[(y)*(cx)+(x)] = (c); }
#elif !GDISP_HARDWARE_BITFILLS
	#error "GDISP: packed pixel formats are only supported for hardware accelerated drivers."
#elif GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB888 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB444 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB666 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_CUSTOM
	#error "GDISP: A packed pixel format has been specified for an unsupported pixel format."
#endif

#if GDISP_NEED_SCROLL && !GDISP_HARDWARE_SCROLL
	#error "GDISP: Hardware scrolling is wanted but not supported."
#endif

#if GDISP_NEED_PIXELREAD && !GDISP_HARDWARE_PIXELREAD
	#error "GDISP: Pixel read-back is wanted but not supported."
#endif

/*===========================================================================*/
/* Driver types.                                                             */
/*===========================================================================*/

/**
 * @brief   The type of a pixel.
 */
typedef color_t		pixel_t;
/**
 * @brief   The type of a font.
 */
typedef const struct font *font_t;
/**
 * @brief   Type for the screen orientation.
 */
typedef enum orientation {GDISP_ROTATE_0, GDISP_ROTATE_90, GDISP_ROTATE_180, GDISP_ROTATE_270} gdisp_orientation_t;
/**
 * @brief   Type for the available power modes for the screen.
 */
typedef enum powermode {powerOff, powerSleep, powerDeepSleep, powerOn} gdisp_powermode_t;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/* Core functions */
	extern bool_t gdisp_lld_init(void);

	/* Some of these functions will be implemented in software by the high level driver
	   depending on the GDISP_HARDWARE_XXX macros defined in gdisp_lld_config.h.
	 */

	/* Drawing functions */
	extern void gdisp_lld_clear(color_t color);
	extern void gdisp_lld_draw_pixel(coord_t x, coord_t y, color_t color);
	extern void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
	extern void gdisp_lld_blit_area_ex(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);
	extern void gdisp_lld_draw_line(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);

	/* Circular Drawing Functions */
	#if GDISP_NEED_CIRCLE
	extern void gdisp_lld_draw_circle(coord_t x, coord_t y, coord_t radius, color_t color);
	extern void gdisp_lld_fill_circle(coord_t x, coord_t y, coord_t radius, color_t color);
	#endif

	#if GDISP_NEED_ELLIPSE
	extern void gdisp_lld_draw_ellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	extern void gdisp_lld_fill_ellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	#endif

	/* Arc Drawing Functions */
	#if GDISP_NEED_ARC
	extern void gdisp_lld_draw_arc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	extern void gdisp_lld_fill_arc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	#endif

	/* Text Rendering Functions */
	#if GDISP_NEED_TEXT
	extern void gdisp_lld_draw_char(coord_t x, coord_t y, char c, font_t font, color_t color);
	extern void gdisp_lld_fill_char(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor);
	#endif

	/* Pixel readback */
	#if GDISP_NEED_PIXELREAD
	extern color_t gdisp_lld_get_pixel_color(coord_t x, coord_t y);
	#endif

	/* Scrolling Function - clears the area scrolled out */
	#if GDISP_NEED_SCROLL
	extern void gdisp_lld_vertical_scroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
	#endif

	/* Set driver specific control */
	#if GDISP_NEED_CONTROL
	extern void gdisp_lld_control(unsigned what, void *value);
	#endif

	/* Query driver specific data */
	extern void *gdisp_lld_query(unsigned what);

	/* Clipping Functions */
	#if GDISP_NEED_CLIP
	extern void gdisp_lld_set_clip(coord_t x, coord_t y, coord_t cx, coord_t cy);
	#endif

	/* Messaging API */
	#if GDISP_NEED_MSGAPI
	#include "gdisp_lld_msgs.h"
	extern void gdisp_lld_msg_dispatch(gdisp_lld_msg_t *msg);
	#endif

#ifdef __cplusplus
}
#endif

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_H */
/** @} */

