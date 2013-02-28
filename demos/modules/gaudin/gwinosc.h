/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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

#ifndef _GWINOSC_H
#define _GWINOSC_H

/**
 * --------------------------- Our Custom GWIN Oscilloscope ---------------
 *
 * This GWIN superset implements a simple audio oscilloscope using the GADC high speed device.
 *
 * It makes many assumptions, the most fundamental of which is that the audio device
 * produces unsigned integer samples.
 *
 * The GMISC module with GMISC_NEED_ARRAYOPS could be used to process the samples more
 * correctly if we were really building something generic.
 */

/* The extent of scaling for our audio data - fixed scale at the moment */
#ifndef SCOPE_Y_BITS
	#define SCOPE_Y_BITS			8					// 8 bits = 0..255
#endif

/* Trigger methods */
#define TRIGGER_NONE			0					/* No triggering */
#define TRIGGER_POSITIVERAMP	1					/* Trigger on a positive going signal */
#define TRIGGER_MINVALUE		2					/* Trigger on reaching the minimum value from the last scope */

/**
 * Which trigger we want to use.
 * Experiments suggests that TRIGGER_MINVALUE gives the best result
 */
#ifndef TRIGGER_METHOD
	#define TRIGGER_METHOD			TRIGGER_MINVALUE
#endif

/* A scope window object. Treat it as a black box */
typedef struct GScopeObject_t {
	GWindowObject		gwin;					// Base Class

	coord_t				*lastscopetrace;		// To store last scope trace
	BinarySemaphore		bsem;					// We get signalled on this
	audin_sample_t		*audiobuf;				// To store audio samples
	GEventAudioIn		myEvent;				// Information on received samples
	coord_t				nextx;					// Where we are up to
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	coord_t				lasty;					// The last y value - used for trigger slope detection
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	coord_t				lasty;					// The last y value - used for trigger slope detection
	coord_t				scopemin;				// The last scopes minimum value
#endif
	} GScopeObject;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Create a scope window.
	 */
	GHandle gwinCreateScope(GScopeObject *gs, coord_t x, coord_t y, coord_t cx, coord_t cy, uint16_t channel, uint32_t frequency);

	/**
	 * Wait for a scope trace to be ready and then draw it.
	 */
	void gwinWaitForScopeTrace(GHandle gh);

	/**
	 * We should also have a special destroy routine here as we have dynamically
	 * allocated some memory. There is no point implementing this however as, for
	 * this demo, we never destroy the window.
	 */

#ifdef __cplusplus
}
#endif

#endif /* _GWINOSC_H */
