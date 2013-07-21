/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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

