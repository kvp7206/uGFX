/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gos/chibios.c
 * @brief   GOS ChibiOS Operating System support.
 */
#include "gfx.h"

#if GFX_USE_OS_CHIBIOS

#include <string.h>

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

void *gfxRealloc(void *ptr, size_t oldsz, size_t newsz) {
	void *np;

	if (newsz <= oldsz)
		return ptr;

	np = gfxAlloc(newsz);
	if (!np)
		return 0;

	if (oldsz)
		memcpy(np, ptr, oldsz);

	return np;
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
void gfxSemDestroy(gfxSem *psem) {
	chSemReset(&psem->sem, 1);
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

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param) {
	if (!stackarea) {
		if (!stacksz) stacksz = 256;
		return chThdCreateFromHeap(0, stacksz, prio, fn, param);
	}

	if (!stacksz) return NULL;
	return chThdCreateStatic(stackarea, stacksz, prio, fn, param);
}

#endif /* GFX_USE_OS_CHIBIOS */
/** @} */
