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
 * @file    include/gtimer/options.h
 * @brief   GTIMER sub-system options header file.
 *
 * @addtogroup GTIMER
 * @{
 */

#ifndef _GTIMER_OPTIONS_H
#define _GTIMER_OPTIONS_H

/**
 * @name    GTIMER Functionality to be included
 * @{
 */
/**
 * @}
 *
 * @name    GTIMER Optional Sizing Parameters
 * @{
 */
	/**
	 * @brief   Defines the size of the timer threads work area (stack+structures).
	 * @details	Defaults to 512 bytes
	 */
	#ifndef GTIMER_THREAD_WORKAREA_SIZE
		#define GTIMER_THREAD_WORKAREA_SIZE		512
	#endif
/** @} */

#endif /* _GTIMER_OPTIONS_H */
/** @} */
