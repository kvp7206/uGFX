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
 * @file    drivers/touchpad/XPT2046/touchpad_lld.c
 * @brief   Touchpad Driver subsystem low level driver source.
 *
 * @addtogroup TOUCHPAD
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "touchpad.h"

#if GFX_USE_TOUCHPAD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
#if !defined(__DOXYGEN__)
    /* Local copy of the current touchpad driver */
    static const TOUCHPADDriver *tpDriver;

    static uint16_t sampleBuf[7];
#endif

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */

/**
 * @brief   Low level Touchpad driver initialization.
 *
 * @param[in] tp	The touchpad driver struct
 *
 * @notapi
 */
void tp_lld_init(const TOUCHPADDriver *tp) {
	tpDriver = tp;

	if(tpDriver->direct_init)
		spiStart(tpDriver->spip, tpDriver->spicfg);
}


/**
 * @brief   Reads a conversion from the touchpad
 *
 * @param[in] cmd    The command bits to send to the touchpad
 *
 * @return  The read value 12-bit right-justified
 *
 * @note    This function only reads data, it is assumed that the pins are
 *          configured properly and the bus has been acquired beforehand
 *
 * @notapi
 */
uint16_t tp_lld_read_value(uint8_t cmd) {
	static uint8_t txbuf[3] = {0};
	static uint8_t rxbuf[3] = {0};
	uint16_t ret;

	txbuf[0] = cmd;

	spiExchange(tpDriver->spip, 3, txbuf, rxbuf);

	ret = (rxbuf[1] << 5) | (rxbuf[2] >> 3);

	return ret;
}

/**
 * @brief   7-point median filtering code for touchpad samples
 *
 * @note    This is an internally used routine only.
 *
 * @notapi
 */
static void tp_lld_filter(void) {
	uint16_t temp;
	int i,j;

	for(i = 0; i < 4; i++) {
		for(j=i; j < 7; j++) {
			if(sampleBuf[i] > sampleBuf[j]) {
				/* Swap the values */
				temp = sampleBuf[i];
				sampleBuf[i] = sampleBuf[j];
				sampleBuf[j] = temp;
			}
		}
	}
}

/**
 * @brief   Reads out the X direction.
 *
 * @note    The samples are median filtered for greater noise reduction
 *
 * @notapi
 */
uint16_t tp_lld_read_x(void) {
	int i;

#if defined(SPI_USE_MUTUAL_EXCLUSION)
	spiAcquireBus(tpDriver->spip);
#endif

	TOUCHPAD_SPI_PROLOGUE();
	palClearPad(tpDriver->spicfg->ssport, tpDriver->spicfg->sspad);

	/* Discard the first conversion - very noisy and keep the ADC on hereafter
	 * till we are done with the sampling. Note that PENIRQ is disabled.
 	 */
	tp_lld_read_value(0xD1);

	for(i = 0; i < 7; i++) {
		sampleBuf[i]=tp_lld_read_value(0xD1);
	}

	/* Switch on PENIRQ once again - perform a dummy read */
	tp_lld_read_value(0xD0);

	palSetPad(tpDriver->spicfg->ssport, tpDriver->spicfg->sspad);
	TOUCHPAD_SPI_EPILOGUE();

#if defined(SPI_USE_MUTUAL_EXCLUSION)
	spiReleaseBus(tpDriver->spip);
#endif

	/* Find the median - use selection sort */
	tp_lld_filter();

	return sampleBuf[3];
}

/*
 * @brief	Reads out the Y direction.
 *
 * @notapi
 */
uint16_t tp_lld_read_y(void) {
	int i;

#if defined(SPI_USE_MUTUAL_EXCLUSION)
	spiAcquireBus(tpDriver->spip);
#endif

	TOUCHPAD_SPI_PROLOGUE();
	palClearPad(tpDriver->spicfg->ssport, tpDriver->spicfg->sspad);

	/* Discard the first conversion - very noisy and keep the ADC on hereafter
	 * till we are done with the sampling. Note that PENIRQ is disabled.
	 */
	tp_lld_read_value(0x91);

	for(i = 0; i < 7; i++) {
		sampleBuf[i] = tp_lld_read_value(0x91);
	}

	/* Switch on PENIRQ once again - perform a dummy read */
	tp_lld_read_value(0x90);

	palSetPad(tpDriver->spicfg->ssport, tpDriver->spicfg->sspad);
	TOUCHPAD_SPI_EPILOGUE();

#ifdef SPI_USE_MUTUAL_EXCLUSION
	spiReleaseBus(tpDriver->spip);
#endif

	/* Find the median - use selection sort */
	tp_lld_filter();

	return sampleBuf[3];
}

/* ---- Optional Routines ---- */
#if TOUCHPAD_HAS_IRQ || defined(__DOXYGEN__)
	/*
	 * @brief	for checking if touchpad is pressed or not.
	 *
	 * @return	1 if pressed / 0 if not pressed
	 *
	 * @notapi
	 */
	uint8_t tp_lld_irq(void) {
		return (!palReadPad(tpDriver->tpIRQPort, tpDriver->tpIRQPin));
	}
#endif

#if TOUCHPAD_HAS_PRESSURE || defined(__DOXYGEN__)
	/*
	 * @brief	Reads out the Z direction / pressure.
	 *
	 * @notapi
	 */
	uint16_t tp_lld_read_z(void) {
		/* ToDo */
		return 42;
	}
#endif

#endif /* GFX_USE_TOUCHPAD */
/** @} */

