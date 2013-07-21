/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gtimer/options.h
 * @brief   GTIMER sub-system options header file.
 *
 * @addtogroup GTIMER
 * @{
 */

#ifndef _GTIMER_OPTIONS_H
#define _GTIMER_OPTIONS_H

/**
 * @name    GTIMER Functionality to be included
 * @{
 */
/**
 * @}
 *
 * @name    GTIMER Optional Sizing Parameters
 * @{
 */
	/**
	 * @brief   Defines the size of the timer threads work area (stack+structures).
	 * @details	Defaults to 512 bytes
	 */
	#ifndef GTIMER_THREAD_WORKAREA_SIZE
		#define GTIMER_THREAD_WORKAREA_SIZE		512
	#endif
/** @} */

#endif /* _GTIMER_OPTIONS_H */
/** @} */
