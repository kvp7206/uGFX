/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    touchpadXPT2046/touchpad_lld.c
 * @brief   Touchpad Driver subsystem low level driver source.
 *
 * @addtogroup TOUCHPAD
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "touchpad.h"

#if HAL_USE_TOUCHPAD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define TP_CS_HIGH      palSetPad(TP_CS_PORT, TP_CS)
#define TP_CS_LOW       palClearPad(TP_CS_PORT, TP_CS)

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
	TOUCHPADDriver Touchpad;
#endif

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
static const SPIConfig spicfg = {
	NULL,
	TP_CS_PORT,
	TP_CS,
	SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */

/**
 * @brief   Low level Touchpad driver initialization.
 *
 * @notapi
 */
void tp_lld_init(TOUCHPADDriver *tp) {
	spiStart(tp->spid, &spicfg);
}

/**
 * @brief   Reads out the X direction.
 *
 * @notapi
 */
uint16_t tp_lld_read_x(void) {
    uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t y;

    txbuf[0] = 0xd0;
    TP_CS_LOW;
    spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    TP_CS_HIGH;

    y = rxbuf[0] << 4;
    y |= rxbuf[1] >> 4;

    return y;
}

/*
 * @brief	Reads out the X direction.
 *
 * @notapi
 */
uint16_t tp_lld_read_y(void) {
    uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t y;

    txbuf[0] = 0x90;
    TP_CS_LOW;
    spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    TP_CS_HIGH;

    y = rxbuf[0] << 4;
    y |= rxbuf[1] >> 4;

    return y;
}

/* ---- Optional Routines ---- */
#if TOUCHPAD_PRESSURE || defined(__DOXYGEN__)
	/*
	 * @brief	Reads out the Z direction / pressure.
	 *
	 * @notapi
	 */
	uint16_t tp_lld_read_z(void) {
		/* ToDo */
		return 42;
	}
#endif

#endif /* HAL_USE_TOUCHPAD */
/** @} */

