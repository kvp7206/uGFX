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
 * @file    drivers/touchscreen/MCU/touchscreen_lld.c
 * @brief   Touchscreen Driver subsystem low level driver source.
 *
 * @addtogroup TOUCHSCREEN
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "touchscreen.h"

#if GFX_USE_TOUCHSCREEN /*|| defined(__DOXYGEN__)*/

#define ADC_NUM_CHANNELS   2
#define ADC_BUF_DEPTH      1

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

static const TouchscreenDriver *ts;

static const ADCConversionGroup adc_y_config = {
    FALSE,
    ADC_NUM_CHANNELS,
    NULL,
    NULL,
    0, 0,                       
    0, 0,                      
    ADC_SQR1_NUM_CH(ADC_NUM_CHANNELS),
    0,                        
    ADC_SQR3_SQ2_N(ADC_CHANNEL_IN12) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN13)
};

static const ADCConversionGroup adc_x_config = {
    FALSE,
    ADC_NUM_CHANNELS,
    NULL,
    NULL,
    0, 0,
    0, 0,
    ADC_SQR1_NUM_CH(ADC_NUM_CHANNELS),
    0,
    ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11)
};

/**
 * @brief   Low level Touchscreen driver initialization.
 *
 * @param[in] ts	The touchscreen driver struct
 *
 * @notapi
 */
void ts_lld_init(const TouchscreenDriver *ts_init) {
	ts = ts_init;

	adcStart(ts->adc_driver, NULL);
}

/**
 * @brief   7-point median filtering code for touchscreen samples
 *
 * @note    This is an internally used routine only.
 *
 * @notapi
 */
static void ts_lld_filter(void) {

	return 0;
}

/**
 * @brief   Reads out the X direction.
 *
 * @note    The samples are median filtered for greater noise reduction
 *
 * @notapi
 */
uint16_t ts_lld_read_x(void) {
	uint16_t val1, val2;
	adcsample_t samples[ADC_NUM_CHANNELS * ADC_BUF_DEPTH];

    palSetPadMode(ts->yd_port, ts->yd_pin, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(ts->yu_port, ts->yu_pin, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(ts->xl_port, ts->xl_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(ts->xr_port, ts->xr_pin, PAL_MODE_OUTPUT_PUSHPULL);
    
    palSetPad(ts->xl_port, ts->xl_pin);
    palClearPad(ts->xr_port, ts->xr_pin);
    chThdSleepMilliseconds(1);
    adcConvert(ts->adc_driver, &adc_x_config, samples, ADC_BUF_DEPTH);  
    val1 = ((samples[0] + samples[1])/2);

    palClearPad(ts->xl_port, ts->xl_pin);
    palSetPad(ts->xr_port, ts->xr_pin);
    chThdSleepMilliseconds(1);
    adcConvert(ts->adc_driver, &adc_x_config, samples, ADC_BUF_DEPTH);
    val2 = ((samples[0] + samples[1])/2);
    
	return ((val1+((1<<12)-val2))/4);
}

/**
 * @brief	Reads out the Y direction.
 *
 * @notapi
 */
uint16_t ts_lld_read_y(void) {
	uint16_t val1, val2;
	adcsample_t samples[ADC_NUM_CHANNELS * ADC_BUF_DEPTH];

	palSetPadMode(ts->xl_port, ts->xl_pin, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(ts->xr_port, ts->xr_pin, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(ts->yd_port, ts->yd_pin, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(ts->yu_port, ts->yu_pin, PAL_MODE_OUTPUT_PUSHPULL);
	
	palSetPad(ts->yu_port, ts->yu_pin);
	palClearPad(ts->yd_port, ts->yd_pin);
	chThdSleepMilliseconds(1);
	adcConvert(ts->adc_driver, &adc_y_config, samples, ADC_BUF_DEPTH);	
	val1 = ((samples[0] + samples[1])/2);

	palClearPad(ts->yu_port, ts->yu_pin);
	palSetPad(ts->yd_port, ts->yd_pin);
	chThdSleepMilliseconds(1);
	adcConvert(ts->adc_driver, &adc_y_config, samples, ADC_BUF_DEPTH);
	val2 = ((samples[0] + samples[1])/2);

	return ((val1+((1<<12)-val2))/4);
}

/*
 * @brief	Checks if touchscreen is pressed or not.
 *
 * @return	1 if pressed, 0 otherwise
 *
 * @notapi
 */
uint8_t ts_lld_pressed(void) {
	palSetPadMode(ts->yd_port, ts->yd_pin, PAL_MODE_INPUT_PULLDOWN);
	palSetPadMode(ts->yu_port, ts->yu_pin, PAL_MODE_INPUT);
	palSetPadMode(ts->xl_port, ts->xl_pin, PAL_MODE_INPUT);
	palSetPadMode(ts->xr_port, ts->xr_pin, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(ts->xr_port, ts->xr_pin);

	return palReadPad(ts->yd_port, ts->yd_pin);
}

#endif /* GFX_USE_TOUCHSCREEN */
/** @} */

