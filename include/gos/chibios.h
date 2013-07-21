/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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

#define MAX_SEMAPHORE_COUNT			((semcount_t)(((unsigned long)((semcount_t)(-1))) >> 1))
#define LOW_PRIORITY				LOWPRIO
#define NORMAL_PRIORITY				NORMALPRIO
#define HIGH_PRIORITY				HIGHPRIO

#define DECLARE_THREAD_STACK(name, sz)			WORKING_AREA(name, sz)
#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t fnName(void *param)

typedef struct {
	Semaphore	sem;
	semcount_t	limit;
	} gfxSem;

typedef Mutex		gfxMutex;
typedef Thread *	gfxThreadHandle;

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
#define gfxMutexDestroy(pmutex)		{}
#define gfxMutexEnter(pmutex)		chMtxLock(pmutex)
#define gfxMutexExit(pmutex)		chMtxUnlock()
void *gfxRealloc(void *ptr, size_t oldsz, size_t newsz);
void gfxSleepMilliseconds(delaytime_t ms);
void gfxSleepMicroseconds(delaytime_t ms);
void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);
void gfxSemDestroy(gfxSem *psem);
bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);
void gfxSemSignal(gfxSem *psem);
void gfxSemSignalI(gfxSem *psem);
#define gfxSemCounterI(psem)		((psem)->sem.s_cnt)
#define gfxSemCounter(psem)			((psem)->sem.s_cnt)
gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param);
#define gfxThreadWait(thread)		chThdWait(thread)
#define gfxThreadMe()				chThdSelf()
#define gfxThreadClose(thread)		{}

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_OS_CHIBIOS */
#endif /* _GOS_CHIBIOS_H */
