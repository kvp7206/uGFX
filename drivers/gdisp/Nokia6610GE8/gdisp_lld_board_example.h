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
 * @file    drivers/gdisp/Nokia6610GE8/gdisp_lld_board_example.h
 * @brief   GDISP Graphic Driver subsystem board interface for the Nokia6610 GE8 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/**
 * @brief   Initialise the board for the display.
 * @notes	Performs the following functions:
 *			1. initialise the spi port used by your display
 *			2. initialise the reset pin (initial state not-in-reset)
 *			3. initialise the chip select pin (initial state not-active)
 *			4. initialise the backlight pin (initial state back-light off)
 *
 * @notapi
 */
static __inline void init_board(void) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for init_board for your board"
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static __inline void setpin_reset(bool_t state) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for setpin_reset for your board"
}

/**
 * @brief   Set the lcd back-light level.
 * @note	For now 0% turns the backlight off, anything else the backlight is on.
 *			While the hardware supports PWM backlight control, we are not using it
 *			yet.
 *
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static __inline void set_backlight(uint8_t percent) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for set_backlight for your board"
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @notapi
 */
static __inline void acquire_bus(void) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for acquire_bus for your board"
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @notapi
 */
static __inline void release_bus(void) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for release_bus for your board"
}

/**
 * @brief   Send an 8 bit command to the lcd.
 *
 * @param[in] cmd		The command to send
 *
 * @notapi
 */
static __inline void write_cmd(uint16_t cmd) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for write_cmd for your board"
}

/**
 * @brief   Send an 8 bit data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static __inline void write_data(uint16_t data) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for write_data for your board"
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
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for read_data for your board"
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
