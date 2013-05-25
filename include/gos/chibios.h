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
 * @file    include/gos/chibios.h
 * @brief   GOS - Operating System Support header file for ChibiOS.
 */

#ifndef _GOS_CHIBIOS_H
#define _GOS_CHIBIOS_H

#if GFX_USE_OS_CHIBIOS

#include "ch.h"
#include "hal.h"

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * bool_t,
 * int8_t, uint8_t,
 * int16_t, uint16_t,
 * int32_t, uint32_t,
 * size_t
 * TRUE, FALSE
 * TIME_IMMEDIATE, TIME_INFINITE
 * are already defined by ChibiOS
 */

typedef systime_t	delaytime_t;
typedef systime_t	systemticks_t;
typedef cnt_t		semcount_t;
typedef msg_t		threadreturn_t;
typedef tprio_t		threadpriority_t;

typedef threadreturn_t (*gfxThreadFunction)(void *param);

#define MAX_SEMAPHORE_COUNT			((semcount_t)(((unsigned long)((semcount_t)(-1))) >> 1))
#define LOW_PRIORITY				LOWPRIO
#define NORMAL_PRIORITY				NORMALPRIO
#define HIGH_PRIORITY				HIGHPRIO
#define DECLARESTACK(name, sz)		WORKING_AREA(name, sz);


typedef struct {
	Semaphore	sem;
	semcount_t	limit;
	} gfxSem;

#define gfxMutex	Mutex

/*===========================================================================*/
/* Function declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define gfxHalt(msg)				chDbgPanic(msg)
#define gfxExit()					chSysHalt()
#define gfxAlloc(sz)				chHeapAlloc(NULL, sz)
#define gfxFree(ptr)				chHeapFree(ptr)
#define gfxYield()					chThdYield()
#define gfxSystemTicks()			chTimeNow()
#define gfxMillisecondsToTicks(ms)	MS2ST(ms)
#define gfxSystemLock()				chSysLock()
#define gfxSystemUnlock()			chSysUnlock()
#define gfxMutexInit(pmutex)		chMtxInit(pmutex)
#define gfxMutexDestroy(pmutex)		;
#define gfxMutexEnter(pmutex)		chMtxLock(pmutex)
#define gfxMutexExit(pmutex)		chMtxUnlock()
void gfxSleepMilliseconds(delaytime_t ms);
void gfxSleepMicroseconds(delaytime_t ms);
void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);
void gfxSemDestroy(gfxSem *psem);
bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);
void gfxSemSignal(gfxSem *psem);
void gfxSemSignalI(gfxSem *psem);
#define gfxSemCounterI(psem)		((psem)->sem.s_cnt)
#define gfxSemCounter(psem)			((psem)->sem.s_cnt)
bool_t gfxCreateThread(void *stackarea, size_t stacksz, threadpriority_t prio, gfxThreadFunction fn, void *param);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_OS_CHIBIOS */
#endif /* _GOS_CHIBIOS_H */
