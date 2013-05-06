/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
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
static inline void init_board(void) {
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
static inline void setpin_reset(bool_t state) {
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
static inline void set_backlight(uint8_t percent) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for set_backlight for your board"
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @notapi
 */
static inline void acquire_bus(void) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for acquire_bus for your board"
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @notapi
 */
static inline void release_bus(void) {
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
static inline void write_cmd(uint16_t cmd) {
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
static inline void write_data(uint16_t data) {
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
static inline uint16_t read_data(void) {
	/* Code here */
	#error "gdispNokia6610GE8: You must supply a definition for read_data for your board"
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
