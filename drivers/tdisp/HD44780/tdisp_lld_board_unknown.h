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
 * @file    drivers/tdisp/HD44780/tdisp_lld_board_unknown.h
 * @brief   TDISP driver subsystem board interface for the HD44780 display
 *
 * @addtogroup TDISP
 * @{
 */

#ifndef _TDISP_LLD_BOARD_H
#define _TDISP_LLD_BOARD_H

/* Configure these to match the hardware connections on your board */
#define BUS_4BITS	FALSE
#define PORT_DATA	GPIOG
#define PORT_CTRL	GPIOE
#define PIN_RS		0
#define PIN_RW		1
#define PIN_EN		2

static void init_board(void) {
	palSetGroupMode(PORT_CTRL, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	palSetGroupMode(PORT_DATA, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(PORT_CTRL, PIN_RW);
}

static void writeToLCD(uint8_t data) {
	palWritePort(PORT_DATA, data);
	palSetPad(PORT_CTRL, PIN_EN);
	chThdSleepMicroseconds(1);
	palClearPad(PORT_CTRL, PIN_EN);
	chThdSleepMicroseconds(5);
}

static void write_cmd(uint8_t data) {
	palClearPad(PORT_CTRL, PIN_RS);
	#if BUS_4BITS
		writeToLCD(data>>4);
	#endif
	writeToLCD(data);
}

static void write_data(uint8_t data) {
	palSetPad(PORT_CTRL, PIN_RS);
	#if BUS_4BITS
		writeToLCD(data>>4);
	#endif
	writeToLCD(data);
}

#endif /* _TDISP_LLD_BOARD_H */
/** @} */
