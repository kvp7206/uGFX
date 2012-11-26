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
/** @} */

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

#ifndef GFX_USE_GDISP
	#define GFX_USE_GDISP	FALSE
#endif
#if GFX_USE_GDISP
	#include "gdisp.h"
#else
	// We require some basic type definitions normally kept in gdisp.h
	typedef int16_t	coord_t;
#endif

#ifndef GFX_USE_GEVENT
	#define	GFX_USE_GEVENT		TRUE
#elif !GFX_USE_GEVENT
	#error "GINPUT: GFX_USE_GEVENT must be defined"
#endif
#include "gevent.h"

#ifndef GFX_USE_GTIMER
	#define	GFX_USE_GTIMER		TRUE
#elif !GFX_USE_GTIMER
	#error "GINPUT: GFX_USE_GTIMER must be defined"
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

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

// Include various ginput types
#include "ginput/ginput_mouse.h"
#include "ginput/ginput_keyboard.h"
#include "ginput/ginput_toggle.h"
#include "ginput/ginput_dial.h"

#endif /* GFX_USE_GINPUT */

#endif /* _GINPUT_H */
/** @} */
