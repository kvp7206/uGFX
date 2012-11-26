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
 * @file    lld/ginput/toggle.h
 * @brief   GINPUT header file for toggle drivers.
 *
 * @addtogroup GINPUT_TOGGLE
 * @{
 */
#ifndef _LLD_GINPUT_TOGGLE_H
#define _LLD_GINPUT_TOGGLE_H

#ifndef GFX_USE_GINPUT
	#define GFX_USE_GINPUT FALSE
#endif

#if GFX_USE_GINPUT || defined(__DOXYGEN__)

#if GINPUT_NEED_TOGGLE
	// Describes how the toggle bits are obtained
	typedef struct GToggleConfig_t {
		void		*id;
		unsigned	mask;
		unsigned	invert;
		iomode_t	mode;
	} GToggleConfig;
#endif

// This must be included after the above type definition
#include "ginput.h"

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#if GINPUT_NEED_TOGGLE
	extern const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES];
	
	void ginput_lld_toggle_init(const GToggleConfig *ptc);
	unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GINPUT */

#endif /* _LLD_GINPUT_TOGGLE_H */
/** @} */
