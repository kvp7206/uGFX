/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/Nokia6610GE8/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the Nokia6610 GE8 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "gfx.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#include "GE8.h"

/* This controller is only ever used with a 130 x 130 display */
#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif
#define GDISP_SCREEN_HEIGHT		130
#define GDISP_SCREEN_WIDTH		130

#define GDISP_SCAN_LINES		132		/* 130 lines + 2 invisible lines */
#define GDISP_RAM_X_OFFSET		0		/* Offset in RAM of visible area */
#define GDISP_RAM_Y_OFFSET		2		/* Offset in RAM of visible area */
#define GDISP_SLEEP_SIZE		32		/* Sleep mode window lines */
#define GDISP_SLEEP_POS			((GDISP_SCAN_LINES-GDISP_SLEEP_SIZE)/2)

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

#include "gdisp_lld_board.h"

// Some macros just to make reading the code easier
#define delayms(ms)						gfxSleepMilliseconds(ms)
#define write_data2(d1, d2)				{ write_data(d1); write_data(d2); }
#define write_data3(d1, d2, d3)			{ write_data(d1); write_data(d2); write_data(d3); }
#define write_data4(d1, d2, d3, d4)		{ write_data(d1); write_data(d2); write_data(d3); write_data(d4); }
#define write_cmd1(cmd, d1)				{ write_cmd(cmd); write_data(d1); }
#define write_cmd2(cmd, d1, d2)			{ write_cmd(cmd); write_data2(d1, d2); }
#define write_cmd3(cmd, d1, d2, d3)		{ write_cmd(cmd); write_data3(d1, d2, d3); }
#define write_cmd4(cmd, d1, d2, d3, d4)	{ write_cmd(cmd); write_data4(d1, d2, d3, d4); }

