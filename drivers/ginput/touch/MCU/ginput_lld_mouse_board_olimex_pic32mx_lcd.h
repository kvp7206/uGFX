/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

/*
    ChibiOS/GFX - Copyright (C) 2013
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

static const ADCConfig ADCC = {
  .vref = ADC_VREF_CFG_AVDD_AVSS,
  .stime = 15,
  .irq = EIC_IRQ_ADC,
  .base = _ADC10_BASE_ADDRESS,
};
static struct ADCDriver ADCD;

#define YNEG 13 // U
#define XNEG 15 // R
#define XPOS 12 // L
#define YPOS 11 // D

#define ADC_MAX 1023

#define TOUCH_THRESHOULD 50

static const ADCConversionGroup ADC_X_CG = {
  .circular = FALSE,
  .num_channels = 1,
  .channels = 1 << XNEG,
};

static const ADCConversionGroup ADC_Y_CG = {
  .circular = FALSE,
  .num_channels = 1,
  .channels = 1 << YPOS,
};

/**
 * @brief   Initialise the board for the touch.
 *
 * @notapi
 */
static inline void init_board(void) {
  adcObjectInit(&ADCD);
  adcStart(&ADCD, &ADCC);
}

/**
 * @brief   Check whether the surface is currently touched
 * @return	TRUE if the surface is currently touched
 *
 * @notapi
 */
static inline bool_t getpin_pressed(void) {
  adcsample_t samples[2] = {0, };

  // Set X+ to ground
  palSetPadMode(IOPORTB, XPOS, PAL_MODE_OUTPUT);
  palClearPad(IOPORTB, XPOS);

  // Set Y- to VCC
  palSetPadMode(IOPORTB, YNEG, PAL_MODE_OUTPUT);
  palSetPad(IOPORTB, YNEG);

  palSetPadMode(IOPORTB, XNEG, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(IOPORTB, YPOS, PAL_MODE_INPUT_ANALOG);

  adcConvert(&ADCD, &ADC_X_CG, &samples[0], 1);
  adcConvert(&ADCD, &ADC_Y_CG, &samples[1], 1);

  return (ADC_MAX - (samples[1] - samples[0])) > TOUCH_THRESHOULD;
}

/**
 * @brief   Aquire the bus ready for readings
 *
 * @notapi
 */
static inline void aquire_bus(void) {
}

/**
 * @brief   Release the bus after readings
 *
 * @notapi
 */
static inline void release_bus(void) {
}

/**
 * @brief   Read an x value from touch controller
 * @return	The value read from the controller
 *
 * @notapi
 */
static inline uint16_t read_x_value(void) {
  adcsample_t sample;

  palSetPadMode(IOPORTB, XPOS, PAL_MODE_OUTPUT);
  palSetPad(IOPORTB, XPOS);

  palSetPadMode(IOPORTB, XNEG, PAL_MODE_OUTPUT);
  palClearPad(IOPORTB, XNEG);

  palSetPadMode(IOPORTB, YNEG, PAL_MODE_INPUT);

  palSetPadMode(IOPORTB, YPOS, PAL_MODE_INPUT_ANALOG);

  adcConvert(&ADCD, &ADC_Y_CG, &sample, 1);

  return ADC_MAX - sample;
}

/**
 * @brief   Read an y value from touch controller
 * @return	The value read from the controller
 *
 * @notapi
 */
static inline uint16_t read_y_value(void) {
  adcsample_t sample;

  palSetPadMode(IOPORTB, YNEG, PAL_MODE_OUTPUT);
  palClearPad(IOPORTB, YNEG);

  palSetPadMode(IOPORTB, YPOS, PAL_MODE_OUTPUT);
  palSetPad(IOPORTB, YPOS);

  palSetPadMode(IOPORTB, XPOS, PAL_MODE_INPUT);

  palSetPadMode(IOPORTB, XNEG, PAL_MODE_INPUT_ANALOG);

  adcConvert(&ADCD, &ADC_X_CG, &sample, 1);

  return ADC_MAX - sample;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
