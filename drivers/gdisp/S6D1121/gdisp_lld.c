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
 * @file    drivers/gdisp/S6D1121/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the S6d1121 display.
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

#if defined(GDISP_SCREEN_HEIGHT)
	#undef GISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_SCREEN_HEIGHT		320
#define GDISP_SCREEN_WIDTH		240

#define GDISP_INITIAL_CONTRAST 		50
#define GDISP_INITIAL_BACKLIGHT 	100

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if defined(GDISP_USE_CUSTOM_BOARD) && GDISP_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#elif defined(BOARD_OLIMEX_STM32_E407)
	#include "gdisp_lld_board_olimex_e407.h"
#else
	#include "gdisp_lld_board.h"
#endif

/* Some common routines and macros */
#define write_reg(reg, data)        { write_index(reg); write_data(data); }
#define stream_start()              write_index(0x0022);
#define stream_stop()
#define delay(us)                   chThdSleepMicroseconds(us)
#define delayms(ms)                 chThdSleepMilliseconds(ms)

static __inline void set_cursor(coord_t x, coord_t y) {
    /* R20h - 8 bit
     * R21h - 9 bit
     */
    switch(GDISP.Orientation) {
        case GDISP_ROTATE_0:
            write_reg(0x0020, x & 0x00FF);
            write_reg(0x0021, y & 0x01FF);
            break;
        case GDISP_ROTATE_90:
            /* Note X has already been mirrored, so we do it directly */
            write_reg(0x0020, y & 0x00FF);
            write_reg(0x0021, x & 0x01FF);
            break;
        case GDISP_ROTATE_180:
            write_reg(0x0020, (GDISP_SCREEN_WIDTH - 1 - x) & 0x00FF);
            write_reg(0x0021, (GDISP_SCREEN_HEIGHT - 1 - y) & 0x01FF);
            break;
        case GDISP_ROTATE_270:
            write_reg(0x0020, (GDISP_SCREEN_WIDTH - 1 - y) & 0x00FF);
            write_reg(0x0021, (GDISP_SCREEN_HEIGHT - 1 - x) & 0x01FF);
            break;
    } 
}

static __inline void set_viewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {
    /* HSA / HEA are 8 bit
     * VSA / VEA are 9 bit
     * use masks 0x00FF and 0x01FF to enforce this
     */

    switch(GDISP.Orientation) {
        case GDISP_ROTATE_0:
            write_reg(0x46, (((x + cx - 1) << 8) & 0xFF00 ) | 
                                      (x & 0x00FF));

            write_reg(0x48, y & 0x01FF);
            write_reg(0x47, (y + cy - 1) & 0x01FF);
            break;
        case GDISP_ROTATE_90:
            write_reg(0x46, (((y + cy - 1) << 8) & 0xFF00) |
                                      (y & 0x00FF));

            write_reg(0x48, x & 0x01FF);
            write_reg(0x47, (x + cx - 1) & 0x01FF);
            break;
        case GDISP_ROTATE_180:
            write_reg(0x46, (((GDISP_SCREEN_WIDTH - x - 1) & 0x00FF) << 8) |
                                      ((GDISP_SCREEN_WIDTH - (x + cx)) & 0x00FF));
            write_reg(0x48, (GDISP_SCREEN_HEIGHT - (y + cy)) & 0x01FF);
            write_reg(0x47, (GDISP_SCREEN_HEIGHT- y - 1) & 0x01FF);
            break;
        case GDISP_ROTATE_270:
            write_reg(0x46, (((GDISP_SCREEN_WIDTH - y - 1) & 0x00FF) << 8) |
                                      ((GDISP_SCREEN_WIDTH - (y + cy)) & 0x00FF));
            write_reg(0x48, (GDISP_SCREEN_HEIGHT - (x + cx)) & 0x01FF);
            write_reg(0x47, (GDISP_SCREEN_HEIGHT - x - 1) & 0x01FF);
            break;
    }   

    set_cursor(x, y);
}

static __inline void reset_viewport(void) {
	switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
		case GDISP_ROTATE_180:
			set_viewport(0, 0, GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT);
			break;
		case GDISP_ROTATE_90:
		case GDISP_ROTATE_270:
			set_viewport(0, 0, GDISP_SCREEN_HEIGHT, GDISP_SCREEN_WIDTH);
			break;
	}
}

