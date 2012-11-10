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
 * @file	include/touchscreen_lld.h
 * @brief	TOUCHSCREEN Driver subsystem low level driver header.
 *
 * @addtogroup TOUCHSCREEN
 * @{
 */

#ifndef TOUCHSCREEN_LLD_H
#define TOUCHSCREEN_LLD_H

#if GFX_USE_TOUCHSCREEN || defined(__DOXYGEN__)

/*===========================================================================*/
/* Include the low level driver configuration information                    */
/*===========================================================================*/

#include "touchscreen_lld_config.h"

/*===========================================================================*/
/* Error checks.                                                             */
/*===========================================================================*/

#ifndef TOUCHSCREEN_NEED_MULTITHREAD
	#define TOUCHSCREEN_NEED_MULTITHREAD	FALSE
#endif

#ifndef TOUCHSCREEN_XY_INVERTED
	#define TOUCHSCREEN_XY_INVERTED	FALSE
#endif

#ifndef TOUCHSCREEN_HAS_IRQ
	#define TOUCHSCREEN_HAS_IRQ	FALSE
#endif

#ifndef TOUCHSCREEN_HAS_PRESSURE
	#define TOUCHSCREEN_HAS_PRESSURE	FALSE
#endif

#ifndef TOUCHSCREEN_SPI_PROLOGUE
    #define TOUCHSCREEN_SPI_PROLOGUE()
#endif

#ifndef TOUCHSCREEN_SPI_EPILOGUE
    #define TOUCHSCREEN_SPI_EPILOGUE()
#endif

/*===========================================================================*/
/* Driver types.                                                             */
/*===========================================================================*/

/**
 * @brief	Structure representing a touchscreen driver.
 */
typedef struct TouchscreenDriver {
    /*
     * @brief   Pointer to SPI driver.
     * @note    SPI driver must be enabled in mcuconf.h and halconf.h
     */
    SPIDriver       *spip;

    /*
     * @brief   Pointer to the SPI configuration structure.
     * @note    The lowest possible speed ~ 1-2MHz is to be used, otherwise
     *          will result in a lot of noise
     */
    const SPIConfig  *spicfg;

    /*
     * @brief   Touchscreen controller TPIRQ pin GPIO port
     */
    ioportid_t      tsIRQPort;

    /*
     * @brief   Touchscreen controller TPIRQ GPIO pin
     * @note    The interface is polled as of now, interrupt support is
     *          to be implemented in the future.
     */
    ioportmask_t    tsIRQPin;

    /*
     * @brief   Initialize the SPI with the configuration struct given or not
     *          If TRUE, spiStart is called by the init, otherwise not
     * @note    This is provided in such a case when SPI port is being shared
     *          across multiple peripherals, so not to disturb the SPI bus.
     *          You can use TOUCHSCREEN_SPI_PROLOGUE() and TOUCHSCREEN_SPI_EPILOGUE()
     *          macros to change the SPI configuration or speed before and
     *          after using the touchpad. An example case would be sharing the
     *          bus with a fast flash memory chip.
     */
    bool_t          direct_init;
} TouchscreenDriver;


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

	/* Core functions */
	void ts_lld_init(const TouchscreenDriver *ts);

	uint16_t ts_lld_read_value(uint8_t cmd);
	uint16_t ts_lld_read_x(void);
	uint16_t ts_lld_read_y(void);

	#if TOUCHSCREEN_HAS_IRQ
	uint8_t ts_lld_pressed(void);
	#endif

	#if TOUCHSCREEN_HAS_PRESSURE
	uint16_t ts_lld_read_z(void);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_TOUCHSCREEN */

#endif /* _TOUCHSCREEN_LLD_H */
/** @} */

