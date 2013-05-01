/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

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
 * @file    drivers/ginput/touch/STMPE811/ginput_lld_mouse_board_example.h
 * @brief   GINPUT Touch low level driver source for the STMPE811 on the example board.
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
static void init_board(void)
{
	/* Code here */
	#error "ginputSTMPE811: You must supply a definition for init_board for your board"
}

/**
 * @brief   Check whether an interrupt is raised
 * @return	TRUE if there is an interrupt signal present
 *
 * @notapi
 */
static inline bool_t getpin_irq(void)
{
	/* Code here */
	#error "ginputSTMPE811: You must supply a definition for getpin_irq for your board"
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
static void write_reg(uint8_t reg, uint8_t n, uint16_t val)
{
	/* Code here */
	#error "ginputSTMPE811: You must supply a definition for write_reg for your board"
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
static uint16_t read_reg(uint8_t reg, uint8_t n)
{
	/* Code here */
	#error "ginputSTMPE811: You must supply a definition for read_reg for your board"
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
