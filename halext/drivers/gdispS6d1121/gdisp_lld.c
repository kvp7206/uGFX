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
 * @file    gdispS6d1121/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the S6d1121 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

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

#include "s6d1121_lld.c.h"

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
bool_t GDISP_LLD(init)(void) {
	palSetPadMode(LCD_RST_GPIO, LCD_RST_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	// A Good idea to reset the module before using
	LCD_RST_LOW;
	s6d1121_delay(2);
	LCD_RST_HIGH;         // Hardware Reset
	s6d1121_delay(2);

	#ifdef LCD_USE_GPIO
		// IO Default Configurations
		palSetPadMode(LCD_CS_GPIO, LCD_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
		palSetPadMode(LCD_WR_GPIO, LCD_WR_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
		palSetPadMode(LCD_RD_GPIO, LCD_RD_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
		palSetPadMode(LCD_RS_GPIO, LCD_RS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
		palSetPadMode(LCD_BL_GPIO, LCD_BL_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

		palSetGroupMode(LCD_D0_GPIO, 0x0000000F, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
		palSetGroupMode(LCD_D4_GPIO, 0x0000FFF0, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

		LCD_CS_HIGH;
		LCD_RD_HIGH;
		LCD_WR_HIGH;
		LCD_BL_LOW;

	#elif defined(LCD_USE_FSMC)
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

		int FSMC_Bank = 0;
		/* FSMC timing */
		FSMC_Bank1->BTCR[FSMC_Bank+1] = (10) | (10 << 8) | (10 << 16);

		/* Bank1 NOR/SRAM control register configuration */
		FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	#endif

	lld_lcdWriteReg(0x11,0x2004);
	lld_lcdWriteReg(0x13,0xCC00);
	lld_lcdWriteReg(0x15,0x2600);
	lld_lcdWriteReg(0x14,0x252A);
	lld_lcdWriteReg(0x12,0x0033);
	lld_lcdWriteReg(0x13,0xCC04);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x13,0xCC06);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x13,0xCC4F);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x13,0x674F);
	lld_lcdWriteReg(0x11,0x2003);

	s6d1121_delay(1);

	// Gamma Setting
	lld_lcdWriteReg(0x30,0x2609);
	lld_lcdWriteReg(0x31,0x242C);
	lld_lcdWriteReg(0x32,0x1F23);
	lld_lcdWriteReg(0x33,0x2425);
	lld_lcdWriteReg(0x34,0x2226);
	lld_lcdWriteReg(0x35,0x2523);
	lld_lcdWriteReg(0x36,0x1C1A);
	lld_lcdWriteReg(0x37,0x131D);
	lld_lcdWriteReg(0x38,0x0B11);
	lld_lcdWriteReg(0x39,0x1210);
	lld_lcdWriteReg(0x3A,0x1315);
	lld_lcdWriteReg(0x3B,0x3619);
	lld_lcdWriteReg(0x3C,0x0D00);
	lld_lcdWriteReg(0x3D,0x000D);

	lld_lcdWriteReg(0x16,0x0007);
	lld_lcdWriteReg(0x02,0x0013);
	lld_lcdWriteReg(0x03,0x0003);
	lld_lcdWriteReg(0x01,0x0127);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x08,0x0303);
	lld_lcdWriteReg(0x0A,0x000B);
	lld_lcdWriteReg(0x0B,0x0003);
	lld_lcdWriteReg(0x0C,0x0000);
	lld_lcdWriteReg(0x41,0x0000);
	lld_lcdWriteReg(0x50,0x0000);
	lld_lcdWriteReg(0x60,0x0005);
	lld_lcdWriteReg(0x70,0x000B);
	lld_lcdWriteReg(0x71,0x0000);
	lld_lcdWriteReg(0x78,0x0000);
	lld_lcdWriteReg(0x7A,0x0000);
	lld_lcdWriteReg(0x79,0x0007);
	lld_lcdWriteReg(0x07,0x0051);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x07,0x0053);
	lld_lcdWriteReg(0x79,0x0000);

	lld_lcdResetViewPort();

	/* Now initialise the GDISP structure */
	GDISP.Width = SCREEN_WIDTH;
	GDISP.Height = SCREEN_HEIGHT;
	GDISP.Orientation = portrait;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
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
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION
		if (x >= GDISP.Width || y >= GDISP.Height) return;
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
	    unsigned i;

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

		unsigned i, area;

		area = cx*cy;
		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();
		for(i = 0; i < area; i++)
			lld_lcdWriteData(color);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
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
		unsigned i, area;

		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP.Width || y >= GDISP.Height) return;
			if (x+cx > GDISP.Width) return;
			if (y+cy > GDISP.Height)	cy = GDISP.Height - y;
		#endif

		area = cx*cy;
		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();
		for(i = 0; i < area; i++)
			lld_lcdWriteData(*buffer++);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
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
	#include "gdisp_fonts.h"
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
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		/* This routine is marked "DO NOT USE" in the original
		 *  GLCD driver. We just keep our GDISP_HARDWARE_READPIXEL
		 *  turned off for now.
		 */
		color_t color;

		#if GDISP_NEED_VALIDATION
			if (x >= GDISP.Width || y >= GDISP.Height) return 0;
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
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		/* This is marked as "TODO: Test this" in the original GLCD driver.
		 * For now we just leave the GDISP_HARDWARE_SCROLL off.
		 */
		static color_t buf[((SCREEN_HEIGHT > SCREEN_WIDTH ) ? SCREEN_HEIGHT : SCREEN_WIDTH)];
		coord_t row0, row1;
		unsigned i, gap, abslines;

		abslines = lines < 0 ? -lines : lines;

		#if GDISP_NEED_VALIDATION
			if (cx < 1 || cy < 1 || x >= GDISP.Width || y >= GDISP.Height) return;
			if (x+cx > GDISP.Width)	cx = GDISP.Width - x;
			if (y+cy > GDISP.Height)	cy = GDISP.Height - y;
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
		lld_lcdResetViewPort();
	}
#endif

#if GDISP_HARDWARE_CONTROL || defined(__DOXYGEN__)
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
					/* 	Code here */
					/* break; */
				case powerOn:
					/* 	Code here */
					/* You may need this ---
						if (GDISP.Powermode != powerSleep)
							GDISP_LLD(init();
					*/
					/* break; */
				case powerSleep:
					/* 	Code here */
					/* break; */
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
				lld_lcdWriteReg(0x0001,0x0127);
				lld_lcdWriteReg(0x03, 0b0011);
				GDISP.Height = SCREEN_HEIGHT;
				GDISP.Width = SCREEN_WIDTH;
				break;
			case landscape:
				lld_lcdWriteReg(0x0001,0x0027);
				lld_lcdWriteReg(0x0003, 0b1011);
				GDISP.Height = SCREEN_WIDTH;
				GDISP.Width = SCREEN_HEIGHT;
				break;
			case portraitInv:
				lld_lcdWriteReg(0x0001,0x0127);
				lld_lcdWriteReg(0x0003, 0b0000);
				GDISP.Height = SCREEN_HEIGHT;
				GDISP.Width = SCREEN_WIDTH;
				break;
			case landscapeInv:
				lld_lcdWriteReg(0x0001,0x0027);
				lld_lcdWriteReg(0x0003, 0b1000);
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
