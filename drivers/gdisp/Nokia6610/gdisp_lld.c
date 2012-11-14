/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    drivers/gdisp/Nokia6610/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the Nokia6610 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp_emulation.c"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/* Controller definitions */
#if defined(GDISP_USE_GE8)
	#include "GE8.h"
#elif defined(GDISP_USE_GE12)
	#include "GE12.h"
#else
	#error "gdispNokia6610: Either GDISP_USE_GE8 or GDISP_USE_GE12 must be defined depending on your controller"
#endif

/* This controller is only ever used with a 132 x 132 display */
#if defined(GDISP_SCREEN_HEIGHT)
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#undef GDISP_SCREEN_WIDTH
#endif
#define GDISP_SCREEN_HEIGHT		132
#define GDISP_SCREEN_WIDTH		132

#define GDISP_INITIAL_CONTRAST	38
#define GDISP_INITIAL_BACKLIGHT	100

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if defined(BOARD_OLIMEX_SAM7_EX256)
	#include "gdisp_lld_board_olimexsam7ex256.h"
#else
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#endif

static __inline void gdisp_lld_setviewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {
	GDISP_LLD(write_cmd)(CASET);			// Column address set
	GDISP_LLD(write_data)(x);
	GDISP_LLD(write_data)(x+cx-1);
	GDISP_LLD(write_cmd)(PASET);			// Page address set
	GDISP_LLD(write_data)(y);
	GDISP_LLD(write_data)(y+cy-1);
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
bool_t GDISP_LLD(init)(void) {
	/* Initialise your display */
	GDISP_LLD(init_board)();

	// Hardware reset
	GDISP_LLD(setpin_reset)(TRUE);
	chThdSleepMilliseconds(20);
	GDISP_LLD(setpin_reset)(FALSE);
	chThdSleepMilliseconds(20);

	#if defined(GDISP_USE_GE8)
			GDISP_LLD(write_cmd)(DISCTL);		// Display control
			GDISP_LLD(write_data)(0x00);					// P1: 0x00 = 2 divisions, switching period=8 (default)
			GDISP_LLD(write_data)(0x20);					// P2: 0x20 = nlines/4 - 1 = 132/4 - 1 = 32)
			GDISP_LLD(write_data)(0x00);					// P3: 0x00 = no inversely highlighted lines
			GDISP_LLD(write_cmd)(COMSCN);		// COM scan
			GDISP_LLD(write_data)(1);					// P1: 0x01 = Scan 1->80, 160<-81
			GDISP_LLD(write_cmd)(OSCON);			// Internal oscilator ON
			GDISP_LLD(write_cmd)(SLPOUT);		// Sleep out
			GDISP_LLD(write_cmd)(PWRCTR);		// Power control
			GDISP_LLD(write_data)(0x0f);					// reference voltage regulator on, circuit voltage follower on, BOOST ON
			GDISP_LLD(write_cmd)(DATCTL);		// Data control
			GDISP_LLD(write_data)(0x48);					// P1: 0x01 = page address inverted, column address normal, address scan in column direction
			GDISP_LLD(write_data)(0x00);					// P2: 0x00 = RGB sequence (default value)
			GDISP_LLD(write_data)(0x02);					// P3: 0x02 = Grayscale -> 16 (selects 12-bit color, type A)
			GDISP_LLD(write_cmd)(VOLCTR);		// Voltage control (contrast setting)
			GDISP_LLD(write_data)(GDISP_INITIAL_CONTRAST);	// P1 = Contrast
			GDISP_LLD(write_data)(3);					// P2 = 3 resistance ratio (only value that works)
			chThdSleepMilliseconds(100);			// allow power supply to stabilize
			GDISP_LLD(write_cmd)(DISON);			// Turn on the display

	#elif defined(GDISP_USE_GE12)
		#if 1
			GDISP_LLD(write_cmd)(SLEEPOUT);		// Sleep out
			GDISP_LLD(write_cmd)(INVON);			// Inversion on: seems to be required for this controller
			GDISP_LLD(write_cmd)(COLMOD);		// Color Interface Pixel Format
			GDISP_LLD(write_data)(0x03);				// 0x03 = 12 bits-per-pixel
			GDISP_LLD(write_cmd)(MADCTL);		// Memory access controler
			GDISP_LLD(write_data)(0xC8);				// 0xC0 = mirror x and y, reverse rgb
			GDISP_LLD(write_cmd)(SETCON);		// Write contrast
			GDISP_LLD(write_data)(GDISP_INITIAL_CONTRAST);	// contrast - experiental value
			chThdSleepMilliseconds(20);
			GDISP_LLD(write_cmd)(DISPON);		// Display On
		#else
			// Alternative
			// Hardware reset commented out
			GDISP_LLD(write_cmd)(SOFTRST);		// Software Reset
			chThdSleepMilliseconds(20);
			GDISP_LLD(write_cmd)(INITESC);		// Initial escape
			chThdSleepMilliseconds(20);
			GDISP_LLD(write_cmd)(REFSET);		// Refresh set
			GDISP_LLD(write_data)(0);
			GDISP_LLD(write_cmd)(DISPCTRL);		// Set Display control
			GDISP_LLD(write_data)(128);					// Set the lenght of one selection term
			GDISP_LLD(write_data)(128);					// Set N inversion -> no N inversion
			GDISP_LLD(write_data)(134);					// Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
			GDISP_LLD(write_data)(84);					// Set duty parameter
			GDISP_LLD(write_data)(69);					// Set duty parameter
			GDISP_LLD(write_data)(82);					// Set duty parameter
			GDISP_LLD(write_data)(67);					// Set duty parameter
			GDISP_LLD(write_cmd)(GRAYSCALE0);	// Grey scale 0 position set - 15 parameters
			GDISP_LLD(write_data)(1);					// GCP1 - gray lavel to be output when the RAM data is "0001"
			GDISP_LLD(write_data)(2);					// GCP2 - gray lavel to be output when the RAM data is "0010"
			GDISP_LLD(write_data)(4);					// GCP3 - gray lavel to be output when the RAM data is "0011"
			GDISP_LLD(write_data)(8);					// GCP4 - gray lavel to be output when the RAM data is "0100"
			GDISP_LLD(write_data)(16);					// GCP5 - gray lavel to be output when the RAM data is "0101"
			GDISP_LLD(write_data)(30);					// GCP6 - gray lavel to be output when the RAM data is "0110"
			GDISP_LLD(write_data)(40);					// GCP7 - gray lavel to be output when the RAM data is "0111"
			GDISP_LLD(write_data)(50);					// GCP8 - gray lavel to be output when the RAM data is "1000"
			GDISP_LLD(write_data)(60);					// GCP9 - gray lavel to be output when the RAM data is "1001"
			GDISP_LLD(write_data)(70);					// GCP10 - gray lavel to be output when the RAM data is "1010"
			GDISP_LLD(write_data)(80);					// GCP11 - gray lavel to be output when the RAM data is "1011"
			GDISP_LLD(write_data)(90);					// GCP12 - gray lavel to be output when the RAM data is "1100"
			GDISP_LLD(write_data)(100);					// GCP13 - gray lavel to be output when the RAM data is "1101"
			GDISP_LLD(write_data)(110);					// GCP14 - gray lavel to be output when the RAM data is "1110"
			GDISP_LLD(write_data)(127);					// GCP15 - gray lavel to be output when the RAM data is "1111"
			GDISP_LLD(write_cmd)(GAMMA);				// Gamma curve set - select gray scale - GRAYSCALE 0 or GREYSCALE 1
			GDISP_LLD(write_data)(1);						// Select grey scale 0
			GDISP_LLD(write_cmd)(COMMONDRV);			// Command driver output
			GDISP_LLD(write_data)(0);						// Set COM1-COM41 side come first, normal mod
			GDISP_LLD(write_cmd)(NORMALMODE);		// Set Normal mode (my)
			// GDISP_LLD(write_cmd)(INVERSIONOFF);	// Inversion off
			GDISP_LLD(write_cmd)(COLADDRSET);		// Column address set
			GDISP_LLD(write_data)(0);
			GDISP_LLD(write_data)(131);
			GDISP_LLD(write_cmd)(PAGEADDRSET);		// Page address set
			GDISP_LLD(write_data)(0);
			GDISP_LLD(write_data)(131);
			GDISP_LLD(write_cmd)(ACCESSCTRL);		// Memory access controler
			GDISP_LLD(write_data)(0x40);						// horizontal
			//GDISP_LLD(write_data)(0x20);					// vertical
			GDISP_LLD(write_cmd)(PWRCTRL);			// Power control
			GDISP_LLD(write_data)(4);						// Internal resistance, V1OUT -> high power mode, oscilator devision rate
			GDISP_LLD(write_cmd)(SLEEPOUT);			// Sleep out
			GDISP_LLD(write_cmd)(VOLTCTRL);			// Voltage control - voltage control and write contrast define LCD electronic volume
			//GDISP_LLD(write_data)(0x7f);					//  full voltage control
			//GDISP_LLD(write_data)(0x03);					//  must be "1"
			GDISP_LLD(write_cmd)(CONTRAST);			// Write contrast
			GDISP_LLD(write_data)(GDISP_INITIAL_CONTRAST);						// contrast
			chThdSleepMilliseconds(20);
			GDISP_LLD(write_cmd)(TEMPGRADIENT);		// Temperature gradient
			for(i=0; i<14; i++) GDISP_LLD(write_data)(0);
			GDISP_LLD(write_cmd)(BOOSTVON);			// Booster voltage ON
			GDISP_LLD(write_cmd)(DISPLAYON);			// Finally - Display On
		#endif
	#endif

	/* Turn on the back-light */
	GDISP_LLD(set_backlight)(GDISP_INITIAL_BACKLIGHT);

	/* Initialise the GDISP structure to match */
	GDISP.Width = GDISP_SCREEN_WIDTH;
	GDISP.Height = GDISP_SCREEN_HEIGHT;
	GDISP.Orientation = GDISP_ROTATE_0;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = GDISP_INITIAL_BACKLIGHT;
	GDISP.Contrast = GDISP_INITIAL_CONTRAST;
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		GDISP.clipx0 = 0;
		GDISP.clipy0 = 0;
		GDISP.clipx1 = GDISP.Width;
		GDISP.clipy1 = GDISP.Height;
	#endif
	return TRUE;
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
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif
	gdisp_lld_setviewport(x, y, 1, 1);
	GDISP_LLD(write_cmd)(RAMWR);
	GDISP_LLD(write_data)(0);
	GDISP_LLD(write_data)((color>>8) & 0x0F);
	GDISP_LLD(write_data)(color & 0xFF);
}

/* ---- Optional Routines ---- */

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a color.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] color    The color of the fill
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		unsigned i, tuples;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		tuples = (cx*cy+1)/2;				// With an odd sized area we over-print by one pixel.
											// This extra pixel is ignored by the controller.

		gdisp_lld_setviewport(x, y, cx, cy);
		GDISP_LLD(write_cmd)(RAMWR);
		for(i=0; i < tuples; i++) {
			GDISP_LLD(write_data)((color >> 4) & 0xFF);
			GDISP_LLD(write_data)(((color << 4) & 0xF0)|((color >> 8) & 0x0F));
			GDISP_LLD(write_data)(color & 0xFF);
		}
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] srcx, srcy   The bitmap position to start the fill from
	 * @param[in] srccx    The width of a line in the bitmap.
	 * @param[in] buffer   The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		coord_t endx, endy, lg;
		color_t	c1, c2;
		#if GDISP_PACKED_PIXELS
			coord_t pos;
			const uint8_t *p;
		#endif

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		/* What are our end points */
		endx = srcx + cx;
		endy = y + cy;

		gdisp_lld_setviewport(x, y, cx, cy);
		GDISP_LLD(write_cmd)(RAMWR);

		#if !GDISP_PACKED_PIXELS
			// Although this controller uses packed pixels we support unpacked pixel
			//  formats in this blit by packing the data as we feed it to the controller.
			lg = srccx - cx;
			buffer += srcy * srccx + srcx;
			x = srcx;
			while (1) {
				/* Get a pixel */
				c1 = *buffer++;
				if (++x >= endx) {
					if (++y >= endy) {
						/* Odd pixel at end */
						GDISP_LLD(write_data)(0);
						GDISP_LLD(write_data)((c1 >> 8) & 0x0F);
						GDISP_LLD(write_data)(c1 & 0xFF);
						break;
					}
					x = srcx;
					buffer += lg;
				}
				/* Get the next pixel */
				c2 = *buffer++;
				GDISP_LLD(write_data)((c1 >> 4) & 0xFF);
				GDISP_LLD(write_data)(((c1 << 4) & 0xF0)|((c2 >> 8) & 0x0F));
				GDISP_LLD(write_data)(c2 & 0xFF);
				if (++x >= endx) {
					if (++y >= endy)
						break;
					x = srcx;
					buffer += lg;
				}
			}

		#else

			// Although this controller uses packed pixels, we may have to feed it into
			//  the controller with different packing to the source bitmap
			#if !GDISP_PACKED_LINES
				srccx = (srccx + 1) & ~1;
			#endif
			pos = srcy*srccx;
			lg = (srccx - cx)/2*3;
			p = ((const uint8_t *)buffer) + ((pos+srcx)/2 * 3);

			x = srcx;
			while (1) {
				/* Get a pixel */
				switch((pos+x)&1) {
				case 0:		c1 = (((color_t)p[0]) << 4)|(((color_t)p[1])>>4);	break;
				case 1:		c1 = (((color_t)p[1]&0x0F) << 8)|((color_t)p[1]);	break;
				}
				if (++x >= endx) {
					if (++y >= endy) {
						/* Odd pixel at end */
						GDISP_LLD(write_data)(0);
						GDISP_LLD(write_data)((c1 >> 8) & 0x0F);
						GDISP_LLD(write_data)(c1 & 0xFF);
						break;
					}
					x = srcx;
					p += lg;
					pos += srccx;
				}
				/* Get the next pixel */
				switch((pos+x)&1) {
				case 0:		c2 = (((color_t)p[0]) << 4)|(((color_t)p[1])>>4);	break;
				case 1:		c2 = (((color_t)p[1]&0x0F) << 8)|((color_t)p[1]);	break;
				}
				GDISP_LLD(write_data)((c1 >> 4) & 0xFF);
				GDISP_LLD(write_data)(((c1 << 4) & 0xF0)|((c2 >> 8) & 0x0F));
				GDISP_LLD(write_data)(c2 & 0xFF);
				if (++x >= endx) {
					if (++y >= endy)
						break;
					x = srcx;
					p += lg;
					pos += srccx;
				}
			}
		#endif
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    If x,y is off the screen, the result is undefined.
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		/* NOT IMPLEMENTED */
		/* Some board hardware might support this in the future.
		 * The Olimex board doesn't.
		 */
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL)
	/**
	 * @brief   Scroll vertically a section of the screen.
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
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		/* NOT IMPLEMENTED */
		/* The hardware seems capable of doing this.
		 * It is just really complex so we leave it out for now.
		 */
	}
