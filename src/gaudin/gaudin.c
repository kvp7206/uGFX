/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gaudin/gaudin.c
 * @brief   GAUDIN sub-system code.
 *
 * @addtogroup GAUDIN
 * @{
 */
#include "gfx.h"

#if GFX_USE_GAUDIN

/* Include the driver defines */
#include "gaudin/lld/gaudin_lld.h"

static gaudin_params	aud;
static gfxSem			*paudSem;
static GEventAudioIn	*paudEvent;
static audin_sample_t	*lastbuffer;
static size_t			lastcount;
static uint16_t			audFlags;
	#define AUDFLG_RUNNING		0x0001
	#define AUDFLG_USE_EVENTS	0x0002

#if GFX_USE_GEVENT
	static GTimer AudGTimer;

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
		gfxSemSignalI(paudSem);

	#if GFX_USE_GEVENT
		if (audFlags & AUDFLG_USE_EVENTS)
			gtimerJabI(&AudGTimer);
	#endif
}

void GAUDIN_ISR_ErrorI(void) {
	/* Ignore any errors for now */
}

/* The module initialiser */
void _gaudinInit(void) {
	#if GFX_USE_GEVENT
		gtimerInit(&AudGTimer);
	#endif
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

void gaudinSetBSem(gfxSem *pbsem, GEventAudioIn *pEvent) {
	gfxSystemLock();
	paudSem = pbsem;
	paudEvent = pEvent;
	gfxSystemUnlock();
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
