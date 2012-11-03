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
 * @file	include/touchpad_lld.h
 * @brief	TOUCHPAD Driver subsystem low level driver header.
 *
 * @addtogroup TOUCHPAD
 * @{
 */

#ifndef _TOUCHPAD_LLD_H
#define _TOUCHPAD_LLD_H

#if GFX_USE_TOUCHPAD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Include the low level driver configuration information                    */
/*===========================================================================*/

#include "touchpad_lld_config.h"

/*===========================================================================*/
/* Error checks.                                                             */
/*===========================================================================*/

#ifndef TOUCHPAD_NEED_MULTITHREAD
	#define TOUCHPAD_NEED_MULTITHREAD	FALSE
#endif

#ifndef TOUCHPAD_XY_INVERTED
	#define TOUCHPAD_XY_INVERTED	FALSE
#endif

#ifndef TOUCHPAD_STORE_CALIBRATION
	#define TOUCHPAD_STORE_CALIBRATION FALSE
#endif 

#ifndef TOUCHPAD_HAS_IRQ
	#define TOUCHPAD_HAS_IRQ	FALSE
#endif

#ifndef TOUCHPAD_HAS_PRESSURE
	#define TOUCHPAD_HAS_PRESSURE	FALSE
#endif

#ifndef TOUCHPAD_SPI_PROLOGUE
    #define TOUCHPAD_SPI_PROLOGUE()
#endif

#ifndef TOUCHPAD_SPI_EPILOGUE
    #define TOUCHPAD_SPI_EPILOGUE()
#endif

/*===========================================================================*/
/* Driver types.                                                             */
/*===========================================================================*/

/**
 * @brief	Structure representing a Touchpad driver.
 */
typedef struct _TOUCHPADDriver {
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
    ioportid_t      tpIRQPort;

    /*
     * @brief   Touchscreen controller TPIRQ GPIO pin
     * @note    The interface is polled as of now, interrupt support is
     *          to be implemented in the future.
     */
    ioportmask_t    tpIRQPin;

    /*
     * @brief   Initialize the SPI with the configuration struct given or not
     *          If TRUE, spiStart is called by the init, otherwise not
     * @note    This is provided in such a case when SPI port is being shared
     *          across multiple peripherals, so not to disturb the SPI bus.
     *          You can use TOUCHPAD_SPI_PROLOGUE() and TOUCHPAD_SPI_EPILOGUE()
     *          macros to change the SPI configuration or speed before and
     *          after using the touchpad. An example case would be sharing the
     *          bus with a fast flash memory chip.
     */
    bool_t          direct_init;
} TOUCHPADDriver;


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

	/* Core functions */
	void tp_lld_init(const TOUCHPADDriver *tp);

	uint16_t tp_lld_read_value(uint8_t cmd);
	uint16_t tp_lld_read_x(void);
	uint16_t tp_lld_read_y(void);

	#if TOUCHPAD_HAS_IRQ
	uint8_t tp_lld_irq(void);
	#endif

	#if TOUCHPAD_HAS_PRESSURE
	uint16_t tp_lld_read_z(void);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_TOUCHPAD */

#endif /* _TOUCHPAD_LLD_H */
/** @} */

