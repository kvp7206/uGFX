/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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
	#define GDISP_SCREEN_HEIGHT		480
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		800
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

__inline void gdisp_lld_bg_dimmer(uint8_t duty_cycle){//duty_cycle is 00..FF
	//Work in progress: the SSD1963 has a built-in PWM, its output can
	//be used by a Dynamic Background Control or by a host (user)
	//Check your LCD's hardware, the PWM connection is default left open and instead
	//connected to a LED connection on the breakout board
	GDISP_LLD(writeindex)(SSD1963_SET_PWM_CONF);//set PWM for BackLight
	GDISP_LLD(writedata)(0x0001);
	GDISP_LLD(writedata)(duty_cycle & 0x00FF);
	GDISP_LLD(writedata)(0x0001);//controlled by host (not DBC), enabled
	GDISP_LLD(writedata)(0x00FF);
	GDISP_LLD(writedata)(0x0060);//don't let it go too dark, avoid a useless LCD
	GDISP_LLD(writedata)(0x000F);//prescaler ???
}

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
	
	/* set pins to FSMC mode */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};
	IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15), 0};
	const unsigned char FSMC_Bank = 0;
	
	#if defined(STM32F1XX) || defined(STM32F3XX) || defined(STM32F10X_HD)
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

	//palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	//palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busD, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetBusMode(&busE, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	
		
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

	palClearPad(GPIOB, GPIOB_LCD_RESET);	//reset
	chThdSleepMilliseconds(500);			//
	palSetPad(GPIOB, GPIOB_LCD_RESET);		//
	chThdSleepMilliseconds(100);
	
	GDISP_LLD(writeindex)(SSD1963_SET_PLL_MN);//set PLL frequency
	GDISP_LLD(writedata)(0x0023);//multiplier, VCO = Reference input clock x this value
	GDISP_LLD(writedata)(0x0002);//divider, PLL frequency = VCO / this value
	GDISP_LLD(writedata)(0x0004);//Effectuate the multiplier and divider value
	
	//Enabling the PLL is a 2-step procedure
	//First use system clock and wait for the PLL to stabilize, then enable PLL
	GDISP_LLD(writeindex)(SSD1963_SET_PLL);// Enable PLL
	GDISP_LLD(writedata)(0x0001);//Use reference clock as system clock and Enable PLL
	chThdSleepMilliseconds(1);
	GDISP_LLD(writeindex)(SSD1963_SET_PLL);// Use PLL
	GDISP_LLD(writedata)(0x0003);//Use PLL output as system clock
	chThdSleepMilliseconds(5);
	
	GDISP_LLD(writeindex)(SSD1963_SOFT_RESET);//Software reset
	chThdSleepMilliseconds(5);
	
	GDISP_LLD(writeindex)(SSD1963_SET_LSHIFT_FREQ);//Set the LSHIFT (pixel clock) frequency called PCLK
	GDISP_LLD(writedata)(0x0004);//this and the next 2 parameters form a 20-bit value called LCDC_FPR
	GDISP_LLD(writedata)(0x0093);// PCLK = PLL Frequency * LCDC_FPR / 2^20
	GDISP_LLD(writedata)(0x00E0);

	GDISP_LLD(writeindex)(SSD1963_SET_GDISP_MODE); //Set the LCD panel mode (RGB TFT or TTL) and pad strength
	GDISP_LLD(writedata)(0x0020);//TFT panel data width: 24-bit ; TFT color depth enhancement: Disable FRC or dithering
	GDISP_LLD(writedata)(0x0000);//LCD panel mode: Hsync+Vsync +DE mode & TFT mode
	GDISP_LLD(writedata)(mHIGH(GDISP_SCREEN_WIDTH-1) & 0x00FF);
	GDISP_LLD(writedata)((GDISP_SCREEN_WIDTH-1) & 0x00FF);
	GDISP_LLD(writedata)(mHIGH(GDISP_SCREEN_HEIGHT-1) & 0x00FF);
	GDISP_LLD(writedata)((GDISP_SCREEN_HEIGHT-1) & 0x00FF);
	GDISP_LLD(writedata)(0x0000);
	
	//Set horizontal timings
	GDISP_LLD(writeindex)(SSD1963_SET_HORI_PERIOD);//Set Horizontal Period HSYNC
	GDISP_LLD(writedata)(mHIGH(SCREEN_HSYNC_PERIOD) & 0x00FF);//Set HT
	GDISP_LLD(writedata)(SCREEN_HSYNC_PERIOD & 0x00FF);
	GDISP_LLD(writedata)(mHIGH(SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH) & 0x00FF);
	GDISP_LLD(writedata)((SCREEN_HSYNC_PULSE + SCREEN_HSYNC_BACK_PORCH) & 0x00FF);
	GDISP_LLD(writedata)(SCREEN_HSYNC_PULSE);
	GDISP_LLD(writedata)(mHIGH(SCREEN_HSTART_POSITION) & 0x00FF);
	GDISP_LLD(writedata)(SCREEN_HSTART_POSITION & 0x00FF);
	GDISP_LLD(writedata)(0x0000);
	//set vertical timings
	GDISP_LLD(writeindex)(SSD1963_SET_VERT_PERIOD);//Set Vertical Period VSYNC
	GDISP_LLD(writedata)(mHIGH(SCREEN_VSYNC_PERIOD) & 0x00FF);//Set VT
	GDISP_LLD(writedata)(SCREEN_VSYNC_PERIOD & 0x00FF);
	GDISP_LLD(writedata)(mHIGH(SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH) & 0x00FF);
	GDISP_LLD(writedata)((SCREEN_VSYNC_PULSE + SCREEN_VSYNC_BACK_PORCH) & 0x00FF);
	GDISP_LLD(writedata)(SCREEN_VSYNC_PULSE);
	GDISP_LLD(writedata)(mHIGH(SCREEN_VSTART_POSITION) & 0x00FF);
	GDISP_LLD(writedata)(SCREEN_VSTART_POSITION & 0x00FF);
	
	//Work in progress: the SSD1963 has 4 GPIO lines that can be configured for custom
	//purpose. The following 5 lines illustrate its use
	GDISP_LLD(writeindex)(SSD1963_SET_GPIO_VALUE);
	GDISP_LLD(writedata)(0x000F);//GPIO[3:0] out 1
	GDISP_LLD(writeindex)(SSD1963_SET_GPIO_CONF);
	GDISP_LLD(writedata)(0x0007);//GPIO3=input, GPIO[2:0]=output
	GDISP_LLD(writedata)(0x0001);//GPIO0 normal
	
	//Set the read order from host processor to frame buffer and from frame buffer to the display panel
	//Use this to rotate, flip and mirror
	GDISP_LLD(writeindex)(SSD1963_SET_ADDRESS_MODE);
	GDISP_LLD(writedata)(0x0000);//Top to bottom, Left to right, Normal column order, LCD refresh from top line to bottom line and left side to right side, RGB, no Horizontal flip
	
	GDISP_LLD(writeindex)(SSD1963_SET_PIXEL_FORMAT);//Set the current pixel format for RGB image data
	GDISP_LLD(writedata)(0x0050);//16-bit per pixel
	chThdSleepMilliseconds(5);
	
	GDISP_LLD(writeindex)(SSD1963_SET_PIXEL_DATA_INTERFACE);//Set the pixel data format to 8-bit / 9-bit / 12-bit / 16-bit / 16-bit(565) / 18-bit / 24-bit in the parallel host processor interface
	GDISP_LLD(writedata)(SSD1963_PDI_16BIT565);
	chThdSleepMilliseconds(5);
	
	GDISP_LLD(writeindex)(SSD1963_SET_GAMMA_CURVE);//Selects the gamma curve used by the display device
	GDISP_LLD(writedata)(0x0008);//Gamma curve 3
	
	GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_ON);//Show the image on the display device
	
	gdisp_lld_bg_dimmer(0xE5);//set to 90% brightness

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
		uint32_t area;
		uint32_t index;
    
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		
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
						chThdSleepMilliseconds(5);
						GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_OFF);
						GDISP_LLD(writeindex)(SSD1963_SET_DEEP_SLEEP); // enter deep sleep mode
						break;
					case powerOn:
						GDISP_LLD(readreg)(0x0000); chThdSleepMilliseconds(5); // 2x Dummy reads to wake up from deep sleep
						GDISP_LLD(readreg)(0x0000); chThdSleepMilliseconds(5);
						if (GDISP.Powermode != powerSleep)
							gdisp_lld_init();
						GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_ON);
	
						break;
					case powerSleep:
						GDISP_LLD(writeindex)(SSD1963_SET_DISPLAY_OFF);
						GDISP_LLD(writeindex)(SSD1963_ENTER_SLEEP_MODE); // enter sleep mode
						chThdSleepMilliseconds(5);
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

