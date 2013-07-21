/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/multiple/Win32/gdisp_lld_config.h
 * @brief   GDISP Graphic Driver subsystem low level driver header for Win32.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_DRIVER_NAME				"Win32"

#define GDISP_HARDWARE_LINES			TRUE
#define GDISP_HARDWARE_FILLS			TRUE
#define GDISP_HARDWARE_BITFILLS			TRUE
#define GDISP_HARDWARE_SCROLL			TRUE
#define GDISP_HARDWARE_PIXELREAD		TRUE
#define GDISP_HARDWARE_CONTROL			TRUE

#define GDISP_PIXELFORMAT				GDISP_PIXELFORMAT_RGB888

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */

