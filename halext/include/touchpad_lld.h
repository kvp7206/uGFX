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
 * @file	touchpad_lld.h
 * @brief	TOUCHPAD Driver subsystem low level driver header.
 *
 * @addgroup TOUCHPAD
 * @{
 */

#ifndef _TOUCHPAD_LLD_H
#define _TOUCHPAD_LLD_H

#if HAL_USE_TOUCHPAD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Include the low level driver configuration information                    */
/*===========================================================================*/

#include "gdisp_lld_config.h"

/*===========================================================================*/
/* Error checks.                                                             */
/*===========================================================================*/

#ifndef TOUCHPAD_PRESSURE
	#define TOUCHPAD_PRESSURE	FALSE
#endif

/*===========================================================================*/
/* Driver types.                                                             */
/*===========================================================================*/

/**
 * @brief	Structure representing a Touchpad driver.
 */
struct TOUCHPADDriver {
	/* ToDo */	
};

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
    extern TOUCHPADDriver Touchpad;
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/* Core functions */
	void tp_lld_init(void);
	uint16_t tp_lld_read_x(void);
	uint16_t tp_lld_read_y(void);

	#if TOUCHPAD_PRESSURE
	uint16_t tp_lld_read_z(void);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_TOUCHPAD */

#endif /* _TOUCHPAD_LLD_H */
/** @} */

