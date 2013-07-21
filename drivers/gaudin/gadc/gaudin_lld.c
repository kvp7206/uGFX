/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gaudin/gadc/gaudin_lld.c
 * @brief   GAUDIN - Driver file for using the cpu ADC (via GADC).
 *
 * @addtogroup GAUDIN
 *
 * @{
 */

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
