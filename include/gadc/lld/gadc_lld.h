/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gadc/lld/gadc_lld.h
 * @brief   GADC - Periodic ADC driver header file.
 *
 * @defgroup Driver Driver
 * @ingroup GADC
 * @{
 */

#ifndef _GADC_LLD_H
#define _GADC_LLD_H

#include "gfx.h"

#if GFX_USE_GADC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief				The structure passed to start a timer conversion
 * @note				We use the structure instead of parameters purely to save
 * 						interrupt stack space which is very limited in some platforms.
 * @{
 */
typedef struct GadcLldTimerData_t {
	uint32_t		physdev;		/* @< A value passed to describe which physical ADC devices/channels to use. */
	adcsample_t		*buffer;		/* @< The static buffer to put the ADC samples into. */
	size_t			count;			/* @< The number of conversions to do before doing a callback and stopping the ADC. */
	bool_t			now;			/* @< Trigger the first conversion now rather than waiting for the first timer interrupt (if possible) */
	} GadcLldTimerData;
/* @} */

/**
 * @brief				The structure passed to start a non-timer conversion
 * @note				We use the structure instead of parameters purely to save
 * 						interrupt stack space which is very limited in some platforms.
 * @{
 */
typedef struct GadcLldNonTimerData_t {
	uint32_t		physdev;		/* @< A value passed to describe which physical ADC devices/channels to use. */
	adcsample_t		*buffer;		/* @< The static buffer to put the ADC samples into. */
	} GadcLldNonTimerData;
/* @} */

/**
 * @brief				These routines are the callbacks that the driver uses.
 * @details				Defined in the high level GADC code.
 *
 * @notapi
 * @{
 */

/**
 * @param[in] adcp		The ADC driver
 * @param[in] buffer	The sample buffer
 * @param[in] n			The amount of samples
 */
extern void GADC_ISR_CompleteI(ADCDriver *adcp, adcsample_t *buffer, size_t n);

/**
 * @param[in] adcp 		The ADC driver
 * @param[in] err 		ADC error
 */
extern void GADC_ISR_ErrorI(ADCDriver *adcp, adcerror_t err);
/**
 * @}
 */

/**
 * @brief				This can be incremented by the low level driver if a timer interrupt is missed.
 * @details				Defined in the high level GADC code.
 *
 * @notapi
 */
extern volatile bool_t GADC_Timer_Missed;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Initialise the driver
 *
 * @api
 */
void gadc_lld_init(void);

/**
 * @brief				Get the number of samples in a conversion.
 * @details				Calculates and returns the number of samples per conversion for the specified physdev.
 *
 * @note				A physdev describing a mono device would return 1, a stereo device would return 2.
 * 						For most ADC's physdev is a bitmap so it is only a matter of counting the bits.
 *
 * @param[in] physdev	A value passed to describe which physical ADC devices/channels to use.
 *
 * @return				Number of samples of the convesion
 * @api
 */
size_t gadc_lld_samples_per_conversion(uint32_t physdev);

/**
 * @brief				Start a periodic timer for high frequency conversions.
 *
 * @param[in] physdev		A value passed to describe which physical ADC devices/channels to use.
 * @param[in] frequency		The frequency to create ADC conversions
 *
 * @note				The exact meaning of physdev is hardware dependent. It describes the channels
 * 						the will be used later on when a "timer" conversion is actually scheduled.
 * @note				It is assumed that the timer is capable of free-running even when the ADC
 * 						is stopped or doing something else.
 * @details				When a timer interrupt occurs a conversion should start if these is a "timer" conversion
 * 						active.
 * @note				If the ADC is stopped, doesn't have a "timer" conversion active or is currently executing
 * 						a non-timer conversion then the interrupt can be ignored other than (optionally) incrementing
 * 						the GADC_Timer_Missed variable.
 *
 * @api
 */
void gadc_lld_start_timer(uint32_t physdev, uint32_t frequency);

/**
 * @brief				Stop the periodic timer for high frequency conversions.
 * @details				Also stops any current "timer" conversion (but not a current "non-timer" conversion).
 *
 * @param[in] physdev	A value passed to describe which physical ADC devices/channels in use.
 *
 * @note				The exact meaning of physdev is hardware dependent.
 *
 * @api
 */
void gadc_lld_stop_timer(uint32_t physdev);

/**
 * @brief				Start a "timer" conversion.
 * @details				Starts a series of conversions triggered by the timer.
 *
 * @param[in] pgtd		Contains the parameters for the timer conversion.
 *
 * @note				The exact meaning of physdev is hardware dependent. It is likely described in the
 * 						drivers gadc_lld_config.h
 * @note				Some versions of ChibiOS actually call the callback function more than once, once
 * 						at the half-way point and once on completion. The high level code handles this.
 * @note				The driver should call @p GADC_ISR_CompleteI() when it completes the operation
 * 						(or at the half-way point), or @p GAD_ISR_ErrorI() on an error.
 * @note				The high level code ensures that this is not called while a non-timer conversion is in
 * 						progress
 *
 * @iclass
 */
void gadc_lld_adc_timerI(GadcLldTimerData *pgtd);

/**
 * @brief				Start a "non-timer" conversion.
 * @details				Starts a single conversion now.
 *
 * @param[in] pgntd		Contains the parameters for the non-timer conversion.
 *
 * @note				The exact meaning of physdev is hardware dependent. It is likely described in the
 * 						drivers gadc_lld_config.h
 * @note				The driver should call @p GADC_ISR_CompleteI() when it completes the operation
 * 						or @p GAD_ISR_ErrorI() on an error.
 * @note				The high level code ensures that this is not called while a timer conversion is in
 * 						progress
 *
 * @iclass
 */
void gadc_lld_adc_nontimerI(GadcLldNonTimerData *pgntd);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GADC */

#endif /* _GADC_LLD_H */
/** @} */
