#include "glcd.h" 
#include "fonts.h"
#include <stdlib.h>
#include <math.h>

uint16_t lcd_width, lcd_height;

void lcdInit(GLCDDriver *glcdp) {

	lld_lcdInit();
	lcd_width = lcdGetWidth();
	lcd_height = lcdGetHeight();

	lcdSetPowerMode(powerOn);
	lcdSetOrientation(portrait);
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

void lcdSetPowerMode(uint8_t powerMode) {
	lld_lcdSetPowerMode(powerMode);
}

void lcdSetOrientation(uint8_t newOrientation) {
	lld_lcdSetOrientation(newOrientation);
}

void lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	lld_lcdSetWindow(x0, y0, x1, y1);
}

void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	lld_lcdFillArea(x0, y0, x1, y1, color);
}

void lcdWriteArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *buffer, size_t n) {
	lld_lcdSetWindow(x0, y0, x1, y1);

	lld_lcdWriteStreamStart();
	lld_lcdWriteStream(buffer, n);
	lld_lcdWriteStreamStop();
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

static void lcdWriteStreamStart(void) {
	lld_lcdWriteStreamStart();
}

static void lcdWriteStreamStop(void) {
	lld_lcdWriteStreamStop();
}

static void lcdWriteStream(uint16_t *buffer, uint16_t size) {
	lld_lcdWriteStream(buffer, size);
}

void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	int16_t dy, dx;
	int16_t addx = 1, addy = 1;
	int16_t P, diff;

	int16_t i = 0;
	dx = abs((int16_t)(x1 - x0));
	dy = abs((int16_t)(y1 - y0));

	if(x0 > x1)
		addx = -1;
	if(y0 > y1)
		addy = -1;

	if(dx >= dy) {
		dy *= 2;
		P = dy - dx;
		diff = P - dx;

		for(; i<=dx; ++i) {
			lcdDrawPixel(x0, y0, color);
			if(P < 0) {
				P  += dy;
				x0 += addx;
			} else {
				P  += diff;
				x0 += addx;
				y0 += addy;
			}
		}
	} else {
		dx *= 2;
		P = dx - dy;
		diff = P - dy;

		for(; i<=dy; ++i) {
			lcdDrawPixel(x0, y0, color);
			if(P < 0) {
				P  += dx;
				y0 += addy;
			} else {
				P  += diff;
				x0 += addx;
				y0 += addy;
			}
		}
	}
}

int lcdDrawChar(uint16_t cx, uint16_t cy, char c, font_t font, uint16_t color, uint16_t bkcolor, bool_t tpText) {
	/* Working pointer */
	const uint8_t* ptr;
	uint8_t x, y;
	
	/* Variables to store character details */
	uint8_t charWidth;
	uint8_t charHeight = lcdGetFontHeight(font);
	uint8_t padAfterChar = font[FONT_TABLE_PAD_AFTER_CHAR_IDX];
	
	/* Local var to hold offset in font table */
	uint16_t charStartOffset;

	/* Working buffer for fast non-transparent text rendering [patch by Badger] */
	static uint16_t buf[20*16];

	/* No support for nongraphic characters, so just ignore them */
	if(c < 0x20 || c > 0x7F) {
		return RDY_OK;
	}

	/* Read the offset of the character data in the font table from the lookup table */
	charStartOffset = *(uint16_t*)(&font[FONT_TABLE_CHAR_LOOKUP_IDX + (c - 0x20) * 2]);

	/* After we're done, position the pointer at the offset.
	 * The first byte that is immediately read will be the font width 
	 * After that, actual 16-bit font data follows, first column down */
	ptr = font + charStartOffset;
	charWidth = *(ptr++);

	/* Loop through the data and display. The font data is LSB first, down the column */
	for(x = 0; x < charWidth; x++) {
		/* Get the font bitmap data for the column */
		uint16_t charData = *(uint16_t*)ptr;
		
		for(y = 0; y < charHeight; y++) {
			/* Draw the LSB on the screen accordingly. */
			if(!tpText) {
				/* Store data into working buffer (patch by Badger),
				 * Then write it all onto the LCD in one stroke */
				buf[y*charWidth + x] = (charData & 0x01) ? color : bkcolor;
			} else {
				/* Just draw the needed pixels onto the LCD */
				if (charData & 0x01)
					lcdDrawPixel(cx+x, cy+y, color);
			}
			
			/* Shift the data down by one bit */	
			charData >>= 1;
		}
		
		/* Increment pointer by 2 bytes to the next column */
		ptr += 2;
	}

	if(!tpText) {
		/* [Patch by Badger] Write all in one stroke */
		lcdWriteArea(cx, cy, cx+charWidth, cy+charHeight, buf, charWidth*charHeight);
		
		/* Do padding after character, if needed for solid text rendering
		 * TODO: To be optimised */
		if (padAfterChar != 0) {
			lcdFillArea(cx+charWidth, cy+charHeight, cx+charWidth+padAfterChar, cy+charHeight, bkcolor);
		}
	}

	/* Return the width of the character, we need it so that lcdDrawString may work
	 * We don't have a static address counter */
	return charWidth + padAfterChar;
}

/* WARNING: No boundary checks! Unpredictable behaviour if text exceeds boundary */
void lcdDrawString(uint16_t x, uint16_t y, const char *str, font_t font, uint16_t color, uint16_t bkcolor, bool_t tpText) {
	uint16_t cx=x, cy=y;
	
	while (*str) {
		cx += lcdDrawChar(cx, cy, *str++, font, color, bkcolor, tpText);
	}
}

uint16_t lcdMeasureChar(char c, font_t font) {
	/* Variables to store character details */
	uint8_t charWidth;
	uint8_t padAfterChar = font[FONT_TABLE_PAD_AFTER_CHAR_IDX];
	
	/* Local var to hold offset in font table */
	uint16_t charStartOffset;

	/* No support for nongraphic characters, so just ignore them */
	if(c < 0x20 || c > 0x7F) {
		return 0;
	}

	/* Read the offset of the character data in the font table from the lookup table */
	charStartOffset = *(uint16_t*)(&font[FONT_TABLE_CHAR_LOOKUP_IDX + (c - 0x20) * 2]);

	/* Retrurn the byte at the offset, that's our charWidth */
	charWidth = *(font + charStartOffset);
	
	return charWidth+padAfterChar;
}

uint16_t lcdMeasureString(const char *str, font_t font) {
	uint16_t result = 0;

	/* Measure each char width, add it, return the result */
	while (*str)
		result += lcdMeasureChar(*str++, font);

	return result;
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

void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, font_t font, uint16_t fontColor, uint16_t bkColor) {
	uint16_t off_left, off_up;

	off_left = ((x1-x0)-lcdMeasureString(str, font))/2;
	off_up = ((y1-y0) - lcdGetFontHeight(font)) / 2;

	lcdDrawRect(x0, y0, x1, y1, 1, bkColor);
	/* Abhishek: default to solid text for this? */
	lcdDrawString(x0+off_left, y0+off_up, str, font, fontColor, bkColor, solid);
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

void lcdDrawEllipse(uint16_t x, uint16_t y, uint16_t a, uint16_t b, uint8_t filled, uint16_t color) {
	int dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
	long a2 = a*a, b2 = b*b;
	long err = b2-(2*b-1)*a2, e2; /* Fehler im 1. Schritt */

	do {
        if(filled){
            lcdDrawLine(x-dx,y+dy,x+dx,y+dy, color);
            lcdDrawLine(x-dx,y-dy,x+dx,y-dy, color);
        }else{
            lcdDrawPixel(x+dx, y+dy, color); /* I. Quadrant */
            lcdDrawPixel(x-dx, y+dy, color); /* II. Quadrant */
            lcdDrawPixel(x-dx, y-dy, color); /* III. Quadrant */
            lcdDrawPixel(x+dx, y-dy, color); /* IV. Quadrant */
        }

		e2 = 2*err;
		if(e2 <  (2*dx+1)*b2) {
			dx++;
			err += (2*dx+1)*b2;
		}
		if(e2 > -(2*dy-1)*a2) {
			dy--;
			err -= (2*dy-1)*a2;
		}
	} while(dy >= 0); 

	while(dx++ < a) { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */
		lcdDrawPixel(x+dx, y, color); /* -> Spitze der Ellipse vollenden */
		lcdDrawPixel(x-dx, y, color);
   }   
}

void lcdVerticalScroll(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t lines) {
	lld_lcdVerticalScroll(x0,y0,x1,y1,lines);
}
