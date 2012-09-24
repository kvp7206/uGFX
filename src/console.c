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

#if GDISP_NEED_CONSOLE

#include "ch.h"
#include "hal.h"

#include "console.h"

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p GConsole virtual methods table.
 */
struct GConsoleVMT {
	_base_asynchronous_channel_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   GConsole class.
 * @details This class extends @p BaseAsynchronousChannel by adding physical
 *          I/O queues.
 */
struct GConsole {
	/** @brief Virtual Methods Table.*/
	const struct GConsoleVMT *vmt;
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
	color_t bkcolor, color;
	/* font size in pixels */
	uint8_t fy;
	/* font inter-character padding in pixels */
	uint8_t fp;
};

/*
 * Interface implementation. The interface is write only
 */
static size_t writes(void *ip, const uint8_t *bp, size_t n) {
	return lcdConsoleWrite((GConsole *)ip, bp, n);
}

static size_t reads(void *ip, uint8_t *bp, size_t n) {
	(void)ip;
	(void)bp;
	(void)n;

	return 0;
}

static msg_t put(void *ip) {
	(void)ip;

	return RDY_OK;
}

static msg_t get(void *ip) {
	(void)ip;

	return RDY_OK;
}

static msg_t putt(void *ip, uint8_t b, systime_t timeout) {
	(void)timeout;

	/* TODO: handle timeout */

	return lcdConsolePut((GConsole *)ip, (char)b);
}

static msg_t gett(void *ip, systime_t timeout) {
	(void)ip;
	(void)timeout;

	return RDY_OK;
}

static size_t writet(void *ip, const uint8_t *bp, size_t n, systime_t time) {
	(void)time;

	return lcdConsoleWrite((GConsole *)ip, bp, n);
}

static size_t readt(void *ip, uint8_t *bp, size_t n, systime_t time) {
	(void)ip;
	(void)bp;
	(void)n;
	(void)time;

	return 0;
}

static ioflags_t getflags(void *ip) {
	_ch_get_and_clear_flags_impl(ip);
}

static const struct GConsoleVMT vmt = {
	writes, reads, put, get,
	putt, gett, writet, readt,
	getflags
};


msg_t lcdConsoleInit(GConsole *console, coord_t x0, coord_t y0, coord_t width, coord_t height, font_t font, pixel_t bkcolor, pixel_t color) {
	console->vmt = &vmt;
	/* read font, get height & padding */
	console->fy = gdispGetFontMetric(font, fontHeight);
	console->fp = gdispGetFontMetric(font, fontCharPadding);

	/* calculate the size of the console as an integer multiple of characters height*/
	console->sx = width;
	console->sy = (((int16_t)(height/console->fy))-1)*console->fy;

	console->cx = 0;
	console->cy = 0;
	console->x0 = x0;
	console->y0 = y0;

	console->bkcolor = bkcolor;
	console->color = color;

	console->font = font;

	gdispFillArea(x0, y0, x0 + width, y0 + height, console->bkcolor);
	return RDY_OK;
}

msg_t lcdConsolePut(GLCDConsole *console, char c) {
	uint8_t width;

	if(c == '\n') {
		/* clear the text at the end of the line */
		if(console->cx < console->sx)
			gdispFillArea(console->x0 + console->cx, console->y0 + console->cy,
						console->sx - console->cx, console->fy,
						console->bkcolor);
		console->cx = 0;
		console->cy += console->fy;
	} else if(c == '\r') {
		/* TODO: work backwards through the buffer to the start of the current line */
		//console->cx = 0;
	} else {
		width = gdispGetCharWidth(c, console->font) + console->fp;
		if((console->cx + width) >= console->sx) {
			/* clear the text at the end of the line */
			if (console->cy <= console->sy)
				gdispFillArea(console->x0 + console->cx, console->y0 + console->cy,
							console->sx - (console->cx + width), console->fy,
							console->bkcolor);
			console->cx = 0;
			console->cy += console->fy;
		}

		if((console->cy > console->sy)) {
#if GDISP_NEED_SCROLL
			/* scroll the console */
			gdispVerticalScroll(console->x0, console->y0, console->sx,
					console->sy + console->fy, console->fy, console->bkcolor);
			/* reset the cursor to the start of the line */
			console->cx = 0;
			console->cy = console->sy;
#else
			/* clear the console */
			gdispFillArea(console->x0, console->y0,
						console->sx, console->sy + console->fy,
						console->bkcolor);
			/* reset the cursor to the top of the console */
			console->cx = 0;
			console->cy = 0;
#endif
		}

		gdispDrawChar(console->x0 + console->cx, console->y0 + console->cy, c,
				console->font, console->color);

		/* update cursor */
		console->cx += width;
	}
	return RDY_OK;
}

msg_t lcdConsoleWrite(GLCDConsole *console, const uint8_t *bp, size_t n) {
	size_t i;
	for(i = 0; i < n; i++)
		lcdConsolePut(console, bp[i]);

	return RDY_OK;
}

#endif /* GDISP_NEED_CONSOLE */
