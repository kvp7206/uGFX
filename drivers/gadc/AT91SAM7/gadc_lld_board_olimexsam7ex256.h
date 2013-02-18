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
 * @file    drivers/gadc/AT91SAM7/gadc_lld_board_olimexsam7ex256.h
 * @brief   GADC Driver config file.
 *
 * @addtogroup GADC
 * @{
 */

#ifndef _GADC_LLD_BOARD_OLIMEXSAM7EX256_H
#define _GADC_LLD_BOARD_OLIMEXSAM7EX256_H

#if GFX_USE_GADC

/*===========================================================================*/
/* Analogue devices on this board                                            */
/*===========================================================================*/

#define GADC_PHYSDEV_MICROPHONE			0x00000080
#define GADC_PHYSDEV_DIAL				0x00000040
#define GADC_PHYSDEV_TEMPERATURE		0x00000020

#endif	/* GFX_USE_GADC */

#endif	/* _GADC_LLD_BOARD_OLIMEXSAM7EX256_H */
/** @} */

