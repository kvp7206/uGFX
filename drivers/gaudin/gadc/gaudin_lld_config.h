/*
    ChibiOS/GFX - Copyright (C) 2012, 2013, 2013
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
 * @file    drivers/gaudin/gadc/gaudin_lld_config.h
 * @brief   GAUDIN Driver config file.
 *
 * @addtogroup GAUDIN
 * @{
 */

#ifndef GAUDIN_LLD_CONFIG_H
#define GAUDIN_LLD_CONFIG_H

#if GFX_USE_GAUDIN

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

/**
 * @brief	The audio input sample type
 * @details	For this driver it matches the cpu sample type
 */
typedef adcsample_t		audin_sample_t;

/**
 * @brief	The maximum sample frequency supported by this audio device
 * @details	For this driver it matches the GADC maximum high speed sample rate
 */
#define GAUDIN_MAX_SAMPLE_FREQUENCY			GADC_MAX_HIGH_SPEED_SAMPLERATE

/**
 * @brief	The number of bits in a sample
 * @details	For this driver it matches the cpu sample bits
 */
#define GAUDIN_BITS_PER_SAMPLE				GADC_BITS_PER_SAMPLE

/**
 * @brief	The format of an audio sample
 * @details	For this driver it matches the cpu sample format
 */
#define GAUDIN_SAMPLE_FORMAT				GADC_SAMPLE_FORMAT

/**
 * For the GAUDIN driver that uses GADC - all the remaining config definitions are specific
 * to the board.
 */
#if defined(GADC_USE_CUSTOM_BOARD) && GADC_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "gaudin_lld_board.h"
#elif defined(BOARD_OLIMEX_SAM7_EX256)
	#include "gaudin_lld_board_olimexsam7ex256.h"
#else
	/* Include the user supplied board definitions */
	#include "gaudin_lld_board.h"
#endif

#endif	/* GFX_USE_GAUDIN */

#endif	/* GAUDIN_LLD_CONFIG_H */
/** @} */
