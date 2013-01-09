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
 * @file    drivers/tdisp/HD44780/tdisp_lld_board_example.h
 * @brief   TDISP driver subsystem board interface for the HD44780 display
 *
 * @addtogroup TDISP
 * @{
 */

#ifndef _TDISP_LLD_BOARD_H
#define _TDISP_LLD_BOARD_H

void init_board(void) {
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	palSetGroupMode(GPIOD, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
}

void setpin_e(bool_t state) {
	if(state)
		palSetPad(GPIOE, 2);
	else
		palClearPad(GPIOE, 2);
}

void setpin_rs(bool_t state) {
	if(state)
		palSetPad(GPIOE, 0);
	else
		palClearPad(GPIOE, 0);
}

void setpin_rw(bool_t state) {
	if(state)
		palSetPad(GPIOE, 1);
	else
		palClearPad(GPIOE, 1);
}

void write_bus(uint8_t data) {
	palWritePort(GPIOD, data);
}

#endif /* _TDISP_LLD_BOARD_H */

