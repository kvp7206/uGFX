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
	 * @brief   The maximum simultaneous GADC low speed device conversions
	 * @details	Defaults to 4
	 * @note	This value must be less than the number of conversions that can occur
	 * 			in a single high speed ADC cycle including the high speed ADC conversion.
	 * 			For example, if the ADC can run at 132k samples per second and the high speed
	 * 			virtual ADC is using 44kHz then GADC_MAX_LOWSPEED_DEVICES should be set to
	 * 			132/44 - 1 = 2
	 */
	#ifndef GADC_MAX_LOWSPEED_DEVICES
		#define GADC_MAX_LOWSPEED_DEVICES	4
	#endif
/** @} */

#endif /* _GADC_OPTIONS_H */
/** @} */
