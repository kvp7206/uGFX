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
#include "hal.h"

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

/* Hack 1: Manually load the low level driver capabilities first so we can
 * 			control what we implement with hardware scrolling and pixel read.
 */
#include "gdisp_lld_config.h"

/* Hack 2: Force on the bits of functionality that glcd needs. */
#define HAL_USE_GDISP			TRUE
#define GDISP_NEED_VALIDATION	FALSE		/* The original glcd didn't so we don't here either */
#define GDISP_NEED_CIRCLE		TRUE
#define GDISP_NEED_ELLIPSE		TRUE
#define GDISP_NEED_TEXT			TRUE
#define GDISP_NEED_SCROLL		GDISP_HARDWARE_SCROLL
#define GDISP_NEED_PIXELREAD	GDISP_HARDWARE_PIXELREAD
#define GDISP_NEED_CONTROL		TRUE
#define GDISP_NEED_MULTITHREAD	FALSE		/* We implement multi-thread here */

/* Now load the low level driver and font structure definitions */
#include "gdisp_lld.h"
#include "gdisp_fonts.h"

/* Hack 3: GLCD only supports RGB565. Anything else would require significant API changes. */
#ifndef GDISP_PIXELFORMAT_RGB565
#error "GLCD only support drivers with RGB565 pixel format"
#endif

/* Now load the glcd headers */
#include "glcd.h"
#include "glcdWorker.h"

/* Hack 4: Include the emulation code and font tables.
 * 			We have to include it here rather than compiling
 * 			the files as we need to pass our control defines
 * 			as they are not being defined by the application.
 * 			We need to turn off the inclusion of gdisp.h due
 * 			to conflicting type defines etc.
 */
#define _GDISP_H						// Prevent gdisp.h from being included
#include "gdisp_emulation.c"
#include "gdisp_fonts.c"

#define EMSG(a)	struct a *emsg = (struct a*)msg

static Thread *workerThread = NULL;

static WORKING_AREA(waGLCDWorkerThread, GLCD_WORKER_SIZE);
static msg_t ThreadGLCDWorker(void *arg) {
	(void)arg;
	Thread *p;
 
	chRegSetThreadName("GLCDWorker");
 
	while(TRUE) {
		/* Wait for msg with work to do. */
		p = chMsgWait();
		struct glcd_msg_base *msg = (struct glcd_msg_base*)chMsgGet(p);
		glcd_result_t result = GLCD_PROGRESS;
 
		/* do work here */
		switch(msg->action) {
			case GLCD_SET_POWERMODE: {
				EMSG(glcd_msg_powermode);
				gdisp_lld_control(GDISP_CONTROL_POWER, (void *)(int)emsg->powermode);
				result = GLCD_DONE;
				break;
			}

			case GLCD_SET_ORIENTATION: {
				EMSG(glcd_msg_orientation);
				gdisp_lld_control(GDISP_CONTROL_ORIENTATION, (void *)(int)emsg->newOrientation);
				result = GLCD_DONE;
				break;
			}
			case GLCD_FILL_AREA: {
				EMSG(glcd_msg_fill_area);
				gdisp_lld_fillarea(emsg->x0, emsg->y0, emsg->x1-emsg->x0+1,emsg->y1-emsg->y0+1,emsg->color);
				result = GLCD_DONE;
				break;
			}

			case GLCD_WRITE_AREA: {
				EMSG(glcd_msg_write_area);
				gdisp_lld_blitarea(emsg->x0, emsg->y0, emsg->x1-emsg->x0+1, emsg->y1-emsg->y0+1, emsg->buffer);
				result = GLCD_DONE;
				break;
			}

			case GLCD_CLEAR: {
				EMSG(glcd_msg_clear);
				gdisp_lld_clear(emsg->color);
				result = GLCD_DONE;
				break;
			}

			case GLCD_GET_PIXEL_COLOR: {
				/* Hack 5: This may now fail if the low level driver doesn't
				 * 			support it. Previously this support was required
				 * 			in the driver by GLCD
				 */
#if GDISP_HARDWARE_READPIXEL
				((struct glcd_msg_get_pixel_color *)emsg)->color =
						gdisp_lld_getpixelcolor(emsg->x, emsg->y);
				result = GLCD_DONE;
#else
				EMSG(glcd_msg_get_pixel_color);
				((struct glcd_msg_get_pixel_color *)emsg)->color = 0;
				result = GLCD_FAILED;
#endif
				break;				
			}

			case GLCD_DRAW_PIXEL: {
				EMSG(glcd_msg_draw_pixel);
				gdisp_lld_drawpixel(emsg->x, emsg->y, emsg->color);
				result = GLCD_DONE;
				break;
			}

			case GLCD_VERTICAL_SCROLL: {
				/* Hack 6: This may now fail if the low level driver doesn't
				 * 			support it. Previously this support was required
				 * 			in the driver by GLCD
				 */
#if GDISP_HARDWARE_SCROLL
				EMSG(glcd_msg_vertical_scroll);
				gdisp_lld_verticalscroll(emsg->x0, emsg->y0, emsg->x1-emsg->x0+1, emsg->y1-emsg->y0+1, emsg->lines, 0);
				result = GLCD_DONE;
#else
				result = GLCD_FAILED;
#endif
				break;
			}

			case GLCD_DRAW_CHAR: {
				EMSG(glcd_msg_draw_char);
				if (emsg->tpText)
					gdisp_lld_drawchar(emsg->cx, emsg->cy, emsg->c, emsg->font, emsg->color);
				else
					gdisp_lld_fillchar(emsg->cx, emsg->cy, emsg->c, emsg->font, emsg->color, emsg->bkcolor);
				/* We can't normally access a high level function here but in this case it is safe
				 * because the routine only accesses const data members (multi-thread safe).
				 */
				emsg->ret_width = lcdMeasureChar(emsg->c, emsg->font);
				result = GLCD_DONE;
				break;
			}

			default: {
				result = GLCD_FAILED;
				break;
			}
		}

		/* Done, release msg again. */
		chMsgRelease(p, (msg_t)result);

	}
 
	return 0;
}

void lcdInit(GLCDDriver *UNUSED(glcdp)) {
	workerThread = chThdCreateStatic(waGLCDWorkerThread, sizeof(waGLCDWorkerThread), NORMALPRIO, ThreadGLCDWorker, NULL);

	gdisp_lld_init();
}

uint16_t lcdGetHeight(void) {
	return GDISP.Height;
}

uint16_t lcdGetWidth(void) {
	return GDISP.Width;
}

uint16_t lcdGetOrientation(void) {
	return GDISP.Orientation;
}

glcd_result_t lcdSetPowerMode(uint8_t powerMode) {
	struct glcd_msg_powermode msg;

	msg.action = GLCD_SET_POWERMODE;
	msg.powermode = powerMode;

	return (glcd_result_t)chMsgSend(workerThread, (msg_t)&msg);
}

glcd_result_t lcdSetOrientation(uint8_t newOrientation) {
	struct glcd_msg_orientation msg;

	msg.action = GLCD_SET_ORIENTATION;
	msg.newOrientation = newOrientation;

	return (glcd_result_t)chMsgSend(workerThread, (msg_t)&msg);
}

glcd_result_t lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	struct glcd_msg_fill_area msg;

	msg.action = GLCD_FILL_AREA;
	msg.x0 = x0;
	msg.y0 = y0;
	msg.x1 = x1;
	msg.y1 = y1;
	msg.color = color;

	return (glcd_result_t)chMsgSend(workerThread, (msg_t)&msg);
}

glcd_result_t lcdWriteArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *buffer, size_t n) {
	struct glcd_msg_write_area msg;

	msg.action = GLCD_WRITE_AREA;
	msg.x0 = x0;
	msg.y0 = y0;
	msg.x1 = x1;
	msg.y1 = y1;
	msg.buffer = buffer;
	msg.size = n;

	return (glcd_result_t)chMsgSend(workerThread, (msg_t)&msg);
}

glcd_result_t lcdClear(uint16_t color) {
	struct glcd_msg_clear msg;

	msg.action = GLCD_CLEAR;
	msg.color = color;

	return (glcd_result_t)chMsgSend(workerThread, (msg_t)&msg);
}

uint16_t lcdGetPixelColor(uint16_t x, uint16_t y) {
	struct glcd_msg_get_pixel_color msg;

	msg.action = GLCD_GET_PIXEL_COLOR;
	msg.x = x;
	msg.y = y;

	chMsgSend(workerThread, (msg_t)&msg);

	return msg.color;
}

glcd_result_t lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color) {
	struct glcd_msg_draw_pixel msg;
 
	msg.action = GLCD_DRAW_PIXEL;
	msg.x = x;
	msg.y = y;
	msg.color = color;

	return (glcd_result_t)chMsgSend(workerThread, (msg_t)&msg);
}

