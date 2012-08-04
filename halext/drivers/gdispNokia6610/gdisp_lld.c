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
 * @file    gdispNokia6610/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the Nokia6610 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if HAL_USE_GDISP || defined(__DOXYGEN__)

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

/* Controller definitions */
#if defined(LCD_USE_GE8)
	#include "GE8.h"
#elif defined(LCD_USE_GE12)
	#include "GE12.h"
#else
	#error "gdispNokia6610: Either LCD_USE_GE8 or LCD_USE_GE12 must be defined depending on your controller"
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
	GDISPDriver GDISP;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#include "gdisp_fonts.h"

#if defined(BOARD_OLIMEX_SAM7_EX256)
	#include "gdisp_lld_board_olimexsam7ex256.h"
#else
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#endif

#define gdisp_lld_write_command(cmd)		gdisp_lld_write_spi((cmd) & ~0x0100)
#define gdisp_lld_write_data(data)			gdisp_lld_write_spi((data) | 0x0100)

static __inline void gdisp_lld_setviewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {
	gdisp_lld_write_command(CASET);			// Column address set
	gdisp_lld_write_data(x);
	gdisp_lld_write_data(x+cx-1);
	gdisp_lld_write_command(PASET);			// Page address set
	gdisp_lld_write_data(y);
	gdisp_lld_write_data(y+cy-1);
}

void Delay (unsigned long a) {
	chThdSleepMilliseconds(a/100);
//	volatile unsigned long d;
//	for(d=a; d; d--);
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */
/*
	The following 2 routines are required.
	All other routines are optional.
*/

/**
 * @brief   Low level GDISP driver initialization.
 *
 * @notapi
 */
void gdisp_lld_init(void) {
	/* Initialise your display */
	gdisp_lld_init_board();

	// Hardware reset
	gdisp_lld_setpin_reset(TRUE);
	Delay(20000);
	gdisp_lld_setpin_reset(FALSE);
	Delay(20000);

	#if defined(LCD_USE_GE8)
		#if 1
			gdisp_lld_write_command(DISCTL);		// Display control
			gdisp_lld_write_data(0x00);					// P1: 0x00 = 2 divisions, switching period=8 (default)
			gdisp_lld_write_data(0x20);					// P2: 0x20 = nlines/4 - 1 = 132/4 - 1 = 32)
			gdisp_lld_write_data(0x00);					// P3: 0x00 = no inversely highlighted lines
			gdisp_lld_write_command(COMSCN);		// COM scan
			gdisp_lld_write_data(1);					// P1: 0x01 = Scan 1->80, 160<-81
			gdisp_lld_write_command(OSCON);			// Internal oscilator ON
			gdisp_lld_write_command(SLPOUT);		// Sleep out
			gdisp_lld_write_command(PWRCTR);		// Power control
			gdisp_lld_write_data(0x0f);					// reference voltage regulator on, circuit voltage follower on, BOOST ON
			// Interesting - all the code seems to say this should be done. But my display doesn't want it!
			//gdisp_lld_write_command(DISINV);		// Inverse display
			gdisp_lld_write_command(DATCTL);		// Data control
			gdisp_lld_write_data(0x01);					// P1: 0x01 = page address inverted, column address normal, address scan in column direction
			gdisp_lld_write_data(0x00);					// P2: 0x00 = RGB sequence (default value)
			gdisp_lld_write_data(0x02);					// P3: 0x02 = Grayscale -> 16 (selects 12-bit color, type A)
			gdisp_lld_write_command(VOLCTR);		// Voltage control (contrast setting)
			gdisp_lld_write_data(32);					// P1 = 32 volume value (experiment with this value to get the best contrast)
			gdisp_lld_write_data(3);					// P2 = 3 resistance ratio (only value that works)
			Delay(100000);							// allow power supply to stabilize
			gdisp_lld_write_command(DISON);			// Turn on the display
		#else
			// Alternative
			gdisp_lld_write_command(DISCTL);		// Display control
			gdisp_lld_write_data(0x00);					// default
			gdisp_lld_write_data(0x20);					// (32 + 1) * 4 = 132 lines (of which 130 are visible)
			gdisp_lld_write_data(0x0a);					// default
			gdisp_lld_write_command(COMSCN);		// COM scan
			gdisp_lld_write_data(0x00);					// Scan 1-80
			gdisp_lld_write_command(OSCON);			// Internal oscilator ON
			Delay(10000);							// wait aproximetly 100ms
			gdisp_lld_write_command(SLPOUT);		// Sleep out
			gdisp_lld_write_command(VOLCTR);		// Voltage control
			gdisp_lld_write_data(0x1F);					// middle value of V1
			gdisp_lld_write_data(0x03);					// middle value of resistance value
			gdisp_lld_write_command(TMPGRD);		// Temperature gradient
			gdisp_lld_write_data(0x00);					// default
			gdisp_lld_write_command(PWRCTR);		// Power control
			gdisp_lld_write_data(0x0f);					// referance voltage regulator on, circuit voltage follower on, BOOST ON
			gdisp_lld_write_command(DISNOR);		// Normal display
			gdisp_lld_write_command(DISINV);		// Inverse display
			gdisp_lld_write_command(PTLOUT);		// Partial area off
			//  gdisp_lld_write_command(ASCSET);	// Scroll area set
			//  gdisp_lld_write_data(0);
			//  gdisp_lld_write_data(0);
			//  gdisp_lld_write_data(40);
			//  gdisp_lld_write_data(3);
			//  gdisp_lld_write_command(SCSTART);	// Vertical scrool address start
			//  gdisp_lld_write_data(0);
			gdisp_lld_write_command(DATCTL);		// Data control
			gdisp_lld_write_data(0x00);					// all inversions off, column direction
			gdisp_lld_write_data(0x03);					// RGB sequence
			gdisp_lld_write_data(0x02);					// Grayscale -> 16
			gdisp_lld_write_command(PASET);			// Page Address set
			gdisp_lld_write_data(0);
			gdisp_lld_write_data(131);
			gdisp_lld_write_command(CASET);			// Page Column set
			gdisp_lld_write_data(0);
			gdisp_lld_write_data(131);
			gdisp_lld_write_command(DISON);			// Turn on the display
		#endif

	#elif defined(LCD_USE_GE12)
		#if 1
			gdisp_lld_write_command(SLEEPOUT);		// Sleep out
			gdisp_lld_write_command(INVON);			// Inversion on: seems to be required for this controller
			gdisp_lld_write_command(COLMOD);		// Color Interface Pixel Format
			gdisp_lld_write_data(0x03);				// 0x03 = 12 bits-per-pixel
			gdisp_lld_write_command(MADCTL);		// Memory access controler
			gdisp_lld_write_data(0xC8);				// 0xC0 = mirror x and y, reverse rgb
			gdisp_lld_write_command(SETCON);		// Write contrast
			gdisp_lld_write_data(0x30);				// contrast - experiental value
			Delay(2000);
			gdisp_lld_write_command(DISPON);		// Display On
		#else
			// Alternative
			// Hardware reset commented out
			gdisp_lld_write_command(SOFTRST);		// Software Reset
			Delay(2000);
			gdisp_lld_write_command(INITESC);		// Initial escape
			Delay(2000);
			gdisp_lld_write_command(REFSET);		// Refresh set
			gdisp_lld_write_data(0);
			gdisp_lld_write_command(DISPCTRL);		// Set Display control
			gdisp_lld_write_data(128);					// Set the lenght of one selection term
			gdisp_lld_write_data(128);					// Set N inversion -> no N inversion
			gdisp_lld_write_data(134);					// Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
			gdisp_lld_write_data(84);					// Set duty parameter
			gdisp_lld_write_data(69);					// Set duty parameter
			gdisp_lld_write_data(82);					// Set duty parameter
			gdisp_lld_write_data(67);					// Set duty parameter
			gdisp_lld_write_command(GRAYSCALE0);	// Grey scale 0 position set - 15 parameters
			gdisp_lld_write_data(1);					// GCP1 - gray lavel to be output when the RAM data is "0001"
			gdisp_lld_write_data(2);					// GCP2 - gray lavel to be output when the RAM data is "0010"
			gdisp_lld_write_data(4);					// GCP3 - gray lavel to be output when the RAM data is "0011"
			gdisp_lld_write_data(8);					// GCP4 - gray lavel to be output when the RAM data is "0100"
			gdisp_lld_write_data(16);					// GCP5 - gray lavel to be output when the RAM data is "0101"
			gdisp_lld_write_data(30);					// GCP6 - gray lavel to be output when the RAM data is "0110"
			gdisp_lld_write_data(40);					// GCP7 - gray lavel to be output when the RAM data is "0111"
			gdisp_lld_write_data(50);					// GCP8 - gray lavel to be output when the RAM data is "1000"
			gdisp_lld_write_data(60);					// GCP9 - gray lavel to be output when the RAM data is "1001"
			gdisp_lld_write_data(70);					// GCP10 - gray lavel to be output when the RAM data is "1010"
			gdisp_lld_write_data(80);					// GCP11 - gray lavel to be output when the RAM data is "1011"
			gdisp_lld_write_data(90);					// GCP12 - gray lavel to be output when the RAM data is "1100"
			gdisp_lld_write_data(100);					// GCP13 - gray lavel to be output when the RAM data is "1101"
			gdisp_lld_write_data(110);					// GCP14 - gray lavel to be output when the RAM data is "1110"
			gdisp_lld_write_data(127);					// GCP15 - gray lavel to be output when the RAM data is "1111"
			gdisp_lld_write_command(GAMMA);				// Gamma curve set - select gray scale - GRAYSCALE 0 or GREYSCALE 1
			gdisp_lld_write_data(1);						// Select grey scale 0
			gdisp_lld_write_command(COMMONDRV);			// Command driver output
			gdisp_lld_write_data(0);						// Set COM1-COM41 side come first, normal mod
			gdisp_lld_write_command(NORMALMODE);		// Set Normal mode (my)
			// gdisp_lld_write_command(INVERSIONOFF);	// Inversion off
			gdisp_lld_write_command(COLADDRSET);		// Column address set
			gdisp_lld_write_data(0);
			gdisp_lld_write_data(131);
			gdisp_lld_write_command(PAGEADDRSET);		// Page address set
			gdisp_lld_write_data(0);
			gdisp_lld_write_data(131);
			gdisp_lld_write_command(ACCESSCTRL);		// Memory access controler
			gdisp_lld_write_data(0x40);						// horizontal
			//gdisp_lld_write_data(0x20);					// vertical
			gdisp_lld_write_command(PWRCTRL);			// Power control
			gdisp_lld_write_data(4);						// Internal resistance, V1OUT -> high power mode, oscilator devision rate
			gdisp_lld_write_command(SLEEPOUT);			// Sleep out
			gdisp_lld_write_command(VOLTCTRL);			// Voltage control - voltage control and write contrast define LCD electronic volume
			//gdisp_lld_write_data(0x7f);					//  full voltage control
			//gdisp_lld_write_data(0x03);					//  must be "1"
			gdisp_lld_write_command(CONTRAST);			// Write contrast
			gdisp_lld_write_data(0x3b);						// contrast
			Delay(2000);
			gdisp_lld_write_command(TEMPGRADIENT);		// Temperature gradient
			for(i=0; i<14; i++) gdisp_lld_write_data(0);
			gdisp_lld_write_command(BOOSTVON);			// Booster voltage ON
			gdisp_lld_write_command(DISPLAYON);			// Finally - Display On
		#endif
	#endif

	/* Turn on the back-light */
	gdisp_lld_setpin_backlight(TRUE);

	/* Initialise the GDISP structure to match */
	GDISP.Width = 132;
	GDISP.Height = 132;
	GDISP.Orientation = portrait;
	GDISP.Powermode = powerOn;
}

/**
 * @brief   Draws a pixel on the display.
 *
 * @param[in] x        X location of the pixel
 * @param[in] y        Y location of the pixel
 * @param[in] color    The color of the pixel
 *
 * @notapi
 */
void gdisp_lld_drawpixel(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION
		if (x >= GDISP.Width || y >= GDISP.Height) return;
	#endif
	gdisp_lld_setviewport(x, y, 1, 1);
	gdisp_lld_write_command(RAMWR);
	gdisp_lld_write_data((color >> 4) & 0xFF);
	gdisp_lld_write_data((color << 4) & 0xF0);
	gdisp_lld_write_command(NOP);
}

/* ---- Optional Routines ---- */
/*
	All the below routines are optional.
	Defining them will increase speed but everything
	will work if they are not defined.
	If you are not using a routine - turn it off using
	the appropriate GDISP_HARDWARE_XXXX macro.
	Don't bother coding for obvious similar routines if
	there is no performance penalty as the emulation software
	makes a good job of using similar routines.
		eg. If gdisp_lld_fillarea() is defined there is little
			point in defining gdisp_lld_clear() unless the
			performance bonus is significant.
	For good performance it is suggested to implement
		gdisp_lld_fillarea() and gdisp_lld_blitarea().
*/

#if GDISP_HARDWARE_CLEARS || defined(__DOXYGEN__)
	/**
	 * @brief   Clear the display.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] color    The color of the pixel
	 *
	 * @notapi
	 */
	void gdisp_lld_clear(color_t color) {
		/* NOT IMPLEMENTED */
		/* Nothing to be gained by implementing this
		 * as fillarea is just as fast.
		 */
	}
#endif

#if GDISP_HARDWARE_LINES || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a line.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x0, y0   The start of the line
	 * @param[in] x1, y1   The end of the line
	 * @param[in] color    The color of the line
	 *
	 * @notapi
	 */
	void gdisp_lld_drawline(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a color.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] color    The color of the fill
	 *
	 * @notapi
	 */
	void gdisp_lld_fillarea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		unsigned i, tuples;

		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP.Width || y >= GDISP.Height) return;
			if (x+cx > GDISP.Width)		cx = GDISP.Width - x;
			if (y+cy > GDISP.Height)	cy = GDISP.Height - y;
		#endif

		tuples = (cx*cy+1)/2;				// With an odd sized area we over-print by one pixel.
											// This extra pixel is ignored by the controller.

		gdisp_lld_setviewport(x, y, cx, cy);
		gdisp_lld_write_command(RAMWR);
		for(i=0; i < tuples; i++) {
			gdisp_lld_write_data((color >> 4) & 0xFF);
			gdisp_lld_write_data(((color << 4) & 0xF0)|((color >> 8) & 0x0F));
			gdisp_lld_write_data(color & 0xFF);
		}
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] buffer   The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void gdisp_lld_blitarea(coord_t x, coord_t y, coord_t cx, coord_t cy, pixel_t *buffer) {
		unsigned i, area, tuples;
		#ifndef GDISP_PACKED_PIXELS
			color_t	c1, c2;
		#endif

		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP.Width || y >= GDISP.Height) return;
			if (x+cx > GDISP.Width) return;
			if (y+cy > GDISP.Height)	cy = GDISP.Height - y;
		#endif

		area = cx*cy;

		gdisp_lld_setviewport(x, y, cx, cy);
		gdisp_lld_write_command(RAMWR);

		#ifdef GDISP_PACKED_PIXELS
			// 3 bytes per 2 pixels + an extra 2 bytes if the total size is odd.
			// Note we can't just over-estimate this and let the controller handle the extra pixel
			//	as that might over-run our source buffer (very bad in some circumstances).
			tuples = (area/2)*3+(area & 0x01)*2;
			for(i=0; i < tuples; i++)
				gdisp_lld_write_data(*buffer++);
			if (area & 0x01)
				gdisp_lld_write_command(NOP);
		#else
			// Although this controller uses packed pixels we support unpacked pixel
			//  formats in this blit by packing the data as we feed it to the controller.
			tuples = area/2;
			for(i=0; i < tuples; i++) {
				c1 = *buffer++;
				c2 = *buffer++;
				gdisp_lld_write_data((c1 >> 4) & 0xFF);
				gdisp_lld_write_data(((c1 << 4) & 0xF0)|((c2 >> 8) & 0x0F));
				gdisp_lld_write_data(c2 & 0xFF);
			}
			if (area & 0x01) {
				c1 = *buffer++;
				gdisp_lld_write_data((c1 >> 4) & 0xFF);
				gdisp_lld_write_data((c1 << 4) & 0xF0);
				gdisp_lld_write_command(NOP);
			}
		#endif
	}
