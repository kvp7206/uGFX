/*
    ChibiOS-LCD-Driver - Copyright (C) 2012
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
 * @file	touchpad.c
 * @brief	Touchpad Driver code.
 *
 * @addgroup TOUCHPAD
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "touchpad.h"

#if HAL_USE_TOUCHPAD || defined(__DOXYGEN__)

#if GDISP_NEED_MULTITHREAD
    #warning "TOUCHPAD: Multithread support not complete"
    #define MUTEX_INIT      /* Not defined yet */
    #define MUTEX_ENTER     /* Not defined yet */
    #define MUTEX_EXIT      /* Not defined yet */
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

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

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

#if TOUCHPAD_NEED_MULTITHREAD || defined(__DOXYGEN__)
    /**
     * @brief	Touchpad Driver initialization.
     * @note    This function is NOT currently implicitly invoked by @p halInit().
     *          It must be called manually.
     *
     * @init
     */
    void tpInit(TOUCHPADDriver * UNUSED(tp)) {
        /* Initialise Mutex */
        MUTEX_INIT

        /* Initialise driver */
        MUTEX_ENTER
        tp_lld_init(tp);
        MUTEX_EXIT
    }
#endif

#if TOUCHPAD_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief	Get the X-Coordinate, relative to screen zero point.
	 *
	 * @return	The X position in pixels.
	 *
	 * @api
	 */
	uint16_t tpReadX(void) {
		return (tp_lld_read_x());
	}
#endif

#if TOUCHPAD_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief	Get the X-Coordinate, relative to screen zero point.
	 *
	 * @return	The Y position in pixels.
	 *
	 * @api
	 */
	uint16_t tpReadY(void) {
		return (tp_lld_read_y());
	}
#endif

#if TOUCHPAD_NEED_MULTITHREAD || defined(__DOXYGEN__)
	/**
	 * @brief	Get the pressure.
	 *
	 * @return	The pressure.
	 *
	 * @api
	 */
	uint16_t tpReadZ(void) {
		return (tp_lld_read_z());
	}
#endif

#endif /* HAL_USE_TOUCHPAD */
/** @} */

