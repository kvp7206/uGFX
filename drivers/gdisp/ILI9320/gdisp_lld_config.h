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
 * @file    drivers/gdisp/ILI9320/gdisp_lld_config.h
 * @brief   GDISP Graphic Driver subsystem low level driver header for the ILI9320 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef GDISP_LLD_CONFIG_H
#define GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_DRIVER_NAME				"ILI9320"
#define GDISP_LLD(x)					gdisp_lld_##x##_ILI9320

#define GDISP_HARDWARE_CLEARS			TRUE
#define GDISP_HARDWARE_FILLS			TRUE
#define GDISP_HARDWARE_BITFILLS			FALSE
#define GDISP_HARDWARE_SCROLL			FALSE
#define GDISP_HARDWARE_PIXELREAD		TRUE
#define GDISP_HARDWARE_CONTROL			TRUE

#define GDISP_PIXELFORMAT				GDISP_PIXELFORMAT_RGB565

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */

