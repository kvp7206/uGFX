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
