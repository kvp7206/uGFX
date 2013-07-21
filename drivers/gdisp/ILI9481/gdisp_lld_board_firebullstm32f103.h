/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/ILI9481/gdisp_lld_board_firebullstm32f103.h
 * @brief   GDISP Graphics Driver subsystem low level driver source for
 * 			the ILI9481 and compatible HVGA display
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define SET_CS		palSetPad(GPIOD, 12);
#define CLR_CS		palClearPad(GPIOD, 12);
#define SET_RS		palSetPad(GPIOD, 13);
#define CLR_RS		palClearPad(GPIOD, 13);
#define SET_WR		palSetPad(GPIOD, 14);
#define CLR_WR		palClearPad(GPIOD, 14);
#define SET_RD		palSetPad(GPIOD, 15);
#define CLR_RD		palClearPad(GPIOD, 15);

/**
 * @brief   Initialise the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 *
 * @notapi
 */
static inline void init_board(void) {
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 13, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, 15, PAL_MODE_OUTPUT_PUSHPULL);
	
	// Configure the pins to a well know state
	SET_RS;
	SET_RD;
	SET_WR;
	CLR_CS;
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
	/* Nothing to do here - reset pin tied to Vcc */
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {
	(void) percent;
	/* Nothing to do here - Backlight always on */
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @notapi
 */
static inline void acquire_bus(void) {
	/* Nothing to do here since LCD is the only device on that bus */
}

/**
 * @brief   Release exclusive control of the bus
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
	palWritePort(GPIOE, index);
	CLR_RS; CLR_WR; SET_WR; SET_RS;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static inline void write_data(uint16_t data) {
	palWritePort(GPIOE, data);
	CLR_WR; SET_WR;
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
	uint16_t	value;
	
	// change pin mode to digital input
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_INPUT);

	CLR_RD;
	value = palReadPort(GPIOE);
	value = palReadPort(GPIOE);
	SET_RD;

	// change pin mode back to digital output
	palSetGroupMode(GPIOE, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
	
	return value;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
