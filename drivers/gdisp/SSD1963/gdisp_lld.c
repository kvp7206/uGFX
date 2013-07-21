/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/SSD1963/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source.
 *
 * @addtogroup GDISP
 * @{
 */

#include "gfx.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

#include "ssd1963.h"

/* All the board specific code should go in these include file so the driver
 * can be ported to another board just by creating a suitable file.
 */
#include "gdisp_lld_board.h"

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
 * @brief   Low level GDISP driver initialisation.
 * @return	TRUE if successful, FALSE on error.
 *
 * @notapi
 */
bool_t gdisp_lld_init(void) {
	/* Initialise the display */

	init_board();
	
	write_index(SSD1963_SOFT_RESET);	
	gfxSleepMicroseconds(100);

	/* Driver PLL config */
	write_index(SSD1963_SET_PLL_MN);
	write_data(35);								 // PLLclk = REFclk (10Mhz) * 36 (360Mhz)
	write_data(2);								 // SYSclk = PLLclk / 3  (120MHz)
	write_data(4);								 // Apply calculation bit, else it is ignored

	write_index(SSD1963_SET_PLL);					// Enable PLL
	write_data(0x01);
	gfxSleepMicroseconds(200);

	write_index(SSD1963_SET_PLL);					// Use PLL
	write_data(0x03);
	gfxSleepMicroseconds(200);

	write_index(SSD1963_SOFT_RESET);	
	gfxSleepMicroseconds(100);

	/* Screen size */
	write_index(SSD1963_SET_GDISP_MODE);
//	write_data(0x0000);
	write_data(0b00011000); //Enabled dithering
	write_data(0x0000);
	write_data(mHIGH((GDISP_SCREEN_WIDTH+1)));
	write_data((GDISP_SCREEN_WIDTH+1));
	write_data(mHIGH((GDISP_SCREEN_HEIGHT+1)));
	write_data((GDISP_SCREEN_HEIGHT+1));
	write_data(0x0000);

	write_index(SSD1963_SET_PIXEL_DATA_INTERFACE);
	write_data(SSD1963_PDI_16BIT565);

	/* LCD Clock specs */
	write_index(SSD1963_SET_LSHIFT_FREQ);
	write_data((GDISP_FPR >> 16) & 0xFF);
	write_data((GDISP_FPR >> 8) & 0xFF);
	write_data(GDISP_FPR & 0xFF);

	write_index(SSD1963_SET_HORI_PERIOD);
	write_data(mHIGH(SCREEN_HSYNC_PERIOD));
	write_data(mLOW(SCREEN_HSYNC_PERIOD));
	write_data(mHIGH((SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH)));
	write_data(mLOW((SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH)));
	write_data(SCREEN_HSYNC_PULSE);
	write_data(0x00);
	write_data(0x00);
	write_data(0x00);

	write_index(SSD1963_SET_VERT_PERIOD);
	write_data(mHIGH(SCREEN_VSYNC_PERIOD));
	write_data(mLOW(SCREEN_VSYNC_PERIOD));
	write_data(mHIGH((SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH)));
	write_data(mLOW((SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH)));
	write_data(SCREEN_VSYNC_PULSE);
	write_data(0x00);
	write_data(0x00);

	/* Tear effect indicator ON. This is used to tell the host MCU when the driver is not refreshing the panel (during front/back porch) */
	write_index(SSD1963_SET_TEAR_ON);
	write_data(0x0000);

	/* Turn on */
	write_index(SSD1963_SET_DISPLAY_ON);
	
	set_backlight(0xE5);//set to 90% brightness
	
	post_init_board();

	/* Initialise the GDISP structure to match */
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

void gdisp_lld_setwindow(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	/* We don't need to validate here as the LLD routines will validate first.
	 *
	 * #if GDISP_NEED_VALIDATION
	 * 	if (x0 >= GDISP.Width || y0 >= GDISP.Height || x0 < 0 || y0 < 0) return;
	 * 	else if (x1 >= GDISP.Width || y1 >= GDISP.Height || y1 < 0 || y2 < 0) return;
	 * #endif
	*/
	write_index(SSD1963_SET_PAGE_ADDRESS);
	write_data((y0 >> 8) & 0xFF);
	write_data((y0 >> 0) & 0xFF);
	write_data((y1 >> 8) & 0xFF);
	write_data((y1 >> 0) & 0xFF);
	write_index(SSD1963_SET_COLUMN_ADDRESS);
	write_data((x0 >> 8) & 0xFF);
	write_data((x0 >> 0) & 0xFF);
	write_data((x1 >> 8) & 0xFF);
	write_data((x1 >> 0) & 0xFF);
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
	
	gdisp_lld_setwindow(x, y, x, y);
	write_index(SSD1963_WRITE_MEMORY_START);
	write_data(color);
}

/* ---- Optional Routines ---- */

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
		uint32_t area;
    
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		
		area = cx*cy;

		gdisp_lld_setwindow(x, y, x+cx-1, y+cy-1);
		write_index(SSD1963_WRITE_MEMORY_START);

		#if defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			uint8_t i;
			dmaStreamSetPeripheral(GDISP_DMA_STREAM, &color);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);  
			for (i = area/65535; i; i--) {
				dmaStreamSetTransactionSize(GDISP_DMA_STREAM, 65535);
				dmaStreamEnable(GDISP_DMA_STREAM);
				dmaWaitCompletion(GDISP_DMA_STREAM);
			}
			dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area%65535);
			dmaStreamEnable(GDISP_DMA_STREAM);
			dmaWaitCompletion(GDISP_DMA_STREAM);
		#else
			uint32_t index;
			for(index = 0; index < area; index++)
				write_data(color);
		#endif  //#ifdef GDISP_USE_DMA
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

		gdisp_lld_setwindow(x, y, x+cx-1, y+cy-1);
		write_index(SSD1963_WRITE_MEMORY_START);

		buffer += srcx + srcy * srccx;
      
		#if defined(GDISP_USE_FSMC) && defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			uint32_t area = cx*cy;
			uint8_t i;
			dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PINC | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);  
			for (i = area/65535; i; i--) {
				dmaStreamSetTransactionSize(GDISP_DMA_STREAM, 65535);
				dmaStreamEnable(GDISP_DMA_STREAM);
				dmaWaitCompletion(GDISP_DMA_STREAM);
			} 
			dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area%65535);
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
		#endif  //#ifdef GDISP_USE_DMA
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
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		/* NOT IMPLEMENTED YET */
		
		/*
		uint16_t size = x1 - x0 ;

		write_index(SSD1963_SET_SCROLL_AREA);
		write_data((x0 >> 8) & 0xFF);
		write_data((x0 >> 0) & 0xFF);
		write_data((size >> 8) & 0xFF);
		write_data((size >> 0) & 0xFF);
		write_data(((lcd_height-x1) >> 8) & 0xFF);
		write_data(((lcd_height-x1) >> 0) & 0xFF);

		write_index(SSD1963_SET_SCROLL_START);
		write_data((lines >> 8) & 0xFF);
		write_data((lines >> 0) & 0xFF);
		*/
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
		/* NOT IMPLEMENTED YET */
		switch(what) {
			case GDISP_CONTROL_POWER:
				if (GDISP.Powermode == (gdisp_powermode_t)value)
					return;
				switch((gdisp_powermode_t)value) {
					case powerOff:
						write_index(SSD1963_EXIT_SLEEP_MODE); // leave sleep mode
						gfxSleepMilliseconds(5);
						write_index(SSD1963_SET_DISPLAY_OFF);
						write_index(SSD1963_SET_DEEP_SLEEP); // enter deep sleep mode
						break;
					case powerOn:
						read_reg(0x0000); gfxSleepMilliseconds(5); // 2x Dummy reads to wake up from deep sleep
						read_reg(0x0000); gfxSleepMilliseconds(5);
						if (GDISP.Powermode != powerSleep)
							gdisp_lld_init();
						write_index(SSD1963_SET_DISPLAY_ON);
	
						break;
					case powerSleep:
						write_index(SSD1963_SET_DISPLAY_OFF);
						write_index(SSD1963_ENTER_SLEEP_MODE); // enter sleep mode
						gfxSleepMilliseconds(5);
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
						/* 	Code here */
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;
					case GDISP_ROTATE_90:
						/* 	Code here */
						GDISP.Height = GDISP_SCREEN_WIDTH;
						GDISP.Width = GDISP_SCREEN_HEIGHT;
						break;
					case GDISP_ROTATE_180:
						/* 	Code here */
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;
					case GDISP_ROTATE_270:
						/* 	Code here */
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
				gdisp_lld_bg_dimmer(54 + ((uint8_t)value) << 1);//turn 0..100% in 54..255
				return;
/*
			case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */

