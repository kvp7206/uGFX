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
 * @file    ginput.h
 * @brief   GINPUT GFX User Input subsystem header file.
 *
 * @addtogroup GINPUT
 * @{
 */
#ifndef _GINPUT_H
#define _GINPUT_H

#ifndef GFX_USE_GINPUT
	#define GFX_USE_GINPUT FALSE
#endif

#if GFX_USE_GINPUT || defined(__DOXYGEN__)

/**
 * @name    GINPUT more complex functionality to be compiled
 * @{
 */
	/**
	 * @brief   Should mouse functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_MOUSE
		#define GINPUT_NEED_MOUSE	FALSE
	#endif
	/**
	 * @brief   Should touch functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_TOUCH
		#define GINPUT_NEED_TOUCH	FALSE
	#endif
	/**
	 * @brief   Should keyboard functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_KEYBOARD
		#define GINPUT_NEED_KEYBOARD	FALSE
	#endif
	/**
	 * @brief   Should hardware toggle/switch/button (pio) functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_TOGGLE
		#define GINPUT_NEED_TOGGLE	FALSE
	#endif
	/**
	 * @brief   Should analog dial functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GINPUT_NEED_DIAL
		#define GINPUT_NEED_DIAL	FALSE
	#endif
/** @} */

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

#ifndef GFX_USE_GDISP
	#define GFX_USE_GDISP	FALSE
#endif
#if GINPUT_NEED_TOUCH || !GFX_USE_GDISP
	#error "GINPUT: GFX_USE_GDISP must be defined for touch functions"
#endif

#if GFX_USE_GDISP
	#include "gdisp.h"
#else
	// We require some basic type definitions normally kept in gdisp.h
	typedef int16_t	coord_t;
#endif

#ifndef GFX_USE_GEVENT
	#define	GFX_USE_GEVENT		TRUE
	#include "gevent.h"
#elif !GFX_USE_GEVENT
	#error "GINPUT: GFX_USE_GEVENT must be defined"
#endif

#ifndef GFX_USE_GTIMER
	#define	GFX_USE_GTIMER		TRUE
	#include "gtimer.h"
#elif !GFX_USE_GTIMER
	#error "GINPUT: GFX_USE_GTIMER must be defined"
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for various ginput sources
#define GEVENT_MOUSE		(GEVENT_GINPUT_FIRST+0)
#define GEVENT_TOUCH		(GEVENT_GINPUT_FIRST+1)
#define GEVENT_KEYBOARD		(GEVENT_GINPUT_FIRST+2)
#define GEVENT_TOGGLE		(GEVENT_GINPUT_FIRST+3)
#define GEVENT_DIAL			(GEVENT_GINPUT_FIRST+4)

#if GINPUT_NEED_MOUSE || GINPUT_NEED_TOUCH
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
			#define GINPUT_TOUCH_PRESSED		0x0001
			#define GINPUT_MOUSE_BTN_LEFT		0x0001
			#define GINPUT_MOUSE_BTN_RIGHT		0x0002
			#define GINPUT_MOUSE_BTN_MIDDLE		0x0004
			#define GINPUT_MOUSE_BTN_4			0x0008
			#define GINPUT_MISSED_MOUSE_EVENT	0x8000
		uint16_t		last_buttons;		// The value of current_buttons on the last event
		enum GMouseMeta_e {
			GMETA_NONE,						// There is no meta event currently happenning
			GMETA_DOWN, GMETA_UP,			// Button 0 has just gone up or down
			GMETA_CLICK,					// Button 0 has just gone through a short down - up cycle
			GMETA_CXTCLICK					// For mice - The right button has just been depressed
											// For touch - a long press has just occurred
		}				meta;
	} GEventMouse, GEventTouch;

	// Mouse/Touch Listen Flags - passed to geventAddSourceToListener()
	#define GLISTEN_MOUSEMETA		0x0001			// Create events for meta events such as CLICK and CXTCLICK
	#define GLISTEN_MOUSEDOWNMOVES	0x0002			// Creates mouse move events when the primary mouse button is down (touch is on the surface)
	#define GLISTEN_MOUSEUPMOVES	0x0004			// Creates mouse move events when the primary mouse button is up (touch is off the surface - if the hardware allows).
	#define GLISTEN_TOUCHMETA		0x0001			//		Ditto for touch
	#define GLISTEN_TOUCHDOWNMOVES	0x0002
	#define GLISTEN_TOUCHUPMOVES	0x0004
#endif

#if GINPUT_NEED_KEYBOARD
	typedef struct GEventKeyboard_t {
		GEventType		type;				// The type of this event (GEVENT_KEYBOARD)
		uint16_t		instance;			// The keyboard instance
		char			c;					// The Ascii code for the current key press.
											//		The only possible values are 0(NUL), 8(BS), 9(TAB), 13(CR), 27(ESC), 32(SPACE) to 126(~), 127(DEL)
											//		0 indicates an extended only key.
		uint16_t		code;				// An extended keyboard code. Codes less than 128 match their ascii equivelent.
			#define GKEY_NULL		0
			#define GKEY_BACKSPACE	8
			#define GKEY_TAB		9
			#define GKEY_CR			13
			#define GKEY_ESC		27
			#define GKEY_SPACE		32
			#define GKEY_DEL		127
			#define GKEY_UP			0x0101
			#define GKEY_DOWN		0x0102
			#define GKEY_LEFT		0x0103
			#define GKEY_RIGHT		0x0104
			#define GKEY_HOME		0x0105
			#define GKEY_END		0x0106
			#define GKEY_PAGEUP		0x0107
			#define GKEY_PAGEDOWN	0x0108
			#define GKEY_INSERT		0x0109
			#define GKEY_DELETE		0x010A
			#define GKEY_SHIFT		0x0201
			#define GKEY_CNTRL		0x0202
			#define GKEY_ALT		0x0203
			#define GKEY_WINKEY		0x0204
			#define GKEY_RCLKEY		0x0205
			#define GKEY_FNKEY		0x0206
			#define GKEY_FN1		0x0301
			#define GKEY_FN2		0x0302
			#define GKEY_FN3		0x0303
			#define GKEY_FN4		0x0304
			#define GKEY_FN5		0x0305
			#define GKEY_FN6		0x0306
			#define GKEY_FN7		0x0307
			#define GKEY_FN8		0x0308
			#define GKEY_FN9		0x0309
			#define GKEY_FN10		0x030A
			#define GKEY_FN11		0x030B
			#define GKEY_FN12		0x030C
		uint16_t		current_buttons;		// A bit is set to indicate various meta status.
			#define GMETA_KEYDN			0x0001
			#define GMETA_SHIFT			0x0002
			#define GMETA_CNTRL			0x0004
			#define GMETA_ALT			0x0008
			#define GMETA_WINKEY		0x0010
			#define GMETA_RCLKKEY		0x0020
			#define GMETA_FNKEY			0x0040
			#define GMETA_MISSED_EVENT	0x8000
		uint16_t		last_buttons;			// The value of current_buttons on the last event
	} GEventKeyboard;

	// Keyboard Listen Flags - passed to geventAddSourceToListener()
	#define GLISTEN_KEYREPEATS		0x0001			// Return key repeats (where the key is held down to get a repeat character)
	#define GLISTEN_KEYCODES		0x0002			// Return all key presses including extended code key presses (not just ascii codes)
	#define GLISTEN_KEYALL			0x0004			// Return keyup's, keydown's and everything in between (but not repeats unless GLISTEN_KEYREPEATS is set).
	#define GLISTEN_KEYSINGLE		0x8000			// Return only when one particular extended code key is pressed or released. The particular extended code is OR'd into this value
													//		eg. (GLISTEN_KEYSINGLE | GKEY_CR)
													//		No other flags may be set with this flag.
#endif

#if GINPUT_NEED_TOGGLE
	typedef struct GEventToggle_t {
		GEventType		type;				// The type of this event (GEVENT_TOGGLE)
		uint16_t		instance;			// The toggle instance
		BOOL			on;					// True if the toggle/button is on
	} GEventToggle;
#endif

