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
		Andrew Hannam aka inmarket	-> framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/**
 * @file    gdisp.c
 * @brief   GDISP Driver code.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"

#if HAL_USE_GDISP || defined(__DOXYGEN__)

#if GDISP_NEED_MULTITHREAD
	#error "GDISP: Multithread support not complete"
	#define MUTEX_ENTER		/* Not defined yet */
	#define MUTEX_EXIT		/* Not defined yet */
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   The size of a font column.
	 * @note	If you font heights > 16 you would need to redefine this
	 *			as a uint32_t instead of a uint16_t. Fonts would then take
	 *			twice the internal program memory.
	 */
	typedef uint16_t	fontcolumn_t

	/**
	 * @brief   Internal font structure.
	 * @note	This structure is followed by:
	 *			An array of column data offsets (relative to the font structure)
	 *			An array of character widths (uint8_t)
	 *			Each characters array of column data (fontcolumn_t)
	 */
	typedef struct font {
		uint8_t		height;
		uint8_t		charPadding;
		uint8_t		lineSpacing;
		uint8_t		descenderHeight;
		uint8_t		minWidth;
		uint8_t		maxWidth;
		char		minChar;
		char		maxChar;
		uint16_t	widthTableOffset;
		uint16_t	offsetTableOffset;
		};

	/**
	 * @brief   Macro's to get to the complex parts of the font structure.
	 */
	#define _getFontPart(f,o,t)		((t)((const uint8_t *)(f)+(o)))
	#define _getCharWidth(f,c)		(((c) < (f)->minChar || (c) > (f)->maxChar) ? 0 : _getFontPart((f), (f)->widthTableOffset, const uint8_t *)[c - (f)->minChar])
	#define _getCharOffset(f,c)		(_getFontPart((f), (f)->offsetTableOffset, const uint16_t *)[c - (f)->minChar])
	#define _getCharData(f,c)		_getFontPart((f), _getCharOffset(c, (f)), const fontcolumn_t *)
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	#include "gdisp_inc_fonts.c"
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* Include the software emulation routines */
#include "gdisp_inc_emulation.c"

#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   GDISP Driver initialization.
	 * @note    This function is NOT currently implicitly invoked by @p halInit().
	 *			It must be called manually.
	 *
	 * @init
	 */
	void gdispInit(GDISPDriver *gdisp) {
		/* No mutex required as nothing should happen until the init is complete */
		gdisp_lld_init();
		
		/* ToDo - Initialise Mutex */
	}
#endif

#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Set the power mode for the display.
	 * @pre     The GDISP unit must have been initialised using @p gdispInit().
	 * @note    Depending on the hardware implementation this function may not
	 *          support powerSleep. If not powerSleep is treated the same as powerOn.
	 *          (sleep allows drawing to the display without the display updating).
	 *
	 * @param[in] powerMode The power mode to use
	 *
	 * @api
	 */
	void gdispSetPowerMode(gdisp_powermode_t powerMode) {
		MUTEX_ENTER
		gdisp_lld_setpowermode(powerMode);
		MUTEX_EXIT
	}
#endif

#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Set the orientation of the display.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 * @note    Depending on the hardware implementation this function may clear
	 *			the display when changing its orientation.
	 *
	 * @param[in] newOrientation The new orientation to use
	 *
	 * @api
	 */
	void gdispSetOrientation(gdisp_orientation_t newOrientation) {
		MUTEX_ENTER
		gdisp_lld_setorientation(newOrientation);
		MUTEX_EXIT
	}
#endif

#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Clear the display to the specified color.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] color The color to use when clearing the screen
	 *
	 * @api
	 */
	void gdispClear(color_t color) {
		MUTEX_ENTER
		gdisp_lld_clear(color);
		MUTEX_EXIT
	}
#endif

#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Set a pixel in the specified color.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x,y   The position to set the pixel.
	 * @param[in] color The color to use
	 *
	 * @api
	 */
	void gdispDrawPixel(coord_t x, coord_t y, color_t color) {
		MUTEX_ENTER
		gdisp_lld_drawpixel(x, y, color);
		MUTEX_EXIT
	}
#endif
	
