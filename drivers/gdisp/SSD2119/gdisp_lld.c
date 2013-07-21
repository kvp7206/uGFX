/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/SSD2119/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the SSD2119 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "gfx.h"

#include "ssd2119.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		240
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		320
#endif

#define GDISP_INITIAL_CONTRAST	50
#define GDISP_INITIAL_BACKLIGHT	100

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#include "gdisp_lld_board.h"

// Some common routines and macros
#define write_reg(reg, data)		{ write_index(reg); write_data(data); }
#define stream_start()				write_index(SSD2119_REG_RAM_DATA);
#define stream_stop()
#define delay(us)					gfxSleepMicroseconds(us)
#define delayms(ms)					gfxSleepMilliseconds(ms)

static inline void set_cursor(coord_t x, coord_t y) {
	/* Reg SSD2119_REG_X_RAM_ADDR is 9 bit value
	 * Reg SSD2119_REG_Y_RAM_ADDR is an 8 bit
	 * Use a bit mask to make sure they are not set too high
	 */
	switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
			write_reg(SSD2119_REG_X_RAM_ADDR, x & 0x01FF);
			write_reg(SSD2119_REG_Y_RAM_ADDR, y & 0x00FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(SSD2119_REG_X_RAM_ADDR, (GDISP_SCREEN_WIDTH - y - 1) & 0x01FF);
			write_reg(SSD2119_REG_Y_RAM_ADDR, (GDISP_SCREEN_HEIGHT - x - 1) & 0x00FF);
			break;
		case GDISP_ROTATE_180:
			write_reg(SSD2119_REG_X_RAM_ADDR, (GDISP_SCREEN_WIDTH - 1 - x) & 0x01FF);
			write_reg(SSD2119_REG_Y_RAM_ADDR, (GDISP_SCREEN_HEIGHT - 1 - y) & 0x00FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(SSD2119_REG_X_RAM_ADDR, y & 0x01FF);
			write_reg(SSD2119_REG_Y_RAM_ADDR, x & 0x00FF);
			break;
	}
}

static void set_viewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {

	set_cursor(x, y);

	/* Reg 0x44 - Vertical RAM address position
	 * 		Upper Byte - VEA
	 * 		Lower Byte - VSA
	 * 		0 <= VSA <= VEA <= 0xEF
	 * Reg 0x45,0x46 - Horizontal RAM address position
	 * 		Lower 9 bits gives 0-511 range in each value, HSA and HEA respectively
	 * 		0 <= HSA <= HEA <= 0x13F
	 */

	switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
			write_reg(SSD2119_REG_V_RAM_POS,   (((y + cy - 1) << 8) & 0xFF00 ) | (y & 0x00FF));
			write_reg(SSD2119_REG_H_RAM_START, (x & 0x01FF));
			write_reg(SSD2119_REG_H_RAM_END,   (x + cx - 1) & 0x01FF);
			break;
		case GDISP_ROTATE_90:
			write_reg(SSD2119_REG_V_RAM_POS,   (((GDISP_SCREEN_HEIGHT - x - 1) & 0x00FF) << 8) | ((GDISP_SCREEN_HEIGHT - (x + cx)) & 0x00FF));
			write_reg(SSD2119_REG_H_RAM_START, (GDISP_SCREEN_WIDTH - (y + cy)) & 0x01FF);
			write_reg(SSD2119_REG_H_RAM_END,   (GDISP_SCREEN_WIDTH - y - 1) & 0x01FF);
			break;
		case GDISP_ROTATE_180:
			write_reg(SSD2119_REG_V_RAM_POS,   (((GDISP_SCREEN_HEIGHT - y - 1) & 0x00FF) << 8) | ((GDISP_SCREEN_HEIGHT - (y + cy)) & 0x00FF));
			write_reg(SSD2119_REG_H_RAM_START, (GDISP_SCREEN_WIDTH - (x + cx)) & 0x01FF);
			write_reg(SSD2119_REG_H_RAM_END,   (GDISP_SCREEN_WIDTH - x - 1) & 0x01FF);
			break;
		case GDISP_ROTATE_270:
			write_reg(SSD2119_REG_V_RAM_POS,   (((x + cx - 1) << 8) & 0xFF00 ) | (x & 0x00FF));
			write_reg(SSD2119_REG_H_RAM_START, (y & 0x01FF));
			write_reg(SSD2119_REG_H_RAM_END,   (y + cy - 1) & 0x01FF);
			break;
	}

	set_cursor(x, y);
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

	// Hardware reset
	setpin_reset(TRUE);
	delayms(20);
	setpin_reset(FALSE);
	delayms(20);

	// Get the bus for the following initialisation commands
	acquire_bus();

	// Enter sleep mode (if we are not already there).
	write_reg(SSD2119_REG_SLEEP_MODE_1, 0x0001);
	delay(5);

	// Set initial power parameters.
	write_reg(SSD2119_REG_PWR_CTRL_5, 0x00B2);
	delay(5);
	write_reg(SSD2119_REG_VCOM_OTP_1, 0x0006);
	delay(5);

	// Start the oscillator.
	write_reg(SSD2119_REG_OSC_START, 0x0001);
	delay(5);

	// Set pixel format and basic display orientation (scanning direction).
	write_reg(SSD2119_REG_OUTPUT_CTRL, 0x30EF);
	delay(5);
	write_reg(SSD2119_REG_LCD_DRIVE_AC_CTRL, 0x0600);
	delay(5);

	// Exit sleep mode.
	write_reg(SSD2119_REG_SLEEP_MODE_1, 0x0000);
	delay(5);

	// Configure pixel color format and MCU interface parameters.
	write_reg(SSD2119_REG_ENTRY_MODE, 0x6830); // ENTRY_MODE_DEFAULT
	delay(5);

	// Set analog parameters.
	write_reg(SSD2119_REG_SLEEP_MODE_2, 0x0999);
	delay(5);
	write_reg(SSD2119_REG_ANALOG_SET, 0x3800);
	delay(5);

	// Enable the display.
	write_reg(SSD2119_REG_DISPLAY_CTRL, 0x0033);
	delay(5);

	// Set VCIX2 voltage to 6.1V.
	write_reg(SSD2119_REG_PWR_CTRL_2, 0x0005);
	delay(5);

	// Configure gamma correction.
	write_reg(SSD2119_REG_GAMMA_CTRL_1, 0x0000);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_2, 0x0303);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_3, 0x0407);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_4, 0x0301);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_5, 0x0301);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_6, 0x0403);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_7, 0x0707);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_8, 0x0400);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_9, 0x0a00);
	delay(5);
	write_reg(SSD2119_REG_GAMMA_CTRL_10, 0x1000);
	delay(5);

	// Configure Vlcd63 and VCOMl.
	write_reg(SSD2119_REG_PWR_CTRL_3, 0x000A);
	delay(5);
	write_reg(SSD2119_REG_PWR_CTRL_4, 0x2E00);
	delay(5);

	// Set the display size and ensure that the GRAM window is set to allow access to the full display buffer.
	write_reg(SSD2119_REG_V_RAM_POS, (GDISP_SCREEN_HEIGHT - 1) << 8);
	delay(5);
	write_reg(SSD2119_REG_H_RAM_START, 0x0000);
	delay(5);
	write_reg(SSD2119_REG_H_RAM_END, GDISP_SCREEN_WIDTH - 1);
	delay(5);

	write_reg(SSD2119_REG_X_RAM_ADDR, 0x00);
	delay(5);
	write_reg(SSD2119_REG_Y_RAM_ADDR, 0x00);
	delay(5);

	// Release the bus
	release_bus();

	/* Turn on the backlight */
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
	write_reg(SSD2119_REG_RAM_DATA, color);
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
		unsigned area;

		area = GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT;

		acquire_bus();
		reset_viewport();
		set_cursor(0, 0);
		stream_start();

		#if defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			uint8_t i;
			dmaStreamSetPeripheral(GDISP_DMA_STREAM, &color);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
			for (i = area / 65535; i; i--) {
				dmaStreamSetTransactionSize(GDISP_DMA_STREAM, 65535);
				dmaStreamEnable(GDISP_DMA_STREAM);
				dmaWaitCompletion(GDISP_DMA_STREAM);
			}
			dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area % 65535);
			dmaStreamEnable(GDISP_DMA_STREAM);
			dmaWaitCompletion(GDISP_DMA_STREAM);
		#else
			uint32_t index;
			for(index = 0; index < area; index++)
				write_data(color);
		#endif // defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)

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
		unsigned area;

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

		#if defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			uint8_t i;
				dmaStreamSetPeripheral(GDISP_DMA_STREAM, &color);
				dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
			for (i = area / 65535; i; i--) {
				dmaStreamSetTransactionSize(GDISP_DMA_STREAM, 65535);
				dmaStreamEnable(GDISP_DMA_STREAM);
				dmaWaitCompletion(GDISP_DMA_STREAM);
			}
			dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area % 65535);
			dmaStreamEnable(GDISP_DMA_STREAM);
			dmaWaitCompletion(GDISP_DMA_STREAM);
		#else
			uint32_t index;
			for(index = 0; index < area; index++)
				write_data(color);
		#endif // defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)

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

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		buffer += srcx + srcy * srccx;

		acquire_bus();
		set_viewport(x, y, cx, cy);
		stream_start();

		#if defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			uint32_t area = cx * cy;
			uint8_t i;
			dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PINC | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
			for (i = area / 65535; i; i--) {
				dmaStreamSetTransactionSize(GDISP_DMA_STREAM, 65535);
				dmaStreamEnable(GDISP_DMA_STREAM);
				dmaWaitCompletion(GDISP_DMA_STREAM);
			}
			dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area % 65535);
			dmaStreamEnable(GDISP_DMA_STREAM);
			dmaWaitCompletion(GDISP_DMA_STREAM);
		#else
			coord_t endx, endy;
			uint32_t lg;
			endx = srcx + cx;
			endy = y + cy;
			lg = srccx - cx;
			for(; y < endy; y++, buffer += lg)
				for(x=srcx; x < endx; x++)
					write_data(*buffer++);
		#endif // defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)

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

		color = read_data(); // dummy read
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
			if (x+cx > GDISP.clipx1) cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1) cy = GDISP.clipy1 - y;
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

				j = read_data(); // dummy read
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
	 * @brief	Driver Control
	 * @details	Unsupported control codes are ignored.
	 * @note	The value parameter should always be typecast to (void *).
	 * @note	There are some predefined and some specific to the low level driver.
	 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
	 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
	 * 			GDISP_CONTROL_BACKLIGHT		- Takes an int from 0 to 100. For a driver
	 * 											that only supports off/on anything other
	 * 											than zero is on.
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
						write_reg(SSD2119_REG_SLEEP_MODE_1,	0x0001);	// Enter sleep mode
						write_reg(SSD2119_REG_DISPLAY_CTRL,	0x0000);	// Display off
						write_reg(SSD2119_REG_OSC_START,	0x0000);	// Turn off oscillator
						release_bus();
						set_backlight(0);
						break;

					case powerOn:
						if (GDISP.Powermode == powerSleep) {
							acquire_bus();
							write_reg(SSD2119_REG_SLEEP_MODE_1, 0x0000);	// Leave sleep mode
							write_reg(SSD2119_REG_DISPLAY_CTRL, 0x0033);	// Display on
							release_bus();
							delayms(170);
						} else if (GDISP.Powermode == powerDeepSleep) {
							acquire_bus();
							write_reg(SSD2119_REG_SLEEP_MODE_2, 0x0999);	// Disable deep sleep function
							write_reg(SSD2119_REG_SLEEP_MODE_1, 0x0000);	// Leave sleep mode
							write_reg(SSD2119_REG_DISPLAY_CTRL, 0x0033);	// Display on
							release_bus();
							delayms(170);
						} else {
							acquire_bus();
							write_reg(SSD2119_REG_SLEEP_MODE_1, 0x0000);	// Leave sleep mode
							release_bus();
							gdisp_lld_init();
						}
						set_backlight(100);
						break;

					case powerSleep:
						acquire_bus();
						write_reg(SSD2119_REG_SLEEP_MODE_1, 0x0001);	// Enter sleep mode
						write_reg(SSD2119_REG_DISPLAY_CTRL, 0x0000);	// Display off
						release_bus();
						set_backlight(0);
						delayms(25);
						break;

					case powerDeepSleep:
						acquire_bus();
						write_reg(SSD2119_REG_SLEEP_MODE_1, 0x0001);	// Enter sleep mode
						write_reg(SSD2119_REG_SLEEP_MODE_2, 0x2999);	// Enable deep sleep function
						write_reg(SSD2119_REG_DISPLAY_CTRL, 0x0000);	// Display off
						release_bus();
						set_backlight(0);
						delayms(25);
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
						/* TB = 0 */
						write_reg(SSD2119_REG_OUTPUT_CTRL, 0x30EF);
						/* ID = 11 AM = 0 */
						write_reg(SSD2119_REG_ENTRY_MODE, 0x6830);
						release_bus();
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;

					case GDISP_ROTATE_90:
						acquire_bus();
						/* TB = 1 */
						write_reg(SSD2119_REG_OUTPUT_CTRL, 0x32EF);
						/* ID = 10 AM = 1 */
						write_reg(SSD2119_REG_ENTRY_MODE, 0x6828);
						release_bus();
						GDISP.Height = GDISP_SCREEN_WIDTH;
						GDISP.Width = GDISP_SCREEN_HEIGHT;
						break;

					case GDISP_ROTATE_180:
						acquire_bus();
						/* TB = 0 */
						write_reg(SSD2119_REG_OUTPUT_CTRL, 0x30EF);
						/* ID = 00 AM = 0 */
						write_reg(SSD2119_REG_ENTRY_MODE, 0x6800);
						release_bus();
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;

					case GDISP_ROTATE_270:
						acquire_bus();
						/* TB = 1 */
						write_reg(SSD2119_REG_OUTPUT_CTRL, 0x32EF);
						/* ID = 01 AM = 1 */
						write_reg(SSD2119_REG_ENTRY_MODE, 0x6818);
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

			case GDISP_CONTROL_BACKLIGHT:
				if ((unsigned)value > 100)
					value = (void *)100;
				set_backlight((unsigned)value);
				GDISP.Backlight = (unsigned)value;
				return;

			default:
				return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */
