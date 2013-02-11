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
 * @file    include/gwin/console.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Console Console
 * @ingroup GWIN
 *
 * @details		GWIN allows it to create a console/terminal like window.
 *				You can simply use chprintf() to print to the terminal.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_CONSOLE must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_CONSOLE_H
#define _GWIN_CONSOLE_H

#if GWIN_NEED_CONSOLE || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

#define GW_CONSOLE				0x0001

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

/**
 * @brief   Create a console window.
 * @details	A console window allows text to be written using chprintf() (and the console functions defined here).
 * @brief	Text in a console window supports newlines and will wrap text as required.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gc		The GConsoleObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @param[in] font		The font to use
 * @note				The console is not automatically cleared on creation. You must do that by calling gwinClear() (possibly after changing your background color)
 * @note				If the dispay does not support scrolling, the window will be cleared when the bottom line is reached.
 * @note				The default drawing color gets set to White and the background drawing color to Black.
 * @note				The dimensions and position may be changed to fit on the real screen.
 *
 * @api
 */
GHandle gwinCreateConsole(GConsoleObject *gc, coord_t x, coord_t y, coord_t width, coord_t height, font_t font);

/**
 * @brief   Get a stream from a console window suitable for use with chprintf().
 * @return	The stream handle or NULL if this is not a console window.
 *
 * @param[in] gh	The window handle (must be a console window)
 *
 * @api
 */
BaseSequentialStream *gwinGetConsoleStream(GHandle gh);

/**
 * @brief   Put a character at the cursor position in the window.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] c		The character to draw
 *
 * @api
 */
void gwinPutChar(GHandle gh, char c);

/**
 * @brief   Put a string at the cursor position in the window. It will wrap lines as required.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] str	The string to draw
 *
 * @api
 */
void gwinPutString(GHandle gh, const char *str);

/**
 * @brief   Put the character array at the cursor position in the window. It will wrap lines as required.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] str	The string to draw
 * @param[in] n		The number of characters to draw
 *
 * @api
 */
void gwinPutCharArray(GHandle gh, const char *str, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* GWIN_NEED_CONSOLE */

#endif /* _GWIN_CONSOLE_H */
/** @} */