#if GINPUT_NEED_DIAL
	typedef struct GEventDial_t {
		GEventType		type;				// The type of this event (GEVENT_DIAL)
		uint16_t		instance;			// The dial instance
		uint16_t		value;				// The dial value
	} GEventDial;
#endif


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* How to use...

	1. Get source handles for all the inputs you are interested in.
		- Attempting to get a handle for one instance of an input more than once will return the same handle
	2. Create a listener
	3. Assign inputs to your listener.
		- Inputs can be assigned or released from a listener at any time.
		- An input can be assigned to more than one listener.
	4. Loop on getting listener events
	5. When complete destroy the listener
*/

#if GINPUT_NEED_MOUSE
	/* Mouse Functions */
	GSourceHandle ginputGetMouse(uint16_t instance);					// Instance = 0 to n-1
	
	/* Get the current mouse position and button status.
	 *	Unlike a listener event, this status cannot record meta events such as "CLICK"
	 *	Returns FALSE on error (eg invalid instance)
	 */
	BOOL ginputGetMouseStatus(uint16_t instance, GEventMouse *pmouse);
#endif

#if GINPUT_NEED_TOUCH
	/* Touch Functions */
	GSourceHandle ginputGetTouch(uint16_t instance);		// Instance = 0 to n-1

	/* Get the current touch position and button status.
	 *	Unlike a listener event, this status cannot record meta events such as "CLICK"
	 *	Returns FALSE on error (eg invalid instance)
	 */
	BOOL ginputGetTouchStatus(uint16_t instance, GEventTouch *ptouch);
	
	/* Run a touch calibration.
	 *	Returns FALSE if the driver doesn't support it or if the handle is invalid.
	 */
	BOOL ginputCalibrateTouch(uint16_t instance);

	/* Set the routines to save and fetch calibration data.
	 * This function should be called before first calling ginputGetTouch() for a particular instance
	 *	as the gdispGetTouch() routine may attempt to fetch calibration data and perform a startup calibration if there is no way to get it.
	 *	If this is called after gdispGetTouch() has been called and the driver requires calibration storage, it will immediately save the data is has already obtained.
	 * The 'requireFree' parameter indicates if the fetch buffer must be free()'d to deallocate the buffer provided by the Fetch routine.
	 */
	typedef void (*)(uint16_t instance, const uint8_t *calbuf, size_t sz) GTouchCalibrationSaveRoutine;			// Save calibration data
	typedef const char * (*)(uint16_t instance) GTouchCalibrationFetchRoutine;									// Fetch calibration data (returns NULL if not data saved)
	void ginputSetTouchCalibrationRoutines(uint16_t instance, GTouchCalibrationSaveRoutine fnsave, GTouchCalibrationFetchRoutine fnfetch, BOOL requireFree);

	/* Test if a particular touch instance requires routines to save its calibration data. */
	BOOL ginputRequireTouchCalibrationStorage(uint16_t instance);
#endif

#if GINPUT_NEED_KEYBOARD
	/* Keyboard Functions */
	GSourceHandle ginputGetKeyboard(uint16_t instance);					// Instance = 0 to n-1
	
	/* Get the current keyboard button status.
	 *	Returns FALSE on error (eg invalid instance)
	 */
	BOOL ginputGetKeyboardStatus(uint16_t instance, GEventKeyboard *pkeyboard);
#endif

#if GINPUT_NEED_TOGGLE
	/* Hardware Toggle/Switch/Button Functions */
	GSourceHandle ginputGetToggle(uint16_t instance);					// Instance = 0 to n-1
	void ginputInvertToggle(uint16_t instance, BOOL invert);			// If invert is true, invert the on/off sense for the toggle

	/* Get the current toggle status.
	 *	Returns FALSE on error (eg invalid instance)
	 */
	BOOL ginputGetToggleStatus(uint16_t instance, GEventToggle *ptoggle);
#endif

#if GINPUT_NEED_DIAL
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
	BOOL ginputGetDialStatus(uint16_t instance, GEventDial *pdial);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GINPUT */

#endif /* _GINPUT_H */
/** @} */
