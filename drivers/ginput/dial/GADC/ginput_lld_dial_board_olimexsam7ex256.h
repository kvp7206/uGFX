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
 * @file    drivers/ginput/dial/GADC/ginput_lld_dial_board_olimexsam7ex256.h
 * @brief   GINPUT Dial Driver config file.
 *
 * @defgroup Dial Dial
 * @ingroup GINPUT
 * @{
 */

#ifndef _GINPUT_LLD_DIAL_BOARD_OLIMEXSAM7EX256_H
#define _GINPUT_LLD_DIAL_BOARD_OLIMEXSAM7EX256_H

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL

/*===========================================================================*/
/* Analogue devices on this board                                            */
/*===========================================================================*/

#define GINPUT_DIAL_NUM_PORTS			1
#define GINPUT_DIAL_DEVICE0				GADC_PHYSDEV_DIAL
#define GINPUT_DIAL_POLL_PERIOD			200
#define GINPUT_DIAL_CYCLE_POLL			FALSE

#endif	/* GFX_USE_GINPUT && GINPUT_NEED_DIAL */

#endif	/* _GINPUT_LLD_DIAL_BOARD_OLIMEXSAM7EX256_H */
/** @} */

