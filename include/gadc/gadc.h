/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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
/**
 * @file    include/gadc/gadc.h
 * @brief   GADC - Periodic ADC subsystem header file.
 *
 * @addtogroup GADC
 *
 * @details	The reason why ChibiOS/GFX has it's own ADC abstraction is because
 *			the Chibi-OS drivers are very CPU specific and do not
 *			provide a way across all hardware platforms to create periodic
 *			ADC conversions. There are also issues with devices with different
 *			characteristics or periodic requirements on the same ADC
 *			device (but different channels). This layer attempts to solve these
 *			problems to provide a architecture neutral API. It also provides extra
 *			features such as multi-buffer chaining for high speed ADC sources.
 *			It provides one high speed virtual ADC device (eg a microphone) and
 *			numerous low speed (less than 100Hz) virtual ADC devices (eg dials,
 *			temperature sensors etc). The high speed device has timer based polling
 *			to ensure exact conversion periods and a buffer management system.
 *			The low speed devices are assumed to be non-critical timing devices
 *			and do not have any buffer management.
 *			Note that while only one high speed device has been provided it can
 *			be used to read multiple physical ADC channels on the one physical
 *			ADC device.
 *			All callback routines are thread based unlike the Chibi-OS interrupt based
 *			routines.
 *
 * @{
 */

#ifndef _GADC_H
#define _GADC_H

#include "gfx.h"

#if GFX_USE_GADC || defined(__DOXYGEN__)

/* Include the driver defines */
#include "gadc_lld_config.h"

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for GADC
#define GEVENT_ADC			(GEVENT_GADC_FIRST+0)

/**
 * @brief   The High Speed ADC event structure.
 * @{
 */
typedef struct GEventADC_t {
	#if GFX_USE_GEVENT || defined(__DOXYGEN__)
		/**
		 * @brief The type of this event (GEVENT_ADC)
		 */
		GEventType		type;
	#endif

	/**
	 * @brief The event flags
	 */
	uint16_t		flags;
		/**
		 * @brief   The event flag values.
		 * @{
		 */
		#define	GADC_HSADC_LOSTEVENT		0x0001		/**< @brief The last GEVENT_HSDADC event was lost */
		/** @} */
	/**
	 * @brief The number of conversions in the buffer
	 */
	size_t			count;
	/**
	 * @brief The buffer containing the conversion samples
	 */
	adcsample_t		*buffer;
	} GEventADC;

/**
 * @brief A callback function (executed in a thread context)
 */
typedef void (*GADCCallbackFunction)(adcsample_t *buffer, void *param);

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Initialise the high speed ADC.
 * @details				Initialises but does not start the conversions.
 *
 * @param[in] physdev		A value passed to describe which physical ADC devices/channels to use.
 * @param[in] frequency		The frequency to create ADC conversions
 * @param[in] buffer		The static buffer to put the ADC samples into.
 * @param[in] bufcount		The total number of conversions that will fit in the buffer.
 * @param[in] countPerEvent	The number of conversions to do before returning an event.
 *
 * @note				If the high speed ADC is running it will be stopped. The Event subsystem is
 * 						disconnected from the high speed ADC and any binary semaphore event is forgotten.
 * @note				bufcount must be greater than countPerEvent (usually 2 or more times) otherwise
 * 						the buffer will be overwitten with new data while the application is still trying
 * 						to process the old data.
 * @note				Due to a bug in Chibi-OS countPerEvent must be even. If bufcount is not
 * 						evenly divisable by countPerEvent, the remainder must also be even.
 * @note				The physdev parameter may be used to turn on more than one ADC channel.
 * 						Each channel is then interleaved into the provided buffer. Note 'bufcount'
 * 						and 'countPerEvent' parameters describe the number of conversions not the
 * 						number of samples.
 * 						As an example, if physdev turns on 2 devices then the buffer contains
 * 						alternate device samples and the buffer must contain 2 * bufcount samples.
 * 						The exact meaning of physdev is hardware dependent.
 * @note				The buffer is circular. When the end of the buffer is reached it will start
 * 						putting data into the beginning of the buffer again.
 * @note				The event listener must process the event (and the data in it) before the
 * 						next event occurs. If not, the following event will be lost.
 * @note				If bufcount is evenly divisable by countPerEvent, then every event will return
 * 						countPerEvent conversions. If bufcount is not evenly divisable, it will return
 * 						a block of samples containing less than countPerEvent samples when it reaches the
 * 						end of the buffer.
 * @note				While the high speed ADC is running, low speed conversions can only occur at
 * 						the frequency of the high speed events. Thus if high speed events are
 * 						being created at 50Hz (eg countPerEvent = 100, frequency = 5kHz) then the maximum
 * 						frequency for low speed conversions is likely to be 50Hz (although it might be
 * 						100Hz on some hardware).
 *
 * @api
 */
