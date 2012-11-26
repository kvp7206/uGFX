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
 * @file    src/gwin/console.c
 * @brief   GWIN Driver code.
 *
 * @addtogroup GWIN_CONSOLE
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gwin.h"

#if (GFX_USE_GWIN && GWIN_NEED_CONSOLE) || defined(__DOXYGEN__)

#include <string.h>
#include "gwin_internal.h"


#define GWIN_CONSOLE_USE_CLEAR_LINES			TRUE
#define GWIN_CONSOLE_USE_FILLED_CHARS			FALSE

/*
 * Stream interface implementation. The interface is write only
 */

#define Stream2GWindow(ip)		((GHandle)(((char *)(ip)) - (size_t)(&(((GConsoleObject *)0)->stream))))

static size_t GWinStreamWrite(void *ip, const uint8_t *bp, size_t n) { gwinPutCharArray(Stream2GWindow(ip), (const char *)bp, n); return RDY_OK; }
static size_t GWinStreamRead(void *ip, uint8_t *bp, size_t n) {	(void)ip; (void)bp; (void)n; return 0; }
static msg_t GWinStreamPut(void *ip, uint8_t b) { gwinPutChar(Stream2GWindow(ip), (char)b); return RDY_OK; }
static msg_t GWinStreamGet(void *ip) {(void)ip; return RDY_OK; }
static msg_t GWinStreamPutTimed(void *ip, uint8_t b, systime_t time) { (void)time; gwinPutChar(Stream2GWindow(ip), (char)b); return RDY_OK; }
static msg_t GWinStreamGetTimed(void *ip, systime_t timeout) { (void)ip; (void)timeout; return RDY_OK; }
static size_t GWinStreamWriteTimed(void *ip, const uint8_t *bp, size_t n, systime_t time) { (void)time; gwinPutCharArray(Stream2GWindow(ip), (const char *)bp, n); return RDY_OK; }
static size_t GWinStreamReadTimed(void *ip, uint8_t *bp, size_t n, systime_t time) { (void)ip; (void)bp; (void)n; (void)time; return 0; }

struct GConsoleWindowVMT_t {
	_base_asynchronous_channel_methods
};

static const struct GConsoleWindowVMT_t GWindowConsoleVMT = {
	GWinStreamWrite,
	GWinStreamRead,
	GWinStreamPut,
	GWinStreamGet,
	GWinStreamPutTimed,
	GWinStreamGetTimed,
	GWinStreamWriteTimed,
	GWinStreamReadTimed
};

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
GHandle gwinCreateConsole(GConsoleObject *gc, coord_t x, coord_t y, coord_t width, coord_t height, font_t font) {
	if (!(gc = (GConsoleObject *)_gwinInit((GWindowObject *)gc, x, y, width, height, sizeof(GConsoleObject))))
		return 0;
	gc->gwin.type = GW_CONSOLE;
	gwinSetFont(&gc->gwin, font);
	gc->stream.vmt = &GWindowConsoleVMT;
	gc->cx = 0;
	gc->cy = 0;
	return (GHandle)gc;
}

/**
 * @brief   Get a stream from a console window suitable for use with chprintf().
 * @return	The stream handle or NULL if this is not a console window.
 *
 * @param[in] gh	The window handle (must be a console window)
 *
 * @api
 */
BaseSequentialStream *gwinGetConsoleStream(GHandle gh) {
	if (gh->type != GW_CONSOLE)
		return 0;
	return (BaseSequentialStream *)&(((GConsoleObject *)(gh))->stream);
}

/**
 * @brief   Put a character at the cursor position in the window.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] c		The character to draw
 *
 * @api
 */
void gwinPutChar(GHandle gh, char c) {
	uint8_t			width;
	#define gcw		((GConsoleObject *)gh)

	if (gh->type != GW_CONSOLE || !gh->font) return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	
	if (c == '\n') {
		gcw->cx = 0;
		gcw->cy += gcw->fy;
		// We use lazy scrolling here and only scroll when the next char arrives
	} else if (c == '\r') {
		// gcw->cx = 0;
	} else {
		width = gdispGetCharWidth(c, gh->font) + gcw->fp;
		if (gcw->cx + width >= gh->width) {
			gcw->cx = 0;
			gcw->cy += gcw->fy;
		}

		if (gcw->cy + gcw->fy > gh->height) {
#if GDISP_NEED_SCROLL
			/* scroll the console */
			gdispVerticalScroll(gh->x, gh->y, gh->width, gh->height, gcw->fy, gh->bgcolor);
			/* reset the cursor to the start of the last line */
			gcw->cx = 0;
			gcw->cy = (((coord_t)(gh->height/gcw->fy))-1)*gcw->fy;
#else
			/* clear the console */
			gdispFillArea(gh->x, gh->y, gh->width, gh->height, gh->bgcolor);
			/* reset the cursor to the top of the window */
			gcw->cx = 0;
			gcw->cy = 0;
#endif
		}

#if GWIN_CONSOLE_USE_CLEAR_LINES
		/* clear to the end of the line */
		if (gcw->cx == 0)
			gdispFillArea(gh->x, gh->y + gcw->cy, gh->width, gcw->fy, gh->bgcolor);
#endif
#if GWIN_CONSOLE_USE_FILLED_CHARS
		gdispFillChar(gh->x + gcw->cx, gh->y + gcw->cy, c, gh->font, gh->color, gh->bgcolor);
#else
		gdispDrawChar(gh->x + gcw->cx, gh->y + gcw->cy, c, gh->font, gh->color);
#endif

		/* update cursor */
		gcw->cx += width;
	}
	#undef gcw
}

/**
 * @brief   Put a string at the cursor position in the window. It will wrap lines as required.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] str	The string to draw
 *
 * @api
 */
void gwinPutString(GHandle gh, const char *str) {
	while(*str)
		gwinPutChar(gh, *str++);
}

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
void gwinPutCharArray(GHandle gh, const char *str, size_t n) {
	while(n--)
		gwinPutChar(gh, *str++);
}

#endif /* GFX_USE_GWIN && GWIN_NEED_CONSOLE */
/** @} */

