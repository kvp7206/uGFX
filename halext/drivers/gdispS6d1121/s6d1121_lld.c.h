/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef S6D1121_H
#define S6D1121_H

// I/O assignments
#define LCD_BL_GPIO			GPIOB
#define LCD_BL_PIN			8

#define LCD_CS_GPIO			GPIOD
#define LCD_CS_PIN			7

#define LCD_RS_GPIO			GPIOD
#define LCD_RS_PIN			11

#define LCD_RST_GPIO		GPIOD
#define LCD_RST_PIN			10

#define LCD_RD_GPIO			GPIOD
#define LCD_RD_PIN			9

#define LCD_WR_GPIO			GPIOD
#define LCD_WR_PIN			8

#define LCD_D0_GPIO			GPIOD
#define LCD_D4_GPIO			GPIOE

/* all interfaces use RST via GPIO */
/* TODO: option to disable RST; assumes RST is tied high */
#define LCD_RST_LOW		palClearPad(LCD_RST_GPIO, LCD_RST_PIN)
#define LCD_RST_HIGH	palSetPad(LCD_RST_GPIO, LCD_RST_PIN)

#define s6d1121_delay(n) 	halPolledDelay(MS2RTT(n));

#if defined(LCD_USE_GPIO)

	#define LCD_CS_LOW		palClearPad(LCD_CS_GPIO, LCD_CS_PIN)
	#define LCD_CS_HIGH		palSetPad(LCD_CS_GPIO, LCD_CS_PIN)

	#define LCD_RS_LOW		palClearPad(LCD_RS_GPIO, LCD_RS_PIN)
	#define LCD_RS_HIGH		palSetPad(LCD_RS_GPIO, LCD_RS_PIN)

	#define LCD_RD_LOW		palClearPad(LCD_RD_GPIO, LCD_RD_PIN)
	#define LCD_RD_HIGH		palSetPad(LCD_RD_GPIO, LCD_RD_PIN)

	#define LCD_WR_LOW		palClearPad(LCD_WR_GPIO, LCD_WR_PIN)
	#define LCD_WR_HIGH		palSetPad(LCD_WR_GPIO, LCD_WR_PIN)

	#define LCD_BL_LOW		palClearPad(LCD_BL_GPIO, LCD_BL_PIN)
	#define LCD_BL_HIGH		palSetPad(LCD_BL_GPIO, LCD_BL_PIN)


	static inline void lld_lcddelay(void)				{ asm volatile ("nop"); asm volatile ("nop"); }
	static inline void lld_lcdwrite(uint16_t db) {
		LCD_D4_GPIO->BSRR.W=((~db&0xFFF0)<<16)|(db&0xFFF0);
		LCD_D0_GPIO->BSRR.W=((~db&0x000F)<<16)|(db&0x000F);
		LCD_WR_LOW;
		lld_lcddelay();
		LCD_WR_HIGH;
	}
	static __inline uint16_t lld_lcdReadData(void) {
		uint16_t value=0;

		LCD_RS_HIGH; LCD_WR_HIGH; LCD_RD_LOW;
		#ifndef STM32F4XX
			// change pin mode to digital input
			LCD_DATA_PORT->CRH = 0x47444444;
			LCD_DATA_PORT->CRL = 0x47444444;
		#endif
		#ifndef STM32F4XX
			// change pin mode back to digital output
			LCD_DATA_PORT->CRH = 0x33333333;
			LCD_DATA_PORT->CRL = 0x33333333;
		#endif
	    LCD_RD_HIGH;
		return value;
	}
	static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
	    uint16_t value;

	    LCD_CS_LOW; LCD_RS_LOW;
	    lld_lcdwrite(lcdReg);
	    LCD_RS_HIGH;
	    lcdRAM = lld_lcdReadData();
	    LCD_CS_HIGH;
	    return lcdRAM;
	}
	static void lld_lcdWriteIndex(uint16_t lcdReg) {
		LCD_RS_LOW;
		lld_lcdwrite(lcdReg);
		LCD_RS_HIGH;
	}
	static void lld_lcdWriteData(uint16_t lcdData) {
		lld_lcdwrite(lcdData);
	}
	static void lld_lcdWriteReg(uint16_t lcdReg, uint16_t lcdRegValue) {
		LCD_CS_LOW;
		lld_lcdWriteIndex(lcdReg);
		lld_lcdWriteData(lcdRegValue);
		LCD_CS_HIGH;
	}
	static __inline void lld_lcdWriteStreamStart(void) {
		LCD_CS_LOW;
		lld_lcdWriteIndex(0x0022);
	}
	static __inline void lld_lcdWriteStreamStop(void) {
		LCD_CS_HIGH;
	}
	static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
		uint16_t i;

		for(i = 0; i < size; i++) { lld_lcdwrite(buffer[i]); }
	}
	static __inline void lld_lcdReadStreamStart(void) {	/* TODO */ }
	static __inline void lld_lcdReadStreamStop(void) { /* TODO */ }
	static __inline void lld_lcdReadStream(uint16_t *UNUSED(buffer), size_t UNUSED(size)) { /* TODO */ }

