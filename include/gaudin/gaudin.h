/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gaudin/gaudin.h
 *
 * @addtogroup GAUDIN
 *
 * @brief	Module to read audio inputs (under development)
 *
 * @{
 */

#ifndef _GAUDIN_H
#define _GAUDIN_H

#include "gfx.h"

#if GFX_USE_GAUDIN || defined(__DOXYGEN__)

/* Include the driver defines */
#include "gaudin_lld_config.h"

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
	#if GFX_USE_GEVENT || defined(__DOXYGEN__)
	/**
	 * @brief The type of this event (GEVENT_AUDIO_IN)
	 */
		GEventType				type;
	#endif
	/**
	 * @brief The current channel
	 */
	uint16_t				channel;
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
	audin_sample_t			*buffer;
} GEventAudioIn;
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief		Initialise (but not start) the Audio Input Subsystem.
 * @details		Returns FALSE for an invalid channel or other invalid parameter.
 *
 * @param[in] channel			The channel to convert. Can be set from 0 to GAUDIN_NUM_CHANNELS - 1.
 * @param[in] frequency			The sample frequency
 * @param[in] buffer			The static buffer to put the samples into.
 * @param[in] bufcount			The total number of conversions that will fit in the buffer.
 * @param[in] samplesPerEvent	The number of conversions to do before returning an event.
 *
 * @note				Only one channel is active at a time. If an audio input is running it will be stopped.
 * 						The Event subsystem is disconnected from the audio subsystem and any binary semaphore
 * 						event is forgotten.
 * @note				Some channels may be stereo channels which return twice as much sample data with
 * 						the left and right channel data interleaved. Other channels may be mono channels.
 * 						Where stereo channels exist it would be common for the low level driver to also
 * 						offer the left and right channels separately.
 * @note				Due to a bug in Chibi-OS countPerEvent must be even if using the GADC low level audio driver.
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
 * @return				FALSE if invalid channel or parameter
 *
 * @api
 */
bool_t gaudinInit(uint16_t channel, uint32_t frequency, audin_sample_t *buffer, size_t bufcount, size_t samplesPerEvent);

#if GFX_USE_GEVENT || defined(__DOXYGEN__)
	/**
	 * @brief   			Turn on sending results to the GEVENT sub-system.
	 * @details				Returns a GSourceHandle to listen for GEVENT_AUDIO_IN events.
	 *
	 * @note				The audio input will not use the GEVENT system unless this is
	 * 						called first. This saves processing time if the application does
	 * 						not want to use the GEVENT sub-system for audio input.
	 * 						Once turned on it can only be turned off by calling @p gadcHighSpeedInit() again.
	 * @note				The audio input is capable of signalling via this method and a binary semaphore
	 * 						at the same time.
	 *
	 * @return				The GSourceHandle
	 *
	 * @api
	 */
	GSourceHandle gaudinGetSource(void);
#endif

/**
 * @brief				Allow retrieving of results from the audio input using a Binary Semaphore and a static event buffer.
 *
 * @param[in] pbsem			The semaphore is signaled when data is available.
 * @param[in] pEvent		The static event buffer to place the result information.
 *
 * @note				Passing a NULL for pbsem or pEvent will turn off signalling via this method.
 * @note				The audio input is capable of signalling via this method and the GEVENT
 * 						sub-system at the same time.
 *
 * @api
 */
void gaudinSetBSem(gfxSem *pbsem, GEventAudioIn *pEvent);

/**
 * @brief   Start the audio input conversions.
 * @pre		It must have been initialised first with @p gaudinInit()
 *
 * @api
 */
void gaudinStart(void);

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

