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

/* Include the hardware interface details */
#if defined(TDISP_USE_CUSTOM_BOARD) && TDISP_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "tdisp_lld_board.h"
#elif defined(BOARD_UNKNOWN)
	#include "gdisp_lld_board_unknown.h"
#else
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#endif

/* The user may override the default display size */
#ifndef TDISP_COLUMNS
	#define TDISP_COLUMNS		16
#endif
#ifndef TDISP_ROWS
	#define TDISP_ROWS			2
#endif

/* Controller Specific Properties */
#define CUSTOM_CHAR_COUNT		8
#define CUSTOM_CHAR_XBITS		5
#define CUSTOM_CHAR_YBITS		8

/* Define the properties of our controller */
tdispStruct	TDISP = {
		TDISP_COLUMNS, TDISP_ROWS,				/* cols, rows */
		CUSTOM_CHAR_XBITS, CUSTOM_CHAR_YBITS,	/* charBitsX, charBitsY */
		CUSTOM_CHAR_COUNT						/* maxCustomChars */
		};

/* Our display control */
#define DISPLAY_ON		0x04
#define CURSOR_ON		0x02
#define CURSOR_BLINK	0x01

static uint8_t	displaycontrol;


bool_t tdisp_lld_init(void) { 
	/* initialise hardware */
	init_board();

	/* wait some time */
	chThdSleepMilliseconds(50);

	write_cmd(0x38);
	chThdSleepMilliseconds(64);

	displaycontrol = DISPLAY_ON | CURSOR_ON | CURSOR_BLINK;		// The default displaycontrol
	write_cmd(0x08 | displaycontrol);
	chThdSleepMicroseconds(50);

	write_cmd(0x01);					// Clear the screen
	chThdSleepMilliseconds(5);

	write_cmd(0x06);
	chThdSleepMicroseconds(50);

	return TRUE;
}

void tdisp_lld_clear(void) {
	write_cmd(0x01);
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
	for(i = 0; i < CUSTOM_CHAR_YBITS; i++)
		write_data(charmap[i]);
}

void tdisp_lld_control(uint16_t what, void *value) {
	switch(what) {
		case TDISP_CTRL_BACKLIGHT:
			if ((uint8_t)value)
				displaycontrol |= DISPLAY_ON;
			else
				displaycontrol &= ~DISPLAY_ON;
			write_cmd(0x08 | displaycontrol);
			break;
		case TDISP_CTRL_CURSOR:
			switch((cursorshape)value) {
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

