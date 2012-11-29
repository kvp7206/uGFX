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
 * @file    drivers/multiple/Win32/ginput_lld_mouse_config.h
 * @brief   GINPUT LLD header file for mouse/touch driver.
 *
 * @addtogroup GINPUT_LLD_MOUSE
 * @{
 */
#ifndef _LLD_GINPUT_MOUSE_CONFIG_H
#define _LLD_GINPUT_MOUSE_CONFIG_H

// This driver supports being both a mouse or a touch device (we don't actually know which it really is)
//	When operating in mouse mode a long left button click does not generate a context click.
//	When operating in touch mode we allow sloppier clicks etc
#if GINPUT_NEED_MOUSE
	#define GINPUT_MOUSE_EVENT_TYPE					GEVENT_MOUSE
	#define GINPUT_MOUSE_CLICK_TIME					TIME_INFINITE			// Long click != Context Click
	#define GINPUT_MOUSE_NEED_CALIBRATION			FALSE
	#define GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE	FALSE
	#define GINPUT_MOUSE_READ_CYCLES				1
	#define GINPUT_MOUSE_MAX_CALIBRATION_ERROR		-1
	#define GINPUT_MOUSE_MAX_CLICK_JITTER			0
	#define GINPUT_MOUSE_MAX_MOVE_JITTER			0
#else
	#define GINPUT_MOUSE_EVENT_TYPE					GEVENT_TOUCH
	#define GINPUT_MOUSE_CLICK_TIME					700						// Long click = Context Click
	#define GINPUT_MOUSE_NEED_CALIBRATION			FALSE					// Can be set to TRUE just for testing
	#define GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE	FALSE
	#define GINPUT_MOUSE_READ_CYCLES				1
	#define GINPUT_MOUSE_MAX_CALIBRATION_ERROR		2
	#define GINPUT_MOUSE_MAX_CLICK_JITTER			2
	#define GINPUT_MOUSE_MAX_MOVE_JITTER			2
#endif

// This driver supports both an "interrupt" mode, and a polled mode
#define GINPUT_MOUSE_POLL_PERIOD				TIME_INFINITE			// Interrupt driven by the Window thread
//#define GINPUT_MOUSE_POLL_PERIOD				25						// Poll driven

#endif /* _LLD_GINPUT_MOUSE_CONFIG_H */
/** @} */
