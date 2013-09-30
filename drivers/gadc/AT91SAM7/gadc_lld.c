/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gadc/AT91SAM7/gadc_lld.c
 * @brief   GADC - Periodic ADC driver source file for the AT91SAM7 cpu.
 *
 * @defgroup Driver Driver
 * @ingroup GADC
 * @{
 */

#include "gfx.h"

#if GFX_USE_GADC

#include "gadc/lld/gadc_lld.h"

static ADCConversionGroup acg = {
		FALSE,					// circular
		1,						// num_channels
		GADC_ISR_CompleteI,		// end_cb
		GADC_ISR_ErrorI,		// error_cb
		0,						// channelselects
		0,						// trigger
		0,						// frequency
		};

void gadc_lld_init(void) {
	adcStart(&ADCD1, NULL);
}

size_t gadc_lld_samples_per_conversion(uint32_t physdev) {
	size_t	cnt;
	int		i;

	/* The AT91SAM7 has AD0..7 - physdev is a bitmap of those channels */
	for(cnt = 0, i = 0; i < 8; i++, physdev >>= 1)
		if (physdev & 0x01)
			cnt++;
	return cnt;
}

void gadc_lld_start_timer(uint32_t physdev, uint32_t frequency) {
	(void) physdev;
	/**
	 * The AT91SAM7 ADC driver supports triggering the ADC using a timer without having to implement
	 * an interrupt handler for the timer. The driver also initialises the timer correctly for us.
	 * Because we aren't trapping the interrupt ourselves we can't increment GADC_Timer_Missed if an
	 * interrupt is missed.
	 */
	acg.frequency = frequency;
}

void gadc_lld_stop_timer(uint32_t physdev) {
	(void) physdev;
	if ((acg.trigger & ~ADC_TRIGGER_SOFTWARE) == ADC_TRIGGER_TIMER)
		adcStop(&ADCD1);
}

void gadc_lld_adc_timerI(GadcLldTimerData *pgtd) {
	/**
	 *  We don't need to calculate num_channels because the AT91SAM7 ADC does this for us.
	 */
	acg.channelselects = pgtd->physdev;
	acg.trigger = pgtd->now ? (ADC_TRIGGER_TIMER|ADC_TRIGGER_SOFTWARE) : ADC_TRIGGER_TIMER;

	adcStartConversionI(&ADCD1, &acg, pgtd->buffer, pgtd->count);

	/* Next time assume the same (still running) timer */
	acg.frequency = 0;
}

void gadc_lld_adc_nontimerI(GadcLldNonTimerData *pgntd) {
	/**
	 *  We don't need to calculate num_channels because the AT91SAM7 ADC does this for us.
	 */
	acg.channelselects = pgntd->physdev;
	acg.trigger = ADC_TRIGGER_SOFTWARE;
	adcStartConversionI(&ADCD1, &acg, pgntd->buffer, 1);
}

#endif /* GFX_USE_GADC */
/** @} */
