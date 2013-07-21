/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/HX8347D/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the HX8347D display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "gfx.h"

#include "HX8347D.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

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
#define GDISP_INITIAL_BACKLIGHT	50

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#include "gdisp_lld_board.h"

// Some common routines and macros
#define write_reg(reg, data)		{ write_index(reg); write_data(data); }
#define write_ram(color1, color2)   { write_index(0x22); write_ram8(color1,color2); }
#define stream_start()				{ write_index(0x22); spiStart(&SPID1, &spi1cfg2);      }
#define stream_stop()               {while(((SPI1->SR & SPI_SR_TXE) == 0) || ((SPI1->SR & SPI_SR_BSY) != 0));palSetPad(GPIOA, 4);spiStart(&SPID1, &spi1cfg1); }
#define delay(us)					gfxSleepMicroseconds(us)
#define delayms(ms)					gfxSleepMilliseconds(ms)

static inline void set_cursor(coord_t x, coord_t y) {
			write_reg(HX8347D_REG_SCL, (uint8_t) x);
		    write_reg(HX8347D_REG_SCH, (uint8_t) (x >> 8));
		    write_reg(HX8347D_REG_SPL, (uint8_t) y);
		    write_reg(HX8347D_REG_SPH, (uint8_t) (y >> 8));
}

