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
 * @file    gtimer.h
 * @brief   GTIMER GFX User Timer subsystem header file.
 *
 * @addtogroup GTIMER
 * @{
 */
#ifndef _GTIMER_H
#define _GTIMER_H

#ifndef GFX_USE_GTIMER
	#define GFX_USE_GTIMER FALSE
#endif

#if GFX_USE_GTIMER || defined(__DOXYGEN__)

/**
 * @name    GTIMER macros and more complex functionality to be compiled
 * @{
 */
	/**
	 * @brief   Data part of a static GTimer initializer.
	 */
	#define _GTIMER_DATA() {0}
	/**
	 * @brief   Static GTimer initializer.
	 */
	#define GTIMER_DECL(name) GTimer name = _GTIMER_DATA()
	/**
	 * @brief   Defines the size of the timer threads work area (stack+structures).
	 * @details	Defaults to 512 bytes
	 */
	#ifndef GTIMER_THREAD_STACK_SIZE
		#define GTIMER_THREAD_STACK_SIZE	512
	#endif
/** @} */

/*===========================================================================*/
/* Low Level Driver details and error checks.                                */
/*===========================================================================*/

#if !CH_USE_MUTEXES || !CH_USE_SEMAPHORES
	#error "GTIMER: CH_USE_MUTEXES and CH_USE_SEMAPHORES must be defined in chconf.h"
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// A callback function (executed in a thread context)
typedef void (*GTimerFunction)(void *param);

/**
 * @brief	 A GTimer structure
 */
typedef struct GTimer_t {
	GTimerFunction		fn;
	void				*param;
	systime_t			when;
	systime_t			period;
	uint16_t			flags;
	struct GTimer_t		*next;
	struct GTimer_t		*prev;
} GTimer;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialise a timer.
 *
 * @param[in] pt pointer to a GTimer structure
 *
 * @api
 */
void gtimerInit(GTimer *pt);

/**
 * @brief   Set a timer going or alter its properties if it is already going.
 *
 * @param[in] pt		Pointer to a GTimer structure
 * @param[in] fn		The callback function
 * @param[in] param		The parameter to pass to the callback function
 * @param[in] periodic	Is the timer a periodic timer? FALSE is a once-only timer.
 * @param[in] millisec	The timer period. The following special values are allowed:
 *							TIME_IMMEDIATE	causes the callback function to be called asap.
 *											A periodic timer with this value will fire once only.
 *							TIME_INFINITE	never timeout (unless triggered by gtimerJab or gtimerJabI)
 *
 * @note				If the timer is already active its properties are updated with the new parameters.
 *						The current period will be immediately canceled (without the callback function being
 *						called) and the timer will be restart with the new timer properties.
 * @note				The callback function should be careful not to over-run the thread stack.
 *						Define a new value for the macro GTIME_THREAD_STACK_SIZE if you want to
 *						change the default size.
 * @note				The callback function should return as quickly as possible as all
 *						timer callbacks are performed by a single thread. If a callback function
 *						takes too long it could affect the timer response for other timers.
 * @note				A timer callback function is not a replacement for a dedicated thread if the
 *						function wants to perform computationally expensive stuff.
 * @note				As the callback function is called on GTIMER's thread, the function must make sure it uses
 *						appropriate synchronisation controls such as semaphores or mutexes around any data
 *						structures it shares with other threads such as the main application thread.
 *
 * @api
 */
void gtimerStart(GTimer *pt, GTimerFunction fn, void *param, bool_t periodic, systime_t millisec);

/**
 * @brief   Stop a timer (periodic or otherwise)
 *
 * @param[in] pt		Pointer to a GTimer structure
 *
 * @note				If the timer is not active this does nothing.
 *
 * @api
 */
void gtimerStop(GTimer *pt);

/**
 * @brief   			Jab a timer causing the current period to immediate expire
 * @details				The callback function will be called as soon as possible.
 *
 * @pre					Use from a normal thread context.
 *
 * @param[in] pt		Pointer to a GTimer structure
 *
 * @note				If the timer is not active this does nothing.
 * @note				Repeated Jabs before the callback function actually happens are ignored.
 *
 * @api
 */
void gtimerJab(GTimer *pt);

/**
 * @brief   			Jab a timer causing the current period to immediate expire
 * @details				The callback function will be called as soon as possible.
 *
 * @pre					Use from an interrupt routine context.
 *
 * @param[in] pt		Pointer to a GTimer structure
 *
 * @note				If the timer is not active this does nothing.
 * @note				Repeated Jabs before the callback function actually happens are ignored.
 *
 * @api
 */
void gtimerJabI(GTimer *pt);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GTIMER */

#endif /* _GTIMER_H */
/** @} */
