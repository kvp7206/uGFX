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
 * @file    src/gevent.c
 * @brief   GEVENT Driver code.
 *
 * @addtogroup GEVENT
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gevent.h"

#if GFX_USE_GEVENT || defined(__DOXYGEN__)

#if GEVENT_ASSERT_NO_RESOURCE
	#define GEVENT_ASSERT(x)		assert(x)
#else
	#define GEVENT_ASSERT(x)
#endif

/* This mutex protects access to our tables */
static MUTEX_DECL(geventMutex);

/* Our table of listener/source pairs */
static GSourceListener		Assignments[MAX_SOURCE_LISTENERS];

/* Loop through the assignment table deleting this listener/source pair. */
/*	Null is treated as a wildcard. */
static void deleteAssignments(GListener *pl, GSourceHandle gsh) {
	GSourceListener *psl;

	for(psl = Assignments; psl < Assignments+MAX_SOURCE_LISTENERS; psl++) {
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

/**
 * @brief	Create a Listener
 * @details	If insufficient resources are available it will either assert or return NULL
 *			depending on the value of GEVENT_ASSERT_NO_RESOURCE.
 *
 * @param[in] pl	A listener
 */
void geventListenerInit(GListener *pl) {
	chSemInit(&pl->waitqueue, 0);			// Next wait'er will block
	chBSemInit(&pl->eventlock, FALSE);		// Only one thread at a time looking at the event buffer
	pl->callback = 0;						// No callback active
	pl->event.type = GEVENT_NULL;			// Always safety
}

/**
 * @brief 	Attach a source to a listener
 * @details	Flags are interpreted by the source when generating events for each listener.
 *			If this source is already assigned to the listener it will update the flags.
 *			If insufficient resources are available it will either assert or return FALSE
 *			depending on the value of GEVENT_ASSERT_NO_RESOURCE.
 *
 * @param[in] pl	The listener
 * @param[in] gsh	The source which has to be attached to the listener
 * @param[in] flags	The flags
 *
 * @return TRUE if succeeded, FALSE otherwise
 */
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
	for(psl = Assignments; psl < Assignments+MAX_SOURCE_LISTENERS; psl++) {
		
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

/**
 * @brief	Detach a source from a listener
 * @details	If gsh is NULL detach all sources from this listener and if there is still
 *			a thread waiting for events on this listener, it is sent the exit event.
 *
 * @param[in] pl	The listener
 * @param[in] gsh	The source
 */
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

/**
 * @brief	Wait for an event on a listener from an assigned source.
 * @details	The type of the event should be checked (pevent->type) and then pevent should
 *			be typecast to the actual event type if it needs to be processed.
 * 			timeout specifies the time to wait in system ticks.
 *			TIME_INFINITE means no timeout - wait forever for an event.
 *			TIME_IMMEDIATE means return immediately
 * @note	The GEvent buffer is staticly allocated within the GListener so the event does not
 *			need to be dynamicly freed however it will get overwritten by the next call to
 *			this routine.
 *
 * @param[in] pl		The listener
 * @param[in] timeout	The timeout
 *
 * @return	NULL on timeout
 */
GEvent *geventEventWait(GListener *pl, systime_t timeout) {
	if (pl->callback || chSemGetCounterI(&pl->waitqueue) < 0)
		return 0;
	return chSemWaitTimeout(&pl->waitqueue, timeout) == RDY_OK ? &pl->event : 0;
}

/* @brief	Register a callback for an event on a listener from an assigned source.
 * @details	The type of the event should be checked (pevent->type) and then pevent should be typecast to the
 *			actual event type if it needs to be processed.
 *
 * @params[in] pl		The Listener
 * @params[in] fn		The function to call back
 * @params[in] param	A parameter to pass the callback function
 *
 * @note	The GEvent buffer is valid only during the time of the callback. The callback MUST NOT save
 * 			a pointer to the buffer for use outside the callback.
 * @note	An existing callback function is de-registered by passing a NULL for 'fn'. Any existing
 * 			callback function is replaced. Any thread currently waiting using geventEventWait will be sent the exit event.
 * @note	Callbacks occur in a thread context but stack space must be kept to a minumum and
 * 			the callback must process quickly as all other events are performed on a single thread.
 * @note	In the callback function you should never call ANY event functions using your own GListener handle
 * 			as it WILL create a deadlock and lock the system up.
 * @note	Applications should not use this call - geventEventWait() is the preferred mechanism for an
 * 			application. This call is provided for GUI objects that may not have their own thread.
 */
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

/**
 * @brief	Called by a source with a possible event to get a listener record.
 * @details	@p lastlr should be NULL on the first call and thereafter the result of the previous call.
 *
 * @param[in] gsh		The source handler
 * @param[in] lastlr	The source listener
 *
 * @return	NULL when there are no more listeners for this source
 */
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
	for(psl = lastlr ? (lastlr+1) : Assignments; psl < Assignments+MAX_SOURCE_LISTENERS; psl++) {
		if (gsh == psl->pSource) {
			chBSemWait(&psl->pListener->eventlock);		// Obtain a lock on the listener event buffer
			chMtxUnlock();
			return psl;
		}
	}
	chMtxUnlock();
	return 0;
}

/**
 * @brief	Get the event buffer from the GSourceListener.
 * @details	A NULL return allows the source to record (perhaps in glr->scrflags) that the listener
 *			has missed events. This can then be notified as part of the next event for the listener.
 *			The buffer can only be accessed untill the next call to geventGetSourceListener
 *			or geventSendEvent
 *
 * @param[in] psl	The source listener
 *
 * @return	NULL if the listener is not currently listening.
 */
GEvent *geventGetEventBuffer(GSourceListener *psl) {
	// We already know we have the event lock
	return &psl->pListener->callback || chSemGetCounterI(&psl->pListener->waitqueue) < 0 ? &psl->pListener->event : 0;
}

/** 
 * @brief	Called by a source to indicate the listener's event buffer has been filled.
 * @details	After calling this function the source must not reference in fields in the GSourceListener or the event buffer.
 *
 * @param[in] psl	The source listener
 */
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

/**
 * @brief	Detach any listener that has this source attached
 *
 * @param[in] gsh	The source handle
 */
void geventDetachSourceListeners(GSourceHandle gsh) {
	chMtxLock(&geventMutex);
	deleteAssignments(0, gsh);
	chMtxUnlock();
}

#endif /* GFX_USE_GEVENT */
/** @} */
