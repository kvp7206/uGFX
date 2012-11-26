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
 * @file    gwin/gwin_console.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @addtogroup GWIN
 * @{
 */
#ifndef _GWIN_CONSOLE_H
#define _GWIN_CONSOLE_H

/**
 * @name    GWIN more complex functionality to be compiled
 * @{
 */
	/**
	 * @brief   Should console functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_CONSOLE
		#define GWIN_NEED_CONSOLE	FALSE
	#endif
/** @} */

#if GWIN_NEED_CONSOLE || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

#define GW_CONSOLE				0x0001

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

#if GWIN_NEED_CONSOLE && !GDISP_NEED_TEXT
	#error "GWIN: Text support (GDISP_NEED_TEXT) is required if GWIN_NEED_CONSOLE is defined."
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

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

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	GHandle gwinCreateConsole(GConsoleObject *gc, coord_t x, coord_t y, coord_t width, coord_t height, font_t font);
	BaseSequentialStream *gwinGetConsoleStream(GHandle gh);
	void gwinPutChar(GHandle gh, char c);
	void gwinPutString(GHandle gh, const char *str);
	void gwinPutCharArray(GHandle gh, const char *str, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* GWIN_NEED_CONSOLE */

#endif /* _GWIN_CONSOLE_H */
/** @} */
