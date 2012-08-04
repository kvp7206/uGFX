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

/**
 * @file    gdisp.c
 * @brief   GDISP Driver code.
 *
 * @addtogroup GDISP
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if HAL_USE_GDISP || defined(__DOXYGEN__)

#ifdef GDISP_NEED_TEXT
	#include "gdisp_fonts.h"
#endif

#if GDISP_NEED_MULTITHREAD
	#warning "GDISP: Multithread support not complete"
	#define MUTEX_INIT		/* Not defined yet */
	#define MUTEX_ENTER		/* Not defined yet */
	#define MUTEX_EXIT		/* Not defined yet */
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

#if GDISP_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief   GDISP Driver initialization.
	 * @note    This function is NOT currently implicitly invoked by @p halInit().
	 *			It must be called manually.
	 *
	 * @init
	 */
	void gdispInit(GDISPDriver * UNUSED(gdisp)) {
		/* Initialise Mutex */
		MUTEX_INIT

		/* Initialise driver */
		MUTEX_ENTER
		gdisp_lld_init();
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
	
#if (GDISP_NEED_CONTROL && GDISP_NEED_MULTITHREAD) || defined(__DOXYGEN__)
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
	void gdispControl(int what, void *value) {
		MUTEX_ENTER
		gdisp_lld_control(what, value);
		MUTEX_EXIT
	}
#endif

/*===========================================================================*/
/* High Level Driver Routines.                                               */
/*===========================================================================*/

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
		/* No mutex required as we only call high level functions which have their own mutex */
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
		coord_t		w, p;
		char		c;
		int			first;
		
		first = 1;
		p = font->charPadding * font->xscale;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c) * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first)
					x += p;
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
		coord_t		w, h, p;
		char		c;
		int			first;
		
		first = 1;
		h = font->height * font->yscale;
		p = font->charPadding * font->xscale;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c) * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first) {
					gdispFillArea(x, y, p, h, bgcolor);
					x += p;
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
	void gdispFillStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgcolor, justify_t justify) {
		/* No mutex required as we only call high level functions which have their own mutex */
		coord_t		w, h, p, ypos, xpos;
		char		c;
		int			first;
		const char *rstr;
		
		h = font->height * font->yscale;
		p = font->charPadding * font->xscale;

		/* Oops - font too large for the area */
		if (h > cy) return;

		/* See if we need to fill above the font */
		ypos = (cy - h + 1)/2;
		if (ypos > 0) {
			gdispFillArea(x, y, cx, ypos, bgcolor);
			y += ypos;
			cy -= ypos;
		}
		
		/* See if we need to fill below the font */
		ypos = cy - h;
		if (ypos > 0) {
			gdispFillArea(x, y+cy-ypos, cx, ypos, bgcolor);
			cy -= ypos;
		}
		
		/* get the start of the printable string and the xpos */
		switch(justify) {
		case justifyCenter:
			/* Get the length of the entire string */
			w = gdispGetStringWidth(str, font);
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
					w = _getCharWidth(font, c) * font->xscale;
					if (!w) continue;
					
					/* Handle inter-character padding */
					if (p) {
						if (!first) {
							xpos += p;
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
			for(rstr = str; *str; str++);
			xpos = x+cx - 2;
			first = 1;
			for(str--; str >= rstr; str--) {
				/* Get the next printable character */
				c = *str;
				w = _getCharWidth(font, c) * font->xscale;
				if (!w) continue;
				
				/* Handle inter-character padding */
				if (p) {
					if (!first) {
						if (xpos - p < x) break;
						xpos -= p;
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
		default:
			xpos = x+1;
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
			w = _getCharWidth(font, c) * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first) {
					if (xpos + p > x+cx) break;
					gdispFillArea(xpos, y, p, cy, bgcolor);
					xpos += p;
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
		case fontHeight:			return font->height * font->yscale;
		case fontDescendersHeight:	return font->descenderHeight * font->yscale;
		case fontLineSpacing:		return font->lineSpacing * font->yscale;
		case fontCharPadding:		return font->charPadding * font->xscale;
		case fontMinWidth:			return font->minWidth * font->xscale;
		case fontMaxWidth:			return font->maxWidth * font->xscale;
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
		return _getCharWidth(font, c) * font->xscale;
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
	coord_t gdispGetStringWidth(const char* str, font_t font) {
		/* No mutex required as we only read static data */
		coord_t		w, h, p, x;
		char		c;
		int			first;
		
		first = 1;
		x = 0;
		h = font->height * font->yscale;
		p = font->charPadding * font->xscale;
		while(*str) {
			/* Get the next printable character */
			c = *str++;
			w = _getCharWidth(font, c)  * font->xscale;
			if (!w) continue;
			
			/* Handle inter-character padding */
			if (p) {
				if (!first)
					x += p;
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

#endif /* HAL_USE_GDISP */
/** @} */
