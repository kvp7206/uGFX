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
 * @file    drivers/gadc/AT91SAM7/gadc_lld_config.h
 * @brief   GADC Driver config file.
 *
 * @addtogroup GADC
 * @{
 */

#ifndef GADC_LLD_CONFIG_H
#define GADC_LLD_CONFIG_H

#if GFX_USE_GADC

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

/**
 * @brief	ChibiOS has a nasty bug in its _adc_isr_full_code() routine (defined in adc.h as a macro).
 * 			Do we have the version of ChibiOS with this bug.
 * @detail	Set to TRUE if ChibiOS has this bug.
 * @note	Fixed in ChibiOS 2.4.4stable and 2.5.2unstable (and the repository from 18th Feb 2013)
 * @note	This bug prevents us re-calling adcStartConversionI() from with the ISR even though
 * 			it is clearly designed to handle it. For some reason (on this micro) the high speed timer
 * 			is not affected only the single sample low speed timer. In that situation we wait until
 * 			we get back to thread land. This is terrible for the accuracy of the high speed timer
 * 			but what can we do (other than fix the bug).
 * @note	For the AT91SAM7 ADC driver, it post-dates the finding of the bug so we safely
 * 			say that the bug doesn't exist for this driver.
 */
#define ADC_ISR_FULL_CODE_BUG				FALSE

/**
 * @brief	The maximum sample frequency supported by this CPU
 */
#define GADC_MAX_SAMPLE_FREQUENCY			132000

/**
 * @brief	The number of bits in a sample
 */
#define GADC_BITS_PER_SAMPLE				AT91_ADC1_RESOLUTION

/* Pull in board specific defines */
#if defined(GADC_USE_CUSTOM_BOARD) && GADC_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "gadc_lld_board.h"
#elif defined(BOARD_OLIMEX_SAM7_EX256)
	#include "gadc_lld_board_olimexsam7ex256.h"
#else
	/* Include the user supplied board definitions */
	#include "gadc_lld_board.h"
#endif

#endif	/* GFX_USE_GADC */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */

