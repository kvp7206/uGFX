/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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
