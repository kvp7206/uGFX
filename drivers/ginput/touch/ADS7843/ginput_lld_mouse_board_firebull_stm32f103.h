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
 * @file    drivers/ginput/touch/ADS7843/ginput_lld_mouse_board_example.h
 * @brief   GINPUT Touch low level driver source for the ADS7843 on the example board.
 *
 * @addtogroup GINPUT_MOUSE
 * @{
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

static const SPIConfig spicfg = { 
    NULL,
	GPIOC, 
    6,
    /* SPI_CR1_BR_2 |*/ SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

/**
 * @brief   Initialise the board for the touch.
 *
 * @notapi
 */
static __inline void init_board(void) {
	spiStart(&SPID1, &spicfg);
}

/**
 * @brief   Check whether the surface is currently touched
 * @return	TRUE if the surface is currently touched
 *
 * @notapi
 */
static __inline bool_t getpin_pressed(void) {
	return (!palReadPad(GPIOC, 4));
}
/**
 * @brief   Aquire the bus ready for readings
 *
 * @notapi
 */
static __inline void aquire_bus(void) {
	spiAcquireBus(&SPID1);
    //TOUCHSCREEN_SPI_PROLOGUE();
    palClearPad(GPIOC, 6);
}

/**
 * @brief   Release the bus after readings
 *
 * @notapi
 */
static __inline void release_bus(void) {
	palSetPad(GPIOC, 6);
	spiReleaseBus(&SPID1);
    //TOUCHSCREEN_SPI_EPILOGUE();
}

/**
 * @brief   Read a value from touch controller
 * @return	The value read from the controller
 *
 * params[in] port	The controller port to read.
 *
 * @notapi
 */
static __inline uint16_t read_value(uint16_t port) {
    static uint8_t txbuf[3] = {0};
    static uint8_t rxbuf[3] = {0};
    uint16_t ret;

    txbuf[0] = port;

    spiExchange(&SPID1, 3, txbuf, rxbuf);

    ret = (rxbuf[1] << 5) | (rxbuf[2] >> 3); 
    
	return ret;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
