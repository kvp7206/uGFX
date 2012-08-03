/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    templates/gdisp_lld_config.h
 * @brief   GDISP Graphic Driver subsystem low level driver header template.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if HAL_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    GDISP hardware accelerated support
 * @{
 */
	/**
	 * @brief   Hardware supports changing the orientation.
	 * @details If set to @p FALSE a software stubb is provided.
	 */
	#define GDISP_HARDWARE_ORIENTATION		FALSE

	/**
	 * @brief   Hardware supports power control.
	 * @details If set to @p FALSE a software stubb is provided.
	 */
	#define GDISP_HARDWARE_POWERCONTROL		FALSE

	/**
	 * @brief   Hardware accelerated line drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_LINES			FALSE

	/**
	 * @brief   Hardware accelerated box drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_BOX				FALSE

	/**
	 * @brief   Hardware accelerated screen clears.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_CLEARS			FALSE

	/**
	 * @brief   Hardware accelerated rectangular fills.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_FILLS			FALSE

	/**
	 * @brief   Hardware accelerated fills from an image.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_BITFILLS			FALSE

	/**
	 * @brief   Hardware accelerated circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_CIRCLES			FALSE

	/**
	 * @brief   Hardware accelerated filled circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_CIRCLEFILLS		FALSE

	/**
	 * @brief   Hardware accelerated ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_ELLIPSES			FALSE

	/**
	 * @brief   Hardware accelerated filled ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_ELLIPSEFILLS		FALSE

	/**
	 * @brief   Hardware accelerated text drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_TEXT				FALSE

	/**
	 * @brief   Hardware accelerated text drawing with a filled background.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#define GDISP_HARDWARE_TEXTFILLS		FALSE

	/**
	 * @brief   Hardware accelerated scrolling.
	 * @details If set to @p FALSE there is no support for scrolling.
	 */
	#define GDISP_HARDWARE_SCROLL			FALSE

	/**
	 * @brief   Reading back of pixel values.
	 * @details If set to @p FALSE there is no support for pixel read-back.
	 */
	#define GDISP_HARDWARE_PIXELREAD		FALSE
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
	#define GDISP_SOFTWARE_TEXTFILLDRAW		FALSE
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

#endif	/* HAL_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */
