/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    include/gwin/internal.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @addtogroup GWIN
 * @{
 */
#ifndef _GWIN_INTERNAL_H
#define _GWIN_INTERNAL_H

#if GFX_USE_GWIN || defined(__DOXYGEN__)

/*===========================================================================*/
/* Sub-system constants.													 */
/*===========================================================================*/

#define GWIN_FLG_DYNAMIC				0x0001
#define GBTN_FLG_ALLOCTXT				0x0002
#define GWIN_FIRST_CONTROL_FLAG			0x0004

#ifdef __cplusplus
extern "C" {
#endif

GHandle _gwinInit(GWindowObject *gw, coord_t x, coord_t y, coord_t width, coord_t height, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GWIN */

#endif /* _GWIN_INTERNAL_H */
/** @} */
