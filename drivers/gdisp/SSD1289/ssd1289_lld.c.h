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

#ifndef SSD1289_H
#define SSD1289_H

#if defined(LCD_USE_GPIO)
	#define Set_CS		palSetPad(LCD_CMD_PORT, LCD_CS);
	#define Clr_CS		palClearPad(LCD_CMD_PORT, LCD_CS);
	#define Set_RS		palSetPad(LCD_CMD_PORT, LCD_RS);
	#define Clr_RS		palClearPad(LCD_CMD_PORT, LCD_RS);
	#define Set_WR		palSetPad(LCD_CMD_PORT, LCD_WR);
	#define Clr_WR		palClearPad(LCD_CMD_PORT, LCD_WR);
	#define Set_RD		palSetPad(LCD_CMD_PORT, LCD_RD);
	#define Clr_RD		palClearPad(LCD_CMD_PORT, LCD_RD);

	static __inline void lld_lcdWriteIndex(uint16_t index)	{
		Clr_RS; Set_RD;
		lld_lcdWriteGPIO(index);
		Clr_WR; Set_WR;
	}
	static __inline void lld_lcdWriteData(uint16_t data)	{
		Set_RS;
		lld_lcdWriteGPIO(data);
		Clr_WR; Set_WR;
	}
	static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) {
		Clr_CS;
		lld_lcdWriteIndex(lcdReg);
		lld_lcdWriteData(lcdRegValue);
		Set_CS;
	}
	static __inline uint16_t lld_lcdReadData(void) {
		uint16_t value;

		Set_RS; Set_WR; Clr_RD;
		value = lld_lcdReadGPIO();
		Set_RD;
		return value;
	}
	static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
		uint16_t value;

		Clr_CS;
		lld_lcdWriteIndex(lcdReg);
		value = lld_lcdReadData();
		Set_CS;
		return value;
	}

	static __inline void lld_lcdWriteStreamStart(void) {
		Clr_CS;
		lld_lcdWriteIndex(0x0022);
	}

	static __inline void lld_lcdWriteStreamStop(void) {
		Set_CS;
	}

	static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
		uint16_t i;

		Set_RS;
		for(i = 0; i < size; i++) {	lld_lcdWriteGPIO(buffer[i]); Clr_WR; Set_WR; }
	}

	static __inline void lld_lcdReadStreamStart(void) {
		Clr_CS
		lld_lcdWriteIndex(0x0022);
	}

	static __inline void lld_lcdReadStreamStop(void) {
		Set_CS;
	}

	static __inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
		uint16_t i;
		volatile uint16_t dummy;

		dummy = lld_lcdReadData();
		for(i = 0; i < size; i++) buffer[i] = lld_lcdReadData();

		(void)dummy;
	}

#elif defined(LCD_USE_FSMC)
	/* LCD Registers */
	#define R0             0x00
	#define R1             0x01
	#define R2             0x02
	#define R3             0x03
	#define R4             0x04
	#define R5             0x05
	#define R6             0x06
	#define R7             0x07
	#define R8             0x08
	#define R9             0x09
	#define R10            0x0A
	#define R12            0x0C
	#define R13            0x0D
	#define R14            0x0E
	#define R15            0x0F
	#define R16            0x10
	#define R17            0x11
	#define R18            0x12
	#define R19            0x13
	#define R20            0x14
	#define R21            0x15
	#define R22            0x16
	#define R23            0x17
	#define R24            0x18
	#define R25            0x19
	#define R26            0x1A
	#define R27            0x1B
	#define R28            0x1C
	#define R29            0x1D
	#define R30            0x1E
	#define R31            0x1F
	#define R32            0x20
	#define R33            0x21
	#define R34            0x22
	#define R36            0x24
	#define R37            0x25
	#define R40            0x28
	#define R41            0x29
	#define R43            0x2B
	#define R45            0x2D
	#define R48            0x30
	#define R49            0x31
	#define R50            0x32
	#define R51            0x33
	#define R52            0x34
	#define R53            0x35
	#define R54            0x36
	#define R55            0x37
	#define R56            0x38
	#define R57            0x39
	#define R59            0x3B
	#define R60            0x3C
	#define R61            0x3D
	#define R62            0x3E
	#define R63            0x3F
	#define R64            0x40
	#define R65            0x41
	#define R66            0x42
	#define R67            0x43
	#define R68            0x44
	#define R69            0x45
	#define R70            0x46
	#define R71            0x47
	#define R72            0x48
	#define R73            0x49
	#define R74            0x4A
	#define R75            0x4B
	#define R76            0x4C
	#define R77            0x4D
	#define R78            0x4E
	#define R79            0x4F
	#define R80            0x50
	#define R81            0x51
	#define R82            0x52
	#define R83            0x53
	#define R96            0x60
	#define R97            0x61
	#define R106           0x6A
	#define R118           0x76
	#define R128           0x80
	#define R129           0x81
	#define R130           0x82
	#define R131           0x83
	#define R132           0x84
	#define R133           0x85
	#define R134           0x86
	#define R135           0x87
	#define R136           0x88
	#define R137           0x89
	#define R139           0x8B
	#define R140           0x8C
	#define R141           0x8D
	#define R143           0x8F
	#define R144           0x90
	#define R145           0x91
	#define R146           0x92
	#define R147           0x93
	#define R148           0x94
	#define R149           0x95
	#define R150           0x96
	#define R151           0x97
	#define R152           0x98
	#define R153           0x99
	#define R154           0x9A
	#define R157           0x9D
	#define R192           0xC0
	#define R193           0xC1
	#define R229           0xE5

	#define LCD_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
	#define LCD_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

	static __inline void lld_lcdWriteIndex(uint16_t index)		{ LCD_REG = index; }
	static __inline void lld_lcdWriteData(uint16_t data)		{ LCD_RAM = data; }
	static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) {
		LCD_REG = lcdReg;
		LCD_RAM = lcdRegValue;
	}
	static __inline uint16_t lld_lcdReadData(void)				{ return (LCD_RAM); }
	static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
		volatile uint16_t dummy;

		LCD_REG = lcdReg;
		dummy = LCD_RAM;
		return (LCD_RAM);
	}
	static __inline void lld_lcdWriteStreamStart(void)			{ LCD_REG = 0x0022; }
	static __inline void lld_lcdWriteStreamStop(void)			{}
	static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
		uint16_t i;

		for(i = 0; i < size; i++) LCD_RAM = buffer[i];
	}
	static __inline void lld_lcdReadStreamStart(void)			{ LCD_REG = 0x0022; }
	static __inline void lld_lcdReadStreamStop(void)			{}
	static __inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
		uint16_t i;
		volatile uint16_t dummy;

		dummy = LCD_RAM; /* throw away first value read */
		for(i = 0; i < size; i++) buffer[i] = LCD_RAM;
	}

