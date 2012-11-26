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
 * @file    gwin_internal.h
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
#define GWIN_FIRST_CONTROL_FLAG			0x0002
#define GBTN_FLG_ALLOCTXT				(GWIN_FIRST_CONTROL_FLAG<<0)

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
