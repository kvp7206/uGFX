/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

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
 * @file    include/ginput/lld/dial.h
 * @brief   GINPUT header file for dial drivers.
 *
 * @defgroup Dial Dial
 * @ingroup GINPUT
 * @{
 */

#ifndef _LLD_GINPUT_DIAL_H
#define _LLD_GINPUT_DIAL_H

#if GINPUT_NEED_DIAL || defined(__DOXYGEN__)

#include "ginput_lld_dial_config.h"

typedef void (*DialCallbackFn)(uint16_t instance, uint16_t rawvalue);

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	void ginput_lld_dial_init(void);
	void ginput_lld_dial_poll(DialCallbackFn fn);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GINPUT && GINPUT_NEED_TOGGLE */

#endif /* _LLD_GINPUT_TOGGLE_H */
/** @} */

