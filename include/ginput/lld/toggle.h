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
 * @file    include/ginput/lld/toggle.h
 * @brief   GINPUT header file for toggle drivers.
 *
 * @defgroup Toggle Toggle
 * @ingroup GINPUT
 * @{
 */

#ifndef _LLD_GINPUT_TOGGLE_H
#define _LLD_GINPUT_TOGGLE_H

#if GINPUT_NEED_TOGGLE || defined(__DOXYGEN__)

// Describes how the toggle bits are obtained
typedef struct GToggleConfig_t {
	void		*id;
	unsigned	mask;
	unsigned	invert;
	iomode_t	mode;
} GToggleConfig;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	extern const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES];
	
	void ginput_lld_toggle_init(const GToggleConfig *ptc);
	unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc);

	/* This routine is provided to low level drivers to wakeup a value read from a thread context.
	 *	Particularly useful if GINPUT_TOGGLE_POLL_PERIOD = TIME_INFINITE
	 */
	void ginputToggleWakeup(void);

	/* This routine is provided to low level drivers to wakeup a value read from an ISR
	 *	Particularly useful if GINPUT_TOGGLE_POLL_PERIOD = TIME_INFINITE
	 */
	void ginputToggleWakeupI(void);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GINPUT && GINPUT_NEED_TOGGLE */

#endif /* _LLD_GINPUT_TOGGLE_H */
/** @} */

