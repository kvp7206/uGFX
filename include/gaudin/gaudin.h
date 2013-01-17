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
/**
 * @file    include/gaudin/gaudin.h
 * @brief   GAUDIN - Audio Input subsystem header file.
 *
 * @addtogroup GAUDIN
 *
 * @{
 */

#ifndef _GAUDIN_H
#define _GAUDIN_H

#include "gfx.h"

#if GFX_USE_GAUDIN || defined(__DOXYGEN__)

/* Include the driver defines */
#include "gaudin_lld_config.h"
//audio_in_sample_t
//GAUDIN_SAMPLE_FORMAT		ARRAY_DATA_10BITUNSIGNED
//GAUDIN_STEREO_DEVICE		FALSE

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Event types for GAUDIN
#define GEVENT_AUDIO_IN			(GEVENT_GAUDIN_FIRST+0)

/**
 * @brief   The Audio Input event structure.
 * @{
 */
typedef struct GEventAudioIn_t {
	/**
	 * @brief The type of this event (GEVENT_AUDIO_IN)
	 */
	GEventType				type;
	/**
	 * @brief The event flags
	 */
	uint16_t				flags;
		/**
		 * @brief   The event flag values.
		 * @{
		 */
		#define	GADC_AUDIO_IN_LOSTEVENT		0x0001		/**< @brief The last GEVENT_AUDIO_IN event was lost */
		/** @} */
	/**
	 * @brief The number of audio samples in the buffer
	 */
	size_t					count;
	/**
	 * @brief The buffer containing the audio samples
	 */
	audio_in_sample_t		*buffer;
	} GEventAudioIn;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Initialise the Audio Input Subsystem.
 * @details				Initialises but does not start the audio in.
 *
 * @param[in] frequency		The sample frequency
 * @param[in] buffer		The static buffer to put the samples into.
 * @param[in] bufcount		The total number of conversions that will fit in the buffer.
 * @param[in] countPerEvent	The number of conversions to do before returning an event.
 *
 * @note				If the audio input is running it will be stopped.
 * @note				Due to a bug in Chibi-OS countPerEvent must be even for the GADC audio driver.
 * 						If bufcount is not evenly divisable by countPerEvent, the remainder must also be even.
 * 						This requirement may not apply to other GAUDIN drivers.
 * @note				The number of samples for stereo devices will be double the number of conversions.
 * 						Make sure you allocate your buffers large enough. Each channel is then interleaved
 * 						into the provided buffer. Note 'bufcount' and 'countPerEvent' parameters describe the
 * 						number of conversions not the number of samples.
 * @note				The buffer is circular. When the end of the buffer is reached it will start
 * 						putting data into the beginning of the buffer again.
 * @note				The event listener must process the event (and the data in it) before the
 * 						next event occurs. If not, the following event will be lost.
 * @note				If bufcount is evenly divisable by countPerEvent, then every event will return
 * 						countPerEvent conversions. If bufcount is not evenly divisable, it will return
 * 						a block of samples containing less than countPerEvent samples when it reaches the
 * 						end of the buffer.
 *
 * @api
 */
void gaudinInit(uint32_t frequency, adcsample_t *buffer, size_t bufcount, size_t samplesPerEvent);

#if GFX_USE_GEVENT || defined(__DOXYGEN__)
	/**
	 * @brief   			Turn on sending results to the GEVENT sub-system.
	 * @details				Returns a GSourceHandle to listen for GEVENT_AUDIO_IN events.
	 *
	 * @note				The audio input will not use the GEVENT system unless this is
	 * 						called first. This saves processing time if the application does
	 * 						not want to use the GEVENT sub-system for audio input.
	 * 						Once turned on it cannot be turned off.
	 * @note				The audio input is capable of signalling via this method and a binary semaphore
	 * 						at the same time.
	 *
	 * @api
	 */
	GSourceHandle gaudinGetSource(void);
#endif

/**
 * @brief				Allow retrieving of results from the audio input using a Binary Semaphore and a static event buffer.
 *
 * @param[in] pbsem			The binary semaphore is signaled when data is available.
 * @param[in] pEvent		The static event buffer to place the result information.
 *
 * @note				Passing a NULL for pbsem or pEvent will turn off signalling via this method.
 * @note				The audio input is capable of signalling via this method and the GEVENT
 * 						sub-system at the same time.
 *
 * @api
 */
void gaudinSetBSem(BinarySemaphore *pbsem, GEventAudioIn *pEvent);

/**
 * @brief   Start the audio input conversions.
 * @pre		It must have been initialised first with @p gaudinInit()
 *
 * @api
 */
GSourceHandle gaudinStart(void);

/**
 * @brief   Stop the audio input conversions.
 *
 * @api
 */
void gaudinStop(void);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GAUDIN */

#endif /* _GAUDIN_H */
/** @} */