#elif defined(LCD_USE_SPI)
	#error "gdispSsd1289: LCD_USE_SPI not implemented yet"

#else
	#error "gdispSsd1289: No known LCD_USE_XXX has been defined"
#endif

static __inline void lld_lcdDelay(uint16_t us) {
	chThdSleepMicroseconds(us);
}

static void lld_lcdSetCursor(uint16_t x, uint16_t y) {
	/* Reg 0x004E is an 8 bit value
	 * Reg 0x004F is 9 bit
	 * Use a bit mask to make sure they are not set too high
	 */
	switch(GDISP.Orientation) {
		case portraitInv:
			lld_lcdWriteReg(0x004e, (SCREEN_WIDTH-1-x) & 0x00FF);
			lld_lcdWriteReg(0x004f, (SCREEN_HEIGHT-1-y) & 0x01FF);
			break;
		case portrait:
			lld_lcdWriteReg(0x004e, x & 0x00FF);
			lld_lcdWriteReg(0x004f, y & 0x01FF);
			break;
		case landscape:
			lld_lcdWriteReg(0x004e, y & 0x00FF);
			lld_lcdWriteReg(0x004f, x & 0x01FF);
			break;
		case landscapeInv:
			lld_lcdWriteReg(0x004e, (SCREEN_WIDTH - y - 1) & 0x00FF);
			lld_lcdWriteReg(0x004f, (SCREEN_HEIGHT - x - 1) & 0x01FF);
			break;
	}
}

static void lld_lcdSetViewPort(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy) {
	lld_lcdSetCursor(x, y);

	/* Reg 0x44 - Horizontal RAM address position
	 * 		Upper Byte - HEA
	 * 		Lower Byte - HSA
	 * 		0 <= HSA <= HEA <= 0xEF
	 * Reg 0x45,0x46 - Vertical RAM address position
	 * 		Lower 9 bits gives 0-511 range in each value
	 * 		0 <= Reg(0x45) <= Reg(0x46) <= 0x13F
	 */

	switch(GDISP.Orientation) {
		case portrait:
			lld_lcdWriteReg(0x44, (((x+cx-1) << 8) & 0xFF00 ) | (x & 0x00FF));
			lld_lcdWriteReg(0x45, y & 0x01FF);
			lld_lcdWriteReg(0x46, (y+cy-1) & 0x01FF);
			break;
		case landscape:
			lld_lcdWriteReg(0x44, (((x+cx-1) << 8) & 0xFF00) | ((y+cy) & 0x00FF));
			lld_lcdWriteReg(0x45, x & 0x01FF);
			lld_lcdWriteReg(0x46, (x+cx-1) & 0x01FF);
			break;
		case portraitInv:
			lld_lcdWriteReg(0x44, (((SCREEN_WIDTH-x-1) & 0x00FF) << 8) | ((SCREEN_WIDTH - (x+cx)) & 0x00FF));
			lld_lcdWriteReg(0x45, (SCREEN_HEIGHT-(y+cy)) & 0x01FF);
			lld_lcdWriteReg(0x46, (SCREEN_HEIGHT-y-1) & 0x01FF);
			break;
		case landscapeInv:
			lld_lcdWriteReg(0x44, (((SCREEN_WIDTH - y - 1) & 0x00FF) << 8) | ((SCREEN_WIDTH - (y+cy)) & 0x00FF));
			lld_lcdWriteReg(0x45, (SCREEN_HEIGHT - (x+cx)) & 0x01FF);
			lld_lcdWriteReg(0x46, (SCREEN_HEIGHT - x - 1) & 0x01FF);
			break;
	}

	lld_lcdSetCursor(x, y);
}

static __inline void lld_lcdResetViewPort(void)							{}

#endif /* SSD1289_H */
