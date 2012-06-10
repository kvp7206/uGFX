#include "glcd.h" 
#include "fonts.h"
#include <math.h>

extern uint8_t orientation;
uint16_t DeviceCode;
uint8_t font_width = 8, font_height = 16;
uint16_t lcd_width, lcd_height;

uint16_t lcdGetHeight(void) {
	return lld_lcdGetHeight();
}

uint16_t lcdGetWidth(void) {
	return lld_lcdGetWidth();
}

static void lcdSetCursor(uint16_t x, uint16_t y) {
	lld_lcdSetCursor(x, y);
}

void lcdSetOrientation(uint8_t newOrientation) {
	lld_lcdSetOrientation(newOrientation);
}

uint16_t lcdGetOrientation(void) {
	return orientation;
}

void lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	lld_lcdSetWindow(x0, y0, x1, y1);
}

void lcdClear(uint16_t color) {
	lld_lcdClear(color);
}

uint16_t lcdGetPixelColor(uint16_t x, uint16_t y) {
	lld_lcdGetPixelColor(x, y);
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

void lcdDrawChar(uint16_t x, uint16_t y, unsigned char c, uint16_t charcolor, uint16_t bkcolor) {
	uint16_t i = 0;
	uint16_t j = 0;
	unsigned char buffer[16];
	unsigned char tmp_char=0;

	GetASCIICode(buffer,c);

	for (i=0;i<16;i++) {
		tmp_char=buffer[i];
		for (j=0;j<8;j++) {
			if (((tmp_char >> (7-j)) & 0x01) == 0x01)
				lcdDrawPixel(x+j,y+i,charcolor);
			else
				lcdDrawPixel(x+j,y+i,bkcolor);
		}
	}
}

void lcdDrawString(uint16_t x, uint16_t y, unsigned char *str, uint16_t color, uint16_t bkcolor) {
	uint8_t TempChar;

	do {
		TempChar = *str++;  
		lcdDrawChar(x, y, TempChar, color, bkcolor);    
		if(x<232) {
			x+=8;
		} else if(y<304) {
			x=0;
			y+=16;
		} else {
			x=0;
			y=0;
		}    
	} while(*str != 0);
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

void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t *str, uint16_t fontColor, uint16_t bkColor) {
	if(((strlen(str)*8) < (x1-x0)) && ((y1-y0) > font_height)) {
		uint16_t off_left, off_up;

		off_left = ((x1-x0) - (strlen(str) * font_width)) / 2;
		off_up = ((y1-y0) - font_height) / 2;

		lcdDrawRect(x0, y0, x1, y1, 1, bkColor);
		lcdDrawString(x0 + off_left, y0 + off_up, str, fontColor, bkColor);
	}
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

void lcdTest(void) {
	uint16_t x;
	uint8_t y;
	uint8_t r,g,b;

	for(y = 0; y < lcd_height; y++) {
		for(x = 0; x < lcd_width; x++) {
			r = x + y;
			g = x - y;
			b = y - x;
			lcdDrawPixel(y, x, RGB565CONVERT(r,g,b));
		}
	}
}

void lcdInit() {
	lld_lcdInit();

	lcd_height = SCREEN_HEIGHT;
	lcd_width = SCREEN_WIDTH;
}

