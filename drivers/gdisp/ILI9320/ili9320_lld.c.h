/*
    ChibiOS/RT - Copyright (C) 2012
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

#ifndef ILI9320_H
#define ILI9320_H

#if defined(GDISP_USE_GPIO)
	#error "ILI9320: GPIO interface not implemented yet"

#elif defined(GDISP_USE_SPI)
	#error "ILI9320: GDISP_USE_SPI not implemented yet"

#elif defined(GDISP_USE_FSMC)
	#define GDISP_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
	#define GDISP_RAM              (*((volatile uint16_t *) 0x60100000)) /* RS = 1 */

	static __inline void lld_lcdWriteIndex(uint16_t index) {
		GDISP_REG = index;
	}
	
	static __inline void lld_lcdWriteData(uint16_t data) {
		GDISP_RAM = data;
	}

	static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) {
		GDISP_REG = lcdReg;
		GDISP_RAM = lcdRegValue;
	}

	static __inline uint16_t lld_lcdReadData(void) {
		return (GDISP_RAM);
	}
	
	static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
		volatile uint16_t dummy;

		GDISP_REG = lcdReg;
		dummy = GDISP_RAM;
		(void)dummy;

		return (GDISP_RAM);
	}

	static __inline void lld_lcdWriteStreamStart(void) {
		GDISP_REG = 0x0022; 
	}
	
	static __inline void lld_lcdWriteStreamStop(void) {
	}

	static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
		uint16_t i;

		for(i = 0; i < size; i++)
			GDISP_RAM = buffer[i];
	}

	static __inline void lld_lcdReadStreamStart(void) {
		GDISP_REG = 0x0022;
	}

	static __inline void lld_lcdReadStreamStop(void) {
	
	}

	static __inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
		uint16_t i;
		volatile uint16_t dummy;

		dummy = GDISP_RAM; /* throw away first value read */
		(void)dummy;

		for(i = 0; i < size; i++)
			buffer[i] = GDISP_RAM;
	}

#endif

static __inline void lld_lcdDelay(uint16_t us) {
	chThdSleepMicroseconds(us);
}

static void lld_lcdSetCursor(uint16_t x, uint16_t y) {
    uint32_t addr;

    addr = y * 0x100 + x;

    lld_lcdWriteReg(0x0020, addr & 0xff);   /* low addr */
    lld_lcdWriteReg(0x0021, (addr >> 8) & 0x1ff); /* high addr */
    GDISP_REG = 0x0022;      /* data reg in IR */
}

static void lld_lcdSetViewPort(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy) {
	lld_lcdWriteReg(0x0050, x - 1);
	lld_lcdWriteReg(0x0051, x + cx - 2);
	lld_lcdWriteReg(0x0052, y - 1);
	lld_lcdWriteReg(0x0053, y + cy - 2);

	lld_lcdSetCursor(x, y);
}

static __inline void lld_lcdResetViewPort(void) { 
	/* ToDo */
}

#endif /* ILI9320_H */

