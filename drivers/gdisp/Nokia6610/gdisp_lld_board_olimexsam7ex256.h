/*
    ChibiOS/RT - Copyright (C) 2012
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
 * @file    gdispNokia6610/gdisp_lld_board_olimexsam7ex256.h
 * @brief   GDISP Graphic Driver subsystem board interface for the Olimex SAM7-EX256 board.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

// mask definitions
#define BIT0 0x00000001
#define BIT1 0x00000002
#define BIT2 0x00000004
#define BIT3 0x00000008
#define BIT4 0x00000010
#define BIT5 0x00000020
#define BIT6 0x00000040
#define BIT7 0x00000080
#define BIT8 0x00000100
#define BIT9 0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000

// ******************************************************
// Pointers to AT91SAM7X256 peripheral data structures
// ******************************************************
volatile AT91PS_PIO pPIOA = AT91C_BASE_PIOA;
volatile AT91PS_PIO pPIOB = AT91C_BASE_PIOB;
volatile AT91PS_SPI pSPI = AT91C_BASE_SPI0;
volatile AT91PS_PMC pPMC = AT91C_BASE_PMC;
volatile AT91PS_PDC pPDC = AT91C_BASE_PDC_SPI0;

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
static __inline void GDISP_LLD(init_board)(void) {
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
	pPIOB->PIO_CODR   = PIOB_GDISP_BL_MASK;    // Set PB20 to LOW
	pPIOB->PIO_OER    = PIOB_GDISP_BL_MASK;    // Configure PB20 as output

	// Reset pin
	pPIOA->PIO_SODR   = PIOA_GDISP_RESET_MASK;     // Set PA2 to HIGH
	pPIOA->PIO_OER    = PIOA_GDISP_RESET_MASK;     // Configure PA2 as output

	// CS pin - this seems to be ignored
	// pPIOA->PIO_SODR   = BIT12;     // Set PA2 to HIGH
	// pPIOA->PIO_OER    = BIT12;     // Configure PA2 as output

	// Init SPI0
	// Disable the following pins from PIO control (will be used instead by the SPI0 peripheral)
	// BIT12 = PA12 -> SPI0_NPCS0 chip select
	// BIT16 = PA16 -> SPI0_MISO Master In - Slave Out (not used in LCD interface)
	// BIT17 = PA17 -> SPI0_MOSI Master Out - Slave In pin (Serial Data to LCD slave)
	// BIT18 = PA18 -> SPI0_SPCK Serial Clock (to LCD slave)
	pPIOA->PIO_PDR = BIT12 | BIT16 | BIT17 | BIT18;
	pPIOA->PIO_ASR = BIT12 | BIT16 | BIT17 | BIT18;
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
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static __inline void GDISP_LLD(setpin_reset)(bool_t state) {
	if (state)
		palClearPad(IOPORT1, PIOA_GDISP_RESET);
//		pPIOA->PIO_CODR = PIOA_GDISP_RESET_MASK;
	else
		palSetPad(IOPORT1, PIOA_GDISP_RESET);
//		pPIOA->PIO_SODR = PIOA_GDISP_RESET_MASK;
}

/**
 * @brief   Set or clear the lcd back-light pin.
 *
 * @param[in] state		TRUE = lcd back-light on, FALSE = lcd back-light off
 * 
 * @notapi
 */
static __inline void GDISP_LLD(setpin_backlight)(bool_t state) {
	if (state)
		palSetPad(IOPORT2, PIOB_GDISP_BL);
//		pPIOB->PIO_SODR = PIOB_GDISP_BL_MASK;
	else
		palClearPad(IOPORT2, PIOB_GDISP_BL);
//		pPIOB->PIO_CODR = PIOB_GDISP_BL_MASK;
}

/**
 * @brief   Send an 8 bit command to the lcd.
 *
 * @param[in] data		The command to send
 *
 * @notapi
 */
static __inline void GDISP_LLD(write_cmd)(uint16_t data) {
	// wait for the previous transfer to complete
	while((pSPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
	// send the command
	pSPI->SPI_TDR = data & 0xFF;
}

/**
 * @brief   Send an 8 bit data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static __inline void GDISP_LLD(write_data)(uint16_t data) {
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
static __inline uint16_t GDISP_LLD(read_data)(void) {
	#error "gdispNokia6610: GDISP_HARDWARE_READPIXEL and GDISP_HARDWARE_SCROLL are not supported on this board"
	return 0;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
