/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/SSD2119/gdisp_lld_board_template.h
 * @brief   GDISP Graphic Driver subsystem board template for the SSD2119 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

/**
 * @brief   Initialise the board for the display.
 * @notes   This board definition uses GPIO and assumes exclusive access to these GPIO pins
 *
 * @notapi
 */
static inline void init_board(void) {

}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 *
 * @notapi
 */
static inline void setpin_reset(bool_t state) {

}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] percent		0 to 100%
 *
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {

}

/**
 * @brief   Take exclusive control of the bus
 * @note	Not needed, not implemented
 *
 * @notapi
 */
static inline void acquire_bus(void) {
	/* Nothing to do here since LCD is the only device on that bus */
}

/**
 * @brief   Release exclusive control of the bus
 * @note	Not needed, not implemented
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

}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 *
 * @notapi
 */
static inline void write_data(uint16_t data) {

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

}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
