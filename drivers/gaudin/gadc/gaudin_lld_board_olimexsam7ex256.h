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