static void set_viewport(coord_t x, coord_t y, coord_t cx, coord_t cy) {
		    write_reg(HX8347D_REG_SCL, (uint8_t) x);
		    write_reg(HX8347D_REG_SCH, (uint8_t) (x >> 8));
		    write_reg(HX8347D_REG_ECL, (uint8_t) (x + cx -1));
		    write_reg(HX8347D_REG_ECH, (uint8_t) ((x + cx -1) >> 8));
		    write_reg(HX8347D_REG_SPL, (uint8_t) y);
		    write_reg(HX8347D_REG_SPH, (uint8_t) (y >> 8));
		    write_reg(HX8347D_REG_EPL, (uint8_t) (y + cy -1));
		    write_reg(HX8347D_REG_EPH, (uint8_t) ((y + cy -1) >> 8));
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
	delayms(1);
	setpin_reset(FALSE);
	delayms(5);

	// Get the bus for the following initialisation commands
	acquire_bus();

	/* Start Initial Sequence ----------------------------------------------------*/
	write_reg(HX8347D_REG_STBAH, 0x00);                 /* Reset Power Control 1                */
	write_reg(HX8347D_REG_STBAL, 0x20);                 /* Power Control 2                      */
	write_reg(HX8347D_REG_PTBAH, 0x0C);                 /* Power Control 1                      */
	write_reg(HX8347D_REG_PTBAL, 0xC4);                 /* Power Control 2                      */
	write_reg(HX8347D_REG_OPONN, 0x40);                 /* Source OPON_N                        */
	write_reg(HX8347D_REG_OPONI, 0x38);                 /* Source OPON_I                        */
	write_reg(HX8347D_REG_DC2, 0xA3);                   /* Display Control 2                    */

	/* Power On sequence ---------------------------------------------------------*/
	write_reg(HX8347D_REG_PWC2, 0x1B);                  /* Power Control 2                      */
	write_reg(HX8347D_REG_PWC1, 0x01);                  /* Power Control 1                      */
	write_reg(HX8347D_REG_VMH, 0x2F);                   /* Vcom Control 2                       */
	write_reg(HX8347D_REG_VML, 0x57);                   /* Vcom Control 3                       */
	write_reg(HX8347D_REG_VMF, 0x8D);                   /* Vcom Control 1                       */

	/* Gamma settings  -----------------------------------------------------------*/
	write_reg(HX8347D_REG_VRP0,0x01);   //   default setup
	write_reg(HX8347D_REG_VRP1,0x0e);   //
	write_reg(HX8347D_REG_VRP2,0x11);   //
	write_reg(HX8347D_REG_VRP3,0x1a);   //
	write_reg(HX8347D_REG_VRP4,0x18);   //
	write_reg(HX8347D_REG_VRP5,0x24);   //
	write_reg(HX8347D_REG_PRP0,0x15);   //
	write_reg(HX8347D_REG_PRP1,0x65);   //
	write_reg(HX8347D_REG_PKP0,0x0b);   //
	write_reg(HX8347D_REG_PKP1,0x18);   //
	write_reg(HX8347D_REG_PKP2,0x19);   //
	write_reg(HX8347D_REG_PKP3,0x1a);   //
	write_reg(HX8347D_REG_PKP4,0x18);   //
	write_reg(HX8347D_REG_VRN0,0x1b);   //
	write_reg(HX8347D_REG_VRN1,0x27);   //
	write_reg(HX8347D_REG_VRN2,0x25);   //
	write_reg(HX8347D_REG_VRN3,0x2e);   //
	write_reg(HX8347D_REG_VRN4,0x31);   //
	write_reg(HX8347D_REG_VRN5,0x3e);   //
	write_reg(HX8347D_REG_PRN0,0x1a);   //
	write_reg(HX8347D_REG_PRN1,0x6a);   //
	write_reg(HX8347D_REG_PKN0,0x07);   //
	write_reg(HX8347D_REG_PKN1,0x05);   //
	write_reg(HX8347D_REG_PKN2,0x06);   //
	write_reg(HX8347D_REG_PKN3,0x0b);   //
	write_reg(HX8347D_REG_PKN4,0x14);   //
	write_reg(HX8347D_REG_CGM,0xcc);    //

	/* Power + Osc ---------------------------------------------------------------*/
	write_reg(HX8347D_REG_OSCCH, 0x36);                  /* OSC Control 1                        */
	write_reg(HX8347D_REG_OSCCL, 0x01);                  /* OSC Control 2                        */
	write_reg(HX8347D_REG_DMODE, 0x00);                  /* Display Mode Control                 */
	write_reg(HX8347D_REG_PWC6, 0x88);                   /* Power Control 6                      */
	delayms(5);                                          /* Delay 5 ms                           */
	write_reg(HX8347D_REG_PWC6, 0x80);                   /* Power Control 6                      */
	delayms(5);                                          /* Delay 5 ms                           */
	write_reg(HX8347D_REG_PWC6, 0x90);                   /* Power Control 6                      */
	delayms(5);                                          /* Delay 5 ms                           */
	write_reg(HX8347D_REG_PWC6, 0xD0);                   /* Power Control 6                      */
	delayms(5);                                          /* Delay 5 ms                           */
	write_reg(HX8347D_REG_COLMOD, 0x05);                 /* Colmod 16Bit/Pixel                   */
	write_reg(HX8347D_REG_PCH, 0x00);                    /* Panel Characteristic                 */
	write_reg(HX8347D_REG_DC3, 0x38);                    /* Display Control 3                    */
	delayms(40);
	write_reg(HX8347D_REG_DC3, 0x3C);                    /* Display Control 3                    */
	write_reg(HX8347D_REG_MAC, 0x08);                    /* Memory access control                */

	write_reg(HX8347D_REG_SCL, 0x00);
	write_reg(HX8347D_REG_SCH, 0x00);
	write_reg(HX8347D_REG_ECL, 0xef);
	write_reg(HX8347D_REG_ECH, 0x00);
	write_reg(HX8347D_REG_SPL, 0x00);
	write_reg(HX8347D_REG_SPH, 0x00);
	write_reg(HX8347D_REG_EPL, 0x3f);
	write_reg(HX8347D_REG_EPH, 0x01);

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
	write_ram((color >> 8) & 0xFF, color & 0xFF);
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
		  write_ram16(color);
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
		  write_ram16(color);
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
		unsigned i, abslines, j;
		static int gap;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

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
			case GDISP_CONTROL_ORIENTATION:
				if (GDISP.Orientation == (gdisp_orientation_t)value)
					return;
				switch((gdisp_orientation_t)value) {
					case GDISP_ROTATE_0:
						acquire_bus();
						write_reg(HX8347D_REG_MAC, 0x08);                    /* Memory access control */
		                write_reg(HX8347D_REG_ECL, 0xef);
		                write_reg(HX8347D_REG_ECH, 0x00);
		                write_reg(HX8347D_REG_EPL, 0x3f);
		                write_reg(HX8347D_REG_EPH, 0x01);
						release_bus();
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;

					case GDISP_ROTATE_90:
						acquire_bus();
						write_reg(HX8347D_REG_MAC, 0x68);                    /* Memory access control */
                        write_reg(HX8347D_REG_ECL, 0x3f);
                        write_reg(HX8347D_REG_ECH, 0x01);
                        write_reg(HX8347D_REG_EPL, 0xef);
                        write_reg(HX8347D_REG_EPH, 0x00);
						release_bus();
						GDISP.Height = GDISP_SCREEN_WIDTH;
						GDISP.Width = GDISP_SCREEN_HEIGHT;
						break;

					case GDISP_ROTATE_180:
						acquire_bus();
						write_reg(HX8347D_REG_MAC, 0xc8);                    /* Memory access control */
		                write_reg(HX8347D_REG_ECL, 0xef);
		                write_reg(HX8347D_REG_ECH, 0x00);
		                write_reg(HX8347D_REG_EPL, 0x3f);
		                write_reg(HX8347D_REG_EPH, 0x01);
						release_bus();
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;

					case GDISP_ROTATE_270:
						acquire_bus();
						write_reg(HX8347D_REG_MAC, 0xa8);                    /* Memory access control */
                        write_reg(HX8347D_REG_ECL, 0x3f);
                        write_reg(HX8347D_REG_ECH, 0x01);
                        write_reg(HX8347D_REG_EPL, 0xef);
                        write_reg(HX8347D_REG_EPH, 0x00);
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
