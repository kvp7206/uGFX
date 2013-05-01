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
 * @file    src/ginput/dial.c
 * @brief   GINPUT dial code.
 *
 * @defgroup Dial Dial
 * @ingroup GINPUT
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL

#include "ginput/lld/dial.h"

static GTIMER_DECL(DialTimer);
static struct DialStatus_t {
	uint16_t	sensitivity;
	uint16_t	lastvalue;
	uint16_t	max;
} DialStatus[GINPUT_DIAL_NUM_PORTS];

// The reading callback function
static void DialCallback(uint16_t instance, uint16_t rawvalue) {
	struct DialStatus_t *pds;
	GSourceListener		*psl;
	GEventDial			*pe;

	/* Get the information we need */
	pds = DialStatus+instance;

	/* Range scale - if needed */
	if (pds->max != GINPUT_DIAL_MAX_VALUE)
		rawvalue = (uint16_t)((uint32_t)rawvalue * pds->max / GINPUT_DIAL_MAX_VALUE);

	/* Forget about changes below our sensitivity threshold */
	if (rawvalue >= pds->lastvalue) {
		if (rawvalue - pds->lastvalue < pds->sensitivity) return;
	} else {
		if (pds->lastvalue - rawvalue < pds->sensitivity) return;
	}

	/* Save the value */
	pds->lastvalue = rawvalue;

	// Send the event to the listeners that are interested.
	psl = 0;
	while ((psl = geventGetSourceListener((GSourceHandle)(DialStatus+instance), psl))) {
		if (!(pe = (GEventDial *)geventGetEventBuffer(psl)))
			continue;
		pe->type = GEVENT_DIAL;
		pe->instance = instance;
		pe->value = pds->lastvalue;
		geventSendEvent(psl);
	}
}

GSourceHandle ginputGetDial(uint16_t instance) {
	struct DialStatus_t *pds;

	if (instance >= GINPUT_DIAL_NUM_PORTS)
		return 0;

	// Do we need to initialise the dial subsystem?
	if (!gtimerIsActive(&DialTimer)) {
		for(pds = DialStatus; pds < DialStatus+GINPUT_DIAL_NUM_PORTS; pds++) {
			pds->max = GINPUT_DIAL_MAX_VALUE;
#if GINPUT_DIAL_MAX_VALUE < 100
			pds->sensitivity = 1;
#else
			pds->sensitivity = GINPUT_DIAL_MAX_VALUE/100;
#endif
			pds->lastvalue = 0;
		}
		ginput_lld_dial_init();
		gtimerStart(&DialTimer, (GTimerFunction)ginput_lld_dial_poll, DialCallback, TRUE, GINPUT_DIAL_POLL_PERIOD);
	}

	// OK - return this input
	return (GSourceHandle)(DialStatus+instance);
}

void ginputResetDialRange(uint16_t instance) {
	if (instance >= GINPUT_DIAL_NUM_PORTS)
		return;

	ginputSetDialRange(instance, GINPUT_DIAL_MAX_VALUE);
}

uint16_t ginputGetDialRange(uint16_t instance) {
	if (instance >= GINPUT_DIAL_NUM_PORTS)
		return 0;

	return DialStatus[instance].max;
}

void ginputSetDialRange(uint16_t instance, uint16_t max) {
	struct DialStatus_t *pds;

	if (instance >= GINPUT_DIAL_NUM_PORTS)
		return;

	pds = DialStatus+instance;

	// Rescale the last value and the sensitivity
	if (max != pds->max) {
		pds->lastvalue = (uint16_t)((uint32_t)pds->lastvalue * max / pds->max);
		pds->sensitivity = (uint16_t)((uint32_t)pds->sensitivity * max / pds->max);
		pds->max = max;
	}
}

/**
 * @brief	Set the level change required before a dial even is generated (threshold)
 * @note	This is done after range scaling
 *
 * @param[in] instance	The ID of the dial input instance
 * @param[in] diff		The amount of level changes
 */
void ginputSetDialSensitivity(uint16_t instance, uint16_t diff) {
	if (instance >= GINPUT_DIAL_NUM_PORTS)
		return;

	DialStatus[instance].sensitivity = diff;
}

/**
 * @brief	Get the current dial status
 *
 * @param[in] instance	The ID of the dial input instance
 * @param[in] pdial		The dial event struct
 *
 * @return	Returns FALSE on an error (eg invalid instance)
 */
bool_t ginputGetDialStatus(uint16_t instance, GEventDial *pdial) {
	if (instance >= GINPUT_DIAL_NUM_PORTS)
		return FALSE;
	pdial->type = GEVENT_DIAL;
	pdial->instance = instance;
	pdial->value = DialStatus[instance].lastvalue;
	return TRUE;
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_DIAL */
/** @} */
