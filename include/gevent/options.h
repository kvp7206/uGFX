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
 * @file    include/gevent/options.h
 * @brief   GEVENT sub-system options header file.
 *
 * @addtogroup GEVENT
 * @{
 */

#ifndef _GEVENT_OPTIONS_H
#define _GEVENT_OPTIONS_H

/**
 * @name    GEVENT Functionality to be included
 * @{
 */
	/**
	 * @brief   Should routines assert() if they run out of resources.
	 * @details	Defaults to FALSE.
	 * @details	If FALSE the application must be prepared to handle these
	 *			failures.
	 */
	#ifndef GEVENT_ASSERT_NO_RESOURCE
		#define GEVENT_ASSERT_NO_RESOURCE		FALSE
	#endif
/**
 * @}
 *
 * @name    GEVENT Optional Sizing Parameters
 * @{
 */
	/**
	 * @brief   Defines the maximum size of an event status variable.
	 * @details	Defaults to 32 bytes
	 */
	#ifndef GEVENT_MAXIMUM_SIZE
		#define GEVENT_MAXIMUM_SIZE				32
	#endif
	/**
	 * @brief   Defines the maximum Source/Listener pairs in the system.
	 * @details	Defaults to 32
	 */
	#ifndef GEVENT_MAX_SOURCE_LISTENERS
		#define GEVENT_MAX_SOURCE_LISTENERS		32
	#endif
/** @} */

#endif /* _GEVENT_OPTIONS_H */
/** @} */
