/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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
 * @file    drivers/ginput/dial/GADC/ginput_lld_dial_config.h
 * @brief   GINPUT Dial Driver configuration header.
 *
 * @defgroup Dial Dial
 * @ingroup GINPUT
 * @{
 */

#ifndef _GINPUT_LLD_DIAL_CONFIG_H
#define _GINPUT_LLD_DIAL_CONFIG_H

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL

#define GINPUT_DIAL_MAX_VALUE	((1<<GADC_BITS_PER_SAMPLE)-1)

#if GINPUT_TOGGLE_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "ginput_lld_dial_board.h"
#elif defined(BOARD_OLIMEX_SAM7_EX256)
	#include "ginput_lld_dial_board_olimexsam7ex256.h"
#else
	/* Include the user supplied board definitions */
	#include "ginput_lld_dial_board.h"
#endif

#endif	/* GFX_USE_GDISP && GINPUT_NEED_DIAL */

#endif	/* _GINPUT_LLD_DIAL_CONFIG_H */
/** @} */
