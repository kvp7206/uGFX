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
 * @file    src/ginput/toggle.c
 * @brief   GINPUT toggle code.
 *
 * @addtogroup GINPUT_TOGGLE
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gtimer.h"
#include "ginput.h"

#if GINPUT_NEED_TOGGLE || defined(__DOXYGEN__)

#include "lld/ginput/toggle.h"

#define GINPUT_TOGGLE_ISON		0x01
#define GINPUT_TOGGLE_INVERT	0x02

static GTIMER_DECL(ToggleTimer);
static struct GEventToggleStatus_t {
	uint8_t		status;
} ToggleStatus[GINPUT_TOGGLE_NUM_PORTS];

// Our polling function
static void TogglePoll(void *param) {
	(void) param;

	const GToggleConfig	*ptc;
	GSourceListener	*psl;
	GEventToggle	*pe;
	unsigned		i, bits, mask;
	uint8_t 		state;
	
	// Loop while there are bits to get
	for(ptc = GInputToggleConfigTable, i=0; i < GINPUT_TOGGLE_NUM_PORTS; ptc++) {
	
		// Get the next block of bits
		bits = ginput_lld_toggle_getbits(ptc) ^ ptc->invert;

		// Extract the bits of use
		for(mask = ptc->mask; i < GINPUT_TOGGLE_NUM_PORTS && mask; mask >>= 1, bits >>= 1) {
			// Ignore bits not in our mask
			if (!(mask & 1))
				continue;
		
			// Calculate our new state
			state = ToggleStatus[i].status & ~GINPUT_TOGGLE_ISON;
			if (state & GINPUT_TOGGLE_INVERT)
				bits ^= 1;
			if (bits & 1)
				state |= GINPUT_TOGGLE_ISON;

			// Has it changed?
			if ((state ^ ToggleStatus[i].status) & GINPUT_TOGGLE_ISON) {
			
				// Save the new state
				ToggleStatus[i].status = state;
				
				// Send the event to the listeners that are interested.
				psl = 0;
				while ((psl = geventGetSourceListener((GSourceHandle)(ToggleStatus+i), psl))) {
					if (!(pe = (GEventToggle *)geventGetEventBuffer(psl)))
						continue;
					if ((state & GINPUT_TOGGLE_ISON)) {
						if ((psl->listenflags & GLISTEN_TOGGLE_ON)) {
							pe->type = GEVENT_TOGGLE;
							pe->instance = i;
							pe->on = TRUE;
							geventSendEvent(psl);
						}
					} else {
						if ((psl->listenflags & GLISTEN_TOGGLE_OFF)) {
							pe->type = GEVENT_TOGGLE;
							pe->instance = i;
							pe->on = FALSE;
							geventSendEvent(psl);
						}
					}
				}
			}

			// Next toggle switch
			i++;
		}
	}
}

/* Hardware Toggle/Switch/Button Functions */
GSourceHandle ginputGetToggle(uint16_t instance) {
	const GToggleConfig	*ptc;

	if (instance >= GINPUT_TOGGLE_NUM_PORTS)
		return 0;

	// Do we need to initialise the toggle subsystem?
	if (!gtimerIsActive(&ToggleTimer)) {
		for(ptc = GInputToggleConfigTable; ptc < GInputToggleConfigTable+sizeof(GInputToggleConfigTable)/sizeof(GInputToggleConfigTable[0]); ptc++)
			ginput_lld_toggle_init(ptc);
		gtimerStart(&ToggleTimer, TogglePoll, 0, TRUE, GINPUT_TOGGLE_POLL_PERIOD);
	}
		
	// OK - return this input
	return (GSourceHandle)(ToggleStatus+instance);
}

// If invert is true, invert the on/off sense for the toggle
void ginputInvertToggle(uint16_t instance, bool_t invert) {
	if (instance >= GINPUT_TOGGLE_NUM_PORTS)
		return;
	if (invert) {
		if (!(ToggleStatus[instance].status & GINPUT_TOGGLE_INVERT)) {
			ToggleStatus[instance].status |= GINPUT_TOGGLE_INVERT;
			ToggleStatus[instance].status ^= GINPUT_TOGGLE_ISON;
		}
	} else {
		if ((ToggleStatus[instance].status & GINPUT_TOGGLE_INVERT)) {
			ToggleStatus[instance].status &= ~GINPUT_TOGGLE_INVERT;
			ToggleStatus[instance].status ^= GINPUT_TOGGLE_ISON;
		}
	}
}

/* Get the current toggle status.
 *	Returns FALSE on error (eg invalid instance)
 */
bool_t ginputGetToggleStatus(uint16_t instance, GEventToggle *ptoggle) {
	if (instance >= GINPUT_TOGGLE_NUM_PORTS)
		return FALSE;
	ptoggle->type = GEVENT_TOGGLE;
	ptoggle->instance = instance;
	ptoggle->on = (ToggleStatus[instance].status & GINPUT_TOGGLE_ISON) ? TRUE : FALSE;
	return TRUE;
}

/* Wake up the mouse driver from an interrupt service routine (there may be new readings available) */
void ginputToggleWakeup(void) {
	gtimerJab(&ToggleTimer);
}

/* Wake up the mouse driver from an interrupt service routine (there may be new readings available) */
void ginputToggleWakeupI(void) {
	gtimerJabI(&ToggleTimer);
}

#endif /* GINPUT_NEED_TOGGLE */
/** @} */
