/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    gdispNokia6610/gdisp_lld_config.h
 * @brief   GDISP Graphic Driver subsystem low level driver header for the Nokia6610 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if HAL_USE_GDISP

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_HARDWARE_LINES			FALSE
#define GDISP_HARDWARE_CLEARS			FALSE
#define GDISP_HARDWARE_FILLS			TRUE
#define GDISP_HARDWARE_BITFILLS			TRUE
#define GDISP_HARDWARE_CIRCLES			FALSE
#define GDISP_HARDWARE_CIRCLEFILLS		FALSE
#define GDISP_HARDWARE_ELLIPSES			FALSE
#define GDISP_HARDWARE_ELLIPSEFILLS		FALSE
#define GDISP_HARDWARE_TEXT				FALSE
#define GDISP_HARDWARE_TEXTFILLS		FALSE
#define GDISP_HARDWARE_SCROLL			FALSE
#define GDISP_HARDWARE_PIXELREAD		FALSE
#define GDISP_HARDWARE_CONTROL			FALSE

#define GDISP_SOFTWARE_TEXTFILLDRAW		TRUE
#define GDISP_SOFTWARE_TEXTBLITCOLUMN	FALSE

#define GDISP_PIXELFORMAT_RGB444
#define GDISP_PACKED_PIXELS				FALSE
#define GDISP_PACKED_LINES				FALSE

/*===========================================================================*/
/* Extra fields for the GDISPDriver structure                                 */
/*===========================================================================*/

/*
#define GDISP_DRIVER_EXT_FIELDS		int abc; int def;
*/

#endif	/* HAL_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */
