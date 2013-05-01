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
 * @file    src/gaudin/gaudin.c
 * @brief   GAUDIN sub-system code.
 *
 * @addtogroup GAUDIN
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GAUDIN

/* Include the driver defines */
#include "gaudin/lld/gaudin_lld.h"

static gaudin_params	aud;
static BinarySemaphore	*paudSem;
static GEventAudioIn	*paudEvent;
static audin_sample_t	*lastbuffer;
static size_t			lastcount;
static uint16_t			audFlags;
	#define AUDFLG_RUNNING		0x0001
	#define AUDFLG_USE_EVENTS	0x0002

#if GFX_USE_GEVENT
	static GTIMER_DECL(AudGTimer);

	static void AudGTimerCallback(void *param) {
		(void) param;
		GSourceListener	*psl;
		GEventADC		*pe;

		psl = 0;
		while ((psl = geventGetSourceListener((GSourceHandle)(&aud), psl))) {
			if (!(pe = (GEventAudioIn *)geventGetEventBuffer(psl))) {
				// This listener is missing - save this.
				psl->srcflags |= GADC_AUDIO_IN_LOSTEVENT;
				continue;
			}

			pe->type = GEVENT_AUDIO_IN;
			pe->channel = aud.channel;
			pe->count = lastcount;
			pe->buffer = lastbuffer;
			pe->flags = psl->srcflags;
			psl->srcflags = 0;
			geventSendEvent(psl);
		}
	}
#endif

void GAUDIN_ISR_CompleteI(audin_sample_t *buffer, size_t n) {
	/* Save the details */
	lastcount = n;
	lastbuffer = buffer;

	/* Signal the user with the data */
	if (paudEvent) {
		#if GFX_USE_GEVENT
			paudEvent->type = GEVENT_AUDIO_IN;
		#endif
		paudEvent->channel = aud.channel;
		paudEvent->count = lastcount;
		paudEvent->buffer = lastbuffer;
		paudEvent->flags = 0;
	}

	/* Our two signalling mechanisms */
	if (paudSem)
		chBSemSignalI(paudSem);

	#if GFX_USE_GEVENT
		if (audFlags & AUDFLG_USE_EVENTS)
			gtimerJabI(&AudGTimer);
	#endif
}

void GAUDIN_ISR_ErrorI(void) {
	/* Ignore any errors for now */
}

bool_t gaudinInit(uint16_t channel, uint32_t frequency, audin_sample_t *buffer, size_t bufcount, size_t samplesPerEvent) {
	/* Check the channel is valid */
	if (channel >= GAUDIN_NUM_CHANNELS || frequency > GAUDIN_MAX_SAMPLE_FREQUENCY)
		return FALSE;

	/* Stop any existing transfers */
	if ((audFlags & AUDFLG_RUNNING))
		gadc_lld_stop();
	audFlags = 0;

	/* Initialise everything */
	aud.channel = channel;
	aud.frequency = frequency;
	aud.buffer = buffer;
	aud.bufcount = bufcount;
	aud.samplesPerEvent = samplesPerEvent;
	paudSem = 0;
	paudEvent = 0;

	/* Set up the low level driver */
	gaudin_lld_init(&aud);
	return TRUE;
}

#if GFX_USE_GEVENT
	GSourceHandle gaudinGetSource(void) {
		if (!gtimerIsActive(&AudGTimer))
			gtimerStart(&AudGTimer, AudGTimerCallback, NULL, TRUE, TIME_INFINITE);
		audFlags |= AUDFLG_USE_EVENTS;
		return (GSourceHandle)&aud;
	}
#endif

void gaudinSetBSem(BinarySemaphore *pbsem, GEventAudioIn *pEvent) {
	chSysLock();
	paudSem = pbsem;
	paudEvent = pEvent;
	chSysUnlock();
}

void gaudinStart(void) {
	if (!(audFlags & AUDFLG_RUNNING)) {
		audFlags |= AUDFLG_RUNNING;
		gadc_lld_start();
	}
}

void gaudinStop(void) {
	if ((audFlags & AUDFLG_RUNNING)) {
		gadc_lld_stop();
		audFlags &= ~AUDFLG_RUNNING;
	}
}

#endif /* GFX_USE_GAUDIN */
/** @} */
