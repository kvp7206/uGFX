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
 * @file    src/gos/chibios.c
 * @brief   GOS ChibiOS Operating System support.
 */
#include "gfx.h"

#if GFX_USE_OS_CHIBIOS

#if !CH_USE_MUTEXES
	#error "GOS: CH_USE_MUTEXES must be defined in chconf.h"
#endif
#if !CH_USE_SEMAPHORES
	#error "GOS: CH_USE_SEMAPHORES must be defined in chconf.h"
#endif

/* Our module initialiser */
void _gosInit(void) {
	/* Don't initialise if the user already has */
	if (!chThdSelf()) {
		halInit();
		chSysInit();
	}
}

void gfxSleepMilliseconds(delaytime_t ms) {
	switch(ms) {
	case TIME_IMMEDIATE:	chThdYield();				return;
	case TIME_INFINITE:		chThdSleep(TIME_INFINITE);	return;
	default:				chThdSleepMilliseconds(ms);	return;
	}
}

void gfxSleepMicroseconds(delaytime_t ms) {
	switch(ms) {
	case TIME_IMMEDIATE:								return;
	case TIME_INFINITE:		chThdSleep(TIME_INFINITE);	return;
	default:				chThdSleepMicroseconds(ms);	return;
	}
}

void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit) {
	if (val > limit) val = limit;
	psem->limit = limit;
	chSemInit(&psem->sem, val);
}

bool_t gfxSemWait(gfxSem *psem, delaytime_t ms) {
	if (ms == TIME_INFINITE) {
		chSemWait(&psem->sem);
		return TRUE;
	}
	return chSemWaitTimeout(&psem->sem, MS2ST(ms)) != RDY_TIMEOUT;
}

void gfxSemSignal(gfxSem *psem) {
	chSysLock();
	if (gfxSemCounterI(psem) < psem->limit)
		chSemSignalI(&psem->sem);
	chSchRescheduleS();
	chSysUnlock();
}

void gfxSemSignalI(gfxSem *psem) {
	if (gfxSemCounterI(psem) < psem->limit)
		chSemSignalI(&psem->sem);
}

void gfxQueueInit(gfxQueue *pqueue) {
	pqueue->head = pqueue->tail = 0;
	chSemInit(&pqueue->sem, 0);
}

gfxQueueItem * gfxQueueGet(gfxQueue *pqueue, delaytime_t ms) {
	gfxQueueItem	*pi;

	chSysLock();
	/* If someone else is waiting or if the queue is empty - wait ourselves */
	if (pqueue->sem.s_cnt < 0 || !pqueue->head) {
		if (chSemWaitTimeoutS(&pqueue->sem, ms == TIME_INFINITE ? TIME_INFINITE : MS2ST(ms)) == RDY_TIMEOUT) {
			chSysUnlock();
			return NULL;
		}
	}
	/* We can now get the head element */
	pi = pqueue->head;
	pqueue->head = pi;
	chSemSignalI(&pi->sem);
	chSysUnlock();
	return pi;
}

bool_t gfxQueuePut(gfxQueue *pqueue, gfxQueueItem *pitem, delaytime_t ms) {
	chSemInit(&pitem->sem, 0);
	chSysLock();
	pitem->next = 0;
	if (!pqueue->head) {
		pqueue->head = pqueue->tail = pitem;
	} else {
		pqueue->tail->next = pitem;
		pqueue->tail = pitem;
	}
	/* Wake up someone who is waiting */
	if (chSemGetCounterI(&pqueue->sem) < 0)
		chSemSignalI(&pqueue->sem);
	chSysUnlock();
	return chSemWaitTimeout(&pitem->sem, ms == TIME_INFINITE ? TIME_INFINITE : MS2ST(ms)) != RDY_TIMEOUT;
}

bool_t gfxQueuePush(gfxQueue *pqueue, gfxQueueItem *pitem, delaytime_t ms) {
	chSemInit(&pitem->sem, 0);
	chSysLock();
	pitem->next = pqueue->head;
	pqueue->head = pitem;
	if (!pitem->next)
		pqueue->tail = pitem;
	/* Wake up someone who is waiting */
	if (chSemGetCounterI(&pqueue->sem) < 0)
		chSemSignalI(&pqueue->sem);
	chSysUnlock();
	return chSemWaitTimeout(&pitem->sem, ms == TIME_INFINITE ? TIME_INFINITE : MS2ST(ms)) != RDY_TIMEOUT;
}

void gfxQueueRemove(gfxQueue *pqueue, gfxQueueItem *pitem) {
	gfxQueueItem	*pi;

	chSysLock();
	if (pqueue->head) {
		if (pqueue->head == pitem) {
			pqueue->head = pitem->next;
			chSemSignalI(&pitem->sem);
		} else {
			for(pi = pqueue->head; pi->next; pi = pi->next) {
				if (pi->next == pitem) {
					pi->next = pitem->next;
					if (pqueue->tail == pitem)
						pqueue->tail = pi;
					chSemSignalI(&pitem->sem);
					break;
				}
			}
		}
	}
	chSysUnlock();
}

bool_t gfxQueueIsEmpty(gfxQueue *pqueue) {
	return pqueue->head == NULL;
}

bool_t gfxQueueIsIn(gfxQueue *pqueue, gfxQueueItem *pitem) {
	gfxQueueItem	*pi;

	chSysLock();
	for(pi = pqueue->head; pi; pi = pi->next) {
		if (pi == pitem) {
			chSysUnlock();
			return TRUE;
		}
	}
	chSysUnlock();
	return FALSE;
}

/**
 * @brief	Start a new thread.
 * @return	Return TRUE if the thread was started, FALSE on an error
 *
 * @param[in]	stackarea	A pointer to the area for the new threads stack or NULL to dynamically allocate it
 * @param[in]	stacksz		The size of the thread stack. 0 means the default operating system size although this
 * 							is only valid when stackarea is dynamically allocated.
 * @param[in]	prio		The priority of the new thread
 * @param[in]	fn			The function the new thread will run
 * @param[in]	param		A parameter to pass the thread function.
 *
 * @api
 */
bool_t gfxCreateThread(void *stackarea, size_t stacksz, threadpriority_t prio, gfxThreadFunction fn, void *param) {
	if (!stackarea) {
		if (!stacksz) stacksz = 256;
		return chThdCreateFromHeap(0, stacksz, prio, fn, param) != 0;
	}

	return stacksz && chThdCreateStatic(stackarea, stacksz, prio, fn, param) != NULL;
}

#endif /* GFX_USE_OS_CHIBIOS */
/** @} */
