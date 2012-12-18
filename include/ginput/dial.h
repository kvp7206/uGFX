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
 * @file    include/ginput/dial.h
 * @brief   GINPUT GFX User Input subsystem header file.
 *
 * @defgroup Dial Dial
 * @ingroup GINPUT
 *
 * @details	A dial provides a powerful way to navigate through menus
 *			on a display. 
 *
 * @pre		GFX_USE_GINPUT must be set to TRUE in your gfxconf.h
 * @pre		GINPUT_NEED_DIAL must be set to TRUE in your gfxconf.h
 *
 * @{
 */
#ifndef _GINPUT_DIAL_H
#define _GINPUT_DIAL_H

#if GINPUT_NEED_DIAL || defined(__DOXYGEN__)

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

	/**
	 * @brief	Create a dial input instance
	 *
	 * @param[in] instance	The ID of the dial input instance (from 0 to 9999)
	 *
	 * @return	The soure handle of the created dial instance
	 */
	GSourceHandle ginputGetDial(uint16_t instance);				
	
	/**
	 * @brief	Reset the value back to the hardware default
	 *
	 * @param[in] instance	The ID of the dial input instance
	 */
	void ginputResetDialRange(uint16_t instance);				

	/**
	 * @brief	Get the maximum value
	 * @details	The readings are scaled to be 0 ... max-1.
	 *			0 means over the full uint16_t range.
	 *
	 * @param[in] instance	The ID of the dial input instance
	 *
	 * @return	The maximum value
	 */
	uint16_t ginputGetDialRange(uint16_t instance);

	/**
	 * @brief	Set the maximum value
	 * @note	This shouldn't be set higher that the hardwares possible maximum value
	 *
	 * @param[in] instance	The ID of the dial input instance
	 * @param[in] max		The maximum value to be set
	 */
	void ginputSetDialRange(uint16_t instance, uint16_t max);

	/**
	 * @brief	Set the level change required before a dial even is generated (threshold)
	 * @note	This is done after range scaling
	 *
	 * @param[in] instance	The ID of the dial input instance
	 * @param[in] diff		The amount of level changes
	 */
	void ginputSetDialSensitivity(uint16_t instance, uint16_t diff);
																	
	/**
	 * @brief	Get the current dial status
	 *
	 * @param[in] instance	The ID of the dial input instance
	 * @param[in] pdial		The dial event struct
	 *
	 * @return	Returns FALSE on an error (eg invalid instance)
	 */
	bool_t ginputGetDialStatus(uint16_t instance, GEventDial *pdial);

#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_DIAL */

#endif /* _GINPUT_DIAL_H */
/** @} */
