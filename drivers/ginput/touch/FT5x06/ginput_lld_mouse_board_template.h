/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/ginput/touch/FT5x06/ginput_lld_mouse_board_template.h
 * @brief   GINPUT Touch low level driver source for the FT5x06 on the example board.
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
static void init_board(void) {

}

/**
 * @brief   Check whether an interrupt is raised
 * @return	TRUE if there is an interrupt signal present
 *
 * @notapi
 */
static inline bool_t getpin_irq(void) {

}

/**
 * @brief	Write a value into a certain register
 *
 * @param[in] reg	The register address
 * @param[in] n		The amount of bytes (one or two)
 * @param[in] val	The value
 *
 * @notapi
 */
static void write_reg(uint8_t reg, uint8_t n, uint16_t val) {

}

/**
 * @brief	Read the value of a certain register
 *
 * @param[in] reg	The register address
 * @param[in] n		The amount of bytes (one or two)
 *
 * @return Data read from device (one byte or two depending on n param)
 *
 * @notapi
 */
static uint16_t read_reg(uint8_t reg, uint8_t n) {

}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */

