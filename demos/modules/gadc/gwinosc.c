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

/**
 * --------------------------- Our Custom GWIN Oscilloscope ---------------
 *
 * This GWIN superset implements a simple audio oscilloscope using the GADC high speed device.
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#include "gwinosc.h"

/* Include internal GWIN routines so we can build our own superset class */
#include "gwin/internal.h"

/* Our GWIN identifier */
#define GW_SCOPE				(GW_FIRST_USER_WINDOW+0)

/* The size of our dynamically allocated audio buffer */
#define AUDIOBUFSZ				64*2

/* How many flat-line sample before we trigger */
#define FLATLINE_SAMPLES		8

GHandle gwinCreateScope(GScopeObject *gs, coord_t x, coord_t y, coord_t cx, coord_t cy, uint32_t physdev, uint32_t frequency) {
	/* Initialise the base class GWIN */
	if (!(gs = (GScopeObject *)_gwinInit((GWindowObject *)gs, x, y, cx, cy, sizeof(GScopeObject))))
		return 0;

	/* Initialise the scope object members and allocate memory for buffers */
	gs->gwin.type = GW_SCOPE;
	chBSemInit(&gs->bsem, TRUE);
	gs->nextx = 0;
	if (!(gs->lastscopetrace = (coord_t *)chHeapAlloc(NULL, gs->gwin.width * sizeof(coord_t))))
		return 0;
	if (!(gs->audiobuf = (adcsample_t *)chHeapAlloc(NULL, AUDIOBUFSZ * sizeof(adcsample_t))))
		return 0;
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	gs->lasty = gs->gwin.height/2;
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	gs->lasty = gs->gwin.height/2;
	gs->scopemin = 0;
#endif

	/* Start the GADC high speed converter */
	gadcHighSpeedInit(physdev, frequency, gs->audiobuf, AUDIOBUFSZ, AUDIOBUFSZ/2);
	gadcHighSpeedSetBSem(&gs->bsem, &gs->myEvent);
	gadcHighSpeedStart();

	return (GHandle)gs;
}

void gwinWaitForScopeTrace(GHandle gh) {
	#define 		gs	((GScopeObject *)(gh))
	int				i;
	coord_t			x, y;
	coord_t			yoffset;
	adcsample_t		*pa;
	coord_t			*pc;
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	bool_t			rdytrigger;
	int				flsamples;
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	bool_t			rdytrigger;
	int				flsamples;
	coord_t			scopemin;
#endif

	/* Wait for a set of audio conversions */
	chBSemWait(&gs->bsem);

	/* Ensure we are drawing in the right area */
	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif

	yoffset = gh->height/2 + (1<<SCOPE_Y_BITS)/2;
	x = gs->nextx;
	pc = gs->lastscopetrace+x;
	pa = gs->myEvent.buffer;
#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
	rdytrigger = FALSE;
	flsamples = 0;
#elif TRIGGER_METHOD == TRIGGER_MINVALUE
	rdytrigger = FALSE;
	flsamples = 0;
	scopemin = 0;
#endif

	for(i = gs->myEvent.count; i; i--) {

		/* Calculate the new scope value - re-scale using simple shifts for efficiency, re-center and y-invert */
		#if GADC_BITS_PER_SAMPLE > SCOPE_Y_BITS
			y = yoffset - (*pa++ >> (GADC_BITS_PER_SAMPLE - SCOPE_Y_BITS));
		#else
			y = yoffset - (*pa++ << (SCOPE_Y_BITS - GADC_BITS_PER_SAMPLE));
		#endif

#if TRIGGER_METHOD == TRIGGER_MINVALUE
		/* Calculate the scopemin ready for the next trace */
		if (y > scopemin)
			scopemin = y;
#endif

		/* Have we reached the end of a scope trace? */
		if (x >= gh->width) {

#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP || TRIGGER_METHOD == TRIGGER_MINVALUE
			/* Handle triggering - we trigger on the next sample minimum (y value maximum) or a flat-line */

			#if TRIGGER_METHOD == TRIGGER_MINVALUE
				/* Arm when we reach the sample minimum (y value maximum) of the previous trace */
				if (!rdytrigger && y >= gs->scopemin)
					rdytrigger = TRUE;
			#endif

			if (y == gs->lasty) {
				/* Trigger if we get too many flat-line samples regardless of the armed state */
				if (++flsamples < FLATLINE_SAMPLES)
					continue;
				flsamples = 0;
			} else if (y > gs->lasty) {
				gs->lasty = y;
				flsamples = 0;
				#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP
					/* Arm the trigger when samples fall (y increases) ie. negative slope */
					rdytrigger = TRUE;
				#endif
				continue;
			} else {
				/* If the trigger is armed, Trigger when samples increases (y decreases) ie. positive slope */
				gs->lasty = y;
				flsamples = 0;
				if (!rdytrigger)
					continue;
			}

			/* Ready for a the next trigger cycle */
			rdytrigger = FALSE;
#endif

			/* Prepare for a scope trace */
			x = 0;
			pc = gs->lastscopetrace;
		}

		/* Clear the old scope pixel and then draw the new scope value */
		gdispDrawPixel(gh->x+x, gh->y+pc[0], gh->bgcolor);
		gdispDrawPixel(gh->x+x, gh->y+y, gh->color);

		/* Save the value */
		*pc++ = y;
		x++;
		#if TRIGGER_METHOD == TRIGGER_POSITIVERAMP || TRIGGER_METHOD == TRIGGER_MINVALUE
			gs->lasty = y;
		#endif
	}
	gs->nextx = x;
#if TRIGGER_METHOD == TRIGGER_MINVALUE
	gs->scopemin = scopemin;
#endif

	#undef gs
}
