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

#ifndef TDISP_ROWS
	#define TDISP_ROWS		16
#endif
#ifndef TDISP_COLUMNS
	#define TDISP_COLUMNS	2
#endif

#include "tdisp_lld_board_example.h"

void TDISP_LLD(write_cmd)(uint8_t data) {
	setpin_rs(FALSE);
	setpin_rw(FALSE);

	write_bus(data);

	setpin_e(TRUE);
	chThdSleepMicroseconds(1);
	setpin_e(FALSE);
	chThdSleepMicroseconds(5);
}

void TDISP_LLD(write_data)(uint8_t data) {
	setpin_rs(TRUE);
	setpin_rw(FALSE);

	write_bus(data);

	setpin_e(TRUE);
	chThdSleepMicroseconds(1);
	setpin_e(FALSE);
	chThdSleepMicroseconds(5);
}

bool_t TDISP_LLD(init)(void) { 
	/* initialise hardware */
	init_board();

	/* initialise controller */
	chThdSleepMilliseconds(50);
	TDISP_LLD(write_cmd)(0x38);
	chThdSleepMilliseconds(64);
	TDISP_LLD(write_cmd)(0x0f);	
	chThdSleepMicroseconds(50);
	TDISP_LLD(write_cmd)(0x01);
	chThdSleepMilliseconds(5);
	TDISP_LLD(write_cmd)(0x06);
	chThdSleepMicroseconds(50);

	return TRUE;
}

#endif /* GFX_USE_TDISP */
/** @} */

