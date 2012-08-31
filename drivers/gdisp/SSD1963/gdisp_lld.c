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

/**
 * @file    templates/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source template.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "ssd1963.h"

#if HAL_USE_GDISP || defined(__DOXYGEN__)

/* Include the emulation code for things we don't support */
#include "gdisp_emulation.c"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if GDISP_NEED_TEXT
	#include "gdisp_fonts.h"
#endif

/* All the board specific code should go in these include file so the driver
 * can be ported to another board just by creating a suitable file.
 */
#if defined(BOARD_YOURBOARDNAME)
	#include "gdisp_lld_board_yourboardname.h"
#else
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#endif

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
__inline void GDISP_LLD(writeindex)(uint8_t cmd) {
  LCD_REG = cmd;
}

__inline void GDISP_LLD(writereg)(uint16_t lcdReg,uint16_t lcdRegValue) {
	LCD_REG = lcdReg;
	LCD_RAM = lcdRegValue;
}

__inline void GDISP_LLD(writedata)(uint16_t data) {
	LCD_RAM = data;
}

__inline uint16_t GDISP_LLD(readdata)(void) {
	return (LCD_RAM);
}

__inline uint8_t GDISP_LLD(readreg)(uint8_t lcdReg) {
	LCD_REG = lcdReg;
	return (LCD_RAM);
}

__inline void GDISP_LLD(writestreamstart)(void) {
	LCD_REG = SSD1963_WRITE_MEMORY_START;
}

__inline void GDISP_LLD(readstreamstart)(void) {
	LCD_REG = SSD1963_READ_MEMORY_START;
}

__inline void GDISP_LLD(writestream)(uint16_t *buffer, uint16_t size) {
	uint16_t i;
	for(i = 0; i < size; i++)
		LCD_RAM = buffer[i];
}

__inline void GDISP_LLD(readstream)(uint16_t *buffer, size_t size) {
	uint16_t i;

	for(i = 0; i < size; i++) {
		buffer[i] = LCD_RAM;
	}
}
/**
 * @brief   Low level GDISP driver initialisation.
 * @return	TRUE if successful, FALSE on error.
 *
 * @notapi
 */
bool_t GDISP_LLD(init)(void) {
	/* Initialise your display */

	/* Initialise the GDISP structure to match */
	GDISP.Width = SCREEN_WIDTH;
	GDISP.Height = SCREEN_HEIGHT;
	GDISP.Orientation = landscape;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
	
#ifdef LCD_USE_FSMC

#if defined(STM32F1XX)
	/* FSMC setup. TODO: this only works for STM32F1 */
	rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

	/* TODO: pin setup */
#elif defined(STM32F4XX)
	/* STM32F4 FSMC init */
	rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

	/* set pins to FSMC mode */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
							(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
						(1 << 13) | (1 << 14) | (1 << 15), 0};

	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));
