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
		Andrew Hannam aka inmarket	-> framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/**
 * @file    templates/gdisp_lld.h
 * @brief   GDISP Graphic Driver subsystem low level driver header template.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_H
#define _GDISP_LLD_H

#if HAL_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    GDISP hardware accelerated support
 * @{
 */
	/**
	 * @brief   Hardware accelerated line drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_LINES		FALSE

	/**
	 * @brief   Hardware accelerated box drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_BOX			FALSE

	/**
	 * @brief   Hardware accelerated screen clears.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_CLEARS		FALSE

	/**
	 * @brief   Hardware accelerated rectangular fills.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_FILLS		FALSE

	/**
	 * @brief   Hardware accelerated fills from an image.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_BITFILLS		FALSE

	/**
	 * @brief   Hardware accelerated circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_CIRCLES		FALSE

	/**
	 * @brief   Hardware accelerated filled circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_CIRCLEFILLS	FALSE

	/**
	 * @brief   Hardware accelerated ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_ELLIPSES		FALSE

	/**
	 * @brief   Hardware accelerated filled ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_ELLIPSEFILLS	FALSE

	/**
	 * @brief   Hardware accelerated text drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_TEXT			FALSE

	/**
	 * @brief   Hardware accelerated text drawing with a filled background.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_TEXTFILLS	FALSE

	/**
	 * @brief   Hardware accelerated scrolling.
	 * @details If set to @p FALSE there is no support for scrolling.
	 */
	#define GDISP_HARDWARE_SCROLL		FALSE

	/**
	 * @brief   Reading back of pixel values.
	 * @details If set to @p FALSE there is no support for pixel read-back.
	 */
	#define GDISP_HARDWARE_PIXELREAD	FALSE
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
	#define GDISP_SOFTWARE_TEXTFILLDRAW	FALSE
	/**
	 * @brief   For filled text drawing, when using a bitmap blit
	 *			use a column by column buffer rather than a full character
	 *			buffer to save memory at a small performance cost.
	 * @details If set to @p TRUE background fill one character column at a time.
	 * @note    This is ignored if software text using blit is not being used.
	 */
	#define GDISP_SOFTWARE_TEXTBLITCOLUMN	FALSE
/** @} */

/**
 * @name    GDISP pixel format choices
 * @{
 */
	/**
	 * @brief   The native pixel format for this device
	 * @note	One of the following should be defined:
	 *				GDISP_PIXELFORMAT_RGB565
	 *				GDISP_PIXELFORMAT_RGB888
	 *				GDISP_PIXELFORMAT_RGB444
	 *				GDISP_PIXELFORMAT_RGB332
	 *				GDISP_PIXELFORMAT_RGB666
	 *				GDISP_PIXELFORMAT_CUSTOM
	 * @note	If you define GDISP_PIXELFORMAT_CUSTOM you need to also define
	 *				color_t, RGB2COLOR(r,g,b), HTML2COLOR(h),
	 *              RED_OF(c), GREEN_OF(c), BLUE_OF(c),
	 *              COLOR(c) and MASKCOLOR.
	 */
	#define GDISP_PIXELFORMAT_RGB565

	/**
	 * @brief   Do pixels require packing for a blit
	 * @note	Is only valid for a pixel format that doesn't fill it's datatype. ie formats:
	 *				GDISP_PIXELFORMAT_RGB888
	 *				GDISP_PIXELFORMAT_RGB444
	 *				GDISP_PIXELFORMAT_RGB666
	 *				GDISP_PIXELFORMAT_CUSTOM
	 * @note	If you use GDISP_PIXELFORMAT_CUSTOM and packed bit fills
	 *				you need to also define @P gdispPackPixels(buf,cx,x,y,c)
	 * @note	If you are using GDISP_HARDWARE_BITFILLS = FALSE then the pixel
	 *				format must not be a packed format as the software blit does
	 *				not support packed pixels
	 * @note	Very few cases should actually require packed pixels as the low
	 *				level driver can also pack on the fly as it is sending it
	 *				to the graphics device.
	 */
	#define GDISP_PACKED_PIXELS			FALSE

	/**
	 * @brief   Do lines of pixels require packing for a blit
	 * @note	Ignored if GDISP_PACKED_PIXELS is FALSE
	 */
	#define GDISP_PACKED_LINES			FALSE

	/**
	 * @brief   Do lines of pixels require packing for a blit
	 * @note	Ignored if GDISP_PACKED_PIXELS is FALSE
	 */
	#define GDISP_PACKED_LINES			FALSE
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Structure representing a GDISP driver.
 * @note    Implementations may extend this structure to contain more,
 *          architecture dependent, fields.
 */
struct GDISPDriver {
	/**
	 * @brief   Width of the screen.
	 * @note    Read-only.
	 */
	coord_t				Width;
	/**
	 * @brief   Height of the screen.
	 * @note    Read-only.
	 */
	coord_t				Height;
	/**
	 * @brief   Current orientation of the screen.
	 * @note    Read-only.
	 */
	gdisp_orientation_t	Orientation;
	/**
	 * @brief   Current power mode of the screen.
	 * @note    Read-only.
	 */
	gdisp_powermode_t	Powermode;
	
	#if defined(GDISP_DRIVER_EXT_FIELDS)
	GDISP_DRIVER_EXT_FIELDS
	#endif
	
	/* End of mandatory fields */
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
	extern GDISPDriver GDISP1;
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/* Core functions */
	void gdisp_lld_init(void);
	void gdisp_lld_setpowermode(gdisp_powermode_t powerMode);
	void gdisp_lld_setorientation(gdisp_orientation_t newOrientation);

	/* Some of these functions will be implemented in software by the high level driver
	   depending on the GDISP_HARDWARE_XXX macros defined above.
	 */

	/* Drawing functions */
	void gdisp_lld_clear(color_t color);
	void gdisp_lld_drawpixel(coord_t x, coord_t y, color_t color);
	void gdisp_lld_drawline(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
	void gdisp_lld_drawbox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
	void gdisp_lld_fillarea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
	void gdisp_lld_blitarea(coord_t x, coord_t y, coord_t cx, coord_t cy, pixel_t *buffer);

	/* Circular Drawing Functions */
	#if GDISP_NEED_CIRCLE
	void gdisp_lld_drawcircle(coord_t x, coord_t y, coord_t radius, color_t color);
	void gdisp_lld_fillcircle(coord_t x, coord_t y, coord_t radius, color_t color);
	#endif

	#if GDISP_NEED_ELLIPSE
	void gdisp_lld_drawellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	void gdisp_lld_fillellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	#endif

	/* Text Rendering Functions */
	#if GDISP_NEED_TEXT
	void gdisp_lld_drawchar(coord_t x, coord_t y, char c, font_t font, color_t color);
	void gdisp_lld_fillchar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor);
	#endif

	/* Pixel readback */
	#if GDISP_NEED_PIXELREAD
	color_t gdisp_lld_getpixelcolor(coord_t x, coord_t y);
	#endif

	/* Scrolling Function - clears the area scrolled out */
	#if GDISP_NEED_SCROLL
	void gdisp_lld_verticalscroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
	#endif

#ifdef __cplusplus
}
#endif

#endif	/* HAL_USE_GDISP */

#endif	/* _GDISP_LLD_H */
/** @} */
