/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_DRIVER_NAME         "ST7565"

#define GDISP_HARDWARE_CONTROL    TRUE

#define GDISP_PIXELFORMAT         GDISP_PIXELFORMAT_MONO

#define GDISP_CONTROL_LLD_FLUSH   (GDISP_CONTROL_LLD + 1)

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
/** @} */

