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
 * @file    include/gmisc/options.h
 * @brief   GMISC - Miscellaneous Routines options header file.
 *
 * @addtogroup GMISC
 * @{
 */

#ifndef _GMISC_OPTIONS_H
#define _GMISC_OPTIONS_H

/**
 * @name    GMISC Functionality to be included
 * @{
 */
	/**
	 * @brief   Include array operation functions
	 * @details	Defaults to FALSE
	 */
	#ifndef GMISC_NEED_ARRAYOPS
		#define GMISC_NEED_ARRAYOPS		FALSE
	#endif
/**
 * @}
 *
 * @name    GMISC Optional Sizing Parameters
 * @{
 */
/** @} */

#endif /* _GMISC_OPTIONS_H */
/** @} */
