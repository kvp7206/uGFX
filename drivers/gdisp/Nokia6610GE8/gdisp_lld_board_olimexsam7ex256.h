/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/Nokia6610GE8/gdisp_lld_board_olimexsam7ex256.h
 * @brief   GDISP Graphic Driver subsystem board interface for the Olimex SAM7-EX256 board.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// ******************************************************
// Pointers to AT91SAM7X256 peripheral data structures
// ******************************************************
static volatile AT91PS_PIO pPIOA = AT91C_BASE_PIOA;
static volatile AT91PS_PIO pPIOB = AT91C_BASE_PIOB;
static volatile AT91PS_SPI pSPI = AT91C_BASE_SPI0;
static volatile AT91PS_PMC pPMC = AT91C_BASE_PMC;
static volatile AT91PS_PDC pPDC = AT91C_BASE_PDC_SPI0;

/* The PWM backlight control is non-linear on this board.
 * We pick values here that make it look a bit more linear.
 */
#define PWM_TOP_VALUE		500
#define PWM_BOTTOM_VALUE	200

#define PWM_VALUE(x)	(PWM_BOTTOM_VALUE+(PWM_TOP_VALUE-PWM_BOTTOM_VALUE)*(x)/100)

/* PWM configuration structure. The LCD Backlight is on PWM1/PB20 ie PWM2/PIN1 in ChibiOS speak */
static const PWMConfig pwmcfg = {
  1000000,		/* 1 MHz PWM clock frequency. Ignored as we are using PWM_MCK_DIV_n */
  1000,			/* PWM period is 1000 cycles. */
  NULL,
  {
   {PWM_MCK_DIV_1 | PWM_OUTPUT_ACTIVE_HIGH | PWM_OUTPUT_PIN1 | PWM_DISABLEPULLUP_PIN1, NULL},
  },
};

static bool_t pwmRunning = FALSE;

/**
 * @brief   Initialise the board for the display.
 * @notes	Performs the following functions:
 *			1. initialise the spi port used by your display
 *			2. initialise the reset pin (initial state not-in-reset)
 *			3. initialise the chip select pin (initial state not-active)
 *			4. initialise the backlight pin (initial state back-light off)
 *
 * @notapi
 */
