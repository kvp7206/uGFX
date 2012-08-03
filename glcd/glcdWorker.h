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

#ifndef GLCD_WORKER_H
#define GLCD_WORKER_H

#include "glcd.h"

#define GLCD_WORKER_SIZE 512

enum glcd_action {	GLCD_SET_POWERMODE,
					GLCD_SET_ORIENTATION,
					GLCD_SET_WINDOW,
					GLCD_FILL_AREA,
					GLCD_WRITE_AREA,
					GLCD_CLEAR,
					GLCD_GET_PIXEL_COLOR,
					GLCD_DRAW_PIXEL,
					GLCD_WRITE_STREAM_START,
					GLCD_WRITE_STREAM_STOP,
					GLCD_WRITE_STREAM,
					GLCD_VERTICAL_SCROLL,
					GLCD_DRAW_CHAR,
				};

#define _glcd_msg_base			\
	enum glcd_action action;

struct glcd_msg_base {
	_glcd_msg_base
};

struct glcd_msg_powermode {
	_glcd_msg_base

	uint8_t powermode;
};

struct glcd_msg_orientation {
	_glcd_msg_base

	uint8_t newOrientation;
};

struct glcd_msg_set_window {
	_glcd_msg_base

	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
};

struct glcd_msg_fill_area {
	_glcd_msg_base

	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint16_t color;
};

struct glcd_msg_write_area {
	_glcd_msg_base

	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint16_t *buffer;
	size_t size;
};

struct glcd_msg_clear {
	_glcd_msg_base

	uint16_t color;
};

struct glcd_msg_get_pixel_color {
	_glcd_msg_base

	uint16_t x;
	uint16_t y;
	uint16_t color;
};

struct glcd_msg_draw_pixel {
	_glcd_msg_base

	uint16_t x;
	uint16_t y;
	uint16_t color;
};

struct glcd_msg_write_stream_start {
	_glcd_msg_base
};

struct glcd_msg_write_stream_stop {
	_glcd_msg_base
};

struct glcd_msg_write_stream {
	_glcd_msg_base

	uint16_t *buffer;
	uint16_t size;
};

struct glcd_msg_vertical_scroll {
	_glcd_msg_base

	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	int16_t lines;
};

struct glcd_msg_draw_char {
	_glcd_msg_base;

	uint16_t cx;
	uint16_t cy;
	uint16_t color;
	uint16_t bkcolor;
	uint16_t ret_width;
	char c;
	font_t font;
	bool_t tpText;
};

#endif