void gadcHighSpeedInit(uint32_t physdev, uint32_t frequency, adcsample_t *buffer, size_t bufcount, size_t samplesPerEvent);

#if GFX_USE_GEVENT || defined(__DOXYGEN__)
	/**
	 * @brief   			Turn on sending results to the GEVENT sub-system.
	 * @details				Returns a GSourceHandle to listen for GEVENT_ADC events.
	 *
	 * @note				The high speed ADC will not use the GEVENT system unless this is
	 * 						called first. This saves processing time if the application does
	 * 						not want to use the GEVENT sub-system for the high speed ADC.
	 * 						Once turned on it can only be turned off by calling @p gadcHighSpeedInit() again.
	 * @note				The high speed ADC is capable of signalling via this method and a binary semaphore
	 * 						at the same time.
	 *
	 * @api
	 */
	GSourceHandle gadcHighSpeedGetSource(void);
#endif

/**
 * @brief				Allow retrieving of results from the high speed ADC using a Binary Semaphore and a static event buffer.
 *
 * @param[in] pbsem			The binary semaphore is signaled when data is available.
 * @param[in] pEvent		The static event buffer to place the result information.
 *
 * @note				Passing a NULL for pbsem or pEvent will turn off signalling via this method as will calling
 * 						@p gadcHighSpeedInit().
 * @note				The high speed ADC is capable of signalling via this method and the GEVENT
 * 						sub-system at the same time.
 *
 * @api
 */
void gadcHighSpeedSetBSem(BinarySemaphore *pbsem, GEventADC *pEvent);

/**
 * @brief   Start the high speed ADC conversions.
 * @pre		It must have been initialised first with @p gadcHighSpeedInit()
 *
 * @api
 */
void gadcHighSpeedStart(void);

/**
 * @brief   Stop the high speed ADC conversions.
 *
 * @api
 */
void gadcHighSpeedStop(void);

/**
 * @brief	Perform a single low speed ADC conversion
 * @details	Blocks until the conversion is complete
 * @pre		This should not be called from within a GTimer callback as this routine
 * 			blocks until the conversion is ready.
 *
 * @param[in] physdev		A value passed to describe which physical ADC devices/channels to use.
 * @param[in] buffer		The static buffer to put the ADC samples into.
 *
 * @note	This may take a while to complete if the high speed ADC is running as the
 * 			conversion is interleaved with the high speed ADC conversions on a buffer
 * 			completion.
 * @note	The result buffer must be large enough to store one sample per device
 * 			described by the 'physdev' parameter.
 * @note	If calling this routine would exceed @p GADC_MAX_LOWSPEED_DEVICES simultaneous low
 * 			speed devices, the routine will wait for an available slot to complete the
 * 			conversion.
 * @note	Specifying more than one device in physdev is possible but discouraged as the
 * 			calculations to ensure the high speed ADC correctness will be incorrect. Symptoms
 * 			from over-running the high speed ADC include high speed samples being lost.
 *
 * @api
 */
void gadcLowSpeedGet(uint32_t physdev, adcsample_t *buffer);

/**
 * @brief	Perform a low speed ADC conversion with callback (in a thread context)
 * @details	Returns FALSE if there are no free low speed ADC slots. See @p GADC_MAX_LOWSPEED_DEVICES for details.
 *
 * @param[in] physdev		A value passed to describe which physical ADC devices/channels to use.
 * @param[in] buffer		The static buffer to put the ADC samples into.
 * @param[in] fn			The callback function to call when the conversion is complete.
 * @param[in] param			A parameter to pass to the callback function.
 *
 * @note	This may be safely called from within a GTimer callback.
 * @note	The callback may take a while to occur if the high speed ADC is running as the
 * 			conversion is interleaved with the high speed ADC conversions on a buffer
 * 			completion.
 * @note	The result buffer must be large enough to store one sample per device
 * 			described by the 'physdev' parameter.
 * @note	As this routine uses a low speed ADC, it asserts if you try to run more than @p GADC_MAX_LOWSPEED_DEVICES
 * 			at the same time.
 * @note	Specifying more than one device in physdev is possible but discouraged as the
 * 			calculations to ensure the high speed ADC correctness will be incorrect. Symptoms
 * 			from over-running the high speed ADC include high speed samples being lost.
 *
 * @api
 */
bool_t gadcLowSpeedStart(uint32_t physdev, adcsample_t *buffer, GADCCallbackFunction fn, void *param);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GADC */

#endif /* _GADC_H */
/** @} */

