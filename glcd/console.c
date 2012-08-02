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

#include "ch.h"

#include "fonts.h"
#include "glcd.h"
#include "console.h"

/*
 * Interface implementation. The interface is write only
 */

static size_t writes(void *ip, const uint8_t *bp, size_t n) {
	return lcdConsoleWrite((GLCDConsole *)ip, bp, n);
}

static size_t reads(void *ip, uint8_t *bp, size_t n) {
	(void)ip;
	(void)bp;
	(void)n;

	return 0;
}

static msg_t put(void *ip, uint8_t b) {
	return lcdConsolePut((GLCDConsole *)ip, (char)b);
}

static msg_t get(void *ip) {
	(void)ip;

	return RDY_OK;
}

static msg_t putt(void *ip, uint8_t b, systime_t timeout) {
	(void)timeout;

	/* TODO: handle timeout */

	return lcdConsolePut((GLCDConsole *)ip, (char)b);
}

static msg_t gett(void *ip, systime_t timeout) {
	(void)ip;
	(void)timeout;

	return RDY_OK;
}

static size_t writet(void *ip, const uint8_t *bp, size_t n, systime_t time) {
	(void)time;

	return lcdConsoleWrite((GLCDConsole *)ip, bp, n);
}

static size_t readt(void *ip, uint8_t *bp, size_t n, systime_t time) {
	(void)ip;
	(void)bp;
	(void)n;
	(void)time;

	return 0;
}

static chnflags_t getflags(void *ip) {
	_chn_get_and_clear_flags_impl(ip);
}

static const struct GLCDConsoleVMT vmt = {
	writes, reads, put, get,
	putt, gett, writet, readt,
	getflags
};


msg_t lcdConsoleInit(GLCDConsole *console, uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, font_t font, uint16_t bkcolor, uint16_t color) {
	const uint8_t* ptr;
	uint16_t chi;
	uint16_t x,y;

	console->vmt = &vmt;
	/* read font, get height */
	console->fy = font[FONT_TABLE_HEIGHT_IDX];

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

	lcdFillArea(x0, y0, x0+width, y0+height, console->bkcolor);
}

msg_t lcdConsolePut(GLCDConsole *console, char c) {
	uint8_t width;

	if(c == '\n') {
		/* clear the text at the end of the line */
		if(console->cx < console->sx)
			lcdFillArea(console->x0 + console->cx, console->y0 + console->cy,
						console->x0 + console->sx, console->y0 + console->cy + console->fy,
						console->bkcolor);
		console->cx = 0;
		console->cy += console->fy;
	} else if(c == '\r') {
		/* TODO: work backwards through the buffer to the start of the current line */
		//console->cx = 0;
	} else {
		width = lcdMeasureChar(c, console->font);
		if((console->cx + width) >= console->sx) {
			/* clear the text at the end of the line */
			lcdFillArea(console->x0 + console->cx, console->y0 + console->cy,
						console->x0 + console->cx + width, console->y0 + console->cy + console->fy,
						console->bkcolor);
			console->cx = 0;
			console->cy += console->fy;
		}

		if((console->cy > console->sy)) {

			lcdVerticalScroll(console->x0, console->y0, console->x0 + console->sx,
					console->y0 + console->sy + console->fy, console->fy);
			/* reset the cursor */
			console->cx = 0;
			console->cy = console->sy;
		}

		lcdDrawChar(console->x0 + console->cx, console->y0 + console->cy, c,
				console->font, console->color, console->bkcolor, solid);

		/* update cursor */
		console->cx += width;
	}

}

msg_t lcdConsoleWrite(GLCDConsole *console, uint8_t *bp, size_t n) {
	size_t i;
	for(i = 0; i < n; i++)
		lcdConsolePut(console, bp[i]);

	return RDY_OK;
}


