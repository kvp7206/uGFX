/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

/**
 * @file    drivers/gdisp/ILI9325/gdisp_lld_board_example.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ILI9325 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

static inline void gdisp_lld_init_board(void) {
	#error "ILI9325: You must implement the init_board routine for your board"
}

static inline void gdisp_lld_reset_pin(bool_t state) {
	#error "ILI9325: You must implement setpin_reset routine for your board"
}

static inline void gdisp_lld_write_index(uint16_t data) {
	#error "ILI9325: You must implement write_index routine for your board"
}

static inline void gdisp_lld_write_data(uint16_t data) {
	#error "ILI9325: You must implement write_data routine for your board"
}

static inline uint16_t gdisp_lld_read_data(void) {
	#error "ILI9325: You must implement read_data routine for your board"
}

/* if not available, just ignore the argument and return */
static inline uint16_t gdisp_lld_backlight(uint8_t percentage) {
	#error "ILI9325: You must implement set_backlight routine for your board"
}

#endif /* GDISP_LLD_BOARD_H */
/** @} */

