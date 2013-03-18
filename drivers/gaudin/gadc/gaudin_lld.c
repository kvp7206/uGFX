/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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
 * @file    drivers/gaudin/gadc/gaudin_lld.c
 * @brief   GAUDIN - Driver file for using the cpu ADC (via GADC).
 *
 * @addtogroup GAUDIN
 *
 * @{
 */
#include "ch.h"
#include "hal.h"

/**
 * We are now implementing the driver - pull in our channel table
 * from the board definitions.
 */
#define GAUDIN_LLD_IMPLEMENTATION


#include "gfx.h"

#if GFX_USE_GAUDIN

/* Double check the GADC system is turned on */
#if !GFX_USE_GADC
	#error "GAUDIN - The GADC driver for GAUDIN requires GFX_USE_GADC to be TRUE"
#endif

/* Include the driver defines */
#include "gaudin/lld/gaudin_lld.h"

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

void gaudin_lld_init(const gaudin_params *paud) {
	/* Setup the high speed GADC */
	gadcHighSpeedInit(gaudin_lld_physdevs[paud->channel], paud->frequency, paud->buffer, paud->bufcount, paud->samplesPerEvent);

	/* Register ourselves for ISR callbacks */
	gadcHighSpeedSetISRCallback(GAUDIN_ISR_CompleteI);

	/**
	 * The gadc driver handles any errors for us by restarting the transaction so there is
	 * no need for us to setup anything for GAUDIN_ISR_ErrorI()
	 */
}

void gadc_lld_start(void) {
	gadcHighSpeedStart();
}

void gadc_lld_stop(void) {
	gadcHighSpeedStop();
}

#endif /* GFX_USE_GAUDIN */
/** @} */
