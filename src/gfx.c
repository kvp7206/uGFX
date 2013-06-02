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
 * @file    src/gfx.c
 * @brief   GFX common routines.
 */

/* Display various warnings from gfx_rules.h */
#define GFX_DISPLAY_RULE_WARNINGS	TRUE

#include "gfx.h"

void DEPRECATED("Use gfxInit() instead") gdispInit() { gfxInit(); }

/* These init functions are defined by each module but not published */
extern void _gosInit(void);
#if GFX_USE_GDISP && (GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC)
	extern void _gdispInit(void);
#endif
#if GFX_USE_TDISP
	extern void _tdispInit(void);
#endif
#if GFX_USE_GWIN
	extern void _gwinInit(void);
#endif
#if GFX_USE_GEVENT
	extern void _geventInit(void);
#endif
#if GFX_USE_GTIMER
	extern void _gtimerInit(void);
#endif
#if GFX_USE_GINPUT
	extern void _ginputInit(void);
#endif
#if GFX_USE_GADC
	extern void _gadcInit(void);
#endif
#if GFX_USE_GAUDIN
	extern void _gaudinInit(void);
#endif
#if GFX_USE_GAUDOUT
	extern void _gaudoutInit(void);
#endif
#if GFX_USE_GMISC
	extern void _gmiscInit(void);
#endif

void gfxInit(void) {
	static bool_t	initDone = FALSE;

	/* Ensure we only initialise once */
	if (initDone)
		return;
	initDone = TRUE;

	/* These must be initialised in the order of their dependancies */
	_gosInit();
	#if GFX_USE_GMISC
		_gmiscInit();
	#endif
	#if GFX_USE_GEVENT
		_geventInit();
	#endif
	#if GFX_USE_GTIMER
		_gtimerInit();
	#endif
	#if GFX_USE_GDISP
		_gdispInit();
		gdispClear(Black);
	#endif
	#if GFX_USE_GWIN
		_gwinInit();
	#endif
	#if GFX_USE_GINPUT
		_ginputInit();
	#endif
	#if GFX_USE_TDISP
		_tdispInit();
		tdispHome();
		tdispClear();
	#endif
	#if GFX_USE_GADC
		_gadcInit();
	#endif
	#if GFX_USE_GAUDIN
		_gaudinInit();
	#endif
	#if GFX_USE_GAUDOUT
		_gaudoutInit();
	#endif
}