#endif

/* Circular Drawing Functions */
#if (GDISP_NEED_CIRCLE && GDISP_HARDWARE_CIRCLES) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a circle.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the circle
	 * @param[in] radius   The radius of the circle
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void gdisp_lld_drawcircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_CIRCLE && GDISP_HARDWARE_CIRCLEFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Create a filled circle.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the circle
	 * @param[in] radius   The radius of the circle
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void gdisp_lld_fillcircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_HARDWARE_ELLIPSES) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an ellipse.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the ellipse is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the ellipse
	 * @param[in] a, b     The dimensions of the ellipse
	 * @param[in] color    The color of the ellipse
	 *
	 * @notapi
	 */
	void gdisp_lld_drawellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_HARDWARE_ELLIPSEFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Create a filled ellipse.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the ellipse is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the ellipse
	 * @param[in] a, b     The dimensions of the ellipse
	 * @param[in] color    The color of the ellipse
	 *
	 * @notapi
	 */
	void gdisp_lld_fillellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_HARDWARE_TEXT) || defined(__DOXYGEN__)
	#include "gdisp_fonts.h"
#endif

#if (GDISP_NEED_TEXT && GDISP_HARDWARE_TEXT) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a character using a transparent background.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The top-left corner of the text
	 * @param[in] c        The character to print
	 * @param[in] color    The color of the character
	 *
	 * @notapi
	 */
	void gdisp_lld_drawchar(coord_t x, coord_t y, char c, font_t font, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_HARDWARE_TEXTFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a character using a filled background.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The top-left corner of the text
	 * @param[in] c        The character to print
	 * @param[in] color    The color of the character
	 * @param[in] bgcolor  The background color
	 *
	 * @notapi
	 */
	void gdisp_lld_fillchar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    Optional.
	 * @note    If x,y is off the screen, the result is undefined.
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t gdisp_lld_getpixelcolor(coord_t x, coord_t y) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @note    Optional.
	 * @note    If x,y + cx,cy is off the screen, the result is undefined.
	 * @note    If lines is >= cy, it is equivelent to a area fill with bgcolor.
	 *
	 * @param[in] x, y     The start of the area to be scrolled
	 * @param[in] cx, cy   The size of the area to be scrolled
	 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor  The color to fill the newly exposed area.
	 *
	 * @notapi
	 */
	void gdisp_lld_verticalscroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		/* NOT IMPLEMENTED */
	}
