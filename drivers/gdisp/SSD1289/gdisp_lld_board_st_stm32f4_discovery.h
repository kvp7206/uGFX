/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/SSD1289/gdisp_lld_board_st_stm32f4_discovery.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1289 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define GDISP_REG              ((volatile uint16_t *) 0x60000000)[0] /* RS = 0 */
#define GDISP_RAM              ((volatile uint16_t *) 0x60020000)[0] /* RS = 1 */

#define GDISP_USE_FSMC
#define GDISP_USE_DMA
#define GDISP_DMA_STREAM STM32_DMA2_STREAM6

const unsigned char FSMC_Bank = 0;

/* PWM configuration structure. We use timer 3 channel 3 */
static const PWMConfig pwmcfg = {
  100000,       /* 100 kHz PWM clock frequency. */
  100,          /* PWM period is 100 cycles. */
  NULL,
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0
};

/**
 * @brief   Initialise the board for the display.
 * @notes	Performs the following functions:
 *			1. initialise the io port used by your display
 *			2. initialise the reset pin (initial state not-in-reset)
 *			3. initialise the chip select pin (initial state not-active)
 *			4. initialise the backlight pin (initial state back-light off)
 *
 * @notapi
 */
static inline void init_board(void) {

	#if defined(STM32F1XX) || defined(STM32F3XX)
		/* FSMC setup for F1/F3 */
		rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

		#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			#error "DMA not implemented for F1/F3 Devices"
		#endif
	#elif defined(STM32F4XX) || defined(STM32F2XX)
		/* STM32F2-F4 FSMC init */
		rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

		#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
			if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, NULL, NULL)) gfxExit();
			dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
		#endif
	#else
		#error "FSMC not implemented for this device"
	#endif

	/* set pins to FSMC mode */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
							(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
						(1 << 13) | (1 << 14) | (1 << 15), 0};

	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;

	/* Bank1 NOR/SRAM control register configuration
	 * This is actually not needed as already set by default after reset */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;

	/* Display backlight control */
	/* TIM3 is an alternate function 2 (AF2) */
	pwmStart(&PWMD3, &pwmcfg);
	palSetPadMode(GPIOB, 0, PAL_MODE_ALTERNATE(2));
	pwmEnableChannel(&PWMD3, 2, 100);

}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static inline void setpin_reset(bool_t state) {
	(void) state;
	/* Nothing to do here */
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] percent       0 to 100%
 *
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {
    pwmEnableChannel(&PWMD3, 2, percent);
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @notapi
 */
static inline void acquire_bus(void) {
	/* Nothing to do here */
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @notapi
 */
static inline void release_bus(void) {
	/* Nothing to do here */
}

/**
 * @brief   Send data to the index register.
 *
 * @param[in] index		The index register to set
 *
 * @notapi
 */
static inline void write_index(uint16_t index) { GDISP_REG = index; }

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static inline void write_data(uint16_t data) { GDISP_RAM = data; }

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
static inline uint16_t read_data(void) { return GDISP_RAM; }
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
