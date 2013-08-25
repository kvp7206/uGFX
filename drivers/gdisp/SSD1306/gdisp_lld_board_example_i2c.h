/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define SSD1306_RESET_PORT		GPIOB
#define SSD1306_RESET_PIN		5

/**
 * The default slave address is 0x3D, (talking about
 * only the real address part here) and the slave
 * address can be changed to 0x3C by soldering the
 * SA0 pads on the bottom side of the module.
 *
 * b7 | b6 | b5 | b4 | b3 | b2 | b1 | b0
 * --------------------------------------
 * 0  | 1  | 1  | 1  | 1  | 0  |SA0 | R/W
 */
#define SSD1306_I2C_ADDRESS   	0x3D
#define SSD1306_SDA_PORT		GPIOB
#define SSD1306_SDA_PIN			7
#define SSD1306_SCL_PORT		GPIOB
#define SSD1306_SCL_PIN			6
#define SET_RST					palSetPad(SSD1306_RESET_PORT, SSD1306_RESET_PIN);
#define CLR_RST					palClearPad(SSD1306_RESET_PORT, SSD1306_RESET_PIN);
int8_t vccstate;
int32_t row_offset = 0;

// I2C configuration structure.
static I2CConfig i2cconfig;


/**
 * @brief   Initialize the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 * @notapi
 */
static inline void init_board(void) {

	// RESET pin.
	palSetPadMode(SSD1306_RESET_PORT, SSD1306_RESET_PIN, PAL_MODE_OUTPUT_PUSHPULL);


	/*
	 * Initializes the I2C driver 1. The I2C1 signals are routed as follows:
	 * PB6 - SCL.
	 * PB7 - SDA.
	 * Timing value comes from ST I2C config tool (xls):
	 * 0x00901D2B;		// 100kHz Standard Mode
	 * 0x00300444;		// 100kHz Fast Mode
	 * 0x0030020A;		// 400kHz Fast Mode
	 * 0x00100002;		// 800kHz Fast Mode +
	 */
	i2cconfig.timingr = 0x00100002;		// 800kHz Fast Mode+
	i2cInit();
	palSetPadMode(SSD1306_SCL_PORT, SSD1306_SCL_PIN, PAL_MODE_ALTERNATE(1));
	palSetPadMode(SSD1306_SDA_PORT, SSD1306_SDA_PIN, PAL_MODE_ALTERNATE(1));
	vccstate = SSD1306_SWITCHCAPVCC;
}

/**
 * @brief   Set or clear the lcd reset pin.
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * @notapi
 */
static inline void setpin_reset(bool_t state) {
	if(state)
		SET_RST
	else
		CLR_RST
}

/**
 * @brief   Set the lcd back-light level.
 * @param[in] percent		0 to 100%
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {
	// Since we are on OLED no backlight needed
}

/**
 * @brief   Take exclusive control of the bus
 * @notapi
 */
static inline void acquire_bus(void) {
		i2cAcquireBus(&I2CD1);
}

/**
 * @brief   Release exclusive control of the bus
 * @notapi
 */
static inline void release_bus(void) {
		i2cReleaseBus(&I2CD1);
}

/**
 * @brief   Send command to the display.
 * @param[in] cmd	The command to send *
 * @notapi
 */
static inline void write_cmd(uint8_t cmd) {
	uint8_t command[] = { 0x00, 		// Co = 0, D/C = 0
						  cmd 		},
						  txLength = sizeof(command)/sizeof(command[0]),
						  rxLength = 0;
	i2cStart(&I2CD1, &i2cconfig);
	i2cMasterTransmitTimeout(&I2CD1, SSD1306_I2C_ADDRESS, command, txLength, NULL, rxLength, MS2ST(10));
	i2cStop(&I2CD1);
}

/**
 * @brief   Send data to the display.
 * @param[in] data	The data to send
 * @notapi
 */
static inline void write_data(uint8_t* data, uint16_t length) {
	uint8_t command[length+1],
			txLength = length+1,
			rxLength = 0;
	command[0] = 0x40; 		// Co = 0, D/C = 1
	memmove(&command[1], data, length);

	i2cStart(&I2CD1, &i2cconfig);
	i2cMasterTransmitTimeout(&I2CD1, SSD1306_I2C_ADDRESS, command, txLength, NULL, rxLength, MS2ST(10));
	i2cStop(&I2CD1);
}

#endif /* _GDISP_LLD_BOARD_H */
/** @} */

