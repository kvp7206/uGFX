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

void gtimerInit(GTimer *pt);
void gtimerStart(GTimer *pt, GTimerFunction fn, void *param, bool_t periodic, systime_t millisec);
void gtimerStop(GTimer *pt);
void gtimerJab(GTimer *pt);
void gtimerJabI(GTimer *pt);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GTIMER */

#endif /* _GTIMER_H */
/** @} */

