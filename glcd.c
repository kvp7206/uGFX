#include "glcd.h" 
#include "fonts.h"
#include <stdlib.h>
#include <math.h>

uint16_t lcd_width, lcd_height;
uint16_t bgcolor=White, fgcolor=Black;
uint16_t cx, cy;
static uint8_t tpText=0;
uint8_t* font;

void lcdInit(void) {
	lld_lcdInit();
	lcd_width = SCREEN_WIDTH;
	lcd_height = SCREEN_HEIGHT;

	lcdSetFont(font_MediumBold);
}

uint16_t lcdGetHeight(void) {
	return lld_lcdGetHeight();
}

uint16_t lcdGetWidth(void) {
	return lld_lcdGetWidth();
}

uint16_t lcdGetOrientation(void) {
	return lld_lcdGetOrientation();
}

static void lcdSetCursor(uint16_t x, uint16_t y) {
	lld_lcdSetCursor(x, y);
}

void lcdSetOrientation(uint8_t newOrientation) {
	lld_lcdSetOrientation(newOrientation);
}

void lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	lld_lcdSetWindow(x0, y0, x1, y1);
}

void lcdClear(uint16_t color) {
	lld_lcdClear(color);
}

uint16_t lcdGetPixelColor(uint16_t x, uint16_t y) {
	return lld_lcdGetPixelColor(x, y);
}

void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color) {
	lld_lcdDrawPixel(x, y, color);
}

void lcdDrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
   int16_t dy, dx;
   int16_t  addx=1, addy=1;
   int16_t P, diff;

   int16_t i=0;
   dx = abs((int16_t)(x2 - x1));
   dy = abs((int16_t)(y2 - y1));

   if(x1 > x2)
      addx = -1;
   if(y1 > y2)
      addy = -1;

   if(dx >= dy)
   {
      dy *= 2;
      P = dy - dx;
      diff = P - dx;

      for(; i<=dx; ++i)
      {
         lcdDrawPixel(x1, y1, color);

         if(P < 0)
         {
            P  += dy;
            x1 += addx;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
   else
   {
      dx *= 2;
      P = dx - dy;
      diff = P - dy;

      for(; i<=dy; ++i)
      {
         lcdDrawPixel(x1, y1, color);

         if(P < 0)
         {
            P  += dx;
            y1 += addy;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
}

void lcdSetFont(uint8_t *fnt) {
	font = fnt;
}

void lcdEnableTransparentText(uint8_t en) {
	tpText = en;
}

void lcdDrawChar(char c) {
	const uint8_t* ptr;
	uint8_t fontHeight = lcdGetCurFontHeight();
	uint8_t sps = font[FONT_TABLE_PAD_AFTER_CHAR_IDX];
	uint16_t chi;
	uint16_t x,y;

	// No support for nongraphic characters, so just ignore them
	if(c < 0x20 || c > 0x7F) {
		if(c=='\n')
			lcdLineBreak();
		return;
	}

	chi = *(uint16_t*)(&font[FONT_TABLE_CHAR_LOOKUP_IDX + (c-0x20)*2]);

	ptr = font + chi;

	uint8_t fontWidth = *(ptr++);

	if(cx + fontWidth > lcdGetWidth())
		lcdLineBreak();

	for(x = 0; x < fontWidth; x++) {
		chi = *(uint16_t*)ptr;
		for(y = 0; y < fontHeight; y++) {
			if(chi & 0x01)
				lcdDrawPixel(cx+x, cy+y, fgcolor);
			else if(!tpText)
				lcdDrawPixel(cx+x, cy+y, bgcolor);

			chi >>= 1;
		}
		ptr += 2;
	}

	cx += fontWidth;
	if(sps != 0) {
		if(!tpText)
			lcdFillArea(cx, cy, sps, fontHeight, fgcolor);
		cx += sps;
	}
}

void lcdPutString(const char *str) {
	while (*str) lcdDrawChar(*str++);
}

void lcdDrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bkcolor) {
	uint16_t _bg = bgcolor, _fg = fgcolor;
	cx = x;
	cy = y;
	bgcolor = bkcolor;
	fgcolor = color;
	lcdPutString(str);
	bgcolor = _bg;
	fgcolor = _fg;
}

uint16_t lcdMeasureChar(char c) {
	const uint8_t *ptr;

	// First get spaces after each character, usually 0 but can change
	uint8_t sps = font[FONT_TABLE_PAD_AFTER_CHAR_IDX];

	uint16_t chi;

	if(c < 0x20 || c > 0x7F)
		return 0;

	chi = *(uint16_t*)(&font[FONT_TABLE_CHAR_LOOKUP_IDX + (c-0x20)*2]);

	ptr = font + chi;

	uint8_t fontWidth = *(ptr++);

	return fontWidth + sps;
}

uint16_t lcdMeasureString(const char *str) {
	uint16_t result = 0;

	while (*str)result += lcdMeasureChar(*str++);

	return result;
}

void lcdLineBreak() {
	// x=0 seems too much on the edge. So I keep it at 3
	cx = 3;
	cy += lcdGetCurFontHeight();
}

uint16_t lcdBGR2RGB(uint16_t color) {
	uint16_t  r, g, b, rgb;

	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
 
	rgb =  (b<<11) + (g<<5) + (r<<0);

	return( rgb );
}

void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	lcdDrawRect(x0, y0, x1, y1, filled, color);
	/*
	uint32_t index = 0, area;

	area = ((x1-x0)*(y1-y0));

    lcdSetWindow(x0, y0, x1, y1);
	Clr_CS; 
    lcdWriteIndex(0x0022);
    for(index = 0; index < area; index++)
        lcdWriteData(color);
    Set_CS;
	*/
}

void lcdDrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t filled, uint16_t color) {
	uint16_t i, TempX;
	uint16_t j, TempY;

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
		for(i=x0; i<x1; i++)
			for(j=y0; j<y1; j++)
				lcdDrawPixel(i , j , color);
	} else {
		lcdDrawLine(x0, y0, x1, y0, color);
		lcdDrawLine(x0, y1, x1, y1, color);
		lcdDrawLine(x0, y0, x0, y1, color);
		lcdDrawLine(x1, y0, x1, y1, color);
	}
}

void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char *str, uint16_t fontColor, uint16_t bkColor) {
	uint16_t off_left, off_up;

	off_left = ((x1-x0)-lcdMeasureString(str))/2;
	off_up = ((y1-y0) - lcdGetCurFontHeight()) / 2;

	lcdDrawRect(x0, y0, x1, y1, 1, bkColor);

	lcdDrawString(x0+off_left, y0+off_up, str, fontColor, bkColor);
}

void lcdDrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color) {
	int16_t a, b, P;
	a = 0;
	b = radius;
	P = 1 - radius;

	do {
		if(filled) {
			lcdDrawLine(x-a, y+b, x+a, y+b, color);
			lcdDrawLine(x-a, y-b, x+a, y-b, color);
			lcdDrawLine(x-b, y+a, x+b, y+a, color);
			lcdDrawLine(x-b, y-a, x+b, y-a, color);
		} else {
			lcdDrawPixel(a+x, b+y, color);
			lcdDrawPixel(b+x, a+y, color);
			lcdDrawPixel(x-a, b+y, color);
			lcdDrawPixel(x-b, a+y, color);
			lcdDrawPixel(b+x, y-a, color);
			lcdDrawPixel(a+x, y-b, color);
			lcdDrawPixel(x-a, y-b, color);
			lcdDrawPixel(x-b, y-a, color);
		}

	if(P < 0)
		P += 3 + 2*a++;
	else
		P += 5 + 2*(a++ - b--);
	} while(a <= b);
}

