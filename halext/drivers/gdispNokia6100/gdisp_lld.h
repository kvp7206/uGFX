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
