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

#ifndef CONSOLE_H
#define CONSOLE_H

#include "ch.h"
#include "hal.h"

#include "gdisp.h"

/**
 * @brief   Structure representing a GLCD driver.
 */
typedef struct GLCDConsole GLCDConsole;

/**
 * @brief   @p GLCDConsole specific methods.
 */
#define _glcd_driver_methods                                              \
	_base_asynchronous_channel_methods

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p GLCDConsole virtual methods table.
 */
struct GLCDConsoleVMT {
	_glcd_driver_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   GLCD Console class.
 * @details This class extends @p BaseAsynchronousChannel by adding physical
 *          I/O queues.
 */
struct GLCDConsole {
	/** @brief Virtual Methods Table.*/
	const struct GLCDConsoleVMT *vmt;
	_base_asynchronous_channel_data
	/* WARNING: Do not add any data to this struct above this comment, only below */
	/* font */
	font_t font;
	/* lcd area to use */
	coord_t x0,y0;
	/* current cursor position, in pixels */
	coord_t cx,cy;
	/* console size in pixels */
	coord_t sx,sy;
	/* foreground and background colour */
	pixel_t bkcolor, color;
	/* font size in pixels */
	uint8_t fy;
};

#ifdef __cplusplus
extern "C" {
#endif

msg_t lcdConsoleInit(GLCDConsole *console, coord_t x0, coord_t y0, coord_t width, coord_t height, font_t font, pixel_t bkcolor, pixel_t color);
msg_t lcdConsolePut(GLCDConsole *console, char c);
msg_t lcdConsoleWrite(GLCDConsole *console, char *bp, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */
