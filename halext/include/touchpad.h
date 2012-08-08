/*
    ChibiOS-LCD-Driver/RT - Copyright (C) 2012
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
 * @file	touchpad.h
 * @brief	TOUCHPAD Touchpad Driver subsystem header file.
 *
 * @addgroup TOUCHPAD
 * @{
 */
#ifndef _TOUCHPAD_H
#define _TOUCHPAD_H

#if HAL_USE_TOUCHPAD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

/* Include the low level driver information */
#include "touchpad_lld.h"

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#if TOUCHPAD_NEED_MULTITHREAD

	void tpInit(TOUCHPADDriver *tp);
	uint16_t tpReadX(void);
	uint16_t tpReadY(void);

	#if TOUCHPAD_PRESSURE

		uint16_t tpReadZ(void);

	#endif

#else

	#define tpInit(tp)		tp_lld_init()
	#define	tpReadX()		tp_lld_read_x()
	#define tpReadY()		tp_lld_read_y()

	#if TOUCHPAD_PRESSURE

		#define tpReadZ()	tp_lld_read_z()

	#endif

#endif


#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_TOUCHPAD */

#endif /* _TOUCHPAD_H */
/** @} */

