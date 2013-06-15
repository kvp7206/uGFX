/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    drivers/ginput/touch/ADS7843/ginput_lld_mouse_board_example.h
 * @brief   GINPUT Touch low level driver source for the ADS7843 on the example board.
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 * @{
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

/**
 * @brief   Initialise the board for the touch.
 *
 * @notapi
 */
static inline void init_board(void) {
	/* Code here */
	#error "ginputADS7843: You must supply a definition for init_board for your board"
}

/**
 * @brief   Check whether the surface is currently touched
 * @return	TRUE if the surface is currently touched
 *
 * @notapi
 */
static inline bool_t getpin_pressed(void) {
	/* Code here */
	#error "ginputADS7843: You must supply a definition for getpin_pressed for your board"
}

/**
 * @brief   Aquire the bus ready for readings
 *
 * @notapi
 */
static inline void aquire_bus(void) {
	/* Code here */
	#error "ginputADS7843: You must supply a definition for aquire_bus for your board"
}

/**
 * @brief   Release the bus after readings
 *
 * @notapi
 */
static inline void release_bus(void) {
	/* Code here */
	#error "ginputADS7843: You must supply a definition for release_bus for your board"
}

/**
 * @brief   Read a value from touch controller
 * @return	The value read from the controller
 *
 * params[in] port	The controller port to read.
 *
 * @notapi
 */
static inline uint16_t read_value(uint16_t port) {
	/* Code here */
	#error "ginputADS7843: You must supply a definition for read_value for your board"
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
