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
 * @file    drivers/gdisp/SSD1963/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source.
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

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		320
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		240
#endif

/* All the board specific code should go in these include file so the driver
 * can be ported to another board just by creating a suitable file.
 */
//#if defined(BOARD_YOURBOARDNAME)
//	#include "gdisp_lld_board_yourboardname.h"
//#else
//	/* Include the user supplied board definitions */
//	#include "gdisp_lld_board.h"
//#endif

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

#include "ssd1963.h"


#if defined(GDISP_USE_FSMC)
__inline void GDISP_LLD(writeindex)(uint8_t cmd) {
  GDISP_REG = cmd;
}

__inline void GDISP_LLD(writereg)(uint16_t lcdReg,uint16_t lcdRegValue) {
	GDISP_REG = lcdReg;
	GDISP_RAM = lcdRegValue;
}

__inline void GDISP_LLD(writedata)(uint16_t data) {
	GDISP_RAM = data;
}

__inline uint16_t GDISP_LLD(readdata)(void) {
	return (GDISP_RAM);
}

__inline uint8_t GDISP_LLD(readreg)(uint8_t lcdReg) {
	GDISP_REG = lcdReg;
	return (GDISP_RAM);
}

__inline void GDISP_LLD(writestreamstart)(void) {
	GDISP_LLD(writeindex)(SSD1963_WRITE_MEMORY_START);
}

__inline void GDISP_LLD(readstreamstart)(void) {
	GDISP_LLD(writeindex)(SSD1963_READ_MEMORY_START);
}

__inline void GDISP_LLD(writestream)(uint16_t *buffer, uint16_t size) {
	uint16_t i;
	for(i = 0; i < size; i++)
		GDISP_RAM = buffer[i];
}

__inline void GDISP_LLD(readstream)(uint16_t *buffer, size_t size) {
	uint16_t i;

	for(i = 0; i < size; i++) {
		buffer[i] = GDISP_RAM;
	}
}

#elif defined(GDISP_USE_GPIO)

__inline void GDISP_LLD(writeindex)(uint8_t cmd) {
	Set_CS; Set_RS; Set_WR; Clr_RD;
	palWritePort(GDISP_DATA_PORT, cmd);
	Clr_CS;
}

__inline void GDISP_LLD(writereg)(uint16_t lcdReg,uint16_t lcdRegValue) {
	Set_CS; Set_RS; Set_WR; Clr_RD;
	palWritePort(GDISP_DATA_PORT, lcdReg);
	Clr_RS;
	palWritePort(GDISP_DATA_PORT, lcdRegValue);
	Clr_CS;
}
__inline void GDISP_LLD(writedata)(uint16_t data) {
	Set_CS; Clr_RS; Set_WR; Clr_RD;
	palWritePort(GDISP_DATA_PORT, data);
	Clr_CS;
}

__inline uint16_t GDISP_LLD(readdata)(void) {
	Set_CS; Clr_RS; Clr_WR; Set_RD;
	uint16_t data = palReadPort(GDISP_DATA_PORT); 
	Clr_CS;
	return data;
}

__inline uint8_t GDISP_LLD(readreg)(uint8_t lcdReg) {
	Set_CS; Set_RS; Clr_WR; Set_RD;
	palWritePort(GDISP_DATA_PORT, lcdReg);
	Clr_RS;
	uint16_t data = palReadPort(GDISP_DATA_PORT);
	Clr_CS;
	return data;
}

__inline void GDISP_LLD(writestreamstart)(void) {
	GDISP_LLD(writeindex)(SSD1963_WRITE_MEMORY_START);
}

__inline void GDISP_LLD(readstreamstart)(void) {
	GDISP_LLD(writeindex)(SSD1963_READ_MEMORY_START);
}

__inline void GDISP_LLD(writestream)(uint16_t *buffer, uint16_t size) {
	uint16_t i;
	Set_CS; Clr_RS; Set_WR; Clr_RD;
	for(i = 0; i < size; i++) {
		Set_WR;
		palWritePort(GDISP_DATA_PORT, buffer[i]);
		Clr_WR;
	}
	Clr_CS;
}

__inline void GDISP_LLD(readstream)(uint16_t *buffer, size_t size) {
	uint16_t i;
	Set_CS; Clr_RS; Clr_WR; Set_RD;
	for(i = 0; i < size; i++) {
		Set_RD;
		buffer[i] = palReadPort(GDISP_DATA_PORT);
		Clr_RD;
	}
}
#endif

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

