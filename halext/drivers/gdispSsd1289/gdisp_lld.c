/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*
   Concepts and parts of this file have been contributed by:
		Joel Bodenmann aka Tectu	-> Maintainer
		Andrew Hannam aka inmarket	-> framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/**
 * @file    gdispSsd1289/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the Ssd1289 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "ssd1289_lld.h"

#if HAL_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
	GDISPDriver GDISP1;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#ifdef LCD_USE_GPIO
	static __inline void lld_lcdWriteIndex(uint16_t index) {
		Clr_RS;
		Set_RD;

		lld_lcdWriteGPIO(index);

		Clr_WR;
		Set_WR;
	}

	static __inline void lld_lcdWriteData(uint16_t data) {
		Set_RS;

		lld_lcdWriteGPIO(data);

		Clr_WR;
		Set_WR;
	}

	static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) {
		Clr_CS;

		lld_lcdWriteIndex(lcdReg);
		lld_lcdWriteData(lcdRegValue);

		Set_CS;
	}

	static __inline uint16_t lld_lcdReadData(void) {
		uint16_t value;

		Set_RS;
		Set_WR;
		Clr_RD;

		value = lld_lcdReadGPIO();

		Set_RD;

		return value;
	}

	static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
		uint16_t lcdRAM;

		Clr_CS;
		lld_lcdWriteIndex(lcdReg);
		lcdRAM = lld_lcdReadData();

		Set_CS;

		return lcdRAM;
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

		for(i = 0; i < size; i++) {
			lld_lcdWriteGPIO(buffer[i]);
			Clr_WR;
			Set_WR;
		}
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
		for(i = 0; i < size; i++)
			buffer[i] = lld_lcdReadData();
	}
#endif // LCD_USE_GPIO

#ifdef LCD_USE_SPI
	/* TODO */
#endif // LCD_USE_SPI

#ifdef LCD_USE_FSMC
	#define LCD_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
	#define LCD_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

	static __inline void lld_lcdWriteIndex(uint16_t index) {
		LCD_REG = index;
	}

	static __inline void lld_lcdWriteData(uint16_t data) {
		LCD_RAM = data;
	}

	static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) {
		LCD_REG = lcdReg;
		LCD_RAM = lcdRegValue;
	}

	static __inline uint16_t lld_lcdReadData(void) {
		return (LCD_RAM);
	}

	static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
		LCD_REG = lcdReg;
		volatile uint16_t dummy = LCD_RAM;
		return (LCD_RAM);
	}

	static __inline void lld_lcdWriteStreamStart(void) {
		LCD_REG = 0x0022;
	}

	static __inline void lld_lcdWriteStreamStop(void) {

	}

	static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
		uint16_t i;
		for(i = 0; i < size; i++)
			LCD_RAM = buffer[i];
	}

	static __inline void lld_lcdReadStreamStart(void) {
		LCD_REG = 0x0022;
	}

	static __inline void lld_lcdReadStreamStop(void) {

	}

	static __inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
		uint16_t i;
		/* throw away first value read */
		volatile uint16_t dummy = LCD_RAM;

		for(i = 0; i < size; i++) {
			buffer[i] = LCD_RAM;
		}
	}
#endif // LCD_USE_FSMC

static __inline void lld_lcdDelay(uint16_t us) {
	chThdSleepMicroseconds(us);
}