#endif

#if GDISP_HARDWARE_CONTROL || defined(__DOXYGEN__)
	/**
	 * @brief   Driver Control
	 * @details	Unsupported control codes are ignored.
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
	void GDISP_LLD(control)(unsigned what, void *value) {
		/* The hardware is capable of supporting...
		 * 	GDISP_CONTROL_POWER				- not implemented yet
		 * 	GDISP_CONTROL_ORIENTATION		- not implemented yet
		 * 	GDISP_CONTROL_BACKLIGHT			- supported (the OlimexSAM7EX256 board.h currently only implements off/on although PWM is supported by the hardware)
		 * 	GDISP_CONTROL_CONTRAST			- supported
		 */
		switch(what) {
#if 0
		// NOT IMPLEMENTED YET
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
				case powerOff:
					// 	Code here
					break;
				case powerOn:
					// 	Code here
					/* You may need this ---
					 *	if (GDISP.Powermode != powerSleep)
					 *		GDISP_LLD(init)();
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
#endif
#if 0
		// NOT IMPLEMENTED YET
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
	//		WriteSpiData(0x48); // no mirror Y (temporary to satisfy Olimex bmptoarray utility)
	//		WriteSpiData(0xC8); // restore to (mirror x and y, reverse rgb)
			switch((gdisp_orientation_t)value) {
				case GDISP_ROTATE_0:
					// 	Code here
					GDISP.Height = GDISP_SCREEN_HEIGHT;
					GDISP.Width = GDISP_SCREEN_WIDTH;
					break;
				case GDISP_ROTATE_90:
					// 	Code here
					GDISP.Height = GDISP_SCREEN_WIDTH;
					GDISP.Width = GDISP_SCREEN_HEIGHT;
					break;
				case GDISP_ROTATE_180:
					// 	Code here
					GDISP.Height = GDISP_SCREEN_HEIGHT;
					GDISP.Width = GDISP_SCREEN_WIDTH;
					break;
				case GDISP_ROTATE_270:
					// 	Code here
					GDISP.Height = GDISP_SCREEN_WIDTH;
					GDISP.Width = GDISP_SCREEN_HEIGHT;
					break;
				default:
					return;
			}
			#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
				GDISP.clipx0 = 0;
				GDISP.clipy0 = 0;
				GDISP.clipx1 = GDISP.Width;
				GDISP.clipy1 = GDISP.Height;
			#endif
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
#endif
		case GDISP_CONTROL_BACKLIGHT:
			if ((unsigned)value > 100) value = (void *)100;
			GDISP_LLD(set_backlight)((uint8_t)(unsigned)value);
			GDISP.Backlight = (unsigned)value;
			return;
		case GDISP_CONTROL_CONTRAST:
			if ((unsigned)value > 100) value = (void *)100;
#if defined(GDISP_USE_GE8)
			GDISP_LLD(write_cmd)(VOLCTR);
			GDISP_LLD(write_data)((unsigned)value);
			GDISP_LLD(write_data)(3);
#elif defined(GDISP_USE_GE12)
			GDISP_LLD(write_cmd)(CONTRAST);
			GDISP_LLD(write_data)((unsigned)value);
#endif
			GDISP.Contrast = (unsigned)value;
			return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */
