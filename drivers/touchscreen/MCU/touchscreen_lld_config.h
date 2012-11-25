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
 * @file	drivers/touchscreen/MCU/touchscreen_lld_config.h
 * @brief   Touchscreen Driver subsystem low level driver.
 *
 * @addtogroup TOUCHSCREEN
 * @{
 */

#ifndef TOUCHSCREEN_LLD_CONFIG_H
#define TOUCHSCREEN_LLD_CONFIG_H

#if GFX_USE_TOUCHSCREEN /*|| defined(__DOXYGEN__)*/

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define TOUCHSCREEN_HAS_PRESSED				TRUE
#define TOUCHSCREEN_HAS_PRESSURE			FALSE

/**
 * @brief	The touchscreen driver struct
 * @details	Pointer to this will be passed to tsInit()
 */
struct TouchscreenDriver {
	ADCDriver		*adc_driver;
	ioportid_t 		yd_port;
	ioportmask_t	yd_pin;
	ioportid_t 		yu_port;
	ioportmask_t	yu_pin;
	ioportid_t 		xl_port;
	ioportmask_t	xl_pin;
	ioportid_t 		xr_port;
	ioportmask_t	xr_pin;
};

#endif	/* GFX_USE_TOUCHSCREEN */

#endif	/* TOUCHSCREEN_LLD_CONFIG_H */
/** @} */

