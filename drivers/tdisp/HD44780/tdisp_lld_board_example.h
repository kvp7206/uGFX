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

/**
 * The board may override the default display size.
 * Uncomment the below if your board needs a non-standard size.
 */
/*
#ifndef TDISP_COLUMNS
	#define TDISP_COLUMNS		16
#endif
#ifndef TDISP_ROWS
	#define TDISP_ROWS			2
#endif
*/

static void init_board(void) {
	/* Code here */
	#error "tdispHD44780: You must supply a definition for init_board for your board"
}

static void write_cmd(uint8_t data) {
	/* Code here */
	#error "tdispHD44780: You must supply a definition for write_cmd for your board"
}

static void write_data(uint8_t data) {
	/* Code here */
	#error "tdispHD44780: You must supply a definition for write_data for your board"
}

#endif /* _TDISP_LLD_BOARD_H */
/** @} */
