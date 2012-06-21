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
	/* read font, get size */
	/* TODO: this is messy. improve font handling.
	 * this assumes that all characters are as wide as A */
	console->fy = font[FONT_TABLE_HEIGHT_IDX];
	chi = *(uint16_t*)(&font[FONT_TABLE_CHAR_LOOKUP_IDX + ('A'-0x20)*2]);
	ptr = font + chi;
	console->fx = *(ptr++);

	/* calculate the size of the console in characters */
	console->sx = (x1-x0)/console->fx;
	console->sy = (y1-y0)/console->fy;

	console->cx = 0;
	console->cy = 0;
	console->x0 = x0;
	console->y0 = y0;

	console->buf = buffer;
	console->bidx = 0;
	console->blen = console->sx*console->sy;
	console->bstrt = 0;

	console->bkcolor = bkcolor;
	console->color = color;

	console->font = font;
	console->full = FALSE;
}

msg_t lcdConsoleUpdate(GLCDConsole *console) {

}

msg_t lcdConsolePut(GLCDConsole *console, char c) {

	if(console->full) {
		return RDY_RESET;
	}

	bool_t redraw = FALSE;
	/* write character to current position in buffer */
	console->buf[console->bidx++] = c;

	if(console->bidx == console->blen) {
		/* wrap around to the beginning */
		console->bidx = 0;
	}

	if(c == '\n') {
		console->cx = 0;
		console->cy++;
		redraw = TRUE;
	} else if(c == '\r') {
		console->cx = 0;
	} else {
		console->cx++;
	}

	if(console->cx > console->sx) {
		/* character has extended beyond end of line */
		redraw = TRUE;
	}

	if(redraw && console->cy == console->sy) {

		/* we've gone past the end of the console */
		console->full = TRUE;
		return RDY_RESET;
		/* start at beginning of buffer and remove the first line */
		uint16_t i;

		/* increment i from bstrt until it has been incremented more than console->sx or finds a new line */
		for(i = console->bstrt; ((i - console->bstrt) < console->sx) && (console->buf[i % console->sx] != '\n'); i++);

		/* update bstrt to the new start point of the console */
		console->bstrt = i;
	} else {
		/* just draw the character at the current position */
		lcdMoveCursor(console->x0 + console->cx*console->fx, console->y0 + console->cy*console->fy,
				console->color, console->bkcolor);

		lcdSetFont(console->font);
		lcdDrawChar(c);
	}
}

msg_t lcdConsoleWrite(GLCDConsole *console, uint8_t *bp, size_t n) {
	size_t i;
	for(i = 0; i < n; i++)
		lcdConsolePut(console, bp[i]);

	return RDY_OK;
}