#if defined(GDISP_USE_FSMC)
	
	#if defined(STM32F1XX) || defined(STM32F3XX)
		/* FSMC setup for F1/F3 */
		rccEnableAHB(RCC_AHBENR_FSMCEN, 0);
	
		#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			#error "DMA not implemented for F1/F3 Devices"
		#endif
	#elif defined(STM32F4XX) || defined(STM32F2XX)
		/* STM32F2-F4 FSMC init */
		rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);
	
		#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, NULL, NULL)) chSysHalt();
			dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);  
		#endif
	#else
		#error "FSMC not implemented for this device"
	#endif
	
	/* set pins to FSMC mode */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
							(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
						(1 << 13) | (1 << 14) | (1 << 15), 0};

	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));
	
	const unsigned char FSMC_Bank = 0;
	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
			| (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
			| (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

	/* Bank1 NOR/SRAM control register configuration
	 * This is actually not needed as already set by default after reset */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	
#elif defined(GDISP_USE_GPIO)
	IOBus busCMD = {GDISP_CMD_PORT, (1 << GDISP_CS) | (1 << GDISP_RS) | (1 << GDISP_WR) | (1 << GDISP_RD), 0};
	IOBus busDATA = {GDISP_CMD_PORT, 0xFFFFF, 0};
	palSetBusMode(&busCMD, PAL_MODE_OUTPUT_PUSHPULL);
	palSetBusMode(&busDATA, PAL_MODE_OUTPUT_PUSHPULL);
	
#else
	#error "Please define GDISP_USE_FSMC or GDISP_USE_GPIO"
#endif	
	GDISP_LLD(writeindex)(SSD1963_SOFT_RESET);	
	chThdSleepMicroseconds(100);

	/* Driver PLL config */
	GDISP_LLD(writeindex)(SSD1963_SET_PLL_MN);
	GDISP_LLD(writedata)(35);								 // PLLclk = REFclk (10Mhz) * 36 (360Mhz)
	GDISP_LLD(writedata)(2);								 // SYSclk = PLLclk / 3  (120MHz)
	GDISP_LLD(writedata)(4);								 // Apply calculation bit, else it is ignored

	GDISP_LLD(writeindex)(SSD1963_SET_PLL);					// Enable PLL
	GDISP_LLD(writedata)(0x01);
	chThdSleepMicroseconds(200);

	GDISP_LLD(writeindex)(SSD1963_SET_PLL);					// Use PLL
	GDISP_LLD(writedata)(0x03);
	chThdSleepMicroseconds(200);

	GDISP_LLD(writeindex)(SSD1963_SOFT_RESET);	
	chThdSleepMicroseconds(100);

	/* Screen size */
	GDISP_LLD(writeindex)(SSD1963_SET_GDISP_MODE);
//	GDISP_LLD(writedata)(0x0000);
	GDISP_LLD(writedata)(0b00011000); //Enabled dithering
	GDISP_LLD(writedata)(0x0000);
	GDISP_LLD(writedata)(mHIGH((GDISP_SCREEN_WIDTH+1)));
	GDISP_LLD(writedata)((GDISP_SCREEN_WIDTH+1));
	GDISP_LLD(writedata)(mHIGH((GDISP_SCREEN_HEIGHT+1)));
	GDISP_LLD(writedata)((GDISP_SCREEN_HEIGHT+1));
	GDISP_LLD(writedata)(0x0000);

	GDISP_LLD(writeindex)(SSD1963_SET_PIXEL_DATA_INTERFACE);
	GDISP_LLD(writedata)(SSD1963_PDI_16BIT565);

	/* LCD Clock specs */
	GDISP_LLD(writeindex)(SSD1963_SET_LSHIFT_FREQ);
	GDISP_LLD(writedata)((GDISP_FPR >> 16) & 0xFF);
	GDISP_LLD(writedata)((GDISP_FPR >> 8) & 0xFF);
	GDISP_LLD(writedata)(GDISP_FPR & 0xFF);

	GDISP_LLD(writeindex)(SSD1963_SET_HORI_PERIOD);
	GDISP_LLD(writedata)(mHIGH(SCREEN_HSYNC_PERIOD));
	GDISP_LLD(writedata)(mLOW(SCREEN_HSYNC_PERIOD));
	GDISP_LLD(writedata)(mHIGH((SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH)));
	GDISP_LLD(writedata)(mLOW((SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH)));
	GDISP_LLD(writedata)(SCREEN_HSYNC_PULSE);
	GDISP_LLD(writedata)(0x00);
	GDISP_LLD(writedata)(0x00);
	GDISP_LLD(writedata)(0x00);

	GDISP_LLD(writeindex)(SSD1963_SET_VERT_PERIOD);
	GDISP_LLD(writedata)(mHIGH(SCREEN_VSYNC_PERIOD));
	GDISP_LLD(writedata)(mLOW(SCREEN_VSYNC_PERIOD));
	GDISP_LLD(writedata)(mHIGH((SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH)));
	GDISP_LLD(writedata)(mLOW((SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH)));
	GDISP_LLD(writedata)(SCREEN_VSYNC_PULSE);
	GDISP_LLD(writedata)(0x00);
	GDISP_LLD(writedata)(0x00);

	/* Tear effect indicator ON. This is used to tell the host MCU when the driver is not refreshing the panel */
	GDISP_LLD(writeindex)(SSD1963_SET_TEAR_ON);
	GDISP_LLD(writedata)(0x0000);

	/* Turn on */
	GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_ON);
	#if defined(GDISP_USE_FSMC)
		/* FSMC delay reduced as the controller now runs at full speed */
		FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;
		FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	#endif

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

void GDISP_LLD(setwindow)(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	/* We don't need to validate here as the LLD routines will validate first.
	 *
	 * #if GDISP_NEED_VALIDATION
	 * 	if (x0 >= GDISP.Width || y0 >= GDISP.Height || x0 < 0 || y0 < 0) return;
	 * 	else if (x1 >= GDISP.Width || y1 >= GDISP.Height || y1 < 0 || y2 < 0) return;
	 * #endif
	*/
	GDISP_LLD(writeindex)(SSD1963_SET_PAGE_ADDRESS);
	GDISP_LLD(writedata)((y0 >> 8) & 0xFF);
	GDISP_LLD(writedata)((y0 >> 0) & 0xFF);
	GDISP_LLD(writedata)((y1 >> 8) & 0xFF);
	GDISP_LLD(writedata)((y1 >> 0) & 0xFF);
	GDISP_LLD(writeindex)(SSD1963_SET_COLUMN_ADDRESS);
	GDISP_LLD(writedata)((x0 >> 8) & 0xFF);
	GDISP_LLD(writedata)((x0 >> 0) & 0xFF);
	GDISP_LLD(writedata)((x1 >> 8) & 0xFF);
	GDISP_LLD(writedata)((x1 >> 0) & 0xFF);
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
	
	GDISP_LLD(setwindow)(x, y, x, y);
	GDISP_LLD(writestreamstart)();
	GDISP_LLD(writedata)(color);
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
    
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		
		uint32_t area;
		area = cx*cy;

		GDISP_LLD(setwindow)(x, y, x+cx-1, y+cy-1);
		GDISP_LLD(writestreamstart)();

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
				GDISP_LLD(writedata)(color);
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

		GDISP_LLD(setwindow)(x, y, x+cx-1, y+cy-1);
		GDISP_LLD(writestreamstart)();

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
			unsigned lg;
			endx = srcx + cx;
			endy = y + cy;
			lg = srccx - cx;
			for(; y < endy; y++, buffer += lg)
				for(x=srcx; x < endx; x++)
					GDISP_LLD(writedata)(*buffer++);
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

		lld_lcdWriteIndex(SSD1963_SET_SCROLL_AREA);
		lld_lcdWriteData((x0 >> 8) & 0xFF);
		lld_lcdWriteData((x0 >> 0) & 0xFF);
		lld_lcdWriteData((size >> 8) & 0xFF);
		lld_lcdWriteData((size >> 0) & 0xFF);
		lld_lcdWriteData(((lcd_height-x1) >> 8) & 0xFF);
		lld_lcdWriteData(((lcd_height-x1) >> 0) & 0xFF);

		lld_lcdWriteIndex(SSD1963_SET_SCROLL_START);
		lld_lcdWriteData((lines >> 8) & 0xFF);
		lld_lcdWriteData((lines >> 0) & 0xFF);
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
					GDISP_LLD(writeindex)(SSD1963_EXIT_SLEEP_MODE); // leave sleep mode
					chThdSleepMicroseconds(5000);
					GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_OFF);
					GDISP_LLD(writeindex)(SSD1963_SET_DEEP_SLEEP); // enter deep sleep mode
					break;
				case powerOn:
					GDISP_LLD(readreg)(0x0000); chThdSleepMicroseconds(5000); // 2x Dummy reads to wake up from deep sleep
					GDISP_LLD(readreg)(0x0000); chThdSleepMicroseconds(5000);
					if (GDISP.Powermode != powerSleep)
						gdisp_lld_init();
					GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_ON);

					break;
				case powerSleep:
					GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_OFF);
					GDISP_LLD(writeindex)(SSD1963_ENTER_SLEEP_MODE); // enter sleep mode
					chThdSleepMicroseconds(5000);
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
/*
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#endif /* GFX_USE_GDISP */
/** @} */
