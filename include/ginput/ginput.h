/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */
#ifndef _GINPUT_H
#define _GINPUT_H

#include "gfx.h"

#if GFX_USE_GINPUT || defined(__DOXYGEN__)

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
#include "ginput/mouse.h"
#include "ginput/keyboard.h"
#include "ginput/toggle.h"
#include "ginput/dial.h"

#endif /* GFX_USE_GINPUT */

#endif /* _GINPUT_H */
/** @} */
