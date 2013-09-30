/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/ILI9481/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for
 * 			the ILI9481 and compatible HVGA display
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

#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_SCREEN_HEIGHT		480
#define GDISP_SCREEN_WIDTH		320

#define GDISP_INITIAL_CONTRAST	50
#define GDISP_INITIAL_BACKLIGHT	100

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#include "gdisp_lld_board.h"

// Some common routines and macros
#define write_reg(reg, data)		{ write_index(reg); write_data(data); }
#define stream_start()				write_index(0x2C);
#define stream_stop()
#define delay(us)					gfxSleepMicroseconds(us)
#define delayms(ms)					gfxSleepMilliseconds(ms)

static inline void set_cursor(coord_t x, coord_t y) {
	write_index(0x2A);
	write_data((x >> 8));
	write_data((uint8_t) x);
	write_data((x) >> 8);
	write_data((uint8_t) (x));

	write_index(0x2B);
	write_data((y >> 8));
	write_data((uint8_t) y);
	write_data((y) >> 8);
	write_data((uint8_t) (y));
}

static void set_viewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {
	write_index(0x2A);
	write_data((x >> 8));
	write_data((uint8_t) x);
	write_data((x + cx - 1) >> 8);
	write_data((uint8_t) (x + cx - 1));

	write_index(0x2B);
	write_data((y >> 8));
	write_data((uint8_t) y);
	write_data((y + cy - 1) >> 8);
	write_data((uint8_t) (y + cy - 1));
}

