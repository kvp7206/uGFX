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
 * @file    drivers/gdisp/SSD1289/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the SSD1289 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "lld/gdisp/emulation.c"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		320
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		240
#endif

#define GDISP_INITIAL_CONTRAST	50
#define GDISP_INITIAL_BACKLIGHT	100

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if defined(GDISP_USE_CUSTOM_BOARD) && GDISP_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#elif defined(BOARD_FIREBULL_STM32_F103)
	#include "gdisp_lld_board_firebullstm32f103.h"
#else
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#endif

// Some common routines and macros
#define write_reg(reg, data)		{ write_index(reg); write_data(data); }
#define stream_start()				write_index(0x0022);
#define stream_stop()
#define delay(us)					chThdSleepMicroseconds(us)
#define delayms(ms)					chThdSleepMilliseconds(ms)

static __inline void set_cursor(coord_t x, coord_t y) {
	/* Reg 0x004E is an 8 bit value
	 * Reg 0x004F is 9 bit
	 * Use a bit mask to make sure they are not set too high
	 */
	switch(GDISP.Orientation) {
		case GDISP_ROTATE_180:
			write_reg(0x004e, (GDISP_SCREEN_WIDTH-1-x) & 0x00FF);
			write_reg(0x004f, (GDISP_SCREEN_HEIGHT-1-y) & 0x01FF);
			break;
		case GDISP_ROTATE_0:
			write_reg(0x004e, x & 0x00FF);
			write_reg(0x004f, y & 0x01FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(0x004e, y & 0x00FF);
			write_reg(0x004f, x & 0x01FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(0x004e, (GDISP_SCREEN_WIDTH - y - 1) & 0x00FF);
			write_reg(0x004f, (GDISP_SCREEN_HEIGHT - x - 1) & 0x01FF);
			break;
	}
}

static __inline void set_viewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {

	set_cursor(x, y);

	/* Reg 0x44 - Horizontal RAM address position
	 * 		Upper Byte - HEA
	 * 		Lower Byte - HSA
	 * 		0 <= HSA <= HEA <= 0xEF
	 * Reg 0x45,0x46 - Vertical RAM address position
	 * 		Lower 9 bits gives 0-511 range in each value
	 * 		0 <= Reg(0x45) <= Reg(0x46) <= 0x13F
	 */

	switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
			write_reg(0x44, (((x+cx-1) << 8) & 0xFF00 ) | (x & 0x00FF));
			write_reg(0x45, y & 0x01FF);
			write_reg(0x46, (y+cy-1) & 0x01FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(0x44, (((x+cx-1) << 8) & 0xFF00 ) | (y & 0x00FF));
			write_reg(0x45, x & 0x01FF);
			write_reg(0x46, (x+cx-1) & 0x01FF);
			break;
		case GDISP_ROTATE_180:
			write_reg(0x44, (((GDISP_SCREEN_WIDTH-x-1) & 0x00FF) << 8) | ((GDISP_SCREEN_WIDTH - (x+cx)) & 0x00FF));
			write_reg(0x45, (GDISP_SCREEN_HEIGHT-(y+cy)) & 0x01FF);
			write_reg(0x46, (GDISP_SCREEN_HEIGHT-y-1) & 0x01FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(0x44, (((GDISP_SCREEN_WIDTH - y - 1) & 0x00FF) << 8) | ((GDISP_SCREEN_WIDTH - (y+cy)) & 0x00FF));
			write_reg(0x45, (GDISP_SCREEN_HEIGHT - (x+cx)) & 0x01FF);
			write_reg(0x46, (GDISP_SCREEN_HEIGHT - x - 1) & 0x01FF);
			break;
	}

	set_cursor(x, y);
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
	init_board();

	// Hardware reset
	setpin_reset(TRUE);
	delayms(20);
	setpin_reset(FALSE);
	delayms(20);

	// Get the bus for the following initialisation commands
	acquire_bus();
	
	write_reg(0x0000,0x0001);		delay(5);
    write_reg(0x0003,0xA8A4);    	delay(5);
    write_reg(0x000C,0x0000);    	delay(5);
    write_reg(0x000D,0x080C);    	delay(5);
    write_reg(0x000E,0x2B00);    	delay(5);
    write_reg(0x001E,0x00B0);    	delay(5);
	write_reg(0x0001,0x2B3F);		delay(5);
    write_reg(0x0002,0x0600);    	delay(5);
    write_reg(0x0010,0x0000);    	delay(5);
    write_reg(0x0011,0x6070);    	delay(5);
    write_reg(0x0005,0x0000);    	delay(5);
    write_reg(0x0006,0x0000);    	delay(5);
    write_reg(0x0016,0xEF1C);    	delay(5);
    write_reg(0x0017,0x0003);    	delay(5);
    write_reg(0x0007,0x0133);    	delay(5);
    write_reg(0x000B,0x0000);    	delay(5);
    write_reg(0x000F,0x0000);    	delay(5);
    write_reg(0x0041,0x0000);    	delay(5);
    write_reg(0x0042,0x0000);    	delay(5);
    write_reg(0x0048,0x0000);    	delay(5);
    write_reg(0x0049,0x013F);    	delay(5);
    write_reg(0x004A,0x0000);    	delay(5);
    write_reg(0x004B,0x0000);    	delay(5);
    write_reg(0x0044,0xEF00);    	delay(5);
    write_reg(0x0045,0x0000);    	delay(5);
    write_reg(0x0046,0x013F);    	delay(5);
    write_reg(0x0030,0x0707);    	delay(5);
    write_reg(0x0031,0x0204);    	delay(5);
    write_reg(0x0032,0x0204);    	delay(5);
    write_reg(0x0033,0x0502);    	delay(5);
    write_reg(0x0034,0x0507);    	delay(5);
    write_reg(0x0035,0x0204);    	delay(5);
    write_reg(0x0036,0x0204);    	delay(5);
    write_reg(0x0037,0x0502);    	delay(5);
    write_reg(0x003A,0x0302);    	delay(5);
    write_reg(0x003B,0x0302);    	delay(5);
    write_reg(0x0023,0x0000);    	delay(5);
    write_reg(0x0024,0x0000);    	delay(5);
    write_reg(0x0025,0x8000);    	delay(5);
    write_reg(0x004f,0x0000);		delay(5);
    write_reg(0x004e,0x0000);		delay(5);

 	// Release the bus
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
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif
	
	acquire_bus();
	set_cursor(x, y);
	write_reg(0x0022, color);
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
	void GDISP_LLD(clear)(color_t color) {
	    unsigned i;

		acquire_bus();
	    set_cursor(0, 0);
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
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
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
	void GDISP_LLD(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
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
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
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
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
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
		if (abslines >= cy) {
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
				for (j = 0; j < cx; j++)
					buf[j] = read_data();
				stream_stop();

				set_viewport(x, row1, cx, 1);
				stream_start();
				for (j = 0; j < cx; j++)
					write_data(buf[j]);
				stream_stop();
			}
		}

		/* fill the remaining gap */
		set_viewport(x, lines > 0 ? (y+gap) : y, cx, abslines);
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
	void GDISP_LLD(control)(unsigned what, void *value) {
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
			case powerOff:
				write_reg(0x0010, 0x0000);	// leave sleep mode
				write_reg(0x0007, 0x0000);	// halt operation
				write_reg(0x0000, 0x0000);	// turn off oszillator
				write_reg(0x0010, 0x0001);	// enter sleepmode
				break;
			case powerOn:
				write_reg(0x0010, 0x0000);	// leave sleep mode
				if (GDISP.Powermode != powerSleep)
					GDISP_LLD(init)();
				break;
			case powerSleep:
				write_reg(0x0010, 0x0001);	// enter sleep mode
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
				write_reg(0x0001, 0x2B3F);
				/* ID = 11 AM = 0 */
				write_reg(0x0011, 0x6070);
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				write_reg(0x0001, 0x293F);
				/* ID = 11 AM = 1 */
				write_reg(0x0011, 0x6078);
				GDISP.Height = GDISP_SCREEN_WIDTH;
				GDISP.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				write_reg(0x0001, 0x2B3F);
				/* ID = 01 AM = 0 */
				write_reg(0x0011, 0x6040);
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				write_reg(0x0001, 0x293F);
				/* ID = 01 AM = 1 */
				write_reg(0x0011, 0x6048);
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
