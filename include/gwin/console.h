/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
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

/* This file is included within "gwin/gwin.h" */

// A console window. Supports wrapped text writing and a cursor.
typedef struct GConsoleObject_t {
	GWindowObject	g;
	coord_t			cx, cy;			// Cursor position

	#if GFX_USE_OS_CHIBIOS && GWIN_CONSOLE_USE_BASESTREAM
		struct GConsoleWindowStream_t {
			const struct GConsoleWindowVMT_t *vmt;
			_base_asynchronous_channel_data
			} stream;
	#endif
	
	} GConsoleObject;

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
 * @param[in] x,y		The screen co-ordinates for the top left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 *
 * @note				The console is not automatically cleared on creation. You must do that by calling gwinClear() (possibly after changing your background color)
 * @note				Don't forget to set the font using @p gwinSetFont() or @p gwinSetDefaultFont()
 * @note				If the dispay does not support scrolling, the window will be cleared when the bottom line is reached.
 * @note				The default drawing color gets set to White and the background drawing color to Black.
 * @note				The dimensions and position may be changed to fit on the real screen.
 *
 * @api
 */
GHandle gwinCreateConsole(GConsoleObject *gc, coord_t x, coord_t y, coord_t width, coord_t height);

#if GFX_USE_OS_CHIBIOS && GWIN_CONSOLE_USE_BASESTREAM
	/**
	 * @brief   Get a stream from a console window suitable for use with chprintf().
	 * @return	The stream handle or NULL if this is not a console window.
	 *
	 * @param[in] gh	The window handle (must be a console window)
	 *
	 * @note		Only useful in ChibiOS
	 *
	 * @api
	 */
	BaseSequentialStream *gwinGetConsoleStream(GHandle gh);
#endif

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

/**
 * @brief   Print a formatted string at the cursor position in the window. It will wrap lines as required.
 * @details This function implements a minimal printf() like functionality
 *          The general parameters format is: %[-][width|*][.precision|*][l|L]p.
 *          The following parameter types (p) are supported:
 *          - <b>x</b> hexadecimal integer.
 *          - <b>X</b> hexadecimal long.
 *          - <b>o</b> octal integer.
 *          - <b>O</b> octal long.
 *          - <b>d</b> decimal signed integer.
 *          - <b>D</b> decimal signed long.
 *          - <b>u</b> decimal unsigned integer.
 *          - <b>U</b> decimal unsigned long.
 *          - <b>c</b> character.
 *          - <b>s</b> string.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] fmt	The format string (as per printf)
 * @param[in] ...	The format string arguments.
 *
 * @api
 */
void gwinPrintf(GHandle gh, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_CONSOLE_H */
/** @} */
