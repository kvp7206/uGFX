/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/ILI9320/gdisp_lld_board_olimex_stm32_lcd.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ILI9320 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

#define GDISP_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint16_t *) 0x60100000)) /* RS = 1 */

static inline void gdisp_lld_init_board(void) {
	/* FSMC setup for F1 */
	rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

    /* set pin modes */
    IOBus busD = {GPIOD, PAL_WHOLE_PORT, 0}; 
    IOBus busE = {GPIOE, PAL_WHOLE_PORT, 0}; 
    palSetBusMode(&busD, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetBusMode(&busE, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOE, GPIOE_TFT_RST, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, GPIOD_TFT_LIGHT, PAL_MODE_OUTPUT_PUSHPULL);

    const unsigned char FSMC_Bank = 0;

    /* FSMC timing */
    FSMC_Bank1->BTCR[FSMC_Bank+1] = (6) | (10 << 8) | (10 << 16);

    /* Bank1 NOR/SRAM control register configuration
     * This is actually not needed as already set by default after reset */
    FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
}

static inline void gdisp_lld_reset_pin(bool_t state) {
	if(state)
		palClearPad(GPIOE, GPIOE_TFT_RST);
	else
		palSetPad(GPIOE, GPIOE_TFT_RST);
}

static inline void acquire_bus(void) {
			/* Nothing to do here since LCD is the only device on that bus */
}

static inline void release_bus(void) {
			/* Nothing to do here since LCD is the only device on that bus */
}

static inline void gdisp_lld_write_index(uint16_t reg) {
	GDISP_REG = reg;
}

static inline void gdisp_lld_write_data(uint16_t data) {
	GDISP_RAM = data;
}

static inline uint16_t gdisp_lld_read_data(void) {
	return GDISP_RAM;
}

static inline void gdisp_lld_backlight(uint8_t percent) {
	if(percent == 100)
		palClearPad(GPIOD, GPIOD_TFT_LIGHT);
	else
		palSetPad(GPIOD, GPIOD_TFT_LIGHT);
}

#endif /* GDISP_LLD_BOARD_H */
/** @} */

