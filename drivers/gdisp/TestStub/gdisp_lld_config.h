/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/TestStub/gdisp_lld_config.h
 * @brief   GDISP Graphic Driver subsystem low level driver header (stub).
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

#define GDISP_DRIVER_NAME				"TestStub"

#define GDISP_HARDWARE_SCROLL			GDISP_NEED_SCROLL
#define GDISP_HARDWARE_PIXELREAD		GDISP_NEED_PIXELREAD

#define GDISP_PIXELFORMAT				GDISP_PIXELFORMAT_RGB565
#define GDISP_PACKED_PIXELS				FALSE
#define GDISP_PACKED_LINES				FALSE

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */
