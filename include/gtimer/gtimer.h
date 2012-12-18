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
 * @file    include/gtimer/gtimer.h
 * @brief   GTIMER GFX User Timer subsystem header file.
 *
 * @addtogroup GTIMER
 *
 * @details	The reason why ChibiOS/GFX has it's own timer abstraction is because
 *			virtual timers provided by ChibiOS/RT are interrupt context only.
 *			While great for what they are designed for, they make coding of the input
 *			drivers much more complex.
 *			For non-performance critical drivers like these input drivers,  it would also
 *			hog an in-ordinate amount of critical (interrupt locked) system time.
 *			This contrary to the goals of a real-time operating system. So a user-land
 *			(thread based) timer mechanism is also required.
 *
 * @pre		GFX_USE_GTIMER must be set to TRUE in your gfxconf.h
 *
 * @{
 */
#ifndef _GTIMER_H
#define _GTIMER_H

#include "gfx.h"

#if GFX_USE_GTIMER || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief   Data part of a static GTimer initializer.
 */
#define _GTIMER_DATA() {0,0,0,0,0,0,0}

/**
 * @brief   Static GTimer initializer.
 */
#define GTIMER_DECL(name) GTimer name = _GTIMER_DATA()

/**
 * @brief   A callback function (executed in a thread context).
 */
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
bool_t gtimerIsActive(GTimer *pt);
void gtimerJab(GTimer *pt);
void gtimerJabI(GTimer *pt);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GTIMER */

#endif /* _GTIMER_H */
/** @} */

