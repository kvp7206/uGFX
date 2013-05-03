/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
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

#if GFX_USE_TDISP /*|| defined(__DOXYGEN__)*/

/* check first if the user has defined his/her own lowlevel-board file */
#if defined(TDISP_USE_CUSTOM_BOARD) && TDISP_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "tdisp_lld_board.h"
#elif defined(BOARD_OLIMEX_STM32_E407)
	#include "tdisp_lld_board_olimex_e407.h"
#elif defined(BOARD_ST_STM32F4_DISCOVERY)
	#include "tdisp_lld_board_example.h"
#endif


/* The user may override the default display size */
#ifndef TDISP_COLUMNS
	#define TDISP_COLUMNS		16
#endif
#ifndef TDISP_ROWS
	#define TDISP_ROWS		2
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

/* Our display control */
#define TDISP_DISPLAY_ON		0x04
#define TDISP_CURSOR_ON			0x02
#define TDISP_CURSOR_BLINK		0x01

static uint8_t	displaycontrol;


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
	
	/* write three times 0x03 to display
	 * with RS = low.
	 */
	palClearPad(PORT_CTRL, PIN_RS);
	#if BUS_4BITS
		writeToLCD(0x03); // 1x
		writeToLCD(0x03); // 2x
		writeToLCD(0x03); // 3x
		/* Put display in 4-bit mode by
		* write 0x02 to display.
		*/
		writeToLCD(0x02); // 4bit-modus
	#else
		writeToLCD(0x30); // 1x
		writeToLCD(0x30); // 2x
		writeToLCD(0x30); // 3x
	#endif	

	/* From this point on, the LCD accepts
	 * bytes sent with highnibbel first and than
	 *the lownibble.
	 */
	
	/* 4-bit modus, 2 lines en 5x7 characters */
	write_cmd(0x28);
	
	displaycontrol = TDISP_DISPLAY_ON;
	
	/* set display on, cursor off and no blinking */
	write_cmd(0x0C);
	/* set cursor move direction */
	write_cmd(0x06);
	/* END OF INITIALISATION */

// 	/* wait some time */
// 	chThdSleepMilliseconds(50);
// 
// 	write_cmd(0x38);
// 	chThdSleepMilliseconds(64);
// 
// 	displaycontrol = DISPLAY_ON | CURSOR_ON | CURSOR_BLINK;		// The default displaycontrol
// 	write_cmd(0x08 | displaycontrol);
// 	chThdSleepMicroseconds(50);
// 
// 	write_cmd(0x01);					// Clear the screen
// 	chThdSleepMilliseconds(5);
// 
// 	write_cmd(0x06);
// 	chThdSleepMicroseconds(50);

	return TRUE;
}

/* Clears the display. The display needs
 * a long time to process this command. So
 * an extra delay is insterted.
 */
void tdisp_lld_clear(void) {
	write_cmd(0x01);
// 	chThdSleepMilliseconds(LONG_DELAY_MS);
}

void tdisp_lld_draw_char(char c) {
	write_data(c);
}

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
}

void tdisp_lld_create_char(uint8_t address, uint8_t *charmap) {
	int i;

	write_cmd(0x40 | (address << 3));
	for(i = 0; i < CUSTOM_CHAR_YBITS; i++) {
		write_data(charmap[i]);
	}

}

void tdisp_lld_control(uint16_t what, uint16_t value) {
      
	switch(what) {
		case TDISP_CTRL_BACKLIGHT:
			if ((uint8_t)value)
				displaycontrol |= DISPLAY_ON;
			else
				displaycontrol &= ~DISPLAY_ON;
			write_cmd(0x08 | displaycontrol);
			break;
		case TDISP_CTRL_CURSOR:
			switch((uint8_t)value) {
			case cursorOff:
				displaycontrol &= ~CURSOR_ON;
				break;
			case cursorBlock:
			case cursorUnderline:
			case cursorBar:
				displaycontrol = (displaycontrol | CURSOR_ON) & ~CURSOR_BLINK;
				break;
			case cursorBlinkingBlock:
			case cursorBlinkingUnderline:
			case cursorBlinkingBar:
			default:
				displaycontrol |= (CURSOR_ON | CURSOR_BLINK);
				break;
			}
			write_cmd(0x08 | displaycontrol);
			break;
	}
}

#endif /* GFX_USE_TDISP */
/** @} */

