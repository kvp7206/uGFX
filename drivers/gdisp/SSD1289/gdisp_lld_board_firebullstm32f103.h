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
 * @file    drivers/gdisp/SSD1289/gdisp_lld_board_firebullstm32f103.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1289 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define SET_CS		palSetPad(GPIOD, 12);
#define CLR_CS		palClearPad(GPIOD, 12);
#define SET_RS		palSetPad(GPIOD, 13);
#define CLR_RS		palClearPad(GPIOD, 13);
#define SET_WR		palSetPad(GPIOD, 14);
#define CLR_WR		palClearPad(GPIOD, 14);
#define SET_RD		palSetPad(GPIOD, 15);
#define CLR_RD		palClearPad(GPIOD, 15);

/**
 * @brief   Initialise the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 *
 * @notapi
 */
static __inline void init_board(void) {
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 13, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 15, PAL_MODE_OUTPUT_PUSHPULL);
	
	// Configure the pins to a well know state
	SET_RS;
	SET_RD;
	SET_WR;
	CLR_CS;
}


/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static __inline void setpin_reset(bool_t state) {
	(void) state;
	/* Nothing to do here - reset pin tied to Vcc */
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static __inline void set_backlight(uint8_t percent) {
	(void) percent;
	/* Nothing to do here - Backlight always on */
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
	palWritePort(GPIOE, index);
	CLR_RS; CLR_WR; SET_WR; SET_RS;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static __inline void write_data(uint16_t data) {
	palWritePort(GPIOE, data);
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
	
	// change pin mode to digital input
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_INPUT);

	CLR_RD;
	value = palReadPort(GPIOE);
	value = palReadPort(GPIOE);
	SET_RD;

	// change pin mode back to digital output
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	
	return value;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