#else
#error "FSMC not implemented for this device"
#endif
	const int FSMC_Bank = 0;
	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
			| (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
			| (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

	/* Bank1 NOR/SRAM control register configuration
	 * This is actually not needed as already set by default after reset */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif

	GDISP_LLD(writeindex)(SSD1963_SOFT_RESET);				chThdSleepMicroseconds(100);

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

	GDISP_LLD(writeindex)(SSD1963_SOFT_RESET);				chThdSleepMicroseconds(100);

	/* Screen size */
	GDISP_LLD(writeindex)(SSD1963_SET_LCD_MODE);
//	GDISP_LLD(writedata)(0x0000);
	GDISP_LLD(writedata)(0b00011000);
	GDISP_LLD(writedata)(0x0000);
	GDISP_LLD(writedata)(mHIGH((SCREEN_WIDTH+1)));
	GDISP_LLD(writedata)((SCREEN_WIDTH+1));
	GDISP_LLD(writedata)(mHIGH((SCREEN_HEIGHT+1)));
	GDISP_LLD(writedata)((SCREEN_HEIGHT+1));
	GDISP_LLD(writedata)(0x0000);

	GDISP_LLD(writeindex)(SSD1963_SET_PIXEL_DATA_INTERFACE);
	GDISP_LLD(writedata)(SSD1963_PDI_16BIT565);

	/* LCD Clock specs */
	GDISP_LLD(writeindex)(SSD1963_SET_LSHIFT_FREQ);
	GDISP_LLD(writedata)((LCD_FPR >> 16) & 0xFF);
	GDISP_LLD(writedata)((LCD_FPR >> 8) & 0xFF);
	GDISP_LLD(writedata)(LCD_FPR & 0xFF);

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
#ifdef LCD_USE_FSMC
	/* FSMC delay reduced as the controller now runs at full speed */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_0 | (FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_0) | FSMC_BTR1_BUSTURN_0 ;
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif

	return TRUE;
}

void GDISP_LLD(setwindow)(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	#if GDISP_NEED_VALIDATION
		if (x0 >= GDISP.Width || y0 >= GDISP.Height) return;
		else if (x1 >= GDISP.Width || y1 >= GDISP.Height) return;
	#endif
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
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION
		if (x >= GDISP.Width || y >= GDISP.Height) return;
	#endif
	
    GDISP_LLD(setwindow)(x, y, x, y);
    GDISP_LLD(writestreamstart)();
    GDISP_LLD(writedata)(color);
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
		eg. If fillarea() is defined there is little
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
	void GDISP_LLD(clear)(color_t color) {
		GDISP_LLD(fillarea)(0, 0, GDISP.Width-1, GDISP.Height-1, color);
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
	void GDISP_LLD(drawline)(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		#if GDISP_NEED_VALIDATION
			/* Need to clip to screen */
		#endif
		/* Code here */
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
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP.Width || y >= GDISP.Height) return;
			if (x+cx > GDISP.Width)	cx = GDISP.Width - x;
			if (y+cy > GDISP.Height)	cy = GDISP.Height - y;
		#endif
		
    uint32_t index = 0, area;
    area = (cx+1)*(cy+1);

      GDISP_LLD(setwindow)(x, y, x+cx, y+cy);
      GDISP_LLD(writestreamstart)();

      for(index = 0; index <= area; index++)
          GDISP_LLD(writedata)(color);

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
	void GDISP_LLD(blitarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, const pixel_t *buffer) {
		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP.Width || y >= GDISP.Height) return;
			if (x+cx > GDISP.Width) return;
			if (y+cy > GDISP.Height)	cy = GDISP.Height - y;
		#endif
		/* Code here */
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
	void GDISP_LLD(drawcircle)(coord_t x, coord_t y, coord_t radius, color_t color) {
		#if GDISP_NEED_VALIDATION
			/* Code here */
		#endif
		/* Code here */
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
	void GDISP_LLD(fillcircle)(coord_t x, coord_t y, coord_t radius, color_t color) {
		#if GDISP_NEED_VALIDATION
			/* Code here */
		#endif
		/* Code here */
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
	void GDISP_LLD(drawellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		#if GDISP_NEED_VALIDATION
			/* Code here */
		#endif
		/* Code here */
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
	void GDISP_LLD(fillellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		#if GDISP_NEED_VALIDATION
			/* Code here */
		#endif
		/* Code here */
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
	void GDISP_LLD(drawchar)(coord_t x, coord_t y, char c, font_t font, color_t color) {
		#if GDISP_NEED_VALIDATION
			/* Code here */
		#endif
		/* Code here */
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
	void GDISP_LLD(fillchar)(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		#if GDISP_NEED_VALIDATION
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    Optional.
	 * @note    If x,y is off the screen, the result is undefined.
	 * @return	The color of the specified pixel.
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		#if GDISP_NEED_VALIDATION
			if (x >= GDISP.Width || y >= GDISP.Height) return 0;
		#endif
		/* Code here */
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
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP.Width || y >= GDISP.Height) return;
			if (x+cx > GDISP.Width)	cx = GDISP.Width - x;
			if (y+cy > GDISP.Height)	cy = GDISP.Height - y;
		#endif
		/* Code here */
		
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
	 * @detail	Unsupported control codes are ignored.
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
	void GDISP_LLD(control)(unsigned what, void *value) {
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
							GDISP_LLD(init)();
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
				case portrait:
					/* 	Code here */
					GDISP.Height = SCREEN_HEIGHT;
					GDISP.Width = SCREEN_WIDTH;
					break;
				case landscape:
					/* 	Code here */
					GDISP.Height = SCREEN_WIDTH;
					GDISP.Width = SCREEN_HEIGHT;
					break;
				case portraitInv:
					/* 	Code here */
					GDISP.Height = SCREEN_HEIGHT;
					GDISP.Width = SCREEN_WIDTH;
					break;
				case landscapeInv:
					/* 	Code here */
					GDISP.Height = SCREEN_WIDTH;
					GDISP.Width = SCREEN_HEIGHT;
					break;
				default:
					return;
			}
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
/*
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#if (GDISP_NEED_QUERY && GDISP_HARDWARE_QUERY) || defined(__DOXYGEN__)
/**
 * @brief   Query a driver value.
 * @detail	Typecase the result to the type you want.
 * @note	GDISP_QUERY_WIDTH			- (coord_t)	Gets the width of the screen
 * 			GDISP_QUERY_HEIGHT			- (coord_t)	Gets the height of the screen
 * 			GDISP_QUERY_POWER			- (gdisp_powermode_t) Get the current powermode
 * 			GDISP_QUERY_ORIENTATION		- (gdisp_orientation_t) Get the current screen orientation
 * 			GDISP_QUERY_BACKLIGHT		- (coord_t) Get the backlight state (0 to 100)
 * 			GDISP_QUERY_CONTRAST		- (coord_t) Get the contrast (0 to 100).
 * 			GDISP_QUERY_LLD				- Low level driver control constants start at
 * 											this value.
 *
 * @param[in] what     What to Query
 *
 * @notapi
 */
void *GDISP_LLD(query)(unsigned what) {
	switch(what) {
	case GDISP_QUERY_WIDTH:			return (void *)(unsigned)GDISP.Width;
	case GDISP_QUERY_HEIGHT:		return (void *)(unsigned)GDISP.Height;
	case GDISP_QUERY_POWER:			return (void *)(unsigned)GDISP.Powermode;
	case GDISP_QUERY_ORIENTATION:	return (void *)(unsigned)GDISP.Orientation;
	case GDISP_QUERY_BACKLIGHT:		return (void *)(unsigned)GDISP.Backlight;
	case GDISP_QUERY_CONTRAST:		return (void *)(unsigned)GDISP.Contrast;
	case GDISP_QUERY_LLD+0:
		/* Code here */
	default:						return (void *)-1;
	}
}
#endif

#endif /* HAL_USE_GDISP */
/** @} */
