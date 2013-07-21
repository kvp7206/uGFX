/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/HX8347D/gdisp_lld_board_st_stm32f4_discovery.h
 * @brief   GDISP Graphic Driver subsystem board SPI interface for the HX8347D display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define SET_RST		palSetPad(GPIOB, 8);
#define CLR_RST		palClearPad(GPIOB, 8);

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

/*
 * SPI1 configuration structure.
 * Speed 42MHz, CPHA=0, CPOL=0, 8bits frames, MSb transmitted first.
 * The slave select line is the pin 4 on the port GPIOA.
 */
static const SPIConfig spi1cfg1 = {
  NULL,
  /* HW dependent part.*/
  GPIOA,
  4,
  0 //SPI_CR1_BR_0
};

/*
 * SPI1 configuration structure.
 * Speed 42MHz, CPHA=0, CPOL=0, 16bits frames, MSb transmitted first.
 * The slave select line is the pin 4 on the port GPIOA.
 */
static const SPIConfig spi1cfg2 = {
  NULL,
  /* HW dependent part.*/
  GPIOA,
  4,
  SPI_CR1_DFF //SPI_CR1_BR_0
};

/**
 * @brief   Initialise the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 *
 * @notapi
 */
static inline void init_board(void) {

	/* Display backlight control */
	/* TIM4 is an alternate function 2 (AF2) */
	pwmStart(&PWMD4, &pwmcfg);
	palSetPadMode(GPIOD, 13, PAL_MODE_ALTERNATE(2));
	pwmEnableChannel(&PWMD4, 1, 100);

	palSetPadMode(GPIOB,  8, PAL_MODE_OUTPUT_PUSHPULL |
	                               PAL_STM32_OSPEED_HIGHEST);           /* RST    */
	palSetPadMode(GPIOB,  9, PAL_MODE_OUTPUT_PUSHPULL |
	                               PAL_STM32_OSPEED_HIGHEST);           /* RS     */
	/*
	   * Initializes the SPI driver 1. The SPI1 signals are routed as follow:
	   * PB12 - NSS.
	   * PB13 - SCK.
	   * PB14 - MISO.
	   * PB15 - MOSI.
	   */
	  spiStart(&SPID1, &spi1cfg1);
	  palSetPad(GPIOA, 4);
	  palSetPadMode(GPIOA,  4, PAL_MODE_OUTPUT_PUSHPULL |
	                           PAL_STM32_OSPEED_HIGHEST);           /* NSS.     */
	  palSetPadMode(GPIOA,  5, PAL_MODE_ALTERNATE(5) |
	                           PAL_STM32_OSPEED_HIGHEST);           /* SCK.     */
	  palSetPadMode(GPIOA,  6, PAL_MODE_ALTERNATE(5));              /* MISO.    */
	  palSetPadMode(GPIOA,  7, PAL_MODE_ALTERNATE(5) |
	                           PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */

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
	spiAcquireBus(&SPID1);
}

/**
 * @brief   Release exclusive control of the bus
 * @note	Not needed, not implemented
 *
 * @notapi
 */
static inline void release_bus(void) {
	spiReleaseBus(&SPID1);
}

/**
 * @brief   Send data to the index register.
 *
 * @param[in] index		The index register to set
 *
 * @notapi
 */
static inline void write_index(uint8_t cmd) {
    palClearPad(GPIOB, 9);
    palClearPad(GPIOA, 4);
    while((SPI1->SR & SPI_SR_TXE) == 0);
    SPI1->DR = cmd;
    while(((SPI1->SR & SPI_SR_TXE) == 0) || ((SPI1->SR & SPI_SR_BSY) != 0));
    palSetPad(GPIOB, 9);
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 *
 * @notapi
 */
static inline void write_data(uint8_t data) {
    SPI1->DR = data;
    while(((SPI1->SR & SPI_SR_TXE) == 0) || ((SPI1->SR & SPI_SR_BSY) != 0));
    palSetPad(GPIOA, 4);
}

static inline void write_ram8(uint8_t data1, uint8_t data2) {
    SPI1->DR = data1;
    while((SPI1->SR & SPI_SR_TXE) == 0);
    SPI1->DR = data2;
    while(((SPI1->SR & SPI_SR_TXE) == 0) || ((SPI1->SR & SPI_SR_BSY) != 0));
    palSetPad(GPIOA, 4);
}

static inline void write_ram16(uint16_t data) {
    while((SPI1->SR & SPI_SR_TXE) == 0);
    SPI1->DR      = data;
}

#if GDISP_HARDWARE_READPIXEL || defined(__DOXYGEN__)
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
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
