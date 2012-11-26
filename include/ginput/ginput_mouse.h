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
 * @file    ginput/ginput_mouse.h
 * @brief   GINPUT GFX User Input subsystem header file for mouse and touch.
 *
 * @addtogroup GINPUT
 * @{
 */
#ifndef _GINPUT_MOUSE_H
#define _GINPUT_MOUSE_H

/**
 * @name    GINPUT more complex functionality to be compiled
 * @{
 */
	/**
	 * @brief   Should mouse/touch functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_MOUSE
		#define GINPUT_NEED_MOUSE	FALSE
	#endif
/** @} */

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

#if GINPUT_NEED_MOUSE || defined(__DOXYGEN__)

/* This type definition is also used by touch */
typedef struct GEventMouse_t {
	GEventType		type;				// The type of this event (GEVENT_MOUSE or GEVENT_TOUCH)
	uint16_t		instance;			// The mouse/touch instance
	coord_t			x, y, z;			// The position of the mouse.
										//		- For touch devices, Z is the current pressure if supported (otherwise 0)
										//		- For mice, Z is the 3rd dimension if supported (otherwise 0)
	uint16_t		current_buttons;	// A bit is set if the button is down.
										//		- For touch only bit 0 is relevant
										//		- For mice the order of the buttons is (from 0 to n)  left, right, middle, any other buttons
										//		- Bit 15 being set indicates that an important mouse event has been missed.
		#define GINPUT_MOUSE_BTN_LEFT		0x0001
		#define GINPUT_MOUSE_BTN_RIGHT		0x0002
		#define GINPUT_MOUSE_BTN_MIDDLE		0x0004
		#define GINPUT_MOUSE_BTN_4			0x0008
		#define GINPUT_MISSED_MOUSE_EVENT	0x8000
		#define GINPUT_TOUCH_PRESSED		GINPUT_MOUSE_BTN_LEFT
	uint16_t		last_buttons;		// The value of current_buttons on the last event
	enum GMouseMeta_e {
		GMETA_NONE = 0,						// There is no meta event currently happening
		GMETA_MOUSE_DOWN = 1,				// Button 0 has just gone down
		GMETA_MOUSE_UP = 2,					// Button 0 has just gone up
		GMETA_MOUSE_CLICK = 4,				// Button 0 has just gone through a short down - up cycle
		GMETA_MOUSE_CXTCLICK = 8			// For mice - The right button has just been depressed
											// For touch - a long press has just occurred
		}				meta;
	} GEventMouse;

// Mouse/Touch Listen Flags - passed to geventAddSourceToListener()
#define GLISTEN_MOUSEMETA			0x0001			// Create events for meta events such as CLICK and CXTCLICK
#define GLISTEN_MOUSEDOWNMOVES		0x0002			// Creates mouse move events when the primary mouse button is down (touch is on the surface)
#define GLISTEN_MOUSEUPMOVES		0x0004			// Creates mouse move events when the primary mouse button is up (touch is off the surface - if the hardware allows).
#define	GLISTEN_MOUSENOFILTER		0x0008			// Don't filter out mouse moves where the position hasn't changed.
#define GLISTEN_TOUCHMETA			GLISTEN_MOUSEMETA
#define GLISTEN_TOUCHDOWNMOVES		GLISTEN_MOUSEDOWNMOVES
#define GLISTEN_TOUCHUPMOVES		GLISTEN_MOUSEUPMOVES
#define	GLISTEN_TOUCHNOFILTER		GLISTEN_MOUSENOFILTER

#define GINPUT_MOUSE_NUM_PORTS		1			// The total number of mouse/touch inputs supported

// Event types for the mouse ginput source
#define GEVENT_MOUSE		(GEVENT_GINPUT_FIRST+0)
#define GEVENT_TOUCH		(GEVENT_GINPUT_FIRST+1)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/* Mouse Functions */
	GSourceHandle ginputGetMouse(uint16_t instance);					// Instance = 0 to n-1
	
	/* Get the current mouse position and button status.
	 *	Unlike a listener event, this status cannot record meta events such as "CLICK"
	 *	Returns FALSE on error (eg invalid instance)
	 */
	bool_t ginputGetMouseStatus(uint16_t instance, GEventMouse *pmouse);

	/* Run a calibration.
	 *	Returns FALSE if the driver doesn't support it or if the handle is invalid.
	 */
	bool_t ginputCalibrateMouse(uint16_t instance);

	/* Set the routines to save and fetch calibration data.
	 * This function should be called before first calling ginputGetMouse() for a particular instance
	 *	as the gdispGetMouse() routine may attempt to fetch calibration data and perform a startup calibration if there is no way to get it.
	 *	If this is called after gdispGetMouse() has been called and the driver requires calibration storage, it will immediately save the data is has already obtained.
	 * The 'requireFree' parameter indicates if the fetch buffer must be free()'d to deallocate the buffer provided by the Fetch routine.
	 */
	typedef void (*GMouseCalibrationSaveRoutine)(uint16_t instance, const uint8_t *calbuf, size_t sz);			// Save calibration data
	typedef const char * (*GMouseCalibrationLoadRoutine)(uint16_t instance);									// Load calibration data (returns NULL if not data saved)
	void ginputSetMouseCalibrationRoutines(uint16_t instance, GMouseCalibrationSaveRoutine fnsave, GMouseCalibrationLoadRoutine fnload, bool_t requireFree);

	/* Test if a particular mouse/touch instance requires routines to save its calibration data. */
	bool_t ginputRequireMouseCalibrationStorage(uint16_t instance);
	
#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_MOUSE */

#endif /* _GINPUT_MOUSE_H */
/** @} */
