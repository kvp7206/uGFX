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

#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef HAL_USE_GDISP

#ifndef GDISP_NEED_CONSOLE
	#define GDISP_NEED_CONSOLE	FALSE
#endif

#if GDISP_NEED_CONSOLE

#include "gdisp.h"

/**
 * @brief   Structure representing a GConsole driver.
 */
typedef struct GConsole GConsole;

#ifdef __cplusplus
extern "C" {
#endif

msg_t lcdConsoleInit(GConsole *console, coord_t x0, coord_t y0, coord_t width, coord_t height, font_t font, pixel_t bkcolor, pixel_t color);
msg_t lcdConsolePut(GConsole *console, char c);
msg_t lcdConsoleWrite(GConsole *console, const uint8_t *bp, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* GDISP_NEED_CONSOLE */
#endif /* HAL_USE_GDISP */
#endif /* CONSOLE_H */
