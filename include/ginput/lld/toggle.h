/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

#ifndef _LLD_GINPUT_TOGGLE_H
#define _LLD_GINPUT_TOGGLE_H

#if GINPUT_NEED_TOGGLE || defined(__DOXYGEN__)

// Describes how the toggle bits are obtained
typedef struct GToggleConfig_t {
	void		*id;
	unsigned	mask;
	unsigned	invert;
	iomode_t	mode;
} GToggleConfig;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	extern const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES];
	
	void ginput_lld_toggle_init(const GToggleConfig *ptc);
	unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc);

	/* This routine is provided to low level drivers to wakeup a value read from a thread context.
	 *	Particularly useful if GINPUT_TOGGLE_POLL_PERIOD = TIME_INFINITE
	 */
	void ginputToggleWakeup(void);

	/* This routine is provided to low level drivers to wakeup a value read from an ISR
	 *	Particularly useful if GINPUT_TOGGLE_POLL_PERIOD = TIME_INFINITE
	 */
	void ginputToggleWakeupI(void);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GINPUT && GINPUT_NEED_TOGGLE */

#endif /* _LLD_GINPUT_TOGGLE_H */
/** @} */

