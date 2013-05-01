/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
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
