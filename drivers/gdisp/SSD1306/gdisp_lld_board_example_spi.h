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
#define SSD1306_MISO_PORT		GPIOB
#define SSD1306_MISO_PIN		8
#define SSD1306_MOSI_PORT		GPIOB
#define SSD1306_MOSI_PIN		7
#define SSD1306_SCK_PORT		GPIOB
#define SSD1306_SCK_PIN			6
#define SSD1306_CS_PORT			GPIOB
#define SSD1306_CS_PIN			5
#define SET_RST					palSetPad(SSD1306_RESET_PORT, SSD1306_RESET_PIN);
#define CLR_RST					palClearPad(SSD1306_RESET_PORT, SSD1306_RESET_PIN);
int8_t vccstate;
int32_t row_offset = 0;

/*
 * SPI1 configuration structure.
 * Speed 42MHz, CPHA=0, CPOL=0, 8bits frames, MSb transmitted first.
 * The slave select line is the pin 4 on the port GPIOA.
 */
static const SPIConfig spi1config = {
	NULL,
	/* HW dependent part.*/
	SSD1306_MISO_PORT,
	SSD1306_MISO_PIN,
	0
	//SPI_CR1_BR_0
};

/**
 * @brief   Initialize the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 * @notapi
 */
static inline void init_board(void) {

	// RESET pin.
	palSetPadMode(SSD1306_RESET_PORT, SSD1306_RESET_PIN, PAL_MODE_OUTPUT_PUSHPULL);

	spiInit();
	palSetPadMode(SSD1306_MISO_PORT, SSD1306_MISO_PIN, 	PAL_MODE_ALTERNATE(1)|
														PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(SSD1306_MOSI_PORT, SSD1306_MOSI_PIN, 	PAL_MODE_ALTERNATE(1)|
														PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(SSD1306_SCK_PORT,  SSD1306_SCK_PIN,  	PAL_MODE_ALTERNATE(1)|
														PAL_STM32_OSPEED_HIGHEST);
	palSetPad(SSD1306_CS_PORT, SSD1306_CS_PIN);
	palSetPadMode(SSD1306_CS_PORT,   SSD1306_CS_PIN,   	PAL_MODE_ALTERNATE(1)|
														PAL_STM32_OSPEED_HIGHEST);
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
	spiAcquireBus(&SPID1);
}

/**
 * @brief   Release exclusive control of the bus
 * @notapi
 */
static inline void release_bus(void) {
	spiReleaseBus(&SPID1);
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

	spiStart(&SPID1, &spi1config);
	spiSelect(&SPID1);
	spiStartSend(&SPID1, txLength, command);
	spiUnselect(&SPID1);
	spiStop(&SPID1);
}

/**
 * @brief   Send data to the display.
 * @param[in] data	The data to send
 * @notapi
 */
static inline void write_data(uint8_t* data, uint16_t length) {
	uint8_t command[length+1],
			txLength = length+1;
	command[0] = 0x40; 		// Co = 0, D/C = 1
	memmove(&command[1], data, length);

	spiStart(&SPID1, &spi1config);
	spiSelect(&SPID1);
	spiStartSend(&SPID1, txLength, command);
	spiUnselect(&SPID1);
	spiStop(&SPID1);
}


#endif /* _GDISP_LLD_BOARD_H */
/** @} */

