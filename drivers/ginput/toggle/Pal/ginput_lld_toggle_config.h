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
 * @file    drivers/ginput/toggle/Pal/ginput_lld_toggle_config.h
 * @brief   GINPUT Toggle Driver configuration header.
 *
 * @defgroup Toggle Toggle
 * @ingroup GINPUT
 * @{
 */

#ifndef _GINPUT_LLD_TOGGLE_CONFIG_H
#define _GINPUT_LLD_TOGGLE_CONFIG_H

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE

#if GINPUT_TOGGLE_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "ginput_lld_toggle_board.h"
#elif defined(BOARD_OLIMEX_SAM7_EX256)
	#include "ginput_lld_toggle_board_olimexsam7ex256.h"
#else
	/* Include the user supplied board definitions */
	#include "ginput_lld_toggle_board.h"
#endif

#endif	/* GFX_USE_GDISP && GINPUT_NEED_TOGGLE */

#endif	/* _GINPUT_LLD_TOGGLE_CONFIG_H */
/** @} */
