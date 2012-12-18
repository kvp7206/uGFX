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
 * @file    drivers/ginput/touch/MCU/ginput_lld_mouse_board_olimex_stm32_lcd.h
 * @brief   GINPUT Touch low level driver source for the MCU on the example board.
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 *
 * @{
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#define ADC_NUM_CHANNELS   2
#define ADC_BUF_DEPTH      1

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
 * @brief   Initialise the board for the touch.
 *
 * @notapi
 */
static __inline void init_board(void) {
	adcStart(&ADCD1, NULL);
}

/**
 * @brief   Check whether the surface is currently touched
 * @return	TRUE if the surface is currently touched
 *
 * @notapi
 */
static __inline bool_t getpin_pressed(void) {
    palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOC, 1, PAL_MODE_INPUT);
    palSetPadMode(GPIOC, 2, PAL_MODE_INPUT);
    palSetPadMode(GPIOC, 3, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOC, 3);
	
	return palReadPad(GPIOC, 0);
}

/**
 * @brief   Aquire the bus ready for readings
 *
 * @notapi
 */
static __inline void aquire_bus(void) {

}

/**
 * @brief   Release the bus after readings
 *
 * @notapi
 */
static __inline void release_bus(void) {

}

/**
 * @brief   Read an x value from touch controller
 * @return	The value read from the controller
 *
 * @notapi
 */
static __inline uint16_t read_x_value(void) {
	uint16_t val1, val2;
    adcsample_t samples[ADC_NUM_CHANNELS * ADC_BUF_DEPTH];

    palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOC, 1, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOC, 2, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOC, 3, PAL_MODE_OUTPUT_PUSHPULL);
    
	palSetPad(GPIOC, 2);
    palClearPad(GPIOC, 3); 
    chThdSleepMilliseconds(1);
    adcConvert(&ADCD1, &adc_x_config, samples, ADC_BUF_DEPTH);  
    val1 = ((samples[0] + samples[1])/2);
    
	palClearPad(GPIOC, 2);
    palSetPad(GPIOC, 3);
    chThdSleepMilliseconds(1);
    adcConvert(&ADCD1, &adc_x_config, samples, ADC_BUF_DEPTH);
    val2 = ((samples[0] + samples[1])/2);
    
	return ((val1+((1<<12)-val2))/4);
}

/**
 * @brief   Read an y value from touch controller
 * @return	The value read from the controller
 *
 * @notapi
 */
static __inline uint16_t read_y_value(void) {
	uint16_t val1, val2;
    adcsample_t samples[ADC_NUM_CHANNELS * ADC_BUF_DEPTH];

    palSetPadMode(GPIOC, 2, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOC, 3, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOC, 0, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOC, 1, PAL_MODE_OUTPUT_PUSHPULL);

    palSetPad(GPIOC, 1);
    palClearPad(GPIOC, 0);
    chThdSleepMilliseconds(1);
    adcConvert(&ADCD1, &adc_y_config, samples, ADC_BUF_DEPTH);
    val1 = ((samples[0] + samples[1])/2);

    palClearPad(GPIOC, 1);
    palSetPad(GPIOC, 0);
    chThdSleepMilliseconds(1);
    adcConvert(&ADCD1, &adc_y_config, samples, ADC_BUF_DEPTH);
    val2 = ((samples[0] + samples[1])/2);
    
	return ((val1+((1<<12)-val2))/4);
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
