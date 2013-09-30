/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/ginput/toggle/Pal/ginput_lld_toggle.c
 * @brief   GINPUT Toggle low level driver source for the ChibiOS PAL hardware.
 *
 * @defgroup Toggle Toggle
 * @ingroup GINPUT
 * @{
 */

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
