/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/multiple/X/gdisp_lld_config.h
 * @brief   GDISP Graphic Driver subsystem low level driver header for the X11 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_DRIVER_NAME			"Linux emulator - X11"

#define GDISP_HARDWARE_CLEARS			FALSE
#define GDISP_HARDWARE_FILLS			TRUE
#define GDISP_HARDWARE_BITFILLS			FALSE
#define GDISP_HARDWARE_SCROLL			FALSE
#define GDISP_HARDWARE_PIXELREAD		FALSE
#define GDISP_HARDWARE_CONTROL			FALSE
#define GDISP_HARDWARE_CIRCLES			FALSE
#define GDISP_HARDWARE_CIRCLEFILLS		FALSE
#define GDISP_HARDWARE_ARCS				FALSE
#define GDISP_HARDWARE_ARCFILLS			FALSE

#define GDISP_PIXELFORMAT			GDISP_PIXELFORMAT_RGB888

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */

