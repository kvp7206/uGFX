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
 * @file    drivers/ginput/touch/MCU/ginput_lld_mouse_config.h
 * @brief   GINPUT LLD header file for touch driver.
 *
 * @addtogroup GINPUT_LLD_MOUSE
 * @{
 */
#ifndef _LLD_GINPUT_MOUSE_CONFIG_H
#define _LLD_GINPUT_MOUSE_CONFIG_H

#define GINPUT_MOUSE_EVENT_TYPE					GEVENT_TOUCH
#define GINPUT_MOUSE_NEED_CALIBRATION			TRUE
#define GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE	FALSE
#define GINPUT_MOUSE_MAX_CALIBRATION_ERROR		10
#define GINPUT_MOUSE_READ_CYCLES				4
#define GINPUT_MOUSE_POLL_PERIOD				100
#define GINPUT_MOUSE_MAX_CLICK_JITTER			4
#define GINPUT_MOUSE_MAX_MOVE_JITTER			4
#define GINPUT_MOUSE_CLICK_TIME					700

#endif /* _LLD_GINPUT_MOUSE_CONFIG_H */
/** @} */