static void lld_lcdSetCursor(uint16_t x, uint16_t y) {
	/* Reg 0x004E is an 8 bit value
	 * Reg 0x004F is 9 bit
	 * Use a bit mask to make sure they are not set too high
	 */
	switch(GDISP1.Orientation) {
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

void lld_lcdSetViewPort(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy) {
    lld_lcdSetCursor(x, y);

    /* Reg 0x44 - Horizontal RAM address position
     * 		Upper Byte - HEA
     * 		Lower Byte - HSA
     * 		0 <= HSA <= HEA <= 0xEF
     * Reg 0x45,0x46 - Vertical RAM address position
     * 		Lower 9 bits gives 0-511 range in each value
     * 		0 <= Reg(0x45) <= Reg(0x46) <= 0x13F
     */

    switch(GDISP1.Orientation) {
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
void gdisp_lld_init(void) {
	uint16_t	deviceCode;

	#ifdef LCD_USE_FSMC
		/* FSMC setup. TODO: this only works for STM32F1 */
		rccEnableAHB(RCC_AHBENR_FSMCEN, 0);
		int FSMC_Bank = 0;
		/* timing structure */
		/* from datasheet:
			address setup: 0ns
			address hold: 0ns
			Data setup: 5ns
			Data hold: 5ns
			Data access: 250ns
			output hold: 100ns
		 */
		FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_1 | FSMC_BTR1_DATAST_1;

		/* Bank1 NOR/SRAM control register configuration */
		FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	#endif

	deviceCode = lld_lcdReadReg(0x0000);

	lld_lcdWriteReg(0x0000,0x0001);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x0003,0xA8A4);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000C,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000D,0x080C);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000E,0x2B00);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x001E,0x00B0);    	lld_lcdDelay(5);
	lld_lcdWriteReg(0x0001,0x2B3F);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x0002,0x0600);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0010,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0011,0x6070);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0005,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0006,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0016,0xEF1C);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0017,0x0003);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0007,0x0133);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000B,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000F,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0041,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0042,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0048,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0049,0x013F);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004A,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004B,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0044,0xEF00);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0045,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0046,0x013F);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0030,0x0707);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0031,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0032,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0033,0x0502);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0034,0x0507);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0035,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0036,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0037,0x0502);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x003A,0x0302);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x003B,0x0302);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0023,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0024,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0025,0x8000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004f,0x0000);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x004e,0x0000);		lld_lcdDelay(5);

    /* Initialise the GDISP structure */
	GDISP1.Width = SCREEN_WIDTH;
	GDISP1.Height = SCREEN_HEIGHT;
	GDISP1.Orientation = portrait;
	GDISP1.Powermode = powerOn;
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
void gdisp_lld_drawpixel(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION
		if (x >= GDISP1.Width || y >= GDISP1.Height) return;
	#endif
	lld_lcdSetCursor(x, y);
	lld_lcdWriteReg(0x0022, color);
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
		eg. If gdisp_lld_fillarea() is defined there is little
			point in defining gdisp_lld_clear() unless the
			performance bonus is significant.
	For good performance it is suggested to implement
		gdisp_lld_fillarea() and gdisp_lld_blitarea().
*/

#if GDISP_HARDWARE_POWERCONTROL || defined(__DOXYGEN__)
	/**
	 * @brief   Sets the power mode for the graphic device.
	 * @note    The power modes are powerOn, powerSleep and powerOff.
	 *          If powerSleep is not supported it is equivelent to powerOn.
	 *
	 * @param[in] powerMode    The new power mode
	 *
	 * @notapi
	 */
	void gdisp_lld_setpowermode(gdisp_powermode_t powerMode) {
		if (GDISP1.Powermode == powerMode)
			return;

		switch(powerMode) {
			case powerOff:
				lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
				lld_lcdWriteReg(0x0007, 0x0000);	// halt operation
				lld_lcdWriteReg(0x0000, 0x0000);	// turn off oszillator
				lld_lcdWriteReg(0x0010, 0x0001);	// enter sleepmode
				break;
			case powerOn:
				lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
				if (GDISP1.Powermode != powerSleep)
					gdisp_lld_init();
				break;
			case powerSleep:
				lld_lcdWriteReg(0x0010, 0x0001);	// enter sleep mode
				break;
			default:
				return;
		}

		GDISP1.Powermode = powerMode;
	}
#endif

#if GDISP_HARDWARE_ORIENTATION || defined(__DOXYGEN__)
	/**
	 * @brief   Sets the orientation of the display.
	 * @note    This may be ignored if not supported by the device.
	 *
	 * @param[in] newOrientation    The new orientation
	 *
	 * @notapi
	 */
	void gdisp_lld_setorientation(gdisp_orientation_t newOrientation) {
		if (GDISP1.Orientation == newOrientation)
			return;

		switch(newOrientation) {
			case portrait:
				lld_lcdWriteReg(0x0001, 0x2B3F);
				/* ID = 11 AM = 0 */
				lld_lcdWriteReg(0x0011, 0x6070);
				GDISP1.Height = SCREEN_HEIGHT;
				GDISP1.Width = SCREEN_WIDTH;
				break;
			case landscape:
				lld_lcdWriteReg(0x0001, 0x293F);
				/* ID = 11 AM = 1 */
				lld_lcdWriteReg(0x0011, 0x6078);
				GDISP1.Height = SCREEN_WIDTH;
				GDISP1.Width = SCREEN_HEIGHT;
				break;
			case portraitInv:
				lld_lcdWriteReg(0x0001, 0x2B3F);
				/* ID = 01 AM = 0 */
				lld_lcdWriteReg(0x0011, 0x6040);
				GDISP1.Height = SCREEN_HEIGHT;
				GDISP1.Width = SCREEN_WIDTH;
				break;
			case landscapeInv:
				lld_lcdWriteReg(0x0001, 0x293F);
				/* ID = 01 AM = 1 */
				lld_lcdWriteReg(0x0011, 0x6048);
				GDISP1.Height = SCREEN_WIDTH;
				GDISP1.Width = SCREEN_HEIGHT;
				break;
			default:
				return;
		}
		GDISP1.Orientation = newOrientation;
	}
#endif

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
	    unsigned i = 0;

	    lld_lcdSetCursor(0, 0);
	    lld_lcdWriteStreamStart();

	    for(i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	    	lld_lcdWriteData(color);

	    lld_lcdWriteStreamStop();
	}
#endif

#if GDISP_HARDWARE_LINES || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a line.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x0, y0   The start of the line
	 * @param[in] x1, y1   The end of the line
	 * @param[in] color    The color of the line
	 *
	 * @notapi
	 */
	void gdisp_lld_drawline(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if GDISP_HARDWARE_BOX || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a box.
	 * @pre     The GDISP unit must be in powerOn or powerSleep mode.
	 *
	 * @param[in] x0,y0   The start position
	 * @param[in] cx,cy   The size of the box (outside dimensions)
	 * @param[in] color   The color to use
	 * @param[in] filled  Should the box should be filled
	 *
	 * @notapi
	 */
	void gdisp_lld_drawbox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		/* NOT IMPLEMENTED */
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
	void gdisp_lld_fillarea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP1.Width || y >= GDISP1.Height) return;
			if (x+cx > GDISP1.Width)	cx = GDISP1.Width - x;
			if (y+cy > GDISP1.Height)	cy = GDISP1.Height - y;
		#endif

		unsigned i, area;

		area = cx*cy;
		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();
		for(i = 0; i < area; i++)
			lld_lcdWriteData(color);
		lld_lcdWriteStreamStop();
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] buffer   The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void gdisp_lld_blitarea(coord_t x, coord_t y, coord_t cx, coord_t cy, pixel_t *buffer) {
		unsigned i, area;

		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP1.Width || y >= GDISP1.Height) return;
			if (x+cx > GDISP1.Width) return;
			if (y+cy > GDISP1.Height)	cy = GDISP1.Height - y;
		#endif

		area = cx*cy;
		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();
		for(i = 0; i < area; i++)
			lld_lcdWriteData(*buffer++);
		lld_lcdWriteStreamStop();
	}
