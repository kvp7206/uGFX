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
 * @file    include/gfx.h
 * @brief   GFX system header file.
 *
 * @addtogroup GFX
 * @{
 */

#ifndef _GFX_H
#define _GFX_H

/* gfxconf.h is the user's project configuration for the GFX system. */
#include "gfxconf.h"

/**
 * @name    GFX sub-systems that can be turned on
 * @{
 */
	/**
	 * @brief   GFX Graphics Display Basic API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware driver to your makefile.
	 * 			Eg.  include $(GFXLIB)/drivers/gdisp/Nokia6610/gdisp_lld.mk
	 */
	#ifndef GFX_USE_GDISP
		#define GFX_USE_GDISP	FALSE
	#endif
	/**
	 * @brief   GFX Text Display Basic API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware driver to your makefile.
	 * 			Eg.  include $(GFXLIB)/drivers/tdisp/HD44780/tdisp_lld.mk
	 */
	#ifndef GFX_USE_TDISP
		#define GFX_USE_TDISP	FALSE
	#endif
	/**
	 * @brief   GFX Graphics Windowing API
	 * @details	Defaults to FALSE
	 * @details	Extends the GDISP API to add the concept of graphic windows.
	 * @note	Also supports high-level "window" objects such as console windows,
	 * 			buttons, graphing etc
	 */
	#ifndef GFX_USE_GWIN
		#define GFX_USE_GWIN	FALSE
	#endif
	/**
	 * @brief   GFX Event API
	 * @details	Defaults to FALSE
	 * @details	Defines the concept of a "Source" that can send "Events" to "Listeners".
	 */
	#ifndef GFX_USE_GEVENT
		#define GFX_USE_GEVENT	FALSE
	#endif
	/**
	 * @brief   GFX Timer API
	 * @details	Defaults to FALSE
	 * @details	Provides thread context timers - both one-shot and periodic.
	 */
	#ifndef GFX_USE_GTIMER
		#define GFX_USE_GTIMER	FALSE
	#endif
	/**
	 * @brief   GFX Input Device API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware drivers to your makefile.
	 * 			Eg.
	 * 				include $(GFXLIB)/drivers/ginput/toggle/Pal/ginput_lld.mk
	 * 			and...
	 * 				include $(GFXLIB)/drivers/ginput/touch/MCU/ginput_lld.mk
	 */
	#ifndef GFX_USE_GINPUT
		#define GFX_USE_GINPUT	FALSE
	#endif
	/**
	 * @brief   GFX Generic Periodic ADC API
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_GADC
		#define GFX_USE_GADC	FALSE
	#endif
	/**
	 * @brief   GFX Audio Input Device API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware drivers to your makefile.
	 * 			Eg.
	 * 				include $(GFXLIB)/drivers/gaudin/GADC/gaudin_lld.mk
	 */
	#ifndef GFX_USE_GAUDIN
		#define GFX_USE_GAUDIN	FALSE
	#endif
	/**
	 * @brief   GFX Audio Output Device API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware drivers to your makefile.
	 * 			Eg.
	 * 				include $(GFXLIB)/drivers/gaudout/PWM/gaudout_lld.mk
	 */
	#ifndef GFX_USE_GAUDOUT
		#define GFX_USE_GAUDOUT	FALSE
	#endif
	/**
	 * @brief   GFX Miscellaneous Routines API
	 * @details	Defaults to FALSE
	 * @note	Turning this on without turning on any GMISC_NEED_xxx macros will result
	 * 			in no extra code being compiled in. GMISC is made up from the sum of its
	 * 			parts.
	 */
	#ifndef GFX_USE_GMISC
		#define GFX_USE_GMISC	FALSE
	#endif
/** @} */

/**
 * Get all the options for each sub-system.
 *
 */
#include "gmisc/options.h"
#include "gevent/options.h"
#include "gtimer/options.h"
#include "gdisp/options.h"
#include "gwin/options.h"
#include "ginput/options.h"
#include "tdisp/options.h"
#include "gadc/options.h"
#include "gaudin/options.h"
#include "gaudout/options.h"

/**
 * Inter-dependancy safety checks on the sub-systems.
 *
 */
#include "gfx_rules.h"

/**
 *  Include the sub-system header files
 */
#include "gevent/gevent.h"
#include "gtimer/gtimer.h"
#include "gdisp/gdisp.h"
#include "gwin/gwin.h"
#include "ginput/ginput.h"
#include "tdisp/tdisp.h"
#include "gadc/gadc.h"
#include "gaudin/gaudin.h"
#include "gaudout/gaudout.h"
#include "gmisc/gmisc.h"

#endif /* _GFX_H */
/** @} */