static inline void init_board(void) {
	// *********************************************************************************************
	// InitSpi( )
	//
	// Sets up SPI channel 0 for communications to Nokia 6610 LCD Display
	//
	// I/O ports used:	PA2  = LCD Reset (set to low to reset)
	//					PA12 = LCD chip select (set to low to select the LCD chip)
	//					PA16 = SPI0_MISO Master In - Slave Out (not used in LCD interface)
	//					PA17 = SPI0_MOSI Master Out - Slave In pin (Serial Data to LCD slave)
	//					PA18 = SPI0_SPCK Serial Clock (to LCD slave)
	//					PB20 = backlight control (normally PWM control, 1 = full on)
	//
	// *********************************************************************************************}

	/* This code should really use the ChibiOS driver for these functions */

	// Pin for backlight
	pPIOB->PIO_CODR   = PIOB_LCD_BL_MASK;    // Set PB20 to LOW
	pPIOB->PIO_OER    = PIOB_LCD_BL_MASK;    // Configure PB20 as output

	// Reset pin
	pPIOA->PIO_SODR   = PIOA_LCD_RESET_MASK;     // Set PA2 to HIGH
	pPIOA->PIO_OER    = PIOA_LCD_RESET_MASK;     // Configure PA2 as output

	// CS pin - this seems to be ignored
	// pPIOA->PIO_SODR   = 1<<12;     // Set PA2 to HIGH
	// pPIOA->PIO_OER    = 1<<12;     // Configure PA2 as output

	// Init SPI0
	// Disable the following pins from PIO control (will be used instead by the SPI0 peripheral)
	// BIT12 = PA12 -> SPI0_NPCS0 chip select
	// BIT16 = PA16 -> SPI0_MISO Master In - Slave Out (not used in LCD interface)
	// BIT17 = PA17 -> SPI0_MOSI Master Out - Slave In pin (Serial Data to LCD slave)
	// BIT18 = PA18 -> SPI0_SPCK Serial Clock (to LCD slave)
	pPIOA->PIO_PDR = (1<<12) | (1<<16) | (1<<17) | (1<<18);
	pPIOA->PIO_ASR = (1<<12) | (1<<16) | (1<<17) | (1<<18);
	pPIOA->PIO_BSR = 0;

	//enable the clock of SPI
	pPMC->PMC_PCER = 1 << AT91C_ID_SPI0;

	// Fixed mode
	pSPI->SPI_CR      = 0x81;               //SPI Enable, Sowtware reset
	pSPI->SPI_CR      = 0x01;               //SPI Enable

	//pSPI->SPI_MR      = 0xE0019;            //Master mode, fixed select, disable decoder, FDIV=1 (MCK), PCS=1110
	pSPI->SPI_MR      = 0xE0011;          //Master mode, fixed select, disable decoder, FDIV=0 (MCK), PCS=1110

	//pSPI->SPI_CSR[0]  = 0x01010C11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*12 = 125kHz
	pSPI->SPI_CSR[0]  = 0x01010311;        //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/8 = 6MHz if using commented MR line above

	/* Display backlight control at 100% */
	pwmRunning = FALSE;
	palSetPad(IOPORT2, PIOB_LCD_BL);
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static inline void setpin_reset(bool_t state) {
	if (state)
		palClearPad(IOPORT1, PIOA_LCD_RESET);
	else
		palSetPad(IOPORT1, PIOA_LCD_RESET);
}

/**
 * @brief   Set the lcd back-light level.
 * @note	For now 0% turns the backlight off, anything else the backlight is on.
 *			While the hardware supports PWM backlight control, we are not using it
 *			yet.
 *
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {
	if (percent == 100) {
		/* Turn the pin on - No PWM */
		if (pwmRunning) {
			pwmStop(&PWMD2);
			pwmRunning = FALSE;
		}
		palSetPad(IOPORT2, PIOB_LCD_BL);
	} else if (percent == 0) {
		/* Turn the pin off - No PWM */
		if (pwmRunning) {
			pwmStop(&PWMD2);
			pwmRunning = FALSE;
		}
		palClearPad(IOPORT2, PIOB_LCD_BL);
	} else {
		/* Use the PWM */
		if (!pwmRunning) {
			pwmStart(&PWMD2, &pwmcfg);
			pwmRunning = TRUE;
		}
		pwmEnableChannel(&PWMD2, 0, PWM_VALUE(percent));
	}
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @notapi
 */
static inline void acquire_bus(void) {
	/* Nothing to do for this board as the LCD is the only device on the SPI port */
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @notapi
 */
static inline void release_bus(void) {
	// Nothing to do for this board as the LCD is the only device on the SPI port
}

/**
 * @brief   Send an 8 bit command to the lcd.
 *
 * @param[in] cmd		The command to send
 *
 * @notapi
 */
static inline void write_cmd(uint16_t cmd) {
	// wait for the previous transfer to complete
	while((pSPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
	// send the command
	pSPI->SPI_TDR = cmd & 0xFF;
}

/**
 * @brief   Send an 8 bit data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static inline void write_data(uint16_t data) {
	// wait for the previous transfer to complete
	while((pSPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
	// send the data
	pSPI->SPI_TDR = data | 0x0100;
}

#if GDISP_HARDWARE_READPIXEL || GDISP_HARDWARE_SCROLL || defined(__DOXYGEN__)
/**
 * @brief   Read data from the lcd.
 *
 * @return	The data from the lcd
 * 
 * @notapi
 */
static inline uint16_t read_data(void) {
	#error "gdispNokia6610GE8: GDISP_HARDWARE_READPIXEL and GDISP_HARDWARE_SCROLL are not supported on this board"
	return 0;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