#elif defined(LCD_USE_FSMC)
	#define LCD_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
	#define LCD_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

	static __inline void lld_lcdWriteIndex(uint16_t index)			{ LCD_REG = index; }
	static __inline void lld_lcdWriteData(uint16_t data)			{ LCD_RAM = data; }
	static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) {
		LCD_REG = lcdReg;
		LCD_RAM = lcdRegValue;
	}
	static __inline uint16_t lld_lcdReadData(void)					{ return (LCD_RAM); }
	static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
		LCD_REG = lcdReg;
		return LCD_RAM;
	}
	static __inline void lld_lcdWriteStreamStart(void)				{ LCD_REG = 0x0022; }
	static __inline void lld_lcdWriteStreamStop(void)				{}
	static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
		uint16_t i;
		for(i = 0; i < size; i++) LCD_RAM = buffer[i];
	}
	static __inline void lld_lcdReadStreamStart(void)				{ LCD_REG = 0x0022; }
	static __inline void lld_lcdReadStreamStop(void)				{}
	static __inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
		uint16_t i;
		volatile uint16_t dummy;

		/* throw away first value read */
		dummy = LCD_RAM;
		for(i = 0; i < size; i++) buffer[i] = LCD_RAM;
	}

#elif defined(LCD_USE_SPI)
	#error "gdispS6d1121: LCD_USE_SPI not implemented yet"

#else
	#error "gdispS6d1121: No known LCD_USE_XXX has been defined"
#endif

static void lld_lcdSetCursor(coord_t x, coord_t y) {
	/* R20h - 8 bit
	 * R21h - 9 bit
	 */
	switch(GDISP.Orientation) {
		case portraitInv:
			lld_lcdWriteReg(0x0020, (SCREEN_WIDTH-1-x) & 0x00FF);
			lld_lcdWriteReg(0x0021, (SCREEN_HEIGHT-1-y) & 0x01FF);
			break;
		case portrait:
			lld_lcdWriteReg(0x0020, x & 0x00FF);
			lld_lcdWriteReg(0x0021, y & 0x01FF);
			break;
		case landscape:
			lld_lcdWriteReg(0x0020, y & 0x00FF);
			lld_lcdWriteReg(0x0021, x & 0x01FF);
			break;
		case landscapeInv:
			lld_lcdWriteReg(0x0020, (SCREEN_WIDTH - y - 1) & 0x00FF);
			lld_lcdWriteReg(0x0021, (SCREEN_HEIGHT - x - 1) & 0x01FF);
			break;
	}
}

static void lld_lcdSetViewPort(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy) {
	/* HSA / HEA are 8 bit
	 * VSA / VEA are 9 bit
	 * use masks 0x00FF and 0x01FF to enforce this
	 */

	switch(GDISP.Orientation) {
		case portrait:
			lld_lcdWriteReg(0x46, (((x+cx-1) << 8) & 0xFF00 ) | (x & 0x00FF));
			lld_lcdWriteReg(0x48, y & 0x01FF);
			lld_lcdWriteReg(0x47, (y+cy-1) & 0x01FF);
			break;
		case landscape:
			lld_lcdWriteReg(0x46, (((x+cx-1) << 8) & 0xFF00) | ((y+cy) & 0x00FF));
			lld_lcdWriteReg(0x48, x & 0x01FF);
			lld_lcdWriteReg(0x47, (x+cx-1) & 0x01FF);
			break;
		case portraitInv:
			lld_lcdWriteReg(0x46, (((SCREEN_WIDTH-x-1) & 0x00FF) << 8) | ((SCREEN_WIDTH - (x+cx)) & 0x00FF));
			lld_lcdWriteReg(0x48, (SCREEN_HEIGHT-(y+cy)) & 0x01FF);
			lld_lcdWriteReg(0x47, (SCREEN_HEIGHT-y-1) & 0x01FF);
			break;
		case landscapeInv:
			lld_lcdWriteReg(0x46, (((SCREEN_WIDTH - y - 1) & 0x00FF) << 8) | ((SCREEN_WIDTH - (y+cy)) & 0x00FF));
			lld_lcdWriteReg(0x48, (SCREEN_HEIGHT - (x+cx)) & 0x01FF);
			lld_lcdWriteReg(0x47, (SCREEN_HEIGHT - x - 1) & 0x01FF);
			break;
	}

	lld_lcdSetCursor(x, y);
}

static void lld_lcdResetViewPort(void) {
	switch(GDISP.Orientation) {
		case portrait:
		case portraitInv:
			lld_lcdSetViewPort(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
		case landscape:
		case landscapeInv:
			lld_lcdSetViewPort(0, 0, SCREEN_HEIGHT, SCREEN_WIDTH);
			break;
	}
}

#endif /* S6D1121_H */