static inline void reset_viewport(void) {
	set_viewport(0, 0, GDISP.Width, GDISP.Height);
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

	/* Hardware reset */
	setpin_reset(TRUE);
	delayms(20);
	setpin_reset(FALSE);
	delayms(20);

	/* Get the bus for the following initialisation commands */
	acquire_bus();

	/* Enable Access to all Manufacturer commands (0xB0 and higher opcodes) */
	write_index(0xB0);
	write_data(0x00);

	/* Frame Memory Access and Interface Setting */
	write_index(0xB3);
	write_data(0x02);
	write_data(0x00);
	write_data(0x00);
	write_data(0x10);

	/* Display Mode and Frame Memory Write Mode Setting (B4h) */
	/* Use internal clock for synchronization */
	/* Use DBI interface (only DB0-17, no HSYNC, VSYNC or CLK) */
	write_index(0xB4);
	write_data(0x00);

	/* Internal Backlight Control */
/*	write_index(0xB9); 	/*PWM Settings for Brightness Control */
/*	write_data(0x01);	/* Disabled by default. */
/*	write_data(0xFF);	/*0xFF = Max brightness */
/*	write_data(0xFF);
/*	write_data(0x18);

	/* Panel Driving settings */
	write_index(0xC0);
	write_data(0x03);
	write_data(0x3B);
	write_data(0x00);
	write_data(0x00);
	write_data(0x00);
	write_data(0x01);
	write_data(0x00);	/* NW */
	write_data(0x43);

	/* Display timings in Operating Mode */
	write_index(0xC1);
	write_data(0x08);
	write_data(0x15);	/* CLOCK */
	write_data(0x08);
	write_data(0x08);

	/* S/VCOM/Gate Driving timing setting */
	write_index(0xC4);
	write_data(0x15);
	write_data(0x03);
	write_data(0x03);
	write_data(0x01);

	/* Interface Setting */
	write_index(0xC6);
	write_data(0x02);

	/* Gamma Setting - should be changed if using a different panel */
	write_index(0xC8);
	write_data(0x0C);
	write_data(0x05);
	write_data(0x0A); /*0X12 */
	write_data(0x6B); /*0x7D */
	write_data(0x04);
	write_data(0x06); /*0x08 */
	write_data(0x15); /*0x0A */
	write_data(0x10);
	write_data(0x00);
	write_data(0x60); /*0x23 */

	/* Address Mode setting */
	write_index(0x36);
	write_data(0x00);

	/* Set Pixel Format = 16 bits per pixel */
	/* The driver supports upto 24 bits per pixel, with dither */
	write_index(0x3A);
	write_data(0x55);

	/* Exit Idle Mode */
	write_index(0x38);

	/* Power Setting */
	write_index(0xD0);
	write_data(0x07);
	write_data(0x07); 	/* VCI = VCI1 */
	write_data(0x14);	/* VRH 0x1D */
	write_data(0xA2);	/* BT 0x06 */

	/* VCOM Setting */
	write_index(0xD1);
	write_data(0x03);
	write_data(0x5A);	/* VCM  0x5A */
	write_data(0x10);	/* VDV */

	/* Power Setting for Normal Mode  */
	write_index(0xD2);
	write_data(0x03);
	write_data(0x04);	/* 0x24  */
	write_data(0x04);

	/* Exit Sleep Mode */
	write_index(0x11);
	delay(150);

	/* Display ON  */
	write_index(0x29);
	delay(30);

 	/* Release the bus */
	release_bus();
	
	/* Turn on the back-light */
	set_backlight(GDISP_INITIAL_BACKLIGHT);

   /* Initialise the GDISP structure */
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

	set_cursor(x, y);
	write_reg(0x002c, color);

	release_bus();
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
		eg. If gfillarea() is defined there is little
			point in defining clear() unless the
			performance bonus is significant.
	For good performance it is suggested to implement
		fillarea() and blitarea().
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
		unsigned i;

		acquire_bus();
		reset_viewport();
		stream_start();
		for(i = 0; i < GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT; i++)
			write_data(color);
		stream_stop();
		release_bus();
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
	void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		unsigned i, area;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		area = cx*cy;

		acquire_bus();

		set_viewport(x, y, cx, cy);

		stream_start();
		for(i = 0; i < area; i++)
			write_data(color);
		stream_stop();

		release_bus();
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 * @note    Optional - The high level driver can emulate using software.
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
		coord_t endx, endy;
		unsigned lg;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		acquire_bus();
		set_viewport(x, y, cx, cy);
		stream_start();

		endx = srcx + cx;
		endy = y + cy;
		lg = srccx - cx;
		buffer += srcx + srcy * srccx;
		for(; y < endy; y++, buffer += lg)
			for(x=srcx; x < endx; x++)
				write_data(*buffer++);
		stream_stop();
		release_bus();
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    Optional.
	 * @note    If x,y is off the screen, the result is undefined.
	 *
	 * @param[in] x, y     The pixel to be read
	 *
	 * @notapi
	 */
	color_t gdisp_lld_get_pixel_color(coord_t x, coord_t y) {
		color_t color;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif

		acquire_bus();
		set_cursor(x, y);
		stream_start();
		color = read_data();			// dummy read
		color = read_data();
		stream_stop();
		release_bus();

		return color;
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
	void gdisp_lld_vertical_scroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		static color_t buf[((GDISP_SCREEN_HEIGHT > GDISP_SCREEN_WIDTH ) ? GDISP_SCREEN_HEIGHT : GDISP_SCREEN_WIDTH)];
		coord_t row0, row1;
		unsigned i, gap, abslines, j;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		abslines = lines < 0 ? -lines : lines;

		acquire_bus();
		if ((coord_t)abslines >= cy) {
			abslines = cy;
			gap = 0;
		} else {
			gap = cy - abslines;
			for(i = 0; i < gap; i++) {
				if(lines > 0) {
					row0 = y + i + lines;
					row1 = y + i;
				} else {
					row0 = (y - i - 1) + lines;
					row1 = (y - i - 1);
				}

				/* read row0 into the buffer and then write at row1*/
				set_viewport(x, row0, cx, 1);
				stream_start();
				j = read_data();			// dummy read
				for (j = 0; (coord_t)j < cx; j++)
					buf[j] = read_data();
				stream_stop();

				set_viewport(x, row1, cx, 1);
				stream_start();
				for (j = 0; (coord_t)j < cx; j++)
					write_data(buf[j]);
				stream_stop();
			}
		}

		/* fill the remaining gap */
		set_viewport(x, lines > 0 ? (y+(coord_t)gap) : y, cx, abslines);
		stream_start();
		gap = cx*abslines;
		for(i = 0; i < gap; i++) write_data(bgcolor);
		stream_stop();
		release_bus();
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL) || defined(__DOXYGEN__)
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
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
			case powerOff:
			  acquire_bus();
			  write_reg(0x0010, 0x0001);	/* enter sleep mode */
			  release_bus();
				break;
			case powerOn:
				acquire_bus();
				write_reg(0x0010, 0x0000);	/* leave sleep mode */
				release_bus();
				if (GDISP.Powermode != powerSleep)
					gdisp_lld_init();
				break;
			case powerSleep:
				acquire_bus();
				write_reg(0x0010, 0x0001);	/* enter sleep mode */
				release_bus();
				break;
			default:
				return;
			}
			GDISP.Powermode = (gdisp_powermode_t)value;
			return;
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
			switch((gdisp_orientation_t)value) {
			case GDISP_ROTATE_0:
				acquire_bus();

				write_reg(0xC0, 0x03);
				write_reg(0x36, 0x00);	/* X and Y axes non-inverted */

				release_bus();
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				acquire_bus();

				write_reg(0xC0, 0x02);
				write_reg(0x36, 0x20);	/* Invert X and Y axes */

				release_bus();
				GDISP.Height = GDISP_SCREEN_WIDTH;
				GDISP.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				acquire_bus();

				write_reg(0xC0, 0x06);
				write_reg(0x36, 0x00);	/* X and Y axes non-inverted */

				release_bus();
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				acquire_bus();

				write_reg(0xC0, 0x07);
				write_reg(0x36, 0x20);	/* Invert X and Y axes */

				release_bus();
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
/*
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */
