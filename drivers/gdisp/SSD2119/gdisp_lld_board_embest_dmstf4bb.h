/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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

/* This board file uses only FSMC, so don't undefine this. */
#define GDISP_USE_FSMC
/* But it is OK to disable DMA use. */
#define GDISP_USE_DMA
#define GDISP_DMA_STREAM STM32_DMA2_STREAM6

/* Using FSMC A19 (PE3) as DC */
#define GDISP_REG (*((volatile uint16_t *) 0x60000000)) /* DC = 0 */
#define GDISP_RAM (*((volatile uint16_t *) 0x60100000)) /* DC = 1 */

#define SET_RST palSetPad(GPIOD, 3);
#define CLR_RST palClearPad(GPIOD, 3);

/*
 * PWM configuration structure. We use timer 4 channel 2 (orange LED on board).
 * The reason for so high clock is that with any lower, onboard coil is squeaking.
 * The major disadvantage of this clock is a lack of linearity between PWM duty
 * cycle width and brightness. In fact only with low preset one sees any change
 * (eg. duty cycle between 1-20). Feel free to adjust this, maybe only my board
 * behaves like this. According to the G5126 datesheet (backlight LED driver)
 * the PWM frequency should be somewhere between 200 Hz to 200 kHz.
 */
static const PWMConfig pwmcfg = {
	1000000,       /* 1 MHz PWM clock frequency. */
	100,           /* PWM period is 100 cycles. */
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
 * @notes   This board definition uses GPIO and assumes exclusive access to these GPIO pins
 *
 * @notapi
 */
static inline void init_board(void) {
	unsigned char FSMC_Bank;

	#ifndef GDISP_USE_FSMC
		#error "This board uses only FSMC, please define GDISP_USE_FSMC"
	#endif

	#if defined(STM32F4XX) || defined(STM32F2XX)
		/* STM32F4 FSMC init */
		rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

		#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, NULL, NULL))
				gfxExit();
			dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
		#endif
	#else
		#error "FSMC not implemented for this device"
	#endif

	/* Group pins */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
							(1 << 9) | (1 << 10) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 3) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
							(1 << 13) | (1 << 14) | (1 << 15), 0};

	/* FSMC is an alternate function 12 (AF12) */
	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

	FSMC_Bank = 0;
	/* FSMC timing register configuration */
	FSMC_Bank1->BTCR[FSMC_Bank + 1] = (FSMC_BTR1_ADDSET_2 | FSMC_BTR1_ADDSET_1) \
			| (FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_1) \
			| FSMC_BTR1_BUSTURN_0;

	/* Bank1 NOR/PSRAM control register configuration
	 * Write enable, memory databus width set to 16 bit, memory bank enable */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_WREN | FSMC_BCR1_MWID_0 | FSMC_BCR1_MBKEN;

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
