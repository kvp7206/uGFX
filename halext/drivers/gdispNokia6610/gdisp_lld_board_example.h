/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*
   Concepts and parts of this file have been contributed by:
		Joel Bodenmann aka Tectu	-> Maintainer
		Andrew Hannam aka inmarket	-> framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/**
 * @file    gdispNokia6610/gdisp_lld_board_example.h
 * @brief   GDISP Graphic Driver subsystem board interface for the Nokia6610 display.
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
static __inline void gdisp_lld_init_board(void) {
	/* Code here */
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static __inline void gdisp_lld_setpin_reset(bool_t state) {
	/* Code here */
}

/**
 * @brief   Set or clear the lcd back-light pin.
 *
 * @param[in] state		TRUE = lcd back-light on, FALSE = lcd back-light off
 * 
 * @notapi
 */
static __inline void gdisp_lld_setpin_backlight(bool_t state) {
	/* Code here */
}

/**
 * @brief   Send a 9 bit command/data to the lcd.
 * @note	The chip select may need to be asserted/de-asserted
 * 			around the actual spi write
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static __inline void gdisp_lld_write_spi(uint16_t data) {
	/* Code here */
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
static __inline uint16_t gdisp_lld_read_spi(void) {
	/* Code here */
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
