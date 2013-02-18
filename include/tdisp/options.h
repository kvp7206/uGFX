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
 * @name    TDISP Functionality to be included
 * @{
 */
/**
 * @}
 *
 * @name    TDISP Multi-Threading Options
 * @{
 */
	/**
	 * @brief   Do the display functions need to be thread-safe.
	 * @details	Defaults to FALSE
	 */
	#ifndef TDISP_NEED_MULTITHREAD
		#define TDISP_NEED_MULTITHREAD	FALSE
	#endif
/**
 * @}
 *
 * @name    TDISP Optional Low Level Driver Defines
 * @{
 */
	/**
	 * @brief   Use a custom board definition even if a board definition exists.
	 * @details	Defaults to FALSE
	 * @details	If TRUE, add tdisp_lld_board.h to your project directory and customise it.
	 * @note	Not all TDISP low level drivers currently use board definition files.
	 */
	#ifndef TDISP_USE_CUSTOM_BOARD
		#define TDISP_USE_CUSTOM_BOARD		FALSE
	#endif
	/**
	 * @brief   Set the screen height and width.
	 * @note	Ignored by some low level GDISP drivers, optional for others.
	 * @note	Where these values are allowed, a default is always provided by the low level driver.
	 */
	/* #define TDISP_COLUMNS		16 */
	/* #define TDISP_ROWS			2  */
/** @} */

#endif /* GFX_USE_TDISP */

#endif /* _TDISP_OPTIONS_H */
/** @} */

