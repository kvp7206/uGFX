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
 * @file    drivers/tdisp/HD44780/tdisp_lld.c
 * @brief   TDISP driver subsystem low level driver source for the HD44780 display
 *
 * @addtogroup TDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_TDISP || defined(__DOXYGEN__)

/* check first if the user has defined his/her own lowlevel-board file */
#if defined(TDISP_USE_CUSTOM_BOARD) && TDISP_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "tdisp_lld_board.h"
#elif defined(BOARD_OLIMEX_STM32_E407)
	#include "tdisp_lld_board_olimex_e407.h"
#elif defined(BOARD_ST_STM32F4_DISCOVERY)
	#include "tdisp_lld_board_example.h"
#endif

/* Controller Specific Properties */
#define CUSTOM_CHAR_COUNT		8
#define CUSTOM_CHAR_XBITS		5
#define CUSTOM_CHAR_YBITS		8

/* Define the properties of our controller */
tdispStruct	TDISP = {
	TDISP_COLUMNS, TDISP_ROWS,		/* cols, rows */
	CUSTOM_CHAR_XBITS, CUSTOM_CHAR_YBITS,	/* charBitsX, charBitsY */
	CUSTOM_CHAR_COUNT			/* maxCustomChars */
};

/* display control settings */
#define TDISP_DISPLAY_ON		0x04
#define TDISP_CURSOR_ON			0x02
#define TDISP_CURSOR_BLINK		0x01

/* cursor movement settings */
#define TDISP_SHIFT_ON			0x01
#define	TDISP_CURSOR_INC		0x02

/* display scroll settings */
#define TDISP_SCROLL_DISPLAY		0x08
#define TDISP_SCROLL_RIGHT		0x04

static uint8_t	displaycontrol;
static uint8_t	cursorcontrol;

bool_t tdisp_lld_init(void) { 
	/* initialise hardware */
	init_board();
	
	/* The first part is the initialing code.
	 * In this part only the lower nibble of the
	 * byte is written directly to the display, thus
	 * without write_cmd, which sends both high and
	 * low nibble.
	 */
	
	/* Give the LCD a little time to wake up */
	chThdSleepMilliseconds(15);
	
	/* clear the RS-line to the display */
	palClearPad(PORT_CTRL, PIN_RS);
// 	#if BUS_4BITS
		/* write three times 0x03 to display (4-bit mode only)
		* with RS = low.
		*/
		writeToLCD(0x03); // 1x
		chThdSleepMilliseconds(20);
		writeToLCD(0x03); // 2x
// 		chThdSleepMilliseconds(20);
		writeToLCD(0x03); // 3x
// 		chThdSleepMilliseconds(20);
		/* Put display in 4-bit mode by
		* write 0x02 to display.
		*/
		writeToLCD(0x02); // 4bit-modus
// 		chThdSleepMilliseconds(20);
// 	#else
		/* write three times 0x30 to display (8-bit mode only)
		* with RS = low.
		*/
// 		writeToLCD(0x30); // 1x
// 		writeToLCD(0x30); // 2x
// 		writeToLCD(0x30); // 3x
// 	#endif	

	/* From this point on, the LCD accepts
	 * bytes sent with highnibbel first and than
	 *the lownibble if working in 4-bit mode.
	 * In 8-bit mode bytes are written in 1 action.
	 */
	
	/* 4-bit modus, 2 lines en 5x7 characters */
	write_cmd(0x28);
// 	chThdSleepMilliseconds(20);
	
	/* set display on, cursor off and no blinking */
	write_cmd(0x0C);
// 	chThdSleepMilliseconds(20);
	/* set cursor increase and direction */
	write_cmd(0x06);
// 	chThdSleepMilliseconds(20);
	
	displaycontrol = TDISP_DISPLAY_ON;
	cursorcontrol = TDISP_CURSOR_INC;
	/* END OF INITIALISATION */

	return TRUE;
}

/* Clears the display. The display needs
 * a long time to process this command. So
 * an extra delay is insterted.
 */
