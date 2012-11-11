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
 * @file	drivers/touchscreen/XPT2046/touchscreen_lld_config.h
 * @brief   Touchscreen Driver subsystem low level driver.
 *
 * @addtogroup TOUCHSCREEN
 * @{
 */

#ifndef TOUCHSCREEN_LLD_CONFIG_H
#define TOUCHSCREEN_LLD_CONFIG_H

#if GFX_USE_TOUCHSCREEN /*|| defined(__DOXYGEN__)*/

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define TOUCHSCREEN_HAS_PRESSED				TRUE
#define TOUCHSCREEN_HAS_PRESSURE			TRUE

struct TouchscreenDriver {
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
};

#endif	/* GFX_USE_TOUCHSCREEN */

#endif	/* TOUCHSCREEN_LLD_CONFIG_H */
/** @} */

