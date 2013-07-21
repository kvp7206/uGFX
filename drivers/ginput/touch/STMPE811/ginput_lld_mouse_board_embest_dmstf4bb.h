/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/ginput/touch/STMPE811/ginput_lld_mouse_board_embest_dmstf4bb.h
 * @brief   GINPUT Touch low level driver source for the STMPE811 on the Embest DM-STF4BB board.
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 * @{
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

static const I2CConfig i2ccfg = {
	OPMODE_I2C,
	400000,
	FAST_DUTY_CYCLE_2,
};

/**
 * @brief   Initialise the board for the touch.
 *
 * @notapi
 */
static void init_board(void)
{
	palSetPadMode(GPIOC, 13, PAL_MODE_INPUT | PAL_STM32_PUDR_FLOATING);			/* TP IRQ */
	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	/* SCL */
	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);	/* SDA */

	i2cStart(&I2CD1, &i2ccfg);
}

/**
 * @brief   Check whether an interrupt is raised
 * @return	TRUE if there is an interrupt signal present
 *
 * @notapi
 */
static inline bool_t getpin_irq(void) {
	return (!(palReadPad(GPIOC, 13)));
}

/**
 * @brief	Write a value into a certain register
 *
 * @param[in] reg	The register address
 * @param[in] n		The amount of bytes (one or two)
 * @param[in] val	The value
 *
 * @notapi
 */
static void write_reg(uint8_t reg, uint8_t n, uint16_t val)
{
	uint8_t txbuf[3];

	i2cAcquireBus(&I2CD1);

	txbuf[0] = reg;

	if (n == 1) {
		txbuf[1] = val;
		i2cMasterTransmitTimeout(&I2CD1, STMPE811_ADDR, txbuf, 2, NULL, 0, MS2ST(STMPE811_TIMEOUT));
	} else if (n == 2) {
		txbuf[1] = ((val & 0xFF00) >> 8);
		txbuf[2] = (val & 0x00FF);
		i2cMasterTransmitTimeout(&I2CD1, STMPE811_ADDR, txbuf, 3, NULL, 0, MS2ST(STMPE811_TIMEOUT));
	}
	
	i2cReleaseBus(&I2CD1);
}
 
/**
 * @brief	Read the value of a certain register
 *
 * @param[in] reg	The register address
 * @param[in] n		The amount of bytes (one or two)
 *
 * @return Data read from device (one byte or two depending on n param)
 *
 * @notapi
 */
static uint16_t read_reg(uint8_t reg, uint8_t n)
{
	uint8_t txbuf[1], rxbuf[2];
	uint16_t ret;

	rxbuf[0] = 0;
	rxbuf[1] = 0;

	i2cAcquireBus(&I2CD1);

	txbuf[0] = reg;
	i2cMasterTransmitTimeout(&I2CD1, STMPE811_ADDR, txbuf, 1, rxbuf, n, MS2ST(STMPE811_TIMEOUT));

	if (n == 1) {
		ret = rxbuf[0];
	} else if (n == 2) {
		ret = ((rxbuf[0] << 8) | (rxbuf[1] & 0xFF));
	}

	i2cReleaseBus(&I2CD1);

	return ret;
}

static void read_reg_n(uint8_t reg, uint8_t n, uint8_t *rxbuf)
{
	uint8_t txbuf[1];

	i2cAcquireBus(&I2CD1);

	txbuf[0] = reg;
	i2cMasterTransmitTimeout(&I2CD1, STMPE811_ADDR, txbuf, 1, rxbuf, n, MS2ST(STMPE811_TIMEOUT));

	i2cReleaseBus(&I2CD1);
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
