/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Supported features of the driver for ED060SC4 PrimeView E-Ink panel. */

#ifndef _GDISP_LLD_CONFIG_H
#define _GDISP_LLD_CONFIG_H

#if GFX_USE_GDISP

#define GDISP_DRIVER_NAME               "ED060SC4"
#define GDISP_HARDWARE_CLEARS           TRUE
#define GDISP_HARDWARE_FILLS            FALSE
#define GDISP_HARDWARE_BITFILLS         FALSE
#define GDISP_HARDWARE_SCROLL           FALSE
#define GDISP_HARDWARE_PIXELREAD        FALSE
#define GDISP_HARDWARE_CONTROL          TRUE

#define GDISP_PIXELFORMAT               GDISP_PIXELFORMAT_MONO

#endif

#endif
