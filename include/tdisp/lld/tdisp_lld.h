/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/tdisp/lld/tdisp_lld.h
 * @brief   TDISP driver subsystem low level driver header.
 *
 * @addtogroup TDISP
 * @{
 */

#ifndef _TDISP_LLD_H
#define _TDISP_LLD_H

#if GFX_USE_TDISP || defined(__DOXYGEN__)

#ifdef __cplusplus
extern "C" {
#endif

	bool_t	tdisp_lld_init(void);
	void	tdisp_lld_clear(void);
	void	tdisp_lld_draw_char(char c);
	void 	tdisp_lld_set_cursor(coord_t col, coord_t row);
	void	tdisp_lld_create_char(uint8_t address, uint8_t *charmap);
	void	tdisp_lld_control(uint16_t what, uint16_t value);
	void	tdisp_lld_scroll(uint16_t direction, uint16_t amount, uint16_t delay);
	void	tdisp_lld_set_backlight(uint16_t percentage);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_TDISP */

#endif /* _TDISP_LLD_H */
/** @} */