#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a line.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The start position
	 * @param[in] x1,y1   The end position
	 * @param[in] color The color to use
	 *
	 * @api
	 */
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		MUTEX_ENTER
		gdisp_lld_drawline(x0, y0, x1, y1, color);
		MUTEX_EXIT
	}
#endif
	
#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a rectangular box.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The start position
	 * @param[in] cx,cy   The size of the box (outside dimensions)
	 * @param[in] color   The color to use
	 * @param[in] filled  Should the box should be filled
	 *
	 * @api
	 */
	void gdispDrawBox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		MUTEX_ENTER
		gdisp_lld_drawbox(x, y, cx, cy, color);
		MUTEX_EXIT
	}
#endif

#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a color.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The start position
	 * @param[in] cx,cy   The size of the box (outside dimensions)
	 * @param[in] color   The color to use
	 *
	 * @api
	 */
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		MUTEX_ENTER
		gdisp_lld_fillarea(x, y, cx, cy, color);
		MUTEX_EXIT
	}
#endif
	
#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area using the supplied bitmap.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 * @details The bitmap is in the pixel format specified by the low level driver
	 * @note	If a packed pixel format is used and the width doesn't
	 *			match a whole number of bytes, the next line will start on a
	 *			non-byte boundary (no end-of-line padding).
	 *
	 * @param[in] x0,y0   The start position
	 * @param[in] cx,cy   The size of the filled area
	 * @param[in] buffer  The bitmap in the driver's pixel format.
	 *
	 * @api
	 */
	void gdispBlitArea(coord_t x, coord_t y, coord_t cx, coord_t cy, pixel_t *buffer) {
		MUTEX_ENTER
		gdisp_lld_blitarea(x, y, cx, cy, buffer);
		MUTEX_EXIT
	}
#endif
	
#if (GDISP_NEED_CIRCLE && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a circle.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The center of the circle
	 * @param[in] radius  The radius of the circle
	 * @param[in] color   The color to use
	 *
	 * @api
	 */
	void gdispDrawCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		MUTEX_ENTER
		gdisp_lld_drawcircle(x, y, radius, color);
		MUTEX_EXIT
	}
#endif
	
#if (GDISP_NEED_CIRCLE && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a filled circle.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The center of the circle
	 * @param[in] radius  The radius of the circle
	 * @param[in] color   The color to use
	 *
	 * @api
	 */
	void gdispFillCircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		MUTEX_ENTER
		gdisp_lld_fillcircle(x, y, radius, color);
		MUTEX_EXIT
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an ellipse.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The center of the ellipse
	 * @param[in] a,b     The dimensions of the ellipse
	 * @param[in] color   The color to use
	 *
	 * @api
	 */
	void gdispDrawEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		MUTEX_ENTER
		gdisp_lld_drawellipse(x, y, a, b, color);
		MUTEX_EXIT
	}
#endif
	
#if (GDISP_NEED_ELLIPSE && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a filled ellipse.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The center of the ellipse
	 * @param[in] a,b     The dimensions of the ellipse
	 * @param[in] color   The color to use
	 *
	 * @api
	 */
	void gdispFillEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		MUTEX_ENTER
		gdisp_lld_fillellipse(x, y, a, b, color);
		MUTEX_EXIT
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a text character.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x,y     The position for the text
	 * @param[in] c       The character to draw
	 * @param[in] color   The color to use
	 *
	 * @api
	 */
	void gdispDrawChar(coord_t x, coord_t y, char c, font_t font, color_t color) {
		MUTEX_ENTER
		gdisp_lld_drawchar(x, y, c, font, color);
		MUTEX_EXIT
	}
#endif
	
#if (GDISP_NEED_TEXT && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a text character with a filled background.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x,y     The position for the text
	 * @param[in] c       The character to draw
	 * @param[in] color   The color to use
	 * @param[in] bgcolor The background color to use
	 *
	 * @api
	 */
	void gdispFillChar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		MUTEX_ENTER
		gdisp_lld_fillchar(x, y, c, font, color, bgcolor);
		MUTEX_EXIT
	}
#endif
	
#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a text string.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x,y     The position for the text
	 * @param[in] str     The string to draw
	 * @param[in] color   The color to use
	 *
	 * @api
	 */
	void gdispDrawString(coord_t x, coord_t y, const char *str, font_t font, color_t color) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w;
		char		c;
		int			first;
		
		first = 1;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(c);
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (font->charPadding) {
				if (!first)
					x += font->charPadding;
				else
					first = 0;
			}
			
			/* Print the character */
			gdispDrawChar(x, y, c, font, color);
			x += w;
		}
	}
#endif
	
#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a text string.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x,y     The position for the text
	 * @param[in] str     The string to draw
	 * @param[in] color   The color to use
	 * @param[in] bgcolor The background color to use
	 *
	 * @api
	 */
	void gdispFillString(coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w;
		char		c;
		int			first;
		
		first = 1;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(c);
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (font->charPadding) {
				if (!first) {
					gdispFillArea(x, y, font->charPadding, font->height, bgcolor);
					x += font->charPadding;
				} else
					first = 0;
			}

			/* Print the character */
			gdispFillChar(x, y, c, font, color, bgcolor);
			x += w;
		}
	}
#endif
	
#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a text string verticly centered within the specified box.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 * @note    The entire box is filled
	 *
	 * @param[in] x,y     The position for the text (need to define top-right or base-line - check code)
	 * @param[in] str     The string to draw
	 * @param[in] color   The color to use
	 * @param[in] bgcolor The background color to use
	 * @param[in] justify Justify the text left, center or right within the box
	 *
	 * @api
	 */
	void gdispDrawStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgcolor, justify_t justify) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w, ypos, xpos;
		char		c;
		int			first;
		const char *rstr;
		
		/* Oops - font too large for the area */
		if (font->height > cy) return;

		/* See if we need to fill above the font */
		ypos = (cy - font->height)/2;
		if (ypos > 0) {
			gdispFillArea(x, y, cx, ypos, bgcolor);
			y += ypos;
			cy -= ypos;
		}
		
		/* See if we need to fill below the font */
		ypos = cy - font->height;
		if (ypos > 0) {
			gdispFillArea(x, y+cy-ypos, cx, ypos, bgcolor);
			cy -= ypos;
		}
		
		/* get the start of the printable string and the xpos */
		switch(justify) {
		case justifyCenter:
			/* Get the length of the entire string */
			w = gdispStringWidth(str, font);
			if (w <= cx)
				xpos = x + (cx - w)/2;
			else {
				/* Calculate how much of the string we need to get rid of */
				ypos = (w - cx)/2;
				xpos = 0;
				first = 1;
				while(*str) {
					/* Get the next printable character */
					c = *str++;
					w = _getCharWidth(c);
					if (!w) continue;
					
					/* Handle inter-character padding */
					if (font->charPadding) {
						if (!first) {
							xpos += font->charPadding;
							if (xpos > ypos) break;
						} else
							first = 0;
					}

					/* Print the character */
					xpos += w;
					if (xpos > ypos) break;
				}
				xpos = ypos - xpos + x;
			}
			break;
		case justifyRight:
			/* Find the end of the string */
			for(rpos = str; *str; str++);
			xpos = x+cx - 2;
			first = 1;
			for(str--; str >= rpos; str--) {
				/* Get the next printable character */
				c = *str;
				w = _getCharWidth(c);
				if (!w) continue;
				
				/* Handle inter-character padding */
				if (font->charPadding) {
					if (!first) {
						if (xpos - font->charPadding < x) break;
						xpos -= font->charPadding;
					} else
						first = 0;
				}

				/* Print the character */
				if (xpos - w < x) break;
				xpos -= w;
			}
			str++;
			break;
		case justifyLeft:
			/* Fall through */
			xpos = x+1;
		default:
			break;
		}
		
		/* Fill any space to the left */
		if (x < xpos)
			gdispFillArea(x, y, xpos-x, cy, bgcolor);
		
		/* Print characters until we run out of room */
		first = 1;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(c);
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (font->charPadding) {
				if (!first) {
					if (xpos + font->charPadding > x+cx) break;
					gdispFillArea(xpos, y, font->charPadding, cy, bgcolor);
					xpos += font->charPadding;
				} else
					first = 0;
			}

			/* Print the character */
			if (xpos + w > x+cx) break;
			gdispFillChar(xpos, y, c, font, color, bgcolor);
			xpos += w;
		}
		
		/* Fill any space to the right */
		if (xpos < x+cx)
			gdispFillArea(xpos, y, x+cx-xpos, cy, bgcolor);
	}