// Set the drawing window on the controller.
// An inline function has been used here incase the parameters have side effects with the internal calculations.
static __inline void setviewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {
	switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
			write_cmd2(CASET, GDISP_RAM_X_OFFSET+x, GDISP_RAM_X_OFFSET+x+cx-1);			// Column address set
			write_cmd2(PASET, GDISP_RAM_Y_OFFSET+y, GDISP_RAM_Y_OFFSET+y+cy-1);			// Page address set
			break;
		case GDISP_ROTATE_90:
			write_cmd2(CASET, GDISP_RAM_X_OFFSET+GDISP.Height-y-cy, GDISP_RAM_X_OFFSET+GDISP.Height-y-1);
			write_cmd2(PASET, GDISP_RAM_Y_OFFSET+x, GDISP_RAM_Y_OFFSET+x+cx-1);
			break;
		case GDISP_ROTATE_180:
			write_cmd2(CASET, GDISP_RAM_X_OFFSET+GDISP.Width-x-cx, GDISP_RAM_X_OFFSET+GDISP.Width-x-1);
			write_cmd2(PASET, GDISP_RAM_Y_OFFSET+GDISP.Height-y-cy, GDISP_RAM_Y_OFFSET+GDISP.Height-y-1);
			break;
		case GDISP_ROTATE_270:
			write_cmd2(CASET, GDISP_RAM_X_OFFSET+y, GDISP_RAM_X_OFFSET+y+cy-1);
			write_cmd2(PASET, GDISP_RAM_Y_OFFSET+GDISP.Width-x-cx, GDISP_RAM_Y_OFFSET+GDISP.Width-x-1);
			break;
	}
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level GDISP driver initialisation.
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
	
	write_cmd4(DISCTL, 0x00, GDISP_SCAN_LINES/4-1, 0x0A, 0x00);	// Display control - How the controller drives the LCD
															// P1: 0x00 = 2 divisions, switching period=8 (default)
															// P2: 0x20 = nlines/4 - 1 = 132/4 - 1 = 32)
															// P3: 0x0A = standard inverse highlight, inversion every frame
															// P4: 0x00 = dispersion on
	write_cmd1(COMSCN, 0x01);							// COM scan - How the LCD is connected to the controller
															// P1: 0x01 = Scan 1->80, 160<-81
	write_cmd(OSCON);									// Internal oscillator ON
	write_cmd(SLPOUT);									// Sleep out
	write_cmd1(PWRCTR, 0x0F);							// Power control - reference voltage regulator on, circuit voltage follower on, BOOST ON
	write_cmd3(DATCTL, 0x00, 0x00, 0x02);				// Data control
															// P1: 0x00 = page address normal, column address normal, address scan in column direction
															// P2: 0x00 = RGB sequence (default value)
															// P3: 0x02 = 4 bits per colour (Type A)
	write_cmd2(VOLCTR, GDISP_INITIAL_CONTRAST, 0x03);	// Voltage control (contrast setting)
															// P1 = Contrast
															// P2 = 3 resistance ratio (only value that works)
	delayms(100);										// Allow power supply to stabilise
	write_cmd(DISON);									// Turn on the display

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
		unsigned tuples;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		tuples = (cx*cy+1)>>1;				// With an odd sized area we over-print by one pixel.
											// This extra pixel overwrites the first pixel (harmless as it is the same colour)

		acquire_bus();
		setviewport(x, y, cx, cy);
		write_cmd(RAMWR);
		while(tuples--)
			write_data3(((color >> 4) & 0xFF), (((color << 4) & 0xF0)|((color >> 8) & 0x0F)), (color & 0xFF));
		release_bus();
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 *
	 * @param[in] x, y			The start filled area
	 * @param[in] cx, cy		The width and height to be filled
	 * @param[in] srcx, srcy	The bitmap position to start the fill from
	 * @param[in] srccx			The width of a line in the bitmap.
	 * @param[in] buffer		The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void gdisp_lld_blit_area_ex(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		coord_t		lg;
		color_t		c1, c2;
		unsigned	tuples;
		#if GDISP_PACKED_PIXELS
			unsigned		pnum, pstart;
			const uint8_t	*p;
		#else
			const pixel_t	*p;
		#endif

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		/* Set up the data window to transfer */
		tuples = (cx * cy + 1)>>1;
		acquire_bus();
		setviewport(x, y, cx, cy);
		write_cmd(RAMWR);

		/*
		 * Due to the way the Nokia6610 handles a decrementing column or page,
		 * we have to make adjustments as to where it is actually drawing from in the bitmap.
		 * For example, for 90 degree rotation the column is decremented on each
		 * memory write. The controller always starts with column 0 and then decrements
		 * to column cx-1, cx-2 etc. We therefore have to write-out the last bitmap line first.
		 */
		switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:		x = 0;		y = 0;		break;
		case GDISP_ROTATE_90:		x = 0;		y = cy-1;	break;
		case GDISP_ROTATE_180:		x = cx-1;	y = cy-1;	break;
		case GDISP_ROTATE_270:		x = cx-1;	y = 0;		break;
		}

		#if !GDISP_PACKED_PIXELS
			// Although this controller uses packed pixels we support unpacked pixel
			//  formats in this blit by packing the data as we feed it to the controller.

			lg = srccx - cx;						// The buffer gap between lines
			buffer += srcy * srccx + srcx;			// The buffer start position
			p = buffer + srccx*y + x;				// Adjustment for controller craziness

			while(tuples--) {
				/* Get a pixel */
				c1 = *p++;

				/* Check for line or buffer wrapping */
				if (++x >= cx) {
					x = 0;
					p += lg;
					if (++y >= cy) {
						y = 0;
						p = buffer;
					}
				}

				/* Get the next pixel */
				c2 = *p++;

				/* Check for line or buffer wrapping */
				if (++x >= cx) {
					x = 0;
					p += lg;
					if (++y >= cy) {
						y = 0;
						p = buffer;
					}
				}

				/* Write the pair of pixels to the display */
				write_data3(((c1 >> 4) & 0xFF), (((c1 << 4) & 0xF0)|((c2 >> 8) & 0x0F)), (c2 & 0xFF));
			}

		#else

			// Although this controller uses packed pixels, we may have to feed it into
			//  the controller with different packing to the source bitmap
			// There are 2 pixels per 3 bytes

			#if !GDISP_PACKED_LINES
				srccx = (srccx + 1) & ~1;
			#endif
			pstart = srcy * srccx + srcx;												// The starting pixel number
			buffer = (const pixel_t)(((const uint8_t *)buffer) + ((pstart>>1) * 3));	// The buffer start position
			lg = ((srccx-cx)>>1)*3;														// The buffer gap between lines
			pnum = pstart + srccx*y + x;												// Adjustment for controller craziness
			p = ((const uint8_t *)buffer) + (((srccx*y + x)>>1)*3);						// Adjustment for controller craziness

			while (tuples--) {
				/* Get a pixel */
				switch(pnum++ & 1) {
				case 0:		c1 = (((color_t)p[0]) << 4)|(((color_t)p[1])>>4);				break;
				case 1:		c1 = (((color_t)p[1]&0x0F) << 8)|((color_t)p[1]);	p += 3;		break;
				}

				/* Check for line or buffer wrapping */
				if (++x >= cx) {
					x = 0;
					p += lg;
					pnum += srccx - cx;
					if (++y >= cy) {
						y = 0;
						p = (const uint8_t *)buffer;
						pnum = pstart;
					}
				}

				/* Get the next pixel */
				switch(pnum++ & 1) {
				case 0:		c1 = (((color_t)p[0]) << 4)|(((color_t)p[1])>>4);				break;
				case 1:		c1 = (((color_t)p[1]&0x0F) << 8)|((color_t)p[1]);	p += 3;		break;
				}

				/* Check for line or buffer wrapping */
				if (++x >= cx) {
					x = 0;
					p += lg;
					pnum += srccx - cx;
					if (++y >= cy) {
						y = 0;
						p = (const uint8_t *)buffer;
						pnum = pstart;
					}
				}

				/* Write the pair of pixels to the display */
				write_data3(((c1 >> 4) & 0xFF), (((c1 << 4) & 0xF0)|((c2 >> 8) & 0x0F)), (c2 & 0xFF));
			}
		#endif

		/* All done */
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
		/* This controller does not support reading back over the SPI interface.
		 * Additionally, the Olimex board doesn't even connect the pin.
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
		/**
		 * The hardware is capable of doing full width vertical scrolls aligned
		 * on a 4 line boundary however that is not sufficient to support this routine.
		 *
		 * We also can't manually do read/modify scrolling because we can't read in SPI mode.
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
		 * 	GDISP_CONTROL_POWER				- supported
		 * 	GDISP_CONTROL_ORIENTATION		- supported
		 * 	GDISP_CONTROL_BACKLIGHT			- supported
		 * 	GDISP_CONTROL_CONTRAST			- supported
		 */
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			acquire_bus();
			switch((gdisp_powermode_t)value) {
				case powerOff:
					set_backlight(0);									// Turn off the backlight
					write_cmd(DISOFF);									// Turn off the display
					write_cmd1(PWRCTR, 0x00);							// Power control - all off
					write_cmd(SLPIN);									// Sleep in
					write_cmd(OSCOFF);									// Internal oscillator off
					break;
				case powerOn:
					write_cmd(OSCON);									// Internal oscillator on
					write_cmd(SLPOUT);									// Sleep out
					write_cmd1(PWRCTR, 0x0F);							// Power control - reference voltage regulator on, circuit voltage follower on, BOOST ON
					write_cmd2(VOLCTR, GDISP.Contrast, 0x03);			// Voltage control (contrast setting)
					delayms(100);										// Allow power supply to stabilise
					write_cmd(DISON);									// Turn on the display
					write_cmd(PTLOUT);									// Remove sleep window
					set_backlight(GDISP.Backlight);						// Turn on the backlight
					break;
				case powerSleep:
					write_cmd(OSCON);									// Internal oscillator on
					write_cmd(SLPOUT);									// Sleep out
					write_cmd1(PWRCTR, 0x0F);							// Power control - reference voltage regulator on, circuit voltage follower on, BOOST ON
					write_cmd2(VOLCTR, GDISP.Contrast, 0x03);			// Voltage control (contrast setting)
					delayms(100);										// Allow power supply to stabilise
					write_cmd(DISON);									// Turn on the display
					write_cmd2(PTLIN, GDISP_SLEEP_POS/4, (GDISP_SLEEP_POS+GDISP_SLEEP_SIZE)/4);	// Sleep Window
					set_backlight(GDISP.Backlight);						// Turn on the backlight
					break;
				case powerDeepSleep:
					write_cmd(OSCON);									// Internal oscillator on
					write_cmd(SLPOUT);									// Sleep out
					write_cmd1(PWRCTR, 0x0F);							// Power control - reference voltage regulator on, circuit voltage follower on, BOOST ON
					write_cmd2(VOLCTR, GDISP.Contrast, 0x03);			// Voltage control (contrast setting)
					delayms(100);										// Allow power supply to stabilise
					write_cmd(DISON);									// Turn on the display
					write_cmd2(PTLIN, GDISP_SLEEP_POS/4, (GDISP_SLEEP_POS+GDISP_SLEEP_SIZE)/4);	// Sleep Window
					set_backlight(0);									// Turn off the backlight
					break;
				default:
					release_bus();
					return;
			}
			release_bus();
			GDISP.Powermode = (gdisp_powermode_t)value;
			return;
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
			acquire_bus();
			switch((gdisp_orientation_t)value) {
				case GDISP_ROTATE_0:
					write_cmd3(DATCTL, 0x00, 0x00, 0x02);	// P1: page normal, column normal, scan in column direction
					GDISP.Height = GDISP_SCREEN_HEIGHT;
					GDISP.Width = GDISP_SCREEN_WIDTH;
					break;
				case GDISP_ROTATE_90:
					write_cmd3(DATCTL, 0x06, 0x00, 0x02);	// P1: page normal, column reverse, scan in page direction
					GDISP.Height = GDISP_SCREEN_WIDTH;
					GDISP.Width = GDISP_SCREEN_HEIGHT;
					break;
				case GDISP_ROTATE_180:
					write_cmd3(DATCTL, 0x03, 0x00, 0x02);	// P1: page reverse, column reverse, scan in column direction
					GDISP.Height = GDISP_SCREEN_HEIGHT;
					GDISP.Width = GDISP_SCREEN_WIDTH;
					break;
				case GDISP_ROTATE_270:
					write_cmd3(DATCTL, 0x05, 0x00, 0x02);	// P1: page reverse, column normal, scan in page direction
					GDISP.Height = GDISP_SCREEN_WIDTH;
					GDISP.Width = GDISP_SCREEN_HEIGHT;
					break;
				default:
					release_bus();
					return;
			}
			release_bus();
			#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
				GDISP.clipx0 = 0;
				GDISP.clipy0 = 0;
				GDISP.clipx1 = GDISP.Width;
				GDISP.clipy1 = GDISP.Height;
			#endif
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
		case GDISP_CONTROL_BACKLIGHT:
			if ((unsigned)value > 100) value = (void *)100;
			set_backlight((unsigned)value);
			GDISP.Backlight = (unsigned)value;
			return;
		case GDISP_CONTROL_CONTRAST:
			if ((unsigned)value > 100) value = (void *)100;
			acquire_bus();
			write_cmd2(VOLCTR, (unsigned)value, 0x03);
			release_bus();
			GDISP.Contrast = (unsigned)value;
			return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */
