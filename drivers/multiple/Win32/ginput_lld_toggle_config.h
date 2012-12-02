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
 * @file    drivers/multiple/Win32/ginput_lld_toggle_config.h
 * @brief   GINPUT Toggle Driver configuration header.
 *
 * @addtogroup GINPUT_TOGGLE
 * @{
 */

#ifndef _GINPUT_LLD_TOGGLE_CONFIG_H
#define _GINPUT_LLD_TOGGLE_CONFIG_H

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE

#define GINPUT_TOGGLE_POLL_PERIOD		TIME_INFINITE	// We are interrupt driven (or polled - ether works here)
#define GINPUT_TOGGLE_NUM_PORTS			8			// The total number of toggle inputs
#define GINPUT_TOGGLE_CONFIG_ENTRIES	1			// The total number of GToggleConfig entries

#define GINPUT_TOGGLE_SW1			0				// Switch 1	- Toggle
#define GINPUT_TOGGLE_SW2			1				// Switch 2	- Toggle
#define GINPUT_TOGGLE_SW3			2				// Switch 3	- Toggle
#define GINPUT_TOGGLE_SW4			3				// Switch 4	- Toggle

#define GINPUT_TOGGLE_MOMENTARY1	4				// Switch 5	- Momentary
#define GINPUT_TOGGLE_MOMENTARY2	5				// Switch 6	- Momentary
#define GINPUT_TOGGLE_MOMENTARY3	6				// Switch 7	- Momentary
#define GINPUT_TOGGLE_MOMENTARY4	7				// Switch 8	- Momentary

#endif	/* GFX_USE_GDISP && GINPUT_NEED_TOGGLE */

#endif	/* _GINPUT_LLD_TOGGLE_CONFIG_H */
/** @} */