#endif
	
#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Get a metric of a font.
	 * @return  The metric requested in pixels.
	 *
	 * @param[in] font    The font to test
	 * @param[in] metric  The metric to measure
	 *
	 * @api
	 */
	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric) {
		/* No mutex required as we only read static data */
		switch(metric) {
		case fontHeight:			return font->height;
		case fontDescendersHeight:	return font->descenderHeight;
		case fontLineSpacing:		return font->lineSpacing;
		case fontCharPadding:		return font->charPadding;
		case fontMinWidth:			return font->minWidth;
		case fontMaxWidth:			return font->maxWidth;
		}
		return 0;
	}
#endif
	
#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Get the pixel width of a character.
	 * @return  The width of the character in pixels. Does not include any between character padding.
	 *
	 * @param[in] c       The character to draw
	 * @param[in] font    The font to use
	 *
	 * @api
	 */
	coord_t gdispGetCharWidth(char c, font_t font) {
		/* No mutex required as we only read static data */
		return _getCharWidth(font, c);
	}
#endif
	
#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Get the pixel width of a string.
	 * @return  The width of the string in pixels.
	 *
	 * @param[in] str     The string to measure
	 * @param[in] font    The font to use
	 *
	 * @api
	 */
	coord_t gdispStringWidth(const char* str, font_t font) {
		/* No mutex required as we only read static data */
		coord_t		w, x;
		char		c;
		int			first;
		
		first = 1;
		x = 0;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(c);
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (font->charPadding) {
				if (!first)
					x += font->charPadding;
				else
					first = 0;
			}
			
			/* Add the character width */
			x += w;
		}
		return x;
	}
#endif

#if (!defined(gdispPackPixels) && !defined(GDISP_PIXELFORMAT_CUSTOM)) || defined(__DOXYGEN__)
	/**
	 * @brief   Pack a pixel into a pixel buffer.
	 * @note    This function performs no buffer boundary checking
	 *			regardless of whether GDISP_NEED_CLIPPING has been specified.
	 *
	 * @param[in] buf		The buffer to put the pixel in
	 * @param[in] cx		The width of a pixel line
	 * @param[in] x, y		The location of the pixel to place
	 * @param[in] color		The color to put into the buffer
	 *
	 * @api
	 */
	void gdispPackPixels(pixel_t *buf, coord_t cx, coord_t x, coord_t y, color_t color) {
		/* No mutex required as we only read static data */
		#if defined(GDISP_PIXELFORMAT_RGB888)
			#error "GDISP: Packed pixels not supported yet"
		#elif defined(GDISP_PIXELFORMAT_RGB444)
			#error "GDISP: Packed pixels not supported yet"
		#elif defined(GDISP_PIXELFORMAT_RGB666)
			#error "GDISP: Packed pixels not supported yet"
		#elif
			#error "GDISP: Unsupported packed pixel format"
		#endif
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a pixel.
	 * @return  The color of the pixel.
	 *
	 * @param[in] x,y     The position of the pixel
	 *
	 * @api
	 */
	color_t gdispGetPixelColor(coord_t x, coord_t y) {
		color_t		c;
		
		MUTEX_ENTER
		c = gdisp_lld_getpixelcolor(x, y);
		MUTEX_EXIT
		
		return c;
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @note    Optional.
	 * @note    If lines is >= cy, it is equivelent to a area fill with bgcolor.
	 *
	 * @param[in] x, y     The start of the area to be scrolled
	 * @param[in] cx, cy   The size of the area to be scrolled
	 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor  The color to fill the newly exposed area.
	 *
	 * @api
	 */
	void gdispVerticalScroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		MUTEX_ENTER
		gdisp_lld_verticalscroll(x, y, cx, cy, lines, bgcolor);
		MUTEX_EXIT
	}
#endif

#endif /* HAL_USE_GDISP */
/** @} */
