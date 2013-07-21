/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/tdisp/HD44780/tdisp_lld_board_st_stm32f4_discovery.h
 * @brief   TDISP driver subsystem board interface for the HD44780 display
 *
 * @addtogroup TDISP
 * @{
 */

#ifndef _TDISP_LLD_BOARD_H
#define _TDISP_LLD_BOARD_H

/* Configure these to match the hardware connections on your board */
#define BUS_4BITS	TRUE

/* Configure the bitoffset in the dataport so they match with the
 * hardware pins. An offset of 0 means bit0 stays at bit0 of the dataport.
 * If the offset is set to 3, bit0 of the nibble will be positioned at
 * P[A..G]3 of the hardware-port.
 */
#define hardware_offset	3

/* The port where the data is sent to. In the
 * low-leveldriver het hardware_offset is taken
 * into account. If for example the hardware_offset
 * is set to 3, then de data will be sent to
 * PE3, PE4, PE5 en PE6, if the dataport where GPIOE.
 */
#define PORT_DATA	GPIOE

/* The port used to controle the controle lines of
 * the display.
 */
#define PORT_CTRL	GPIOD
/* Pin to controle the R/S-line of the display */
#define PIN_RS		0
/* Pin to controle the EN-line of the display */
#define PIN_EN		1
/* Pin to controle the R/W-pin of the display.
 * If reading of the display is not used disable
 * reading in the gfxconf.h and put a dummy value here
 * as it will not be used.
 */
#define PIN_RW		7


static void init_board(void) {
	/* Initialize the ports for data and controle-lines */
	palSetGroupMode(PORT_CTRL, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	palSetGroupMode(PORT_DATA, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	/* Set alle controle pins to low */
	palClearPad(PORT_CTRL, PIN_RS);
	palClearPad(PORT_CTRL, PIN_EN);
	#if TDISP_NEED_READ
	  palClearPad(PORT_CTRL, PIN_RW);
	#endif
}

/* This is the low-level routine for sending the bits
 * to the LCD-display. This routine shifts
 * the bits so they match the hardware port.
 */
static void writeToLCD(uint8_t data) {
	palWritePort(PORT_DATA, data<<hardware_offset);
	palSetPad(PORT_CTRL, PIN_EN);
	gfxSleepMicroseconds(1);
	palClearPad(PORT_CTRL, PIN_EN);
	/* wait a little while so that de display can process the data */
	gfxSleepMicroseconds(5);
}

/* Writes a command to the display. The
 * RS-line is pulled low and than the
 * data is send.
 */
static void write_cmd(uint8_t data) {
	palClearPad(PORT_CTRL, PIN_RS);
	#if BUS_4BITS
		/* first send the high-nibble */
		writeToLCD(data>>4);
	#endif
	/* send the low-nibble */
	#if BUS_4BITS
		/* in 4-bit mode the high-nibble is zeroed out */
		writeToLCD(data & 0x0F);
	#else
		writeToLCD(data);
	#endif
}

// static void write_initcmd(uint8_t data) {
//   write_cmd(data);
// }

/* Write data to the display. The
 * RS-line is pulled high and than the
 * data is send.
 */
static void write_data(uint8_t data) {
	palSetPad(PORT_CTRL, PIN_RS);
	#if BUS_4BITS
		/* first send the high-nibble */
		writeToLCD(data>>4);
	#endif
	/* send the low-nibble */
	#if BUS_4BITS
		/* in 4-bit mode the high-nibble is zeroed out */
		writeToLCD(data & 0x0F);
	#else
		writeToLCD(data);
	#endif
}

#endif /* _TDISP_LLD_BOARD_H */
/** @} */

