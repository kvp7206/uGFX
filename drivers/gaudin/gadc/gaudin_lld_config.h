/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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
