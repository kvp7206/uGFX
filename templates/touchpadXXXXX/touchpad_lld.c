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

/* put needed macros for your interface/driver here.
 * when using SPI, macros for setting and clearing CS line
 */
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
	/* do communicate with the touchpad controller
	 * to do any inizialisation (mostly spiStart())
	 */
}

/**
 * @brief   Reads out the X direction.
 *
 * @notapi
 */
uint16_t tp_lld_read_x(void) {
    uint16_t x;

	/* do communicate with the touchpad controller
	 * to receive the X-Coordinate
	 */
	x = 0;

    return x;
}

/*
 * @brief	Reads out the Y direction.
 *
 * @notapi
 */
uint16_t tp_lld_read_y(void) {
	uint16_t y;

	/* do communicate with the touchpad controller
	 * to receive the Y-Coordinate
	 */
	y = 0;

	return y;
}

/* ---- Optional Routines ---- */
#if TOUCHPAD_HAS_IRQ || defined(__DOXYGEN__)
	/*
	 * @brief	for checking if touchpad is pressed or not.
	 *
	 * @return	1 if pressed / 0 if not pressed
	 *
	 * @notapi
	 */
	uint8_t tp_lld_irq(void) {
		/* do return PEN IRQ state if your
		 * touchpad controller does have any
		 */
		return 0;
	}
#endif

#if TOUCHPAD_HAS_PRESSURE || defined(__DOXYGEN__)
	/*
	 * @brief	Reads out the Z direction / pressure.
	 *
	 * @notapi
	 */
	uint16_t tp_lld_read_z(void) {
		/* return the pressure */
		return 0;
	}
#endif

#endif /* HAL_USE_TOUCHPAD */
/** @} */

