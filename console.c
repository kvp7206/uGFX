/*
 * console.c
 *
 *  Created on: 20 Jun 2012
 *      Author: Thomas Saunders AKA "Badger"
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


msg_t lcdConsoleInit(GLCDConsole *console, uint16_t x0, uint16_t y0, uint16_t x1,
		uint16_t y1, font_t font, uint8_t *buffer, uint16_t bkcolor, uint16_t color) {
	const uint8_t* ptr;
	uint16_t chi;
	uint16_t x,y;

	console->vmt = &vmt;
	/* read font, get height */
	console->fy = font[FONT_TABLE_HEIGHT_IDX];


	/* calculate the size of the console in characters */
	console->sx = (x1-x0);
	console->sy = (((int16_t)((y1-y0)/console->fy))-1)*console->fy;

	console->cx = 0;
	console->cy = 0;
	console->x0 = x0;
	console->y0 = y0;

	console->buf = buffer;
	console->wptr = 0;
	console->blen = console->sx*console->sy;
	console->bstrt = 0;

	console->bkcolor = bkcolor;
	console->color = color;

	console->font = font;
}

msg_t lcdConsolePut(GLCDConsole *console, char c) {
	uint8_t width;
	bool_t newline = FALSE;


	if(c == '\n') {
		/* clear the text at the end of the line */
		if(console->cx < console->sx)
			lcdDrawRect(console->cx, console->cy, console->sx, console->cy + console->fy,
					1, console->bkcolor);
		console->cx = 0;
		console->cy += console->fy;
	} else if(c == '\r') {
		/* TODO: work backwards through the buffer to the start of the current line */
		//console->cx = 0;
	} else {
		width = lcdMeasureChar(c, console->font);
		if((console->cx + width) >= console->sx) {
			chprintf(&SD1, "[1] ");
			console->cx = 0;
			console->cy += console->fy;
		}

		if(
				(console->cy > console->sy)) {
			chprintf(&SD1, "[2] ");
			if(newline)
				chprintf(&SD1, "* ");
			lcdVerticalScroll(console->x0, console->y0, console->x0 + console->sx,
					console->y0 + console->sy, console->fy);
			/* reset the cursor */
			console->cx = 0;
			console->cy = console->sy;
		} else if(newline) {
			chprintf(&SD1, "[3] ");
			console->cy += console->fy;
		}
		//chprintf(&SD1, "'%c' at [%d, %d]\r\n", c, console->x0 + console->cx, console->y0 + console->cy);
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


