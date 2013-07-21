/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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
#if GFX_USE_GDISP
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
