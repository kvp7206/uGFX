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
 * @file    drivers/ginput/toggle/Pal/ginput_lld_toggle.c
 * @brief   GINPUT Toggle low level driver source for the ChibiOS PAL hardware.
 *
 * @defgroup Toggle Toggle
 * @ingroup GINPUT
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if (GFX_USE_GINPUT && GINPUT_NEED_TOGGLE) /*|| defined(__DOXYGEN__)*/

#include "ginput/lld/toggle.h"

GINPUT_TOGGLE_DECLARE_STRUCTURE();

/**
 * @brief   Initialise the port.
 *
 * @param[in] ptc	A pointer to one of the entries in GInputToggleConfigTable
 *
 * @notapi
 */
void ginput_lld_toggle_init(const GToggleConfig *ptc) {
	palSetGroupMode(((IOBus *)ptc->id)->portid, ptc->mask, 0, ptc->mode);
}

/**
 * @brief   Get the bits from the port.
 *
 * @param[in] ptc	A pointer to one of the entries in GInputToggleConfigTable
 *
 * @notapi
 */
unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc) {
	return palReadBus((IOBus *)ptc->id);
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_TOGGLE */
/** @} */
