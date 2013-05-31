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
 * bit3 of the hardware-port.
 */
#define hardware_offset	0

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

#if TDISP_USE_BACKLIGHT
/* PWM configuration structure. We use timer 4 channel 2 (orange LED on board). */
static const PWMConfig pwmcfg = {
  1000000,		/* 1 MHz PWM clock frequency. */
  100,			/* PWM period is 100 cycles. */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0
};
#endif

/* Initializes the board.
 * If you are using a specific setup, than copy the boardfile to your
 * project directory and set in gfxconf.h that you are using a custom board file
 * The next thing you have to do is to initialize the hardware port(s) you
 * are using to communicate with the board.
 */
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

	#if TDISP_USE_BACKLIGHT
	/* Display backlight control */
	/* TIM4 is an alternate function 2 (AF2) */
	pwmStart(&PWMD4, &pwmcfg);
	palSetPadMode(GPIOD, 13, PAL_MODE_ALTERNATE(2));
	pwmEnableChannel(&PWMD4, 1, 100);
	#endif
}

/* This is the low-level routine for sending the bits
 * to the LCD-display. This routine shifts
 * the bits so they match the hardware port.
 */
static void writeToLCD(uint8_t data) {
	palWritePort(PORT_DATA, data);
	palSetPad(PORT_CTRL, PIN_EN);
	chThdSleepMicroseconds(5);
// 	chThdSleepMilliseconds(10);
	palClearPad(PORT_CTRL, PIN_EN);
	chThdSleepMicroseconds(5);
// 	chThdSleepMilliseconds(10);
	/* wait a little while so that de display can process the data */
}

/* Writes a command to the display. The
 * RS-line is pulled low and than the
 * data is send.
 */
static void write_cmd(uint8_t data) {
	palClearPad(PORT_CTRL, PIN_RS);
	#if BUS_4BITS
		/* first send the high-nibble */
		writeToLCD((data & 0xF0)>>4);
		/* in 4-bit mode the high-nibble is zeroed out */
		writeToLCD((data & 0x0F));
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
		writeToLCD((data & 0xF0)>>4);
		/* in 4-bit mode the high-nibble is zeroed out */
		writeToLCD((data & 0x0F));
	#else
		writeToLCD(data);
	#endif
}

#if TDISP_USE_BACKLIGHT

/* Sets the brightness of the backlight of the display, using
 * the PWM channel */
static void set_backlight(uint16_t percentage) {
  pwmEnableChannel(&PWMD4, 1, percentage);
}
#endif

#endif /* _TDISP_LLD_BOARD_H */
/** @} */

