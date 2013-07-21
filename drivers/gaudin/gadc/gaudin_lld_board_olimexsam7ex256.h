/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gaudin/gadc/gaudin_lld_board_olimexsam7ex256.h
 * @brief   GAUDIN Driver board config file for the Olimex SAM7EX256 board
 *
 * @addtogroup GAUDIN
 * @{
 */

#ifndef _GAUDIN_LLD_BOARD_OLIMEXSAM7EX256_H
#define _GAUDIN_LLD_BOARD_OLIMEXSAM7EX256_H

/*===========================================================================*/
/* Audio inputs on this board                                                */
/*===========================================================================*/

/**
 * @brief	The number of audio channels supported by this driver
 */
#define GAUDIN_NUM_CHANNELS					1

/**
 * @brief	The list of audio channels and their uses
 * @{
 */
#define	GAUDIN_MICROPHONE					0
/** @} */

/**
 * @brief	The following defines are for the low level driver use only
 * @{
 */
#ifdef GAUDIN_LLD_IMPLEMENTATION
	static uint32_t gaudin_lld_physdevs[GAUDIN_NUM_CHANNELS] = {
			GADC_PHYSDEV_MICROPHONE,
			};
#endif
/** @} */

#endif	/* _GAUDIN_LLD_BOARD_OLIMEXSAM7EX256_H */
/** @} */
