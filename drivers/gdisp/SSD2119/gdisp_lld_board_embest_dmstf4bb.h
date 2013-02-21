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
 * @file    drivers/gdisp/SSD2119/gdisp_lld_board_embest_dmstf4bb.h
 * @brief   GDISP Graphic Driver subsystem board GPIO interface for the SSD2119 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define SET_CS		palSetPad(GPIOD, 7);
#define CLR_CS		palClearPad(GPIOD, 7);
#define SET_DC		palSetPad(GPIOE, 3);
#define CLR_DC		palClearPad(GPIOE, 3);
#define SET_WR		palSetPad(GPIOD, 5);
#define CLR_WR		palClearPad(GPIOD, 5);
#define SET_RD		palSetPad(GPIOD, 4);
#define CLR_RD		palClearPad(GPIOD, 4);
#define SET_RST		palSetPad(GPIOD, 3);
#define CLR_RST		palClearPad(GPIOD, 3);

/**
 * @brief   Initialise the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 *
 * @notapi
 */
static __inline void init_board(void) {
	// D0 - D15
	palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 15, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD,  0, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD,  1, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE,  7, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE,  8, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE,  9, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE, 10, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE, 11, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE, 12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE, 13, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE, 14, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOE, 15, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD,  8, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD,  9, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 10, PAL_MODE_OUTPUT_PUSHPULL);

	// RST
	palSetPadMode(GPIOD,  3, PAL_MODE_OUTPUT_PUSHPULL);

	// CS
	palSetPadMode(GPIOD,  7, PAL_MODE_OUTPUT_PUSHPULL);
	// DC
	palSetPadMode(GPIOE,  3, PAL_MODE_OUTPUT_PUSHPULL);
	// RD
	palSetPadMode(GPIOD,  4, PAL_MODE_OUTPUT_PUSHPULL);
	// WR
	palSetPadMode(GPIOD,  5, PAL_MODE_OUTPUT_PUSHPULL);

	/* Configure the pins to a well know state */
	SET_DC;
	SET_RD;
	SET_WR;
	CLR_CS;
	//SET_RST;

	/* Display backlight control */
	/* TIM4 is an alternate function 2 (AF2) */
	pwmStart(&PWMD4, &pwmcfg);
	palSetPadMode(GPIOD, 13, PAL_MODE_ALTERNATE(2));
	pwmEnableChannel(&PWMD4, 1, 100);
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 *
 * @notapi
 */
static __inline void setpin_reset(bool_t state) {
	if (state) {
		CLR_RST;
	} else {
		SET_RST;
	}
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] percent		0 to 100%
 *
 * @notapi
 */
static __inline void set_backlight(uint8_t percent) {
	pwmEnableChannel(&PWMD4, 1, percent);
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @notapi
 */
static __inline void acquire_bus(void) {
	/* Nothing to do here since LCD is the only device on that bus */
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @notapi
 */
static __inline void release_bus(void) {
	/* Nothing to do here since LCD is the only device on that bus */
}

/**
 * @brief   Send data to the index register.
 *
 * @param[in] index		The index register to set
 *
 * @notapi
 */
static __inline void write_index(uint16_t index) {
	// D0 - D15
	palWritePad(GPIOD, 14, index & 1);
	palWritePad(GPIOD, 15, (index >> 1) & 1);
	palWritePad(GPIOD,  0, (index >> 2) & 1);
	palWritePad(GPIOD,  1, (index >> 3) & 1);
	palWritePad(GPIOE,  7, (index >> 4) & 1);
	palWritePad(GPIOE,  8, (index >> 5) & 1);
	palWritePad(GPIOE,  9, (index >> 6) & 1);
	palWritePad(GPIOE, 10, (index >> 7) & 1);
	palWritePad(GPIOE, 11, (index >> 8) & 1);
	palWritePad(GPIOE, 12, (index >> 9) & 1);
	palWritePad(GPIOE, 13, (index >> 10) & 1);
	palWritePad(GPIOE, 14, (index >> 11) & 1);
	palWritePad(GPIOE, 15, (index >> 12) & 1);
	palWritePad(GPIOD,  8, (index >> 13) & 1);
	palWritePad(GPIOD,  9, (index >> 14) & 1);
	palWritePad(GPIOD, 10, (index >> 15) & 1);

	/* Control lines */
	CLR_DC; CLR_WR; SET_WR; SET_DC;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 *
 * @notapi
 */
static __inline void write_data(uint16_t data) {
	// D0 - D15
	palWritePad(GPIOD, 14, data & 1);
	palWritePad(GPIOD, 15, (data >> 1) & 1);
	palWritePad(GPIOD,  0, (data >> 2) & 1);
	palWritePad(GPIOD,  1, (data >> 3) & 1);
	palWritePad(GPIOE,  7, (data >> 4) & 1);
	palWritePad(GPIOE,  8, (data >> 5) & 1);
	palWritePad(GPIOE,  9, (data >> 6) & 1);
	palWritePad(GPIOE, 10, (data >> 7) & 1);
	palWritePad(GPIOE, 11, (data >> 8) & 1);
	palWritePad(GPIOE, 12, (data >> 9) & 1);
	palWritePad(GPIOE, 13, (data >> 10) & 1);
	palWritePad(GPIOE, 14, (data >> 11) & 1);
	palWritePad(GPIOE, 15, (data >> 12) & 1);
	palWritePad(GPIOD,  8, (data >> 13) & 1);
	palWritePad(GPIOD,  9, (data >> 14) & 1);
	palWritePad(GPIOD, 10, (data >> 15) & 1);

	/* Control lines */
	CLR_WR; SET_WR;
}

#if GDISP_HARDWARE_READPIXEL || GDISP_HARDWARE_SCROLL || defined(__DOXYGEN__)
/**
 * @brief   Read data from the lcd.
 *
 * @return	The data from the lcd
 * @note	The chip select may need to be asserted/de-asserted
 * 			around the actual spi read
 *
 * @notapi
 */
static __inline uint16_t read_data(void) {
	uint16_t	value;
/*
	// change pin mode to digital input
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_INPUT);

	CLR_RD;
	value = palReadPort(GPIOE);
	value = palReadPort(GPIOE);
	SET_RD;

	// change pin mode back to digital output
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
*/
	return value;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