#endif

#if GDISP_HARDWARE_CONTROL || defined(__DOXYGEN__)
	/**
	 * @brief   Driver Control
	 * @detail	Unsupported control codes are ignored.
	 * @note	The value parameter should always be typecast to (void *).
	 * @note	There are some predefined and some specific to the low level driver.
	 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
	 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
	 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
	 * 											that only supports off/on anything other
	 * 											than zero is on.
	 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
	 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
	 * 											this value.
	 *
	 * @param[in] what		What to do.
	 * @param[in] value		The value to use (always cast to a void *).
	 *
	 * @notapi
	 */
	void gdisp_lld_control(int what, void *value) {
		/* NOT IMPLEMENTED YET */
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
				case powerOff:
					/* 	Code here */
					break;
				case powerOn:
					/* 	Code here */
					/* You may need this ---
						if (GDISP.Powermode != powerSleep)
							gdisp_lld_init();
					*/
					break;
				case powerSleep:
					/* 	Code here */
					break;
				default:
					return;
			}
			GDISP.Powermode = (gdisp_powermode_t)value;
			return;
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
	//		WriteSpiData(0x48); // no mirror Y (temporary to satisfy Olimex bmptoarray utility)
	//		WriteSpiData(0xC8); // restore to (mirror x and y, reverse rgb)
			switch((gdisp_orientation_t)value) {
				case portrait:
					/* 	Code here */
					GDISP.Height = SCREEN_HEIGHT;
					GDISP.Width = SCREEN_WIDTH;
					break;
				case landscape:
					/* 	Code here */
					GDISP.Height = SCREEN_WIDTH;
					GDISP.Width = SCREEN_HEIGHT;
					break;
				case portraitInv:
					/* 	Code here */
					GDISP.Height = SCREEN_HEIGHT;
					GDISP.Width = SCREEN_WIDTH;
					break;
				case landscapeInv:
					/* 	Code here */
					GDISP.Height = SCREEN_WIDTH;
					GDISP.Width = SCREEN_HEIGHT;
					break;
				default:
					return;
			}
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
/*
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#endif /* HAL_USE_GDISP */
/** @} */
