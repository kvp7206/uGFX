/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    drivers/gdisp/SSD1963/gdisp_lld_board_example_gpio.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1963 display.
 *
 * @brief	This file demonstrates how to interface an SSD1963 based display using
 *			a GPIO interface of an STM32 out of ChibiOS/RT.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/**
 * @brief	The GPIO pin config
 *
 * @details	This block of defines tell your driver how you wired your display
 *			controller to your MCU. Please change them accordingly.
 */
#define GDISP_CMD_PORT		GPIOC
#define GDISP_DATA_PORT		GPIOD
#define GDISP_CS			0
#define GDISP_RS			1
#define GDISP_WR			2
#define GDISP_RD			3

#define Set_CS		palSetPad(GDISP_CMD_PORT, GDISP_CS);
#define Clr_CS		palClearPad(GDISP_CMD_PORT, GDISP_CS);
#define Set_RS		palSetPad(GDISP_CMD_PORT, GDISP_RS);
#define Clr_RS		palClearPad(GDISP_CMD_PORT, GDISP_RS);
#define Set_WR		palSetPad(GDISP_CMD_PORT, GDISP_WR);
#define Clr_WR		palClearPad(GDISP_CMD_PORT, GDISP_WR);
#define Set_RD		palSetPad(GDISP_CMD_PORT, GDISP_RD);
#define Clr_RD		palClearPad(GDISP_CMD_PORT, GDISP_RD);

/**
 * @brief   Send data to the index register.
 *
 * @param[in] index		The index register to set
 *
 * @notapi
 */
static inline void write_index(uint16_t index) {
	Set_CS; Set_RS; Set_WR; Clr_RD;

	palWritePort(GDISP_DATA_PORT, index);

	Clr_CS;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static inline void write_data(uint16_t data) {
	Set_CS; Clr_RS; Set_WR; Clr_RD;

	palWritePort(GDISP_DATA_PORT, data);

	Clr_CS;
}

/**
 * @brief   Initialise the board for the display.
 *
 * @notapi
 */
static inline void init_board(void) {
	IOBus busCMD = {GDISP_CMD_PORT, (1 << GDISP_CS) | (1 << GDISP_RS) | (1 << GDISP_WR) | (1 << GDISP_RD), 0};
	IOBus busDATA = {GDISP_CMD_PORT, 0xFFFFF, 0};
	palSetBusMode(&busCMD, PAL_MODE_OUTPUT_PUSHPULL);
	palSetBusMode(&busDATA, PAL_MODE_OUTPUT_PUSHPULL);
}

static inline void post_init_board(void) {
	/* Nothing to do here */
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static inline void setpin_reset(bool_t state) {
	/* optional */
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
	//Work in progress: the SSD1963 has a built-in PWM, its output can
	//be used by a Dynamic Background Control or by a host (user)
	//Check your LCD's hardware, the PWM connection is default left open and instead
	//connected to a LED connection on the breakout board
	write_index(SSD1963_SET_PWM_CONF);//set PWM for BackLight
	write_data(0x0001);
	write_data(percent & 0x00FF);
	write_data(0x0001);//controlled by host (not DBC), enabled
	write_data(0x00FF);
	write_data(0x0060);//don't let it go too dark, avoid a useless LCD
	write_data(0x000F);//prescaler ???
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

__inline void write_stream(uint16_t *buffer, uint16_t size) {
	uint16_t i;

	Set_CS; Clr_RS; Set_WR; Clr_RD;

	for (i = 0; i < size; i++) {
		Set_WR;
		palWritePort(GDISP_DATA_PORT, buffer[i]);
		Clr_WR;
	}

	Clr_CS;
}

__inline void read_stream(uint16_t *buffer, size_t size) {
	uint16_t i;
	
	Set_CS; Clr_RS; Clr_WR; Set_RD;
	
	for (i = 0; i < size; i++) {
		Set_RD;
		buffer[i] = palReadPort(GDISP_DATA_PORT);
		Clr_RD;
	}
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
	uint16_t data;

	Set_CS; Clr_RS; Clr_WR; Set_RD;
	
	data = palReadPort(GDISP_DATA_PORT); 

	Clr_CS;

	return data;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
