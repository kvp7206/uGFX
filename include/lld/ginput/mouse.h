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
 * @file    lld/ginput/mouse.h
 * @brief   GINPUT LLD header file for mouse/touch drivers.
 *
 * @addtogroup GINPUT_MOUSE
 * @{
 */
#ifndef _LLD_GINPUT_MOUSE_H
#define _LLD_GINPUT_MOUSE_H

#ifndef GINPUT_NEED_MOUSE
	#define GINPUT_NEED_MOUSE FALSE
#endif
#ifndef GINPUT_NEED_TOUCH
	#define GINPUT_NEED_TOUCH FALSE
#endif

#if GINPUT_NEED_MOUSE || GINPUT_NEED_TOUCH

#include "ginput_lld_mouse_config.h"

// GEVENT_MOUSE or GEVENT_TOUCH - What type of device is this.
#ifndef GINPUT_MOUSE_EVENT_TYPE
	#define GINPUT_MOUSE_EVENT_TYPE					GEVENT_MOUSE
#endif

// TRUE/FALSE - Does the mouse/touch driver require calibration?
#ifndef GINPUT_MOUSE_NEED_CALIBRATION
	#define GINPUT_MOUSE_NEED_CALIBRATION			FALSE
#endif

// TRUE/FALSE	- Can the mouse/touch driver itself save calibration data?
#ifndef GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE
	#define GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE	FALSE
#endif

// n or -1		- n means to test calibration result (+/- pixels), -1 means not to.
#ifndef GINPUT_MOUSE_MAX_CALIBRATION_ERROR
	#define GINPUT_MOUSE_MAX_CALIBRATION_ERROR		-1
#endif

// n			- How many times to read (and average) per poll
#ifndef GINPUT_MOUSE_READ_CYCLES
	#define GINPUT_MOUSE_READ_CYCLES				1
#endif

// n			 - Millisecs between poll's
#ifndef GINPUT_MOUSE_POLL_PERIOD
	#define GINPUT_MOUSE_POLL_PERIOD				25
#endif

// n			- Movement allowed without discarding the CLICK or CLICKCXT event (+/- pixels)
#ifndef GINPUT_MOUSE_MAX_CLICK_JITTER
	#define GINPUT_MOUSE_MAX_CLICK_JITTER			1
#endif

// n			- Movement allowed without discarding the MOVE event (+/- pixels)
#ifndef GINPUT_MOUSE_MAX_MOVE_JITTER
	#define GINPUT_MOUSE_MAX_MOVE_JITTER			0
#endif

// ms			- Millisecs seperating a CLICK from a CXTCLICK
#ifndef GINPUT_MOUSE_CLICK_TIME
	#define GINPUT_MOUSE_CLICK_TIME					700
#endif


typedef struct MouseReading_t {
	coord_t		x, y, z;
	uint16_t	buttons;
	} MouseReading;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	void ginput_lld_mouse_init(void);
	void ginput_lld_mouse_get_reading(MouseReading *pt);

	#if GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE
		const char *ginput_lld_mouse_calibration_load(uint16_t instance);
		void ginput_lld_mouse_calibration_save(uint16_t instance, const uint8_t *calbuf, size_t sz);
	#endif

	/* This routine is provided to low level drivers to wakeup a value read from a thread context.
	 *	Particularly useful if GINPUT_MOUSE_POLL_PERIOD = TIME_INFINITE
	 */
	void ginputMouseWakeup(void);

	/* This routine is provided to low level drivers to wakeup a value read from an ISR
	 *	Particularly useful if GINPUT_MOUSE_POLL_PERIOD = TIME_INFINITE
	 */
	void ginputMouseWakeupI(void);

#ifdef __cplusplus
}
#endif

#endif /* GINPUT_NEED_MOUSE || GINPUT_NEED_TOUCH */

#endif /* _LLD_GINPUT_MOUSE_H */
/** @} */
