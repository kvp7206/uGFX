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
 * @file    drivers/gdisp/SSD2119/gdisp_lld_board_embest_dmstf4bb.h
 * @brief   GDISP Graphic Driver subsystem board FSMC interface for the SSD2119 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/* Using FSMC A19 (PE3) as DC */
#define GDISP_REG	(*((volatile uint16_t *) 0x60000000)) /* DC = 0 */
#define GDISP_RAM	(*((volatile uint16_t *) 0x60100000)) /* DC = 1 */

#define SET_RST		palSetPad(GPIOD, 3);
#define CLR_RST		palClearPad(GPIOD, 3);

/* PWM configuration structure. We use timer 4 channel 2 (orange LED on board). */
static const PWMConfig pwmcfg = {
  1000000,		/* 1 MHz PWM clock frequency. */
  100,			/* PWM period is 100 cycles. */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0
};

/**
 * @brief   Initialise the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 *
 * @notapi
 */
static inline void init_board(void) {
	unsigned char FSMC_Bank;

	/* STM32F4 FSMC init */
	rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

	/* Group pins */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
							(1 << 9) | (1 << 10) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 3) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
							(1 << 13) | (1 << 14) | (1 << 15), 0};

	/* FSMC is an alternate function 12 (AF12) */
	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

	FSMC_Bank = 0;

	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
			| (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
			| (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

	/* Bank1 NOR/SRAM control register configuration
	 * This is actually not needed as already set by default after reset */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;

	/* Display backlight control */
	/* TIM4 is an alternate function 2 (AF2) */
	pwmStart(&PWMD4, &pwmcfg);
	palSetPadMode(GPIOD, 13, PAL_MODE_ALTERNATE(2));
	pwmEnableChannel(&PWMD4, 1, 100);
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 *
 * @notapi
 */
static inline void setpin_reset(bool_t state) {
	if (state) {
		CLR_RST;
	} else {
		SET_RST;
	}
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] percent		0 to 100%
 *
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {
	pwmEnableChannel(&PWMD4, 1, percent);
}

/**
 * @brief   Take exclusive control of the bus
 * @note	Not needed, not implemented
 *
 * @notapi
 */
static inline void acquire_bus(void) {
	/* Nothing to do here since LCD is the only device on that bus */
}

/**
 * @brief   Release exclusive control of the bus
 * @note	Not needed, not implemented
 *
 * @notapi
 */
static inline void release_bus(void) {
	/* Nothing to do here since LCD is the only device on that bus */
}

/**
 * @brief   Send data to the index register.
 *
 * @param[in] index		The index register to set
 *
 * @notapi
 */
static inline void write_index(uint16_t index) {
	GDISP_REG = index;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 *
 * @notapi
 */
static inline void write_data(uint16_t data) {
	GDISP_RAM = data;
}

#if GDISP_HARDWARE_READPIXEL || GDISP_HARDWARE_SCROLL || defined(__DOXYGEN__)
/**
 * @brief   Read data from the lcd.
 *
 * @return	The data from the lcd
 * @note	The chip select may need to be asserted/de-asserted
 * 			around the actual spi read
 *
 * @notapi
 */
static inline uint16_t read_data(void) {
	return GDISP_RAM;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
