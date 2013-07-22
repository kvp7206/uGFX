/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/RA8875/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source.
 *
 * @addtogroup GDISP
 * @{
 */

#include "gfx.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

#include "ra8875.h"

/* include the board abstraction */
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

	/* soft reset */
	write_index(0x01);
	write_data(0x01);
	write_data(0x00);
	gfxSleepMilliseconds(1);

	/* Driver PLL config 480x272*/
	write_index(0x88);
	write_data(0x08);
	gfxSleepMilliseconds(1);
	 write_index(0x89);
	write_data(0x02);
	gfxSleepMilliseconds(1);

	write_index(0x10);	//SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8bit MPU interface
	write_data(0x0F);	// if 16bit MCU interface   and 65k color display

	write_index(0x04);	//set PCLK invers
	write_data(0x82);
	gfxSleepMilliseconds(1);

	//Horizontal set
	write_index(0x14); //HDWR//Horizontal Display Width Setting Bit[6:0]
	write_data(0x3B);//Horizontal display width(pixels) = (HDWR + 1)*8
    write_index(0x15); //Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)
    write_data(0x00);//Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
    write_index(0x16); //HNDR//Horizontal Non-Display Period Bit[4:0]
    write_data(0x01);//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8
    write_index(0x17); //HSTR//HSYNC Start Position[4:0]
    write_data(0x00);//HSYNC Start Position(PCLK) = (HSTR + 1)*8
    write_index(0x18); //HPWR//HSYNC Polarity ,The period width of HSYNC.
    write_data(0x05);//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8

    //Vertical set
    write_index(0x19); //VDHR0 //Vertical Display Height Bit [7:0]
    write_data(0x0f); //Vertical pixels = VDHR + 1
    write_index(0x1a); //VDHR1 //Vertical Display Height Bit [8]
    write_data(0x01); //Vertical pixels = VDHR + 1
    write_index(0x1b);  //VNDR0 //Vertical Non-Display Period Bit [7:0]
    write_data(0x02);  //VSYNC Start Position(PCLK) = (VSTR + 1)
    write_index(0x1c);  //VNDR1 //Vertical Non-Display Period Bit [8]
    write_data(0x00); //Vertical Non-Display area = (VNDR + 1)
    write_index(0x1d); //VSTR0 //VSYNC Start Position[7:0]
    write_data(0x07);//VSYNC Start Position(PCLK) = (VSTR + 1)
    write_index(0x1e); //VSTR1 //VSYNC Start Position[8]
    write_data(0x00);//VSYNC Start Position(PCLK) = (VSTR + 1)
    write_index(0x1f);    //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]
    write_data(0x09);   //VSYNC Pulse Width(PCLK) = (VPWR + 1)

	//Active window  set
	//setting active window X
    write_index(0x30); //Horizontal Start Point 0 of Active Window (HSAW0)
    write_data(0x00); //Horizontal Start Point of Active Window [7:0]
    write_index(0x31); //Horizontal Start Point 1 of Active Window (HSAW1)
    write_data(0x00); //Horizontal Start Point of Active Window [9:8]
    write_index(0x34); //Horizontal End Point 0 of Active Window (HEAW0)
    write_data(0xDF); //Horizontal End Point of Active Window [7:0]
    write_index(0x35); //Horizontal End Point 1 of Active Window (HEAW1)
    write_data(0x01); //Horizontal End Point of Active Window [9:8]

	//setting active window Y
    write_index(0x32); //Vertical Start Point 0 of Active Window (VSAW0)
    write_data(0x00); //Vertical Start Point of Active Window [7:0]
    write_index(0x33); //Vertical Start Point 1 of Active Window (VSAW1)
    write_data(0x00); //Vertical Start Point of Active Window [8]
    write_index(0x36); //Vertical End Point of Active Window 0 (VEAW0)
    write_data(0x0F); //Vertical End Point of Active Window [7:0]
    write_index(0x37); //Vertical End Point of Active Window 1 (VEAW1)
    write_data(0x01); //Vertical End Point of Active Window [8]

	// Display ON
	write_index(0x01);	//PWRR
	write_data(0x80);

	// GPO0 DISP high
	write_index(0x13);	//GPO
	write_data(0x01);

	set_backlight(0x80);	//set to 90% brightness
	
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

	//setting active window X
	write_index(0x30);	//HSAW0
	write_data(x0);
	write_index(0x31);	//HSAW1
	write_data(x0>>8);

	write_index(0x34);	//HEAW0
	write_data(x1);
	write_index(0x35);	//HEAW1
	write_data(x1>>8);

	//setting active window Y
	write_index(0x32);	//VSAW0
	write_data(y0);
	write_index(0x33);	//VSAW1
	write_data(y0>>8);

	write_index(0x36);	//VEAW0
	write_data(y1);
	write_index(0x37);	//VEAW1
	write_data(y1>>8);

	write_index(0x46);
	write_data(x0);
	write_index(0x47);
	write_data(x0>>8);

	write_index(0x48);
	write_data(y0);
	write_index(0x49);
	write_data(y0>>8);
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
	write_index(RA8875_WRITE_MEMORY_START);
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
		write_index(RA8875_WRITE_MEMORY_START);

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
		write_index(RA8875_WRITE_MEMORY_START);

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
						/* ToDo */
						break;
					case powerOn:
						/* ToDo */
						break;
					case powerSleep:
						/* ToDo */
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
				set_backlight((uint8_t )value);
				//gdisp_lld_bg_dimmer(54 + ((uint8_t)value) << 1);//turn 0..100% in 54..255
				return;
/*
			case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */

