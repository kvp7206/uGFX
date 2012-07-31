/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*
   Concepts and parts of this file have been contributed by:
		Joel Bodenmann aka Tectu	-> Maintainer
		Andrew Hannam aka inmarket	-> Framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/*
	Emulation routines included into gdisp.c
*/

/*
	Even though this is a software emulation of a low level driver
	most validation doesn't need to happen here as eventually
	we call a real low level driver routine and if validation is
	required - it will do it.
*/

#if !GDISP_HARDWARE_CLEARS 
	void gdisp_lld_clear(color_t color) {
		gdisp_lld_fillarea(0, 0, GDISP1.Width, GDISP1.Height, color);
	}
#endif

#if !GDISP_HARDWARE_LINES 
	void gdisp_lld_drawline(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		int16_t dy, dx;
		int16_t addx, addy;
		int16_t P, diff, i;

		#if GDISP_HARDWARE_FILLS || GDISP_HARDWARE_SCROLL
		// speed improvement if vertical or horizontal
		if (x0 == x1) {
			if (y1 > y0)
				gdisp_lld_fillarea(x0, y0, 1, y1-y0+1, color);
			else
				gdisp_lld_fillarea(x0, y1, 1, y0-y1+1, color);
			return;
		}
		if (y0 == y1) {
			if (x1 > x0)
				gdisp_lld_fillarea(x0, y0, x1-x0+1, 1, color);
			else
				gdisp_lld_fillarea(x0, y1, x0-x1+1, 1, color);
			return;
		}
		#endif

		if (x1 >= x0) {
			dx = x1 - x0;
			addx = 1;
		} else {
			dx = x0 - x1;
			addx = -1;
		}
		if (y1 >= y0) {
			dy = y1 - y0;
			addy = 1;
		} else {
			dy = y0 - y1;
			addy = -1;
		}

		if (dx >= dy) {
			dy *= 2;
			P = dy - dx;
			diff = P - dx;

			for(i=0; i<=dx; ++i) {
				gdisp_lld_drawpixel(x0, y0, color);
				if (P < 0) {
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

			for(i=0; i<=dy; ++i) {
				gdisp_lld_drawpixel(x0, y0, color);
				if (P < 0) {
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
#endif

#if !GDISP_HARDWARE_BOX
	void gdisp_lld_drawbox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		coord_t	x1, y1;
		
		x1 = x+cx-1;
		y1 = y+cy-1;

		if (cx > 2) {
			if (cy >= 1) {
				gdisp_lld_drawline(x, y, x1, y, color);
				if (cy >= 2) {
					gdisp_lld_drawline(x, y1, x1, y1, color);
					if (cy > 2) {
						gdisp_lld_drawline(x, y+1, x, y1-1, color);
						gdisp_lld_drawline(x1, y+1, x1, y1-1, color);
					}
				}
			}
		} else if (cx == 2) {
			gdisp_lld_drawline(x, y, x, y1, color);
			gdisp_lld_drawline(x1, y, x1, y1, color);
		} else if (cx == 1) {
			gdisp_lld_drawline(x, y, x, y1, color);
		}
	}
#endif

#if !GDISP_HARDWARE_FILLS
	void gdisp_lld_fillarea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_HARDWARE_SCROLL
			gdisp_lld_verticalscroll(x, y, cx, cy, cy, color);
		#elif GDISP_HARDWARE_LINES
			coord_t x1, y1;
			
			x1 = x + cx - 1;
			y1 = y + cy;
			for(; y < y1; y++)
				gdisp_lld_drawline(x, y, x1, y, color);
		#else
			coord_t x0, x1, y1;
			
			x0 = x;
			x1 = x + cx;
			y1 = y + cy;
			for(; y < y1; y++)
				for(x = x0; x < x1; x++)
					gdisp_lld_drawpixel(x, y, color);
		#endif
	}
#endif

#if !GDISP_HARDWARE_BITFILLS
	void gdisp_lld_blitarea(coord_t x, coord_t y, coord_t cx, coord_t cy, pixel_t *buffer) {
			coord_t x0, x1, y1;
			
			x0 = x;
			x1 = x + cx;
			y1 = y + cy;
			for(; y < y1; y++)
				for(x = x0; x < x1; x++)
					gdisp_lld_drawpixel(x, y, *buffer++);
	}
#endif

#if GDISP_NEED_CIRCLE && !GDISP_HARDWARE_CIRCLES
	void gdisp_lld_drawcircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		int16_t a, b, P;

		a = 0;
		b = radius;
		P = 1 - radius;

		do {
			gdisp_lld_drawpixel(a+x, b+y, color);
			gdisp_lld_drawpixel(b+x, a+y, color);
			gdisp_lld_drawpixel(x-a, b+y, color);
			gdisp_lld_drawpixel(x-b, a+y, color);
			gdisp_lld_drawpixel(b+x, y-a, color);
			gdisp_lld_drawpixel(a+x, y-b, color);
			gdisp_lld_drawpixel(x-a, y-b, color);
			gdisp_lld_drawpixel(x-b, y-a, color);
			if (P < 0)
				P += 3 + 2*a++;
			else
				P += 5 + 2*(a++ - b--);
		} while(a <= b);
	}
#endif

#if GDISP_NEED_CIRCLE && !GDISP_HARDWARE_CIRCLEFILLS
	void gdisp_lld_fillcircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		int16_t a, b, P;
		
		a = 0;
		b = radius;
		P = 1 - radius;

		do {
			gdisp_lld_drawline(x-a, y+b, x+a, y+b, color);
			gdisp_lld_drawline(x-a, y-b, x+a, y-b, color);
			gdisp_lld_drawline(x-b, y+a, x+b, y+a, color);
			gdisp_lld_drawline(x-b, y-a, x+b, y-a, color);
			if (P < 0)
				P += 3 + 2*a++;
			else
				P += 5 + 2*(a++ - b--);
		} while(a <= b);
	}
#endif

#if GDISP_NEED_ELLIPSE && !GDISP_HARDWARE_ELLIPSES
	void gdisp_lld_drawellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		int  dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
		long a2 = a*a, b2 = b*b;
		long err = b2-(2*b-1)*a2, e2; /* Fehler im 1. Schritt */

		do {
			gdisp_lld_drawpixel(x+dx, y+dy, color); /* I. Quadrant */
			gdisp_lld_drawpixel(x-dx, y+dy, color); /* II. Quadrant */
			gdisp_lld_drawpixel(x-dx, y-dy, color); /* III. Quadrant */
			gdisp_lld_drawpixel(x+dx, y-dy, color); /* IV. Quadrant */

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
			gdisp_lld_drawpixel(x+dx, y, color); /* -> Spitze der Ellipse vollenden */
			gdisp_lld_drawpixel(x-dx, y, color);
	   }   
	}
#endif

#if GDISP_NEED_ELLIPSE && !GDISP_HARDWARE_ELLIPSEFILLS
	void gdisp_lld_fillellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		int  dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
		long a2 = a*a, b2 = b*b;
		long err = b2-(2*b-1)*a2, e2; /* Fehler im 1. Schritt */

		do {
			gdisp_lld_drawline(x-dx,y+dy,x+dx,y+dy, color);
			gdisp_lld_drawline(x-dx,y-dy,x+dx,y-dy, color);

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
			gdisp_lld_drawpixel(x+dx, y, color); /* -> Spitze der Ellipse vollenden */
			gdisp_lld_drawpixel(x-dx, y, color);
	   }   
	}
#endif

#if GDISP_NEED_TEXT && !GDISP_HARDWARE_TEXT
	void gdisp_lld_drawchar(coord_t x, coord_t y, char c, font_t font, color_t color) {
		const fontcolumn_t	*ptr;
		fontcolumn_t		column;
		coord_t				width, i, j;

		/* Check we actually have something to print */
		width = _getCharWidth(font, c);
		if (!width) return;
		
		ptr = _getCharData(font, c);

		/* Loop through the data and display. The font data is LSBit first, down the column */
		for(i = 0; i < width; i++) {
			/* Get the font bitmap data for the column */
			column = *ptr++;
			
			/* Draw each pixel */
			for(j = 0; j < font->height; j++, column >>= 1) {
				if (column & 0x01)
					gdisp_lld_drawpixel(x+i, y+j, color);
			}
		}
	}
#endif

#if GDISP_NEED_TEXT && !GDISP_HARDWARE_TEXTFILLS
	void gdisp_lld_fillchar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		coord_t			width;
		
		/* Check we actually have something to print */
		width = _getCharWidth(font, c);
		if (!width) return;

		/* Method 1: Use background fill and then draw the text */
		#if GDISP_HARDWARE_TEXT || GDISP_SOFTWARE_TEXTFILLDRAW
			
			/* Fill the area */
			gdisp_lld_fillarea(x, y, width, font->height, bgcolor);
			
			/* Draw the text */
			gdisp_lld_drawchar(x, y, c, font, color);

		/* Method 2: Create a single column bitmap and then blit it */
		#elif GDISP_HARDWARE_BITFILLS && GDISP_SOFTWARE_TEXTBLITCOLUMN
		{
			const fontcolumn_t	*ptr;
			fontcolumn_t		column;
			coord_t				i, j;
			
			/* Working buffer for fast non-transparent text rendering [patch by Badger] */
			static pixel_t		buf[sizeof(fontcolumn_t)*8];

			ptr = _getCharData(font, c);

			/* Loop through the data and display. The font data is LSBit first, down the column */
			for(i = 0; i < width; i++) {
				/* Get the font bitmap data for the column */
				column = *ptr++;
				
				/* Draw each pixel */
				for(j = 0; j < font->height; j++, column >>= 1) {
					gdispPackPixels(buf, 1, i, 0, (column & 0x01) ? color : bgcolor);
				}

				gdisp_lld_blitarea(x+i, y, 1, font->height, buf);
			}
		}

		/* Method 3: Create a character bitmap and then blit it */
		#elif GDISP_HARDWARE_BITFILLS
		{
			const fontcolumn_t	*ptr;
			fontcolumn_t		column;
			coord_t				i, j;
			
			/* Working buffer for fast non-transparent text rendering [patch by Badger]
				This needs to be larger than the largest character we can print.
			*/
			static pixel_t		buf[20*(sizeof(fontcolumn_t)*8)];

			#if GDISP_NEED_VALIDATION
				/* Check our buffer is big enough */
				if (width * font->height > sizeof(buf)/sizeof(buf[0]))	return;
			#endif

			ptr = _getCharData(font, c);

			/* Loop through the data and display. The font data is LSBit first, down the column */
			for(i = 0; i < width; i++) {
				/* Get the font bitmap data for the column */
				column = *ptr++;
				
				/* Draw each pixel */
				for(j = 0; j < font->height; j++, column >>= 1) {
					gdispPackPixels(buf, width, i, j, (column & 0x01) ? color : bgcolor);
				}
			}

			/* [Patch by Badger] Write all in one stroke */
			gdisp_lld_blitarea(x, y, width, font->height, buf);
		}

		/* Method 4: Draw pixel by pixel */
		#else
		{
			const fontcolumn_t	*ptr;
			fontcolumn_t		column;
			coord_t				i, j;
			
			ptr = _getCharData(font, c);

			/* Loop through the data and display. The font data is LSBit first, down the column */
			for(i = 0; i < width; i++) {
				/* Get the font bitmap data for the column */
				column = *ptr++;
				
				/* Draw each pixel */
				for(j = 0; j < font->height; j++, column >>= 1) {
					gdisp_lld_drawpixel(x+i, y+j, (column & 0x01) ? color : bgcolor);
				}
			}
		}
		#endif
	}
#endif
