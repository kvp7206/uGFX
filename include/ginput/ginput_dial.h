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
 * @file    ginput/ginput_dial.h
 * @brief   GINPUT GFX User Input subsystem header file.
 *
 * @addtogroup GINPUT
 * @{
 */
#ifndef _GINPUT_DIAL_H
#define _GINPUT_DIAL_H

/**
 * @name    GINPUT more complex functionality to be compiled
 * @{
 */
	/**
	 * @brief   Should analog dial functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_DIAL
		#define GINPUT_NEED_DIAL	FALSE
	#endif
/** @} */

#if GINPUT_NEED_DIAL || defined(__DOXYGEN__)

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for various ginput sources
#define GEVENT_DIAL			(GEVENT_GINPUT_FIRST+4)

typedef struct GEventDial_t {
	GEventType		type;				// The type of this event (GEVENT_DIAL)
	uint16_t		instance;			// The dial instance
	uint16_t		value;				// The dial value
	} GEventDial;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/* Dial Functions */
	GSourceHandle ginputGetDial(uint16_t instance);						// Instance = 0 to n-1
	void ginputResetDialRange(uint16_t instance);						// Reset the maximum value back to the hardware default.
	uint16_t ginputGetDialRange(uint16_t instance);						// Get the maximum value. The readings are scaled to be 0...max-1. 0 means over the full uint16_t range.
	void ginputSetDialRange(uint16_t instance, uint16_t max);			// Set the maximum value.
	void ginputSetDialSensitivity(uint16_t instance, uint16_t diff);	// Set the level change required before a dial event is generated.
																		//		- This is done after range scaling
	/* Get the current keyboard button status.
	 *	Returns FALSE on error (eg invalid instance)
	 */
	bool_t ginputGetDialStatus(uint16_t instance, GEventDial *pdial);

#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_DIAL */

#endif /* _GINPUT_DIAL_H */
/** @} */
