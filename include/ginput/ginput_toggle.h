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
 * @file    ginput/ginput_toggle.h
 * @brief   GINPUT GFX User Input subsystem header file.
 *
 * @addtogroup GINPUT
 * @{
 */
#ifndef _GINPUT_TOGGLE_H
#define _GINPUT_TOGGLE_H

/**
 * @name    GINPUT more complex functionality to be compiled
 * @{
 */
	/**
	 * @brief   Should hardware toggle/switch/button (pio) functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_TOGGLE
		#define GINPUT_NEED_TOGGLE	FALSE
	#endif
/** @} */

#if GINPUT_NEED_TOGGLE || defined(__DOXYGEN__)

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for various ginput sources
#define GEVENT_TOGGLE		(GEVENT_GINPUT_FIRST+3)

// Get the hardware definitions - Number of instances etc.
#include "ginput_lld_toggle_config.h"

typedef struct GEventToggle_t {
	GEventType		type;				// The type of this event (GEVENT_TOGGLE)
	uint16_t		instance;			// The toggle instance
	bool_t			on;					// True if the toggle/button is on
	} GEventToggle;

// Toggle Listen Flags - passed to geventAddSourceToListener()
#define GLISTEN_TOGGLE_ON		0x0001			// Return an event when the toggle turns on
#define GLISTEN_TOGGLE_OFF		0x0002			// Return an event when the toggle turns off

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/* Hardware Toggle/Switch/Button Functions */
	GSourceHandle ginputGetToggle(uint16_t instance);					// Instance = 0 to n-1
	void ginputInvertToggle(uint16_t instance, bool_t invert);			// If invert is true, invert the on/off sense for the toggle

	/* Get the current toggle status.
	 *	Returns FALSE on error (eg invalid instance)
	 */
	bool_t ginputGetToggleStatus(uint16_t instance, GEventToggle *ptoggle);

#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_TOGGLE */

#endif /* _GINPUT_TOGGLE_H */
/** @} */
