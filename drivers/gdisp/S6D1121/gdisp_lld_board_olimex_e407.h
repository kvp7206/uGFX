/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/S6D1121/gdisp_lld_board_olimex_e407.h
 * @brief   GDISP Graphic Driver subsystem board interface for the S6D1121 display
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define GDISP_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

static inline void init_board(void) {
	int FSMC_Bank = 0;

	/* STM32F4 FSMC init */
	rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

	/* set pins to FSMC mode */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
 						(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
						(1 << 13) | (1 << 14) | (1 << 15), 0};

	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = (6) | (10 << 8) | (10 << 16);

	/* Bank1 NOR/SRAM control register configuration */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;	
}

static inline void setpin_reset(bool_t state) {
	(void)state;

	/* Nothing to do here */
}

static inline void set_backlight(uint8_t percent) {
	(void)percent;

	/* Nothing to do here */
}

static inline void acquire_bus(void) {
	/* Nothing to do here */
}

static inline void release_bus(void) {
	/* Nothing to do here */
}

static inline void write_index(uint16_t index) {
	GDISP_REG = index;
}

static inline void write_data(uint16_t data) {
	GDISP_RAM = data;
}

#if GDISP_HARDWARE_READPIXEL || GDISP_HARDWARE_SCROLL || defined(__DOXYGEN__)
static inline uint16_t read_data(void) {
	return GDISP_RAM;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */

