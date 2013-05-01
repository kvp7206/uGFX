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
 * @file    include/gadc/options.h
 * @brief   GADC - Periodic ADC subsystem options header file.
 *
 * @addtogroup GADC
 * @{
 */

#ifndef _GADC_OPTIONS_H
#define _GADC_OPTIONS_H

/**
 * @name    GADC Functionality to be included
 * @{
 */
/**
 * @}
 *
 * @name    GADC Optional Sizing Parameters
 * @{
 */
	/**
	 * @brief   The maximum GADC sample rate
	 * @details	Defaults to 44000
	 * @note	This value must be less than half the maximum sample rate allowed by the CPU.
	 * 			This is to ensure there is time between high speed samples to perform low
	 * 			speed device sampling.
	 */
	#ifndef GADC_MAX_HIGH_SPEED_SAMPLERATE
		#define GADC_MAX_HIGH_SPEED_SAMPLERATE	44000
	#endif
/** @} */

#endif /* _GADC_OPTIONS_H */
/** @} */
