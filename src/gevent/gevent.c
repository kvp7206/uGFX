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
 * @file    src/gevent/gevent.c
 * @brief   GEVENT Driver code.
 *
 * @addtogroup GEVENT
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GEVENT || defined(__DOXYGEN__)

#if GEVENT_ASSERT_NO_RESOURCE
	#define GEVENT_ASSERT(x)		assert(x)
#else
	#define GEVENT_ASSERT(x)
#endif

/* This mutex protects access to our tables */
static MUTEX_DECL(geventMutex);

/* Our table of listener/source pairs */
static GSourceListener		Assignments[GEVENT_MAX_SOURCE_LISTENERS];

/* Loop through the assignment table deleting this listener/source pair. */
/*	Null is treated as a wildcard. */
static void deleteAssignments(GListener *pl, GSourceHandle gsh) {
	GSourceListener *psl;

	for(psl = Assignments; psl < Assignments+GEVENT_MAX_SOURCE_LISTENERS; psl++) {
		if ((!pl || psl->pListener == pl) && (!gsh || psl->pSource == gsh)) {
			if (chSemGetCounterI(&psl->pListener->waitqueue) < 0) {
				chBSemWait(&psl->pListener->eventlock);			// Obtain the buffer lock
				psl->pListener->event.type = GEVENT_EXIT;		// Set up the EXIT event
				chSemSignal(&psl->pListener->waitqueue);			// Wake up the listener
				chBSemSignal(&psl->pListener->eventlock);		// Release the buffer lock
			}
			psl->pListener = 0;
		}
	}
}

void geventListenerInit(GListener *pl) {
	chSemInit(&pl->waitqueue, 0);			// Next wait'er will block
	chBSemInit(&pl->eventlock, FALSE);		// Only one thread at a time looking at the event buffer
	pl->callback = 0;						// No callback active
	pl->event.type = GEVENT_NULL;			// Always safety
}

bool_t geventAttachSource(GListener *pl, GSourceHandle gsh, unsigned flags) {
	GSourceListener *psl, *pslfree;

	// Safety first
	if (!pl || !gsh) {
		GEVENT_ASSERT(FALSE);
		return FALSE;
	}

	chMtxLock(&geventMutex);

	// Check if this pair is already in the table (scan for a free slot at the same time)
	pslfree = 0;
	for(psl = Assignments; psl < Assignments+GEVENT_MAX_SOURCE_LISTENERS; psl++) {
		
		if (pl == psl->pListener && gsh == psl->pSource) {
			// Just update the flags
			chBSemWait(&pl->eventlock);				// Safety first - just in case a source is using it
			psl->listenflags = flags;
			chBSemSignal(&pl->eventlock);			// Release this lock
			chMtxUnlock();
			return TRUE;
		}
		if (!pslfree && !psl->pListener)
			pslfree = psl;
	}
	
	// A free slot was found - allocate it
	if (pslfree) {
		pslfree->pListener = pl;
		pslfree->pSource = gsh;
		pslfree->listenflags = flags;
		pslfree->srcflags = 0;
	}
	chMtxUnlock();
	GEVENT_ASSERT(pslfree != 0);
	return pslfree != 0;
}

void geventDetachSource(GListener *pl, GSourceHandle gsh) {
	if (pl && gsh) {
		chMtxLock(&geventMutex);
		deleteAssignments(pl, gsh);
		if (!gsh && chSemGetCounterI(&pl->waitqueue) < 0) {
			chBSemWait(&pl->eventlock);				// Obtain the buffer lock
			pl->event.type = GEVENT_EXIT;			// Set up the EXIT event
			chSemSignal(&pl->waitqueue);			// Wake up the listener
			chBSemSignal(&pl->eventlock);			// Release the buffer lock
		}
		chMtxUnlock();
	}
}

GEvent *geventEventWait(GListener *pl, systime_t timeout) {
	if (pl->callback || chSemGetCounterI(&pl->waitqueue) < 0)
		return 0;
	return chSemWaitTimeout(&pl->waitqueue, timeout) == RDY_OK ? &pl->event : 0;
}

void geventRegisterCallback(GListener *pl, GEventCallbackFn fn, void *param) {
	if (pl) {
		chMtxLock(&geventMutex);
		chBSemWait(&pl->eventlock);				// Obtain the buffer lock
		pl->param = param;						// Set the param
		pl->callback = fn;						// Set the callback function
		if (chSemGetCounterI(&pl->waitqueue) < 0) {
			pl->event.type = GEVENT_EXIT;			// Set up the EXIT event
			chSemSignal(&pl->waitqueue);			// Wake up the listener
		}
		chBSemSignal(&pl->eventlock);			// Release the buffer lock
		chMtxUnlock();
	}
}

GSourceListener *geventGetSourceListener(GSourceHandle gsh, GSourceListener *lastlr) {
	GSourceListener *psl;

	// Safety first
	if (!gsh)
		return 0;

	chMtxLock(&geventMutex);

	// Unlock the last listener event buffer
	if (lastlr)
		chBSemSignal(&lastlr->pListener->eventlock);
		
	// Loop through the table looking for attachments to this source
	for(psl = lastlr ? (lastlr+1) : Assignments; psl < Assignments+GEVENT_MAX_SOURCE_LISTENERS; psl++) {
		if (gsh == psl->pSource) {
			chBSemWait(&psl->pListener->eventlock);		// Obtain a lock on the listener event buffer
			chMtxUnlock();
			return psl;
		}
	}
	chMtxUnlock();
	return 0;
}

GEvent *geventGetEventBuffer(GSourceListener *psl) {
	// We already know we have the event lock
	return &psl->pListener->callback || chSemGetCounterI(&psl->pListener->waitqueue) < 0 ? &psl->pListener->event : 0;
}

void geventSendEvent(GSourceListener *psl) {
	chMtxLock(&geventMutex);
	if (psl->pListener->callback) {				// This test needs to be taken inside the mutex
		chMtxUnlock();
		// We already know we have the event lock
		psl->pListener->callback(psl->pListener->param, &psl->pListener->event);

	} else {
		// Wake up the listener
		if (chSemGetCounterI(&psl->pListener->waitqueue) < 0)
			chSemSignal(&psl->pListener->waitqueue);
		chMtxUnlock();
	}
}

void geventDetachSourceListeners(GSourceHandle gsh) {
	chMtxLock(&geventMutex);
	deleteAssignments(0, gsh);
	chMtxUnlock();
}

#endif /* GFX_USE_GEVENT */
/** @} */
