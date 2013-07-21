/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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
