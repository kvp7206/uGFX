/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/tdisp/HD44780/tdisp_lld_board_olimex_e407.h
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
	gfxSleepMicroseconds(1);
	palClearPad(PORT_CTRL, PIN_EN);
	gfxSleepMicroseconds(5);
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

