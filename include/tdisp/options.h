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
 * @file    include/tdisp/options.h
 * @brief   TDISP sub-system options header file.
 *
 * @addtogroup TDISP
 * @{
 */

#ifndef _TDISP_OPTIONS_H
#define _TDISP_OPTIONS_H

#if GFX_USE_TDISP
/**
 * @name    TDISP configuration
 * @{
 */

	/**
	 * @brief	How many rows of characters the TDISP provides
	 */
	#ifndef TDISP_ROWS
		#define TDISP_ROWS		2
	#endif

	/**
	 * @brief	How many columns of characters the TDISP provides
	 */
	#ifndef TDISP_COLUMNS
		#define TDISP_COLUMNS	16
	#endif

/** @} */

/**
 * @name	TDISP interface configuration
 * @note	Only one of these interfaces can be selected at a time!
 * @{
 */
	/**
	 * @brief	Use the 4-bit paralle interface
	 */
	#ifndef TDISP_NEED_4BIT_MODE
		#define TDISP_NEED_4BIT_MODE	FALSE
	#endif

	/**
	 * @brief	Use the 8-bit parallel interface
	 */
	#ifndef TDISP_NEED_8BIT_MODE
		#define TDISP_NEED_8BIT_MODE	FALSE
	#endif

	#if (!TDISP_NEED_4BIT_MODE && !TDISP_NEED_8BIT_MODE)
		#error "Either TDISP_NEED_4BIT_MODE or TDISP_NEED_8BIT_MODE needs to be set to TRUE in your gfxconf.h!"
	#endif

	#if (TDISP_NEED_4BIT_MODE && TDISP_NEED_8BIT_MODE)
		#error "Only TDISP_NEED_4BIT_MODE or TDISP_NEED_8BIT_MODE can be set to TRUE, not both at one!"
	#endif

/** @} */

#endif /* GFX_USE_TDISP */

#endif /* _TDISP_OPTIONS_H */
/** @} */

