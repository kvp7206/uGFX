/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gwin/options.h
 * @brief   GWIN sub-system options header file.
 *
 * @addtogroup GWIN
 * @brief	Module which provides a complete GUI toolkit based on widgets
 * 
 * @{
 */

#ifndef _GWIN_OPTIONS_H
#define _GWIN_OPTIONS_H

/**
 * @name    GWIN Functionality to be included
 * @{
 */
	/**
	 * @brief   Should window manager support be included
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_WINDOWMANAGER
		#define GWIN_NEED_WIDGET	FALSE
	#endif
	/**
	 * @brief   Should widget functions be included. Needed for any widget (eg Buttons, Sliders etc)
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_WIDGET
		#define GWIN_NEED_WIDGET	FALSE
	#endif
	/**
	 * @brief   Should console functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_CONSOLE
		#define GWIN_NEED_CONSOLE	FALSE
	#endif
	/**
	 * @brief   Should graph functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_GRAPH
		#define GWIN_NEED_GRAPH		FALSE
	#endif
	/**
	 * @brief   Should button functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_BUTTON
		#define GWIN_NEED_BUTTON	FALSE
	#endif
	/**
	 * @brief   Should slider functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_SLIDER
		#define GWIN_NEED_SLIDER	FALSE
	#endif
	/**
	 * @brief   Should checkbox functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_CHECKBOX
		#define GWIN_NEED_CHECKBOX	FALSE
	#endif
	/**
	 * @brief   Should image functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_IMAGE
		#define GWIN_NEED_IMAGE		FALSE
	#endif
	/**
	 * @brief   Should label functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_LABEL
		#define GWIN_NEED_LABEL		FALSE
	#endif
	/**
	 * @brief   Should radio button functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_RADIO
		#define GWIN_NEED_RADIO		FALSE
	#endif
/**
 * @}
 *
 * @name    GWIN Optional Parameters
 * @{
 */
	/**
	 * @brief   Buttons should not insist the mouse is over the button on mouse release
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_BUTTON_LAZY_RELEASE
		#define GWIN_BUTTON_LAZY_RELEASE		FALSE
	#endif
	/**
	 * @brief   Console Windows need floating point support in @p gwinPrintf
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_CONSOLE_USE_FLOAT
		#define GWIN_CONSOLE_USE_FLOAT			FALSE
	#endif
	/**
	 * @brief   Console Windows need BaseStreamSequential support (ChibiOS only)
	 * @details	Defaults to FALSE
	 * @note	To use the ChibiOS basestream functions such as chprintf()
	 * 			for printing in a console window you need to set this option to
	 * 			TRUE in your gfxconf.h and include in your application source file...
	 * 			\#include "chprintf.h"
	 * 			In your makefile, as part of your list of C source files, include
	 * 			${CHIBIOS}/os/various/chprintf.c
	 */
	#ifndef GWIN_CONSOLE_USE_BASESTREAM
		#define GWIN_CONSOLE_USE_BASESTREAM		FALSE
	#endif
	/**
	 * @brief   Image windows can optionally support animated images
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_IMAGE_ANIMATION
		#define GWIN_NEED_IMAGE_ANIMATION		FALSE
	#endif
/** @} */

#endif /* _GWIN_OPTIONS_H */
/** @} */
