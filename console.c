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
		uint16_t y1, const uint8_t *font, uint8_t *buffer, uint16_t bkcolor, uint16_t color) {
	const uint8_t* ptr;
	uint16_t chi;
	uint16_t x,y;

	console->vmt = &vmt;
	/* read font, get height */
	console->fy = font[FONT_TABLE_HEIGHT_IDX];


	/* calculate the size of the console in characters */
	console->sx = (x1-x0);
	console->sy = (y1-y0);

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

msg_t lcdConsoleUpdate(GLCDConsole *console) {

}

msg_t lcdConsolePut(GLCDConsole *console, char c) {
	uint16_t i;
	uint16_t s = console->wptr;
	uint8_t width;
	bool_t redraw = FALSE;

	/* write character to current position in buffer and update wptr */
	console->buf[console->wptr] = c;

	if(++console->wptr == console->blen) {
		/* wrap around to the beginning */
		console->wptr = 0;
	}

	lcdSetFont(console->font);
	lcdSetFontTransparency(solid);
	/* keep looping until we've finished writing
	 * we may write more than one character if the console needs to be re-drawn
	 * at the end of the loop leave the cursor set to the position for the next character
	 * checks to see if this is out of range will be performed at the start of that character
	 */
	do {
		width = lcdMeasureChar(console->buf[s]);
		if(console->buf[s] == '\n') {
			/* clear the text at the end of the line */
			if(console->cx < console->sx)
				lcdDrawRect(console->cx, console->cy, console->sx, console->cy + console->fy,
						1, console->bkcolor);
			console->cx = 0;
			console->cy += console->fy;
		} else if(console->buf[s] == '\r') {
			/* TODO: work backwards through the buffer to the start of the current line */
			//console->cx = 0;
		} else {
			if((console->cx + width) >= console->sx) {
				console->cx = 0;
				console->cy += console->fy;
			}

			if((console->cy + console->fy) >= console->sy) {
				/* we've gone beyond the end of the console */
				/* start at beginning of buffer and remove the first line */

				/* increment s from bstrt until it has been incremented more than
				 * console->sx or finds a new line */
				s = console->bstrt;
				console->cx = 0;

				while((console->cx <= console->sx) && (console->buf[s % console->blen] != '\n')) {
					s++;
					/* TODO: increment based on the width of the character at s */
					/* TODO: this doesn't handle carriage return */
					console->cx += lcdMeasureChar(console->buf[s % console->blen]);
				}

				/* update bstrt to the new start point of the console */
				console->bstrt = s;

				/* reset the cursor */
				console->cx = 0;
				console->cy = 0;
			}
			lcdMoveCursor(console->x0 + console->cx, console->y0 + console->cy,
					console->color, console->bkcolor);
			lcdDrawChar(console->buf[s]);

			/* update cursor */
			console->cx += width;
		}

		/* finally increment index */
		if(++s == console->blen)
			s = 0;

	} while(s != console->wptr);

}

msg_t lcdConsoleWrite(GLCDConsole *console, uint8_t *bp, size_t n) {
	size_t i;
	for(i = 0; i < n; i++)
		lcdConsolePut(console, bp[i]);

	return RDY_OK;
}


