/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gaudin/lld/gaudin_lld.h
 * @brief   GAUDIN - Audio Input driver header file.
 *
 * @defgroup Driver Driver
 * @ingroup GAUDIN
 * @{
 */

#ifndef _GAUDIN_LLD_H
#define _GAUDIN_LLD_H

#include "gfx.h"

#if GFX_USE_GADC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief				The structure passed to start a audio conversion
 * @note				We use the structure instead of parameters purely to save
 * 						interrupt stack space which is very limited in some platforms.
 * @{
 */
typedef struct gaudin_params_t {
	uint16_t		channel;
	uint32_t		frequency;
	audin_sample_t	*buffer;
	size_t			bufcount;
	size_t			samplesPerEvent;
	} gaudin_params;
/** @} */

/**
 * @brief				These routines are the callbacks that the driver uses.
 * @details				Defined in the high level GAUDIN code.
 *
 * @iclass
 * @notapi
 *
 * @{
 */

/**
 * @param[in] buffer	The buffer
 * @param[in] n			The amount of samples
 * */
extern void GAUDIN_ISR_CompleteI(audin_sample_t *buffer, size_t n);

extern void GAUDIN_ISR_ErrorI(void);
/**
 * @}
 */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Initialise the driver
 *
 * @param[in] paud		Initialisation parameters
 *
 * @api
 */
void gaudin_lld_init(const gaudin_params *paud);

/**
 * @brief				Start the audio input sampling
 *
 * @api
 */
void gadc_lld_start(void);

/**
 * @brief				Stop the audio input sampling
 *
 * @api
 */
void gadc_lld_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GADC */

#endif /* _GADC_LLD_H */
/** @} */