#endif

/* Circular Drawing Functions */
#if (GDISP_NEED_CIRCLE && GDISP_HARDWARE_CIRCLES) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a circle.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the circle
	 * @param[in] radius   The radius of the circle
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void gdisp_lld_drawcircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_CIRCLE && GDISP_HARDWARE_CIRCLEFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Create a filled circle.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the circle
	 * @param[in] radius   The radius of the circle
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void gdisp_lld_fillcircle(coord_t x, coord_t y, coord_t radius, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_HARDWARE_ELLIPSES) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an ellipse.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the ellipse is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the ellipse
	 * @param[in] a, b     The dimensions of the ellipse
	 * @param[in] color    The color of the ellipse
	 *
	 * @notapi
	 */
	void gdisp_lld_drawellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_HARDWARE_ELLIPSEFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Create a filled ellipse.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the ellipse is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the ellipse
	 * @param[in] a, b     The dimensions of the ellipse
	 * @param[in] color    The color of the ellipse
	 *
	 * @notapi
	 */
	void gdisp_lld_fillellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_HARDWARE_TEXT) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a character using a transparent background.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The top-left corner of the text
	 * @param[in] c        The character to print
	 * @param[in] color    The color of the character
	 *
	 * @notapi
	 */
	void gdisp_lld_drawchar(coord_t x, coord_t y, char c, font_t font, color_t color) {
		/* NOT IMPLEMENTED */
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_HARDWARE_TEXTFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a character using a filled background.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The top-left corner of the text
	 * @param[in] c        The character to print
	 * @param[in] color    The color of the character
	 * @param[in] bgcolor  The background color
	 *
	 * @notapi
	 */
	void gdisp_lld_fillchar(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		/* NOT IMPLEMENTED */
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
	color_t gdisp_lld_getpixelcolor(coord_t x, coord_t y) {
		color_t color;

		#if GDISP_NEED_VALIDATION
			if (x >= GDISP1.Width || y >= GDISP1.Height) return 0;
		#endif

		lld_lcdSetCursor(x, y);
		lld_lcdWriteStreamStart();

		color = lld_lcdReadData();
		color = lld_lcdReadData();

		lld_lcdWriteStreamStop();

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
	void gdisp_lld_verticalscroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		static color_t buf[((SCREEN_HEIGHT > SCREEN_WIDTH ) ? SCREEN_HEIGHT : SCREEN_WIDTH)];
		coord_t row0, row1;
		unsigned i, gap, abslines;

		abslines = lines < 0 ? -lines : lines;

		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP1.Width || y >= GDISP1.Height) return;
			if (x+cx > GDISP1.Width)	cx = GDISP1.Width - x;
			if (y+cy > GDISP1.Height)	cy = GDISP1.Height - y;
		#endif

		if (!abslines) return;
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
				lld_lcdSetViewPort(x, row0, cx, 1);
				lld_lcdReadStreamStart();
				lld_lcdReadStream(buf, cx);
				lld_lcdReadStreamStop();

				lld_lcdSetViewPort(x, row1, cx, 1);
				lld_lcdWriteStreamStart();
				lld_lcdWriteStream(buf, cx);
				lld_lcdWriteStreamStop();
			}
		}

		/* fill the remaining gap */
		lld_lcdSetViewPort(x, lines > 0 ? (y+gap) : y, cx, abslines);
		lld_lcdWriteStreamStart();
		gap = cx*abslines;
		for(i = 0; i < gap; i++) lld_lcdWriteData(color);
		lld_lcdWriteStreamStop();
	}
#endif

#endif /* HAL_USE_GDISP */
/** @} */
