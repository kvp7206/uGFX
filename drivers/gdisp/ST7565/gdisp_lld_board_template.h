/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/**
 * @brief   Initialize the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 * @notapi
 */
static inline void init_board(void) {
  /* Configure SPI bus here. Up to 10Mhz, SPI Mode = 3 (CPOL = 1, CPHA = 0) */
  /* Configure A0, !CS, !RST pin for output */
}

/**
 * @brief   Set or clear the lcd reset pin.
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * @notapi
 */
static inline void setpin_reset(bool_t state) {
  /* Set !CS to low */
  if (state) {
    /* Set !RST to low */
  } else {
    /* Set !RST to high */
  }
}

/**
 * @brief   Set the lcd back-light level.
 * @param[in] percent		0 to 100%
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {

}

/**
 * @brief   Take exclusive control of the bus
 * @notapi
 */
static inline void acquire_bus(void) {

}

/**
 * @brief   Release exclusive control of the bus
 * @notapi
 */
static inline void release_bus(void) {

}

/**
 * @brief   Send command to the display.
 * @param[in] cmd	The command to send
 * @notapi
 */
static inline void write_cmd(uint8_t cmd) {
  /* Set A0 to low */
  /* Transmit cmd over SPI */
}

/**
 * @brief   Send data to the display.
 * @param[in] data	The data to send
 * @notapi
 */
static inline void write_data(uint8_t* data, uint16_t length) {
  /* Set A0 to high */
  /* Transmit data for length over SPI. DMA is recommended. */
}

#endif /* _GDISP_LLD_BOARD_H */
/** @} */

