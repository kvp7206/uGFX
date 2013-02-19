/**
 * This file has a different license to the rest of the GFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

/**
 * Copy this file into your project directory and rename it as gfxconf.h
 * Edit your copy to turn on the GFX features you want to use.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* GFX sub-systems to turn on */
#define GFX_USE_GDISP			TRUE
#define GFX_USE_TDISP			FALSE
#define GFX_USE_GWIN			TRUE
#define GFX_USE_GEVENT			FALSE
#define GFX_USE_GTIMER			TRUE
#define GFX_USE_GINPUT			FALSE
#define GFX_USE_GADC			TRUE
#define GFX_USE_GAUDIN			FALSE
#define GFX_USE_GAUDOUT			FALSE
#define GFX_USE_GMISC			FALSE

/* Features for the GDISP sub-system. */
#define GDISP_NEED_VALIDATION	TRUE
#define GDISP_NEED_CLIP			TRUE
#define GDISP_NEED_TEXT			TRUE
#define GDISP_NEED_CIRCLE		FALSE
#define GDISP_NEED_ELLIPSE		FALSE
#define GDISP_NEED_ARC			FALSE
#define GDISP_NEED_SCROLL		FALSE
#define GDISP_NEED_PIXELREAD	FALSE
#define GDISP_NEED_CONTROL		TRUE
#define GDISP_NEED_MULTITHREAD	TRUE
#define GDISP_NEED_ASYNC		FALSE
#define GDISP_NEED_MSGAPI		FALSE

/* GDISP - builtin fonts */
#define GDISP_OLD_FONT_DEFINITIONS		FALSE
#define GDISP_INCLUDE_FONT_SMALL		FALSE
#define GDISP_INCLUDE_FONT_LARGER		FALSE
#define GDISP_INCLUDE_FONT_UI1			FALSE
#define GDISP_INCLUDE_FONT_UI2			TRUE
#define GDISP_INCLUDE_FONT_LARGENUMBERS	FALSE

/* Features for the TDISP subsystem. */
#define TDISP_NEED_MULTITHREAD	FALSE

/* Features for the GWIN sub-system. */
#define GWIN_NEED_BUTTON		FALSE
#define GWIN_NEED_CONSOLE		TRUE
#define GWIN_NEED_GRAPH			FALSE

/* Features for the GEVENT sub-system. */
#define GEVENT_ASSERT_NO_RESOURCE	FALSE

/* Features for the GTIMER sub-system. */
/* NONE */

/* Features for the GINPUT sub-system. */
#define GINPUT_NEED_MOUSE		FALSE
#define GINPUT_NEED_KEYBOARD	FALSE
#define GINPUT_NEED_TOGGLE		FALSE
#define GINPUT_NEED_DIAL		FALSE

/* Features for the GADC sub-system. */
/* NONE */

/* Features for the GAUDIN sub-system. */
/* NONE */

/* Features for the GAUDOUT sub-system. */
/* NONE */

/* Features for the GMISC sub-system. */
#define GMISC_NEED_ARRAYOPS		FALSE

/* Optional Parameters for various sub-systems */
/*
	#define GDISP_MAX_FONT_HEIGHT			16
	#define GEVENT_MAXIMUM_SIZE				32
	#define GEVENT_MAX_SOURCE_LISTENERS		32
	#define GTIMER_THREAD_WORKAREA_SIZE		512
	#define GADC_MAX_LOWSPEED_DEVICES		4
*/

/* Optional Low Level Driver Definitions */
/*
	#define GDISP_USE_CUSTOM_BOARD		FALSE
	#define GDISP_SCREEN_WIDTH			320
	#define GDISP_SCREEN_HEIGHT			240
	#define GDISP_USE_FSMC
	#define GDISP_USE_GPIO
	#define GDISP_VMT_NAME1(x)			x##YourDriver1
	#define GDISP_VMT_NAME2(x)			x##YourDriver2
	#define TDISP_COLUMNS				16
	#define TDISP_ROWS					2
*/

#endif /* _GFXCONF_H */
