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

/* GFX subsystems to turn on */
#define GFX_USE_GDISP			FALSE
#define GFX_USE_TDISP			FALSE
#define GFX_USE_GWIN			FALSE
#define GFX_USE_GEVENT			FALSE
#define GFX_USE_GTIMER			FALSE
#define GFX_USE_GINPUT			FALSE
#define GFX_USE_GADC			FALSE
#define GFX_USE_GAUDIN			FALSE
#define GFX_USE_GAUDOUT			FALSE
#define GFX_USE_GMISC			FALSE

/* Features for the GDISP subsystem */
#define GDISP_NEED_VALIDATION	TRUE
#define GDISP_NEED_CLIP			TRUE
#define GDISP_NEED_TEXT			TRUE
#define GDISP_NEED_CIRCLE		TRUE
#define GDISP_NEED_ELLIPSE		TRUE
#define GDISP_NEED_ARC			FALSE
#define GDISP_NEED_SCROLL		FALSE
#define GDISP_NEED_PIXELREAD	FALSE
#define GDISP_NEED_CONTROL		FALSE
#define GDISP_NEED_MULTITHREAD	FALSE
#define GDISP_NEED_ASYNC		FALSE
#define GDISP_NEED_MSGAPI		FALSE

/* GDISP - builtin fonts */
#define GDISP_OLD_FONT_DEFINITIONS		FALSE
#define GDISP_INCLUDE_FONT_SMALL		TRUE
#define GDISP_INCLUDE_FONT_LARGER		TRUE
#define GDISP_INCLUDE_FONT_UI1			TRUE
#define GDISP_INCLUDE_FONT_UI2			TRUE
#define GDISP_INCLUDE_FONT_LARGENUMBERS	TRUE

/* TDISP options */
#define TDISP_COLUMNS			16
#define TDISP_ROWS				2
#define TDISP_NEED_4BIT_MODE	FALSE
#define TDISP_NEED_8BIT_MODE	FALSE

/* Features for the GWIN subsystem. */
#define GWIN_NEED_BUTTON		FALSE
#define GWIN_NEED_CONSOLE		FALSE
#define GWIN_NEED_GRAPH			FALSE

/* Features for the GEVENT subsystem. */
#define GEVENT_ASSERT_NO_RESOURCE	FALSE

/* Features for the GTIMER subsystem. */
/* NONE */

/* Features for the GINPUT subsystem. */
#define GINPUT_NEED_MOUSE		FALSE
#define GINPUT_NEED_KEYBOARD	FALSE
#define GINPUT_NEED_TOGGLE		FALSE
#define GINPUT_NEED_DIAL		FALSE

/* Features for the GADC subsystem. */
/* NONE */

/* Features for the GAUDIN subsystem. */
/* NONE */

/* Features for the GAUDOUT subsystem. */
/* NONE */

/* Features for the GMISC subsystem. */
#define GMISC_NEED_ARRAYOPS		FALSE

/* Optional Parameters for various subsystems */
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
*/

#endif /* _GFXCONF_H */
