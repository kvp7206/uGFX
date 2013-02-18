/*
    ChibiOS/GFX - Copyright (C) 2012
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
 * @file    drivers/gdisp/Nokia6610GE12/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the Nokia6610 GE12 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/* Controller definitions */
#include "GE12.h"

/* This controller is only ever used with a 132 x 132 display */
#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
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

#if defined(GDISP_USE_CUSTOM_BOARD) && GDISP_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#elif defined(BOARD_OLIMEX_SAM7_EX256)
	#include "gdisp_lld_board_olimexsam7ex256.h"
#else
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#endif

// Some macros just to make reading the code easier
#define delayms(ms)					chThdSleepMilliseconds(ms)
#define write_data2(d1, d2)			{ write_data(d1); write_data(d2); }
#define write_data3(d1, d2, d3)		{ write_data(d1); write_data(d2); write_data(d3); }
#define write_cmd1(cmd, d1)			{ write_cmd(cmd); write_data(d1); }
#define write_cmd2(cmd, d1, d2)		{ write_cmd(cmd); write_data2(d1, d2); }
#define write_cmd3(cmd, d1, d2, d3)	{ write_cmd(cmd); write_data3(d1, d2, d3); }

// A very common thing to do.
// An inline function has been used here incase the parameters have side effects with the internal calculations.
static __inline void setviewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {
	write_cmd2(CASET, x, x+cx-1);			// Column address set
	write_cmd2(PASET, y, y+cy-1);			// Page address set
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
bool_t gdisp_lld_init(void) {
	/* Initialise your display */
	init_board();

	// Hardware reset
	setpin_reset(TRUE);
	delayms(20);
	setpin_reset(FALSE);
	delayms(20);

	// Get the bus for the following initialisation commands
	acquire_bus();
	
	// UNTESTED
	#if 1
		write_cmd(SLEEPOUT);								// Sleep out
		write_cmd(INVON);									// Inversion on: seems to be required for this controller
		write_cmd1(COLMOD, 0x03);							// Color Interface Pixel Format - 0x03 = 12 bits-per-pixel
		write_cmd1(MADCTL, 0xC8);							// Memory access controler - 0xC0 = mirror x and y, reverse rgb
		write_cmd1(SETCON, GDISP_INITIAL_CONTRAST);			// Write contrast
		delayms(20);
		write_cmd(DISPON);									// Display On
	#else
		// Alternative
		write_cmd(SOFTRST);								// Software Reset
		delayms(20);
		write_cmd(INITESC);								// Initial escape
		delayms(20);
		write_cmd1(REFSET, 0x00);						// Refresh set
		write_cmd(DISPCTRL);							// Set Display control - really 7 bytes of data
			write_data(128);								// Set the lenght of one selection term
			write_data(128);								// Set N inversion -> no N inversion
			write_data(134);								// Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
			write_data(84);									// Set duty parameter
			write_data(69);									// Set duty parameter
			write_data(82);									// Set duty parameter
			write_data(67);									// Set duty parameter
		write_cmd(GRAYSCALE0);							// Grey scale 0 position set - really 15 bytes of data
			write_data(1);									// GCP1 - gray lavel to be output when the RAM data is "0001"
			write_data(2);									// GCP2 - gray lavel to be output when the RAM data is "0010"
			write_data(4);									// GCP3 - gray lavel to be output when the RAM data is "0011"
			write_data(8);									// GCP4 - gray lavel to be output when the RAM data is "0100"
			write_data(16);									// GCP5 - gray lavel to be output when the RAM data is "0101"
			write_data(30);									// GCP6 - gray lavel to be output when the RAM data is "0110"
			write_data(40);									// GCP7 - gray lavel to be output when the RAM data is "0111"
			write_data(50);									// GCP8 - gray lavel to be output when the RAM data is "1000"
			write_data(60);									// GCP9 - gray lavel to be output when the RAM data is "1001"
			write_data(70);									// GCP10 - gray lavel to be output when the RAM data is "1010"
			write_data(80);									// GCP11 - gray lavel to be output when the RAM data is "1011"
			write_data(90);									// GCP12 - gray lavel to be output when the RAM data is "1100"
			write_data(100);								// GCP13 - gray lavel to be output when the RAM data is "1101"
			write_data(110);								// GCP14 - gray lavel to be output when the RAM data is "1110"
			write_data(127);								// GCP15 - gray lavel to be output when the RAM data is "1111"
		write_cmd1(GAMMA, 0x01);						// Gamma curve set - select gray scale - GRAYSCALE 0 or GREYSCALE 1 - Select grey scale 0
		write_cmd1(COMMONDRV, 0x00);					// Command driver output - Set COM1-COM41 side come first, normal mod
		write_cmd(NORMALMODE);							// Set Normal mode (my)
		// write_cmd(INVERSIONOFF);						// Inversion off
		write_cmd2(COLADDRSET, 0, 131);					// Column address set
		write_cmd2(PAGEADDRSET, 0, 131);				// Page address set
		write_cmd1(ACCESSCTRL, 0x40);					// Memory access controler - 0x40 horizontal
		// write_data(0x20);								// vertical
		write_cmd1(PWRCTRL, 0x04);						// Power control - Internal resistance, V1OUT -> high power mode, oscilator devision rate
		write_cmd(SLEEPOUT);							// Sleep out
		write_cmd(VOLTCTRL);							// Voltage control - voltage control and write contrast define LCD electronic volume
		// write_data(0x7f);								//  full voltage control
		// write_data(0x03);								//  must be "1"
		write_cmd1(CONTRAST, GDISP_INITIAL_CONTRAST);	// Write contrast
		delayms(20);
		write_cmd(TEMPGRADIENT);						// Temperature gradient - really 14 bytes of data
		for(i=0; i<14; i++)
			write_data(0);
		write_cmd(BOOSTVON);							// Booster voltage ON
		write_cmd(DISPLAYON);							// Finally - Display On
	#endif

	// Release the bus
	release_bus();
	
	/* Turn on the back-light */
	set_backlight(GDISP_INITIAL_BACKLIGHT);

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
void gdisp_lld_draw_pixel(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif
	acquire_bus();
	setviewport(x, y, 1, 1);
	write_cmd3(RAMWR, 0, (color>>8) & 0x0F, color & 0xFF);
	release_bus();
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
	void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
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

		acquire_bus();
		setviewport(x, y, cx, cy);
		write_cmd(RAMWR);
		for(i=0; i < tuples; i++)
			write_data3(((color >> 4) & 0xFF), (((color << 4) & 0xF0)|((color >> 8) & 0x0F)), (color & 0xFF));
		release_bus();
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
	void gdisp_lld_blit_area_ex(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
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

		acquire_bus();
		setviewport(x, y, cx, cy);
		write_cmd(RAMWR);

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
						write_data3(0, ((c1 >> 8) & 0x0F), (c1 & 0xFF));
						break;
					}
					x = srcx;
					buffer += lg;
				}
				/* Get the next pixel */
				c2 = *buffer++;
				write_data3(((c1 >> 4) & 0xFF), (((c1 << 4) & 0xF0)|((c2 >> 8) & 0x0F)), (c2 & 0xFF));
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
						write_data3(0, ((c1 >> 8) & 0x0F), (c1 & 0xFF));
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
				write_data3(((c1 >> 4) & 0xFF), (((c1 << 4) & 0xF0)|((c2 >> 8) & 0x0F)), (c2 & 0xFF));
				if (++x >= endx) {
					if (++y >= endy)
						break;
					x = srcx;
					p += lg;
					pos += srccx;
				}
			}
		#endif
		release_bus();
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
	color_t gdisp_lld_get_pixel_color(coord_t x, coord_t y) {
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
	void gdisp_lld_vertical_scroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
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
	void gdisp_lld_control(unsigned what, void *value) {
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
					 *		gdisp_lld_init();
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
			set_backlight((unsigned)value);
			GDISP.Backlight = (unsigned)value;
			return;
		case GDISP_CONTROL_CONTRAST:
			if ((unsigned)value > 100) value = (void *)100;
			acquire_bus();
			write_cmd1(CONTRAST,(unsigned)value);
			release_bus();
			GDISP.Contrast = (unsigned)value;
			return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */
