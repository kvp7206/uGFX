/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/RA8875/gdisp_lld_board_marlin.h
 * @brief   GDISP Graphic Driver subsystem board interface for the RA8875 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/* Using FSMC A16 as RS */
#define GDISP_RAM              (*((volatile uint16_t *) 0x68000000)) /* RS = 0 */
#define GDISP_REG              (*((volatile uint16_t *) 0x68020000)) /* RS = 1 */

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

/**
 * @brief   Read data from the lcd.
 *
 * @return  The data from the lcd
 * @note The chip select may need to be asserted/de-asserted
 *          around the actual spi read
 *
 * @notapi
 */
static inline uint16_t read_data(void) {
	return GDISP_RAM;
}

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
	const unsigned char FSMC_Bank = 4;
	
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
			if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, NULL, NULL)) chSysHalt();
			dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
			dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);  
		#endif
	#else
		#error "FSMC not implemented for this device"
	#endif

	/* set pins to FSMC mode */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 8) |
							(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
						(1 << 13) | (1 << 14) | (1 << 15), 0};

   IOBus busG = {GPIOG, (1 << 10), 0};

	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));
   palSetBusMode(&busG, PAL_MODE_ALTERNATE(12));
	
	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3) \
			| (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3) \
			| (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3) ;

	/* Bank1 NOR/SRAM control register configuration
	 * This is actually not needed as already set by default after reset */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
}

static inline void post_init_board(void) {
	const unsigned char FSMC_Bank = 2;
	/* FSMC delay reduced as the controller now runs at full speed */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_BUSTURN_0 ;
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
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
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {

	//duty_cycle is 00..FF
	//Work in progress: the RA8875 has a built-in PWM, its output can
	//be used by a Dynamic Background Control or by a host (user)

   uint8_t temp, temp1;
   // Enable PWM1
   write_index(0x8a);//MCLR
   temp = read_data();
   temp |= 1<<7 ;
   write_data(temp);

   // PWM1 function select
   write_index(0x8a);//MCLR
   temp = read_data();
   temp &= ~(1<<4);
   write_data(temp);

   // PWM1 Clock ratio
   temp1= 0x0b&0x0f;
   write_index(0x8a);//MCLR
   temp = read_data();
   temp &= 0xf0;
   temp |= temp1 ;
   write_data(temp);

   // Write duty cycle
   write_index(0x8b);//PTNO
   write_data(percent);
   // PWM1 duty cycle
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

#endif /* _GDISP_LLD_BOARD_H */
/** @} */