void tdisp_lld_clear(void) {
	write_cmd(0x01);
}

/* Writes a character to the display */
void tdisp_lld_draw_char(char c) {
	write_data(c);
}

/* Set cursor position */
void tdisp_lld_set_cursor(coord_t col, coord_t row) {
	static const uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

	/*
	 *  Short-cut:
	 *
	 *  If x and y = 0 then use the home command.
	 *
	 *  Note: There is probably no advantage as both commands are a single byte
	 */
//	if (col == 0 && row == 0) {
//		write_cmd(0x02);
//		return;
//	}

	write_cmd(0x80 | (col + row_offsets[row]));
// 	write_cmd(0x80 | (0x40 + col));
}

/* Create a custom character to the display */
void tdisp_lld_create_char(uint8_t address, uint8_t *charmap) {
	int i;

	write_cmd(0x40 | (address << 3));
	for(i = 0; i < CUSTOM_CHAR_YBITS; i++) {
		write_data(charmap[i]);
	}

}

/* Write display control commands to the display */
void tdisp_lld_control(uint16_t what, uint16_t value) {
	switch(what) {
		      case TDISP_CTRL_DISPLAY:
				switch (value) {
					case displayOff:
						displaycontrol &= ~TDISP_DISPLAY_ON;
						break;
					case displayOn:
						displaycontrol |= TDISP_DISPLAY_ON;
						break;
				}
				write_cmd(0x08 | displaycontrol);
				break;
		      case TDISP_CTRL_CURSOR:
				switch (value) {
					case cursorBlinkingBlock:
					case cursorBlinkingUnderline:
					case cursorBlinkingBar:
						displaycontrol |= TDISP_CURSOR_ON + TDISP_CURSOR_BLINK;
						break;
					case cursorBlock:
					case cursorUnderline:
					case cursorBar:
						displaycontrol = (displaycontrol | TDISP_CURSOR_ON) & ~TDISP_CURSOR_BLINK;
						break;
					case cursorOff:
					default:
						displaycontrol &= ~(TDISP_CURSOR_ON | TDISP_CURSOR_BLINK); // zet alleen de cursor uit. Bewaar de overige instellingen
						break;
				}
				write_cmd(0x08 | displaycontrol);
				break;
			case TDISP_CTRL_MOVE:
				switch (value) {
					case cursorIncrease:
						cursorcontrol |= TDISP_CURSOR_INC; // increase cursor position
						break;
					case cursorDecrease:
						cursorcontrol &= ~TDISP_CURSOR_INC; // decrease cursor position
						break;
				}
				write_cmd(0x04 | cursorcontrol);
				break;
			case TDISP_CTRL_SHIFT:
				switch (value) {
					case shiftOn:
						cursorcontrol |= TDISP_SHIFT_ON;
						break;
					case shiftOff:
						cursorcontrol &= ~TDISP_SHIFT_ON;
						break;
				}
				write_cmd(0x04 | cursorcontrol);
				break;
	}
}

/* Scrolls the display in a particular direction with an amount of characters and delays the scroll between each scroll step */
void tdisp_lld_scroll(uint16_t direction, uint16_t amount, uint16_t delay) {
  uint16_t scrollcontrol = 0;
  uint16_t i;
  
  switch (direction) {
    case displayScrollLeft:
      scrollcontrol = TDISP_SCROLL_DISPLAY;
      break;
    case displayScrollRight:
      scrollcontrol = TDISP_SCROLL_DISPLAY | TDISP_SCROLL_RIGHT;
      break;
  }
  
  for(i = 0; i < amount; i++) {
    write_cmd(0x10 | scrollcontrol);
    chThdSleepMilliseconds(delay);	
  }
}

#if TDISP_USE_BACKLIGHT
/* sets the brightness of the display backlight */
void tdisp_lld_set_backlight(uint16_t percentage) {
  set_backlight(percentage);
}
#endif

#endif /* GFX_USE_TDISP */
/** @} */

