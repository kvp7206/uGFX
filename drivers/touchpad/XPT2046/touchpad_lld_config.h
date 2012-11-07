/*
    ChibiOS/GFX - Copyright (C) 2012
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
 * @file	drivers/touchpad/XPT2046/touchpad_lld_config.h
 * @brief   Touchppad Driver subsystem low level driver.
 *
 * @addtogroup TOUCHPAD
 * @{
 */

#ifndef _TOUCHPAD_LLD_CONFIG_H
#define _TOUCHPAD_LLD_CONFIG_H

#if GFX_USE_TOUCHPAD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define TOUCHPAD_HAS_IRQ				TRUE
#define TOUCHPAD_HAS_PRESSURE			TRUE

#endif	/* GFX_USE_TOUCHPAD */

#endif	/* _TOUCHPAD_LLD_CONFIG_H */
/** @} */

