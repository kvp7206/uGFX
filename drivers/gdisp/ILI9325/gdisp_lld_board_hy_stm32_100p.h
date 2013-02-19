/*
    ChibiOS/GFX - Copyright (C) 2012
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

/*
	driver quickly hacked together from a chinese sourcecode that came
	with the board and existing ili9320 code by Chris van Dongen (sjaak)
	(sjaak2002 at msn.com)
	
	Also added rotation for 180 and 270 degrees and minor tweaks to
	setcursor
	
	Added code comes without warranty and free bugs. Feel free to use 
	or misuse the added code :D
*/


/**
 * @file    drivers/gdisp/ILI9325/gdisp_lld_board_hy_stm32_100p.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ILI9325 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

#define GDISP_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

static __inline void gdisp_lld_init_board(void) {
	/* FSMC setup for F1 */
	rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

    /* set pin modes */
/*	IOBus busD = {GPIOD, PAL_WHOLE_PORT, 0}; 
    IOBus busE = {GPIOE, PAL_WHOLE_PORT, 0}; 
    palSetBusMode(&busD, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetBusMode(&busE, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(GPIOE, GPIOE_TFT_RST, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOD, GPIOD_TFT_LIGHT, PAL_MODE_OUTPUT_PUSHPULL); */

    const unsigned char FSMC_Bank = 0;

    /* FSMC timing */
    FSMC_Bank1->BTCR[FSMC_Bank+1] = (6) | (10 << 8) | (10 << 16);

    /* Bank1 NOR/SRAM control register configuration
     * This is actually not needed as already set by default after reset */
    FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
	
}

static __inline void gdisp_lld_reset_pin(bool_t state) {
	if(state)
		palClearPad(GPIOE, GPIOE_TFT_RST);
	else
		palSetPad(GPIOE, GPIOE_TFT_RST);
}

static __inline void gdisp_lld_write_index(uint16_t reg) {
	GDISP_REG = reg;
}

static __inline void gdisp_lld_write_data(uint16_t data) {
	GDISP_RAM = data;
}

static __inline uint16_t gdisp_lld_read_data(void) {
	return GDISP_RAM;
}

static __inline void gdisp_lld_backlight(uint8_t percent) {
	percent=percent;	// avoid a warning
}

#endif /* GDISP_LLD_BOARD_H */
/** @} */

