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
 * @file    include/gos/gos.h
 * @brief   GOS - Operating System Support header file.
 *
 * @addtogroup GOS
 * @brief	GOS provides the operating system interface with the GFX library.
 * @note	Some of the routines specified below may be implemented simply as
 * 			a macro to the real operating system call.
 * @{
 */

#ifndef _GOS_H
#define _GOS_H

#if defined(__DOXYGEN__)
	/*===========================================================================*/
	/* Type definitions                                                          */
	/*===========================================================================*/

	/**
	 * @brief	A function for a new thread to execute.
	 */
	typedef threadreturn_t (*gfxThreadFunction)(void *param);

	/**
	 * @brief	Various integer sizes
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	typedef unsigned char	bool_t;
	typedef char			int8_t;
	typedef unsigned char	uint8_t;
	typedef short			int16_t;
	typedef unsigned short	uint16_t;
	typedef long			int32_t;
	typedef unsigned long	uint32_t;
	/**
	 * @}
	 *
	 * @brief	Various platform (and operating system) dependent types
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	typedef unsigned long	size_t;
	typedef unsigned long	delaytime_t;
	typedef unsigned long	systemticks_t;
	typedef short			semcount_t;
	typedef int				threadreturn_t;
	typedef int				threadpriority_t;
	/**
	 * @}
	 *
	 * @brief	Various platform (and operating system) constants
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	#define FALSE						0
	#define TRUE						1
	#define TIME_IMMEDIATE				0
	#define TIME_INFINITE				((delaytime_t)-1)
	#define MAX_SEMAPHORE_COUNT			((semcount_t)-1)
	#define LOW_PRIORITY				0
	#define NORMAL_PRIORITY				1
	#define HIGH_PRIORITY				2
	#define DECLARESTACK(name, sz)		uint8_t name[sz];
	/* @} */

	/**
	 * @brief	A semaphore
	 * @note	Your operating system will have a proper definition for this structure
	 */
	typedef struct {} gfxSem;

	/**
	 * @brief	A mutex
	 * @note	Your operating system will have a proper definition for this structure
	 */
	typedef struct {} gfxMutex;

	/**
	 * @brief	A queue
	 * @note	Your operating system will have a proper definition for this structure
	 */
	typedef struct {} gfxQueue;

	/**
	 * @brief	A queue item
	 * @note	Your operating system will have a proper definition for this structure
	 */
	typedef struct {} gfxQueueItem;

	/*===========================================================================*/
	/* Function declarations.                                                    */
	/*===========================================================================*/

	#ifdef __cplusplus
	extern "C" {
	#endif

	/**
	 * @brief	Halt the GFX application due to an error.
	 *
	 * @param[in] msg	An optional debug message to show (Can be NULL)
	 *
	 * @api
	 */
	void gfxHalt(const char *msg);

	/**
	 * @brief	Exit the GFX application.
	 *
	 * @api
	 */
	void gfxExit(void);

	/**
	 * @brief	Allocate memory
	 * @return	A pointer to the memory allocated or NULL if there is no more memory available
	 *
	 * @param[in] sz	The size in bytes of the area to allocate
	 *
	 * @api
	 */
	void *gfxAlloc(size_t sz);

	/**
	 * @brief	Free memory
	 *
	 * @param[in] ptr	The memory to free
	 *
	 * @api
	 */
	void gfxFree(void *ptr);

	/**
	 * @brief	Put the current thread to sleep for the specified period in milliseconds
	 *
	 * @param[in] ms	The number milliseconds to sleep
	 *
	 * @note		Specifying TIME_IMMEDIATE will yield the current thread but return
	 * 				on the next time slice.
	 * @note		Specifying TIME_INFINITE will sleep forever.
	 *
	 * @api
	 */
	void gfxSleepMilliseconds(delaytime_t ms);

	/**
	 * @brief	Put the current thread to sleep for the specified period in microseconds
	 *
	 * @param[in] ms	The number microseconds to sleep
	 *
	 * @note		Specifying TIME_IMMEDIATE will return immediately (no sleeping)
	 * @note		Specifying TIME_INFINITE will sleep forever.
	 *
	 * @api
	 */
	void gfxSleepMicroseconds(delaytime_t ms);

	/**
	 * @brief	Initialise a Counted Semaphore
	 *
	 * @param[in] psem		A pointer to the semaphore
	 * @param[in] val		The initial value of the semaphore
	 * @param[in] limit		The maxmimum value of the semaphore
	 *
	 * @note	Operations defined for counted semaphores:
	 * 				Signal: The semaphore counter is increased and if the result is non-positive then a waiting thread
	 * 						 is queued for execution. Note that once the thread reaches "limit", further signals are
	 * 						 ignored.
	 * 				Wait: The semaphore counter is decreased and if the result becomes negative the thread is queued
	 * 						in the semaphore and suspended.
	 *
	 * @api
	 */
	void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);

	/**
	 * @brief	Wait on a semaphore
	 * @details	The semaphore counter is decreased and if the result becomes negative the thread waits for it to become
	 * 				non-negative again
	 * @return	FALSE if the wait timeout occurred otherwise TRUE
	 *
	 * @param[in] psem		A pointer to the semaphore
	 * @param[in] ms		The maxmimum time to wait for the semaphore
	 *
	 * @api
	 */
	bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);

	/**
	 * @brief	Signal a semaphore
	 * @details	The semaphore counter is increased and if the result is non-positive then a waiting thread
	 * 						 is queued for execution. Note that once the thread reaches "limit", further signals are
	 * 						 ignored.
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @api
	 */
	void gfxSemSignal(gfxSem *psem);

	/**
	 * @brief	Signal a semaphore
	 * @details	The semaphore counter is increased and if the result is non-positive then a waiting thread
	 * 						 is queued for execution. Note that once the thread reaches "limit", further signals are
	 * 						 ignored.
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @iclass
	 * @api
	 */
	void gfxSemSignalI(gfxSem *psem);

	/**
	 * @brief	Get the current semaphore count
	 * @return	The current semaphore count
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @api
	 */
	semcount_t gfxSemCounter(gfxSem *pSem);

	/**
	 * @brief	Get the current semaphore count
	 * @return	The current semaphore count
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @iclass
	 * @api
	 */
	semcount_t gfxSemCounterI(gfxSem *pSem);

	/**
	 * @brief	Get the current operating system tick time
	 * @return	The current tick time
	 *
	 * @note	A "tick" is an arbitrary period of time that the operating
	 * 			system uses to mark time.
	 * @note	The absolute value of this call is relatively meaningless. Its usefulness
	 * 			is in calculating periods between two calls to this function.
	 * @note	As the value from this function can wrap it is important that any periods are calculated
	 * 			as t2 - t1 and then compared to the desired period rather than comparing
	 * 			t1 + period to t2
	 *
	 * @api
	 */
	systemticks_t gfxSystemTicks(void);

	/**
	 * @brief	Convert a given number of millseconds to a number of operating system ticks
	 * @return	The period in system ticks.
	 *
	 * @note	A "tick" is an arbitrary period of time that the operating
	 * 			system uses to mark time.
	 *
	 * @param[in] ms	The number of millseconds
	 *
	 * @api
	 */
	systemticks_t gfxMillisecondsToTicks(delaytime_t ms);

	/**
	 * @brief	Yield the current thread
	 * @details	Give up the rest of the current time slice for this thread in order to give other threads
	 * 			a chance to run.
	 *
	 * @api
	 */
	void gfxYield(void);

	/**
	 * @brief	Lock the operating system to protect a sequence of code
	 *
	 * @note	Calling this will lock out all other threads from executing even at interrupt level
	 * 			within the GFX system. On hardware this may be implemented as a disabling of interrupts,
	 * 			however in an operating system which hides real interrupt level code it may simply use a
	 * 			mutex lock.
	 * @note	The thread MUST NOT block whilst the system is locked. It must execute in this state for
	 * 			as short a period as possible as this can seriously affect interrupt latency on some
	 * 			platforms.
	 * @note	While locked only interrupt level (iclass) GFX routines may be called.
	 *
	 * @api
	 */
	void gfxSystemLock(void);

	/**
	 * @brief	Unlock the operating system previous locked by gfxSystemLock()
	 *
	 * @api
	 */
	void gfxSystemUnlock(void);

	/**
	 * @brief	Initialise a mutex to protect a region of code from other threads.
	 *
	 * @param[in]	pmutex	A pointer to the mutex
	 *
	 * @note	Whilst a counting semaphore with a limit of 1 can be used for similiar purposes
	 * 			on many operating systems using a seperate mutex structure is more efficient.
	 *
	 * @api
	 */
	void gfxMutexInit(gfxMutex *pmutex);

	/**
	 * @brief	Enter the critical code region protected by the mutex.
	 * @details	Blocks until there is no other thread in the critical region.
	 *
	 * @param[in]	pmutex	A pointer to the mutex
	 *
	 * @api
	 */
	void gfxMutexEnter(gfxMutex *pmutex);

	/**
	 * @brief	Exit the critical code region protected by the mutex.
	 * @details	May cause another thread waiting on the mutex to now be placed into the run queue.
	 *
	 * @param[in]	pmutex	A pointer to the mutex
	 *
	 * @api
	 */
	void gfxMutexExit(gfxMutex *pmutex);

	/**
	 * @brief	Initialise a queue.
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 *
	 * @note	Whilst queues are normally FIFO, a GFX queue also supports push and pop operations.
	 * 			A pop operation is the same as normal get from the queue but a push places the item
	 * 			at the head of the queue instead of the tail (as a put would).
	 *
	 * @api
	 */
	void gfxQueueInit(gfxQueue *pqueue);

	/**
	 * @brief	Get an item from the head of the queue.
	 * @return	NULL if the timeout expires before an item is available
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 * @param[in]	ms		The maxmimum time to wait for an item
	 *
	 * @api
	 */
	gfxQueueItem * gfxQueueGet(gfxQueue *pqueue, delaytime_t ms);

	/**
	 * @brief	Put an item on the end of the queue.
	 * @return	FALSE on timeout, otherwise TRUE
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 * @param[in]	pitem	A pointer to the queue item
	 * @param[in]	ms		The maxmimum time to wait for an item to be removed from the queue
	 *
	 * @note		Use a delay time of TIME_IMMEDIATE if you don't want to wait until the
	 * 				item is removed from the queue
	 *
	 * @api
	 */
	bool_t gfxQueuePut(gfxQueue *pqueue, gfxQueueItem *pitem, delaytime_t ms);

	/**
	 * @brief	Pop an item from the head of the queue.
	 * @return	NULL if there are no more items on the queue
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 *
	 * @api
	 */
	#define gfxQueuePop(pqueue, ms)		gfxQueueGet(pqueue, ms)

	/**
	 * @brief	Push an item into the start of the queue.
	 * @return	FALSE on timeout, otherwise TRUE
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 * @param[in]	pitem	A pointer to the queue item
	 * @param[in]	ms		The maxmimum time to wait for an item to be popped
	 *
	 * @note		Use a delay time of TIME_IMMEDIATE if you don't want to wait until the
	 * 				item is removed from the queue
	 *
	 * @api
	 */
	bool_t gfxQueuePush(gfxQueue *pqueue, gfxQueueItem *pitem, delaytime_t ms);

	/**
	 * @brief	Remove an item from the queue.
	 * @note	Removes the specified item from the queue whereever it is in the queue
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 * @param[in]	pitem	A pointer to the queue item
	 *
	 * @note	If the item isn't in the queue the routine just returns.
	 *
	 * @api
	 */
	void gfxQueueRemove(gfxQueue *pqueue, gfxQueueItem *pitem);

	/**
	 * @brief	Is the queue empty?
	 * @return	TRUE if the queue is empty
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 *
	 * @api
	 */
	bool_t gfxQueueIsEmpty(gfxQueue *pqueue);

	/**
	 * @brief	Is an item in the queue?
	 * @return	TRUE if the item is in the queue?
	 *
	 * @param[in]	pqueue	A pointer to the queue
	 * @param[in]	pitem	A pointer to the queue item
	 *
	 * @note	This operation may be expensive.
	 *
	 * @api
	 */
	bool_t gfxQueueIsIn(gfxQueue *pqueue, gfxQueueItem *pitem);

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
	bool_t gfxCreateThread(void *stackarea, size_t stacksz, threadpriority_t prio, gfxThreadFunction fn, void *param);

	#ifdef __cplusplus
	}
	#endif

/**
 * All the above was just for the doxygen documentation. All the implementation of the above
 * (without any of the documentation overheads) is in the files below.
 */
#elif GFX_USE_OS_CHIBIOS
	#include "gos/chibios.h"
#elif GFX_USE_OS_WIN32
	#include "gos/win32.h"
#elif GFX_USE_OS_POSIX
	#include "gos/posix.h"
#else
	#error "Your operating system is not supported yet"
#endif

#endif /* _GOS_H */
/** @} */
