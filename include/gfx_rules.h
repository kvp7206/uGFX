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
 * @file    include/gfx_rules.h
 * @brief   GFX system safety rules header file.
 *
 * @addtogroup GFX
 * @{
 */

#ifndef _GFX_RULES_H
#define _GFX_RULES_H

/**
 * Safety checks on all the defines.
 *
 * These are defined in the order of their inter-dependancies.
 */

#if GFX_USE_GWIN
	#if !GFX_USE_GDISP
		#error "GWIN: GFX_USE_GDISP must be TRUE when using GWIN"
	#endif
	#if !GDISP_NEED_CLIP
		#warning "GWIN: Drawing can occur outside the defined windows as GDISP_NEED_CLIP is FALSE"
	#endif
	#if GWIN_NEED_BUTTON
		#if !GDISP_NEED_TEXT
			#error "GWIN: GDISP_NEED_TEXT is required if GWIN_NEED_BUTTON is TRUE."
		#endif
		#if !GFX_USE_GEVENT
			#warning "GWIN: GFX_USE_GEVENT is required if GWIN_NEED_BUTTON is TRUE. It has been turned on for you."
			#undef GFX_USE_GEVENT
			#define GFX_USE_GEVENT	TRUE
		#endif
		#if !GFX_USE_GINPUT || !(GINPUT_NEED_MOUSE || GINPUT_NEED_TOGGLE)
			#warning "GWIN: You have set GWIN_NEED_BUTTON to TRUE but no supported GINPUT (mouse/toggle) devices have been included"
		#endif
		#if !GDISP_NEED_MULTITHREAD && !GDISP_NEED_ASYNC
			#warning "GWIN: Either GDISP_NEED_MULTITHREAD or GDISP_NEED_ASYNC is required if GWIN_NEED_BUTTON is TRUE."
			#warning "GWIN: GDISP_NEED_MULTITHREAD has been turned on for you."
			#undef GDISP_NEED_MULTITHREAD
			#define GDISP_NEED_MULTITHREAD	TRUE
		#endif
	#endif
	#if GWIN_NEED_CONSOLE
		#if !GDISP_NEED_TEXT
			#error "GWIN: GDISP_NEED_TEXT is required if GWIN_NEED_CONSOLE is TRUE."
		#endif
	#endif
	#if GWIN_NEED_GRAPH
	#endif
#endif

#if GFX_USE_GINPUT
	#if !GFX_USE_GEVENT
		#warning "GINPUT: GFX_USE_GEVENT is required if GFX_USE_GINPUT is TRUE. It has been turned on for you."
		#undef GFX_USE_GEVENT
		#define	GFX_USE_GEVENT		TRUE
	#endif
	#if !GFX_USE_GTIMER
		#warning "GINPUT: GFX_USE_GTIMER is required if GFX_USE_GINPUT is TRUE. It has been turned on for you."
		#undef GFX_USE_GTIMER
		#define	GFX_USE_GTIMER		TRUE
	#endif
#endif

#if GFX_USE_GDISP
	#if GDISP_NEED_MULTITHREAD && GDISP_NEED_ASYNC
		#error "GDISP: Only one of GDISP_NEED_MULTITHREAD and GDISP_NEED_ASYNC should be defined."
	#endif
	#if GDISP_NEED_ASYNC
		#if !GDISP_NEED_MSGAPI
			#warning "GDISP: Messaging API is required for Async Multi-Thread. It has been turned on for you."
			#undef GDISP_NEED_MSGAPI
			#define GDISP_NEED_MSGAPI	TRUE
		#endif
	#endif
#endif

#if GFX_USE_TDISP
#endif

#if GFX_USE_GADC
	#if !CH_USE_MUTEXES || !CH_USE_SEMAPHORES
		#error "GADC: CH_USE_MUTEXES and CH_USE_SEMAPHORES must be defined in chconf.h"
	#endif
	#if !GFX_USE_GTIMER
		#warning "GADC: GFX_USE_GTIMER is required if GFX_USE_GADC is TRUE. It has been turned on for you."
		#undef GFX_USE_GTIMER
		#define	GFX_USE_GTIMER		TRUE
	#endif
#endif

#if GFX_USE_GEVENT
	#if !CH_USE_MUTEXES || !CH_USE_SEMAPHORES
		#error "GEVENT: CH_USE_MUTEXES and CH_USE_SEMAPHORES must be defined in chconf.h"
	#endif
#endif

#if GFX_USE_GTIMER
	#if GFX_USE_GDISP && !GDISP_NEED_MULTITHREAD && !GDISP_NEED_ASYNC
		#warning "GTIMER: Neither GDISP_NEED_MULTITHREAD nor GDISP_NEED_ASYNC has been specified."
		#warning "GTIMER: Make sure you are not performing any GDISP/GWIN drawing operations in the timer callback!"
	#endif
#endif

#if GFX_USE_GAUDIN
#endif

#if GFX_USE_GAUDOUT
#endif

#if GFX_USE_GMISC
#endif

#endif /* _GFX_H */
/** @} */
