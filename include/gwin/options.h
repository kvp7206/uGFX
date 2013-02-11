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
 * @}
 *
 * @name    GWIN Optional Sizing Parameters
 * @{
 */
/** @} */

#endif /* _GWIN_OPTIONS_H */
/** @} */
