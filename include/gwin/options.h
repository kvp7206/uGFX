/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    include/gwin/options.h
 * @brief   GWIN sub-system options header file.
 *
 * @addtogroup GWIN
 * @{
 */

#ifndef _GWIN_OPTIONS_H
#define _GWIN_OPTIONS_H

/**
 * @name    GWIN Functionality to be included
 * @{
 */
	/**
	 * @brief   Should button functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_BUTTON
		#define GWIN_NEED_BUTTON	FALSE
	#endif
	/**
	 * @brief   Should console functions be included.
	 * @details	Defaults to FALSE
	 * @note	To use chprintf() for printing in a console window you need to
	 * 			include in your application source file...
	 * 			\#include "chprintf.h"
	 * 			Also in your makefile, as part of your list of C source files, include
	 * 			${CHIBIOS}/os/various/chprintf.c
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
	 * @brief   Should slider functions be included.
	 * @details	Defaults to FALSE
	 */
	#ifndef GWIN_NEED_SLIDER
		#define GWIN_NEED_SLIDER	FALSE
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
	 */
	#ifndef GWIN_CONSOLE_USE_BASESTREAM
		#define GWIN_CONSOLE_USE_BASESTREAM		FALSE
	#endif
/** @} */

#endif /* _GWIN_OPTIONS_H */
/** @} */