glcd_result_t lcdVerticalScroll(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t lines) {
	struct glcd_msg_vertical_scroll msg;

	msg.action = GLCD_VERTICAL_SCROLL;
	msg.x0 = x0;
	msg.y0 = y0;
	msg.x1 = x1;
	msg.y1 = y1;
	msg.lines = lines;

	return (glcd_result_t)chMsgSend(workerThread, (msg_t)&msg);
}

void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	struct glcd_msg_draw_line msg;

	msg.action = GLCD_DRAW_LINE;
	msg.x0 = x0;
	msg.y0 = y0;
	msg.x1 = x1;
	msg.y1 = y1;
	msg.color = color;

	chMsgSend(workerThread, (msg_t)&msg);
}

uint16_t lcdDrawChar(uint16_t cx, uint16_t cy, char c, font_t font, uint16_t color, uint16_t bkcolor, bool_t tpText) {
	struct glcd_msg_draw_char msg;

	msg.action = GLCD_DRAW_CHAR;
	msg.cx = cx;
	msg.cy = cy;
	msg.c = c;
	msg.font = font;
	msg.color = color;
	msg.bkcolor = bkcolor;
	msg.tpText = tpText;
	msg.ret_width = 0;

	chMsgSend(workerThread, (msg_t)&msg);

	return msg.ret_width;
}

void lcdDrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color) {
	struct glcd_msg_draw_circle msg;

	msg.action = GLCD_DRAW_CIRCLE;
	msg.x = x;
	msg.y = y;
	msg.radius = radius;
	msg.filled = filled;
	msg.color = color;

	chMsgSend(workerThread, (msg_t)&msg);
}

void lcdDrawEllipse(uint16_t x, uint16_t y, uint16_t a, uint16_t b, uint8_t filled, uint16_t color) {
	struct glcd_msg_draw_ellipse msg;

	msg.action = GLCD_DRAW_ELLIPSE;
	msg.x = x;
	msg.y = y;
	msg.a = a;
	msg.b = b;
	msg.filled = filled;
	msg.color = color;

	chMsgSend(workerThread, (msg_t)&msg);
}

/* WARNING: No boundary checks! Unpredictable behaviour if text exceeds boundary */
void lcdDrawString(uint16_t x, uint16_t y, const char *str, font_t font, uint16_t color, uint16_t bkcolor, bool_t tpText) {
	uint16_t cx = x, cy = y;
	
	while (*str) {
		cx += lcdDrawChar(cx, cy, *str++, font, color, bkcolor, tpText);
	}
}

uint16_t lcdMeasureChar(char c, font_t font) {
	return (_getCharWidth(font, c)+font->charPadding) * font->xscale;
}

uint16_t lcdMeasureString(const char *str, font_t font) {
	uint16_t result = 0;

	/* Measure each char width, add it, return the result */
	while (*str)
		result += lcdMeasureChar(*str++, font);

	return result;
}

uint16_t lcdGetFontHeight(font_t font) {
	return font->height;
}

uint16_t lcdBGR2RGB(uint16_t color) {
	uint16_t  r, g, b, rgb;

	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
 
	rgb =  (b<<11) + (g<<5) + (r<<0);

	return( rgb );
}

void lcdDrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t filled, uint16_t color) {
	uint16_t TempX;
	uint16_t TempY;

	if (x0 > x1) {
		TempX = x1;
		x1 = x0;
		x0 = TempX;
	}
	if (y0 > y1) {
		TempY = y1;
		y1 = y0;
		y0 = TempY;
	}
	if(filled) {
		lcdFillArea(x0, y0, x1+1, y1+1, color);
	} else {
		lcdDrawLine(x0, y0, x1, y0, color);
		lcdDrawLine(x0, y1, x1, y1, color);
		lcdDrawLine(x0, y0, x0, y1, color);
		lcdDrawLine(x1, y0, x1, y1+1, color);
	}
}

void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, font_t font, uint16_t fontColor, uint16_t bkColor) {
	uint16_t off_left, off_up;

	off_left = ((x1-x0)-lcdMeasureString(str, font))/2;
	off_up = ((y1-y0) - lcdGetFontHeight(font)) / 2;

	lcdDrawRect(x0, y0, x1, y1, filled, bkColor);
	/* Abhishek: default to solid text for this? */
	lcdDrawString(x0+off_left, y0+off_up, str, font, fontColor, bkColor, solid);
}