bool_t GDISP_LLD(init)(void) {
	/* initialize the hardware */
	init_board();

	/* Hardware reset */
	setpin_reset(TRUE);
	delayms(20);
	setpin_reset(TRUE);
	delayms(20);

	/* Get the bus for the following initialisation commands */
	acquire_bus();

	write_reg(0x11,0x2004);
	write_reg(0x13,0xCC00);
	write_reg(0x15,0x2600);
	write_reg(0x14,0x252A);
	write_reg(0x12,0x0033);
	write_reg(0x13,0xCC04);

	delayms(1);

	write_reg(0x13,0xCC06);

	delayms(1);

	write_reg(0x13,0xCC4F);

	delayms(1);

	write_reg(0x13,0x674F);
	write_reg(0x11,0x2003);

	delayms(1);

	// Gamma Setting
	write_reg(0x30,0x2609);
	write_reg(0x31,0x242C);
	write_reg(0x32,0x1F23);
	write_reg(0x33,0x2425);
	write_reg(0x34,0x2226);
	write_reg(0x35,0x2523);
	write_reg(0x36,0x1C1A);
	write_reg(0x37,0x131D);
	write_reg(0x38,0x0B11);
	write_reg(0x39,0x1210);
	write_reg(0x3A,0x1315);
	write_reg(0x3B,0x3619);
	write_reg(0x3C,0x0D00);
	write_reg(0x3D,0x000D);

	write_reg(0x16,0x0007);
	write_reg(0x02,0x0013);
	write_reg(0x03,0x0003);
	write_reg(0x01,0x0127);

	delayms(1);

	write_reg(0x08,0x0303);
	write_reg(0x0A,0x000B);
	write_reg(0x0B,0x0003);
	write_reg(0x0C,0x0000);
	write_reg(0x41,0x0000);
	write_reg(0x50,0x0000);
	write_reg(0x60,0x0005);
	write_reg(0x70,0x000B);
	write_reg(0x71,0x0000);
	write_reg(0x78,0x0000);
	write_reg(0x7A,0x0000);
	write_reg(0x79,0x0007);
	write_reg(0x07,0x0051);

	delayms(1);

	write_reg(0x07,0x0053);
	write_reg(0x79,0x0000);

	reset_viewport();
	set_backlight(GDISP_INITIAL_BACKLIGHT);

	/* Now initialise the GDISP structure */
	GDISP.Width = GDISP_SCREEN_WIDTH;
	GDISP.Height = GDISP_SCREEN_HEIGHT;
	GDISP.Orientation = GDISP_ROTATE_0;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
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
		reset_viewport();
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
		reset_viewport();
		release_bus();
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
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		/* This routine is marked "DO NOT USE" in the original
		 *  GLCD driver. We just keep our GDISP_HARDWARE_READPIXEL
		 *  turned off for now.
		 */
		color_t color;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif

		aquire_bus();
		set_cursor(x, y);
		stream_start();

		color = lld_lcdReadData();
		color = lld_lcdReadData();

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
		/* This is marked as "TODO: Test this" in the original GLCD driver.
		 * For now we just leave the GDISP_HARDWARE_SCROLL off.
		 */
		static color_t buf[((GDISP_SCREEN_HEIGHT > GDISP_SCREEN_WIDTH ) ? GDISP_SCREEN_HEIGHT : GDISP_SCREEN_WIDTH)];
		coord_t row0, row1;
		unsigned i, gap, abslines;

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
				lld_lcdReadStreamStart();
				lld_lcdReadStream(buf, cx);
				lld_lcdReadStreamStop();

				set_viewport(x, row1, cx, 1);
				stream_start();
				write_data(buf, cx);
				stream_stop();
			}
		}

		/* fill the remaining gap */
		set_viewport(x, lines > 0 ? (y+gap) : y, cx, abslines);
		stream_start();
		gap = cx*abslines;
		for(i = 0; i < gap; i++) write_data(bgcolor);
		stream_stop();
		reset_viewport();
		release_bus();
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
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
				case powerOff:
					/* 	Code here */
					/* break; */
				case powerOn:
					/* 	Code here */
					/* You may need this ---
						if (GDISP.Powermode != powerSleep)
							GDISP_LLD(init();
					*/
					/* break; */
				case powerSleep:
					/* 	Code here */
					/* break; */
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
				write_reg(0x0001,0x0127);
				write_reg(0x03, 0b0011);
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_90:
				write_reg(0x0001,0x0027);
				write_reg(0x0003, 0b1011);
				GDISP.Height = GDISP_SCREEN_WIDTH;
				GDISP.Width = GDISP_SCREEN_HEIGHT;
				break;
			case GDISP_ROTATE_180:
				write_reg(0x0001,0x0127);
				write_reg(0x0003, 0b0000);
				GDISP.Height = GDISP_SCREEN_HEIGHT;
				GDISP.Width = GDISP_SCREEN_WIDTH;
				break;
			case GDISP_ROTATE_270:
				write_reg(0x0001,0x0027);
				write_reg(0x0003, 0b1000);
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

