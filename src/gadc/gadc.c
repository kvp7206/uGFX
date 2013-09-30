/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gadc/gadc.c
 * @brief   GADC sub-system code.
 *
 * @addtogroup GADC
 * @{
 */
#include "gfx.h"

#if GFX_USE_GADC

/* Include the driver defines */
#include "gadc/lld/gadc_lld.h"

#if GADC_MAX_HIGH_SPEED_SAMPLERATE > GADC_MAX_SAMPLE_FREQUENCY/2
	#error "GADC: GADC_MAX_HIGH_SPEED_SAMPLERATE has been set too high. It must be less than half the maximum CPU rate"
#endif

#define GADC_MAX_LOWSPEED_DEVICES	((GADC_MAX_SAMPLE_FREQUENCY/GADC_MAX_HIGH_SPEED_SAMPLERATE)-1)
#if GADC_MAX_LOWSPEED_DEVICES > 4
	#undef GADC_MAX_LOWSPEED_DEVICES
	#define GADC_MAX_LOWSPEED_DEVICES	4
#endif

volatile bool_t GADC_Timer_Missed;

static gfxSem	gadcsem;
static gfxMutex	gadcmutex;
static GTimer	LowSpeedGTimer;
#if GFX_USE_GEVENT
	static GTimer	HighSpeedGTimer;
#endif

static volatile uint16_t	gflags = 0;
	#define GADC_GFLG_ISACTIVE	0x0001

#define GADC_FLG_ISACTIVE	0x0001
#define GADC_FLG_ISDONE		0x0002
#define GADC_FLG_ERROR		0x0004
#define GADC_FLG_GTIMER		0x0008

static struct hsdev {
	// Our status flags
	uint16_t				flags;

	// What we started with
	uint32_t				frequency;
	adcsample_t				*buffer;
	size_t					bufcount;
	size_t					samplesPerEvent;

	// The last set of results
	size_t					lastcount;
	adcsample_t				*lastbuffer;
	uint16_t				lastflags;

	// Other stuff we need to track progress and for signaling
	GadcLldTimerData		lld;
	size_t					samplesPerConversion;
	size_t					remaining;
	gfxSem					*bsem;
	GEventADC				*pEvent;
	GADCISRCallbackFunction	isrfn;
	} hs;

static struct lsdev {
	// Our status flags
	uint16_t				flags;

	// What we started with
	GadcLldNonTimerData		lld;
	GADCCallbackFunction	fn;
	void					*param;
	} ls[GADC_MAX_LOWSPEED_DEVICES];

static struct lsdev *curlsdev;

/* Find the next conversion to activate */
static inline void FindNextConversionI(void) {
	if (curlsdev) {
		/**
		 * Now we have done a low speed conversion - start looking for the next conversion
		 * We only look forward to ensure we get a high speed conversion at least once
		 * every GADC_MAX_LOWSPEED_DEVICES conversions.
		 */
		curlsdev++;

	} else {

		/* Now we have done a high speed conversion - start looking for low speed conversions */
		curlsdev = ls;
	}

	/**
	 * Look for the next thing to do.
	 */
	while(curlsdev < &ls[GADC_MAX_LOWSPEED_DEVICES]) {
		if ((curlsdev->flags & (GADC_FLG_ISACTIVE|GADC_FLG_ISDONE)) == GADC_FLG_ISACTIVE) {
			gadc_lld_adc_nontimerI(&curlsdev->lld);
			return;
		}
		curlsdev++;
	}
	curlsdev = 0;

	/* No more low speed devices - do a high speed conversion */
	if (hs.flags & GADC_FLG_ISACTIVE) {
		hs.lld.now = GADC_Timer_Missed ? TRUE : FALSE;
		GADC_Timer_Missed = 0;
		gadc_lld_adc_timerI(&hs.lld);
		return;
	}

	/* Nothing more to do */
	gflags &= ~GADC_GFLG_ISACTIVE;
}

void GADC_ISR_CompleteI(ADCDriver *adcp, adcsample_t *buffer, size_t n) {
	(void) adcp;

	if (curlsdev) {
		/* This interrupt must be in relation to the low speed device */

		if (curlsdev->flags & GADC_FLG_ISACTIVE) {
			/**
			 * As we only handle a single low speed conversion at a time, we know
			 * we know we won't get any half completion interrupts.
			 */
			curlsdev->flags |= GADC_FLG_ISDONE;
			gtimerJabI(&LowSpeedGTimer);
		}

		#if ADC_ISR_FULL_CODE_BUG
			/**
			 * Oops - We have just finished a low speed conversion but a bug prevents us
			 * restarting the ADC here. Other code will restart it in the thread based
			 * ADC handler.
			 */
			gflags &= ~GADC_GFLG_ISACTIVE;
			return;

		#endif

	} else {
		/* This interrupt must be in relation to the high speed device */

		if (hs.flags & GADC_FLG_ISACTIVE) {
			/* Save the details */
			hs.lastcount = n;
			hs.lastbuffer = buffer;
			hs.lastflags = GADC_Timer_Missed ? GADC_HSADC_LOSTEVENT : 0;

			/* Signal the user with the data */
			if (hs.pEvent) {
				#if GFX_USE_GEVENT
					hs.pEvent->type = GEVENT_ADC;
				#endif
				hs.pEvent->count = hs.lastcount;
				hs.pEvent->buffer = hs.lastbuffer;
				hs.pEvent->flags = hs.lastflags;
			}

			/* Our three signalling mechanisms */
			if (hs.isrfn)
				hs.isrfn(buffer, n);

			if (hs.bsem)
				gfxSemSignalI(hs.bsem);

			#if GFX_USE_GEVENT
				if (hs.flags & GADC_FLG_GTIMER)
					gtimerJabI(&HighSpeedGTimer);
			#endif

			/* Adjust what we have left to do */
			hs.lld.count -= n;
			hs.remaining -= n;

			/* Half completion - We have done all we can for now - wait for the next interrupt */
			if (hs.lld.count)
				return;

			/* Our buffer is cyclic - set up the new buffer pointers */
			if (hs.remaining) {
				hs.lld.buffer = buffer + (n * hs.samplesPerConversion);
			} else {
				hs.remaining = hs.bufcount;
				hs.lld.buffer = hs.buffer;
			}
			hs.lld.count = hs.remaining < hs.samplesPerEvent ? hs.remaining : hs.samplesPerEvent;
		}
	}

	/**
	 * Look for the next thing to do.
	 */
	FindNextConversionI();
}

void GADC_ISR_ErrorI(ADCDriver *adcp, adcerror_t err) {
	(void) adcp;
	(void) err;

	if (curlsdev) {
		if ((curlsdev->flags & (GADC_FLG_ISACTIVE|GADC_FLG_ISDONE)) == GADC_FLG_ISACTIVE)
			/* Mark the error then try to repeat it */
			curlsdev->flags |= GADC_FLG_ERROR;

		#if ADC_ISR_FULL_CODE_BUG
			/**
			 * Oops - We have just finished a low speed conversion but a bug prevents us
			 * restarting the ADC here. Other code will restart it in the thread based
			 * ADC handler.
			 */
			gflags &= ~GADC_GFLG_ISACTIVE;
			gtimerJabI(&LowSpeedGTimer);
			return;

		#endif

	} else {
		if (hs.flags & GADC_FLG_ISACTIVE)
			/* Mark the error and then try to repeat it */
			hs.flags |= GADC_FLG_ERROR;
	}

	/* Start the next conversion */
	FindNextConversionI();
}

/* Our module initialiser */
void _gadcInit(void) {
	gadc_lld_init();
	gfxSemInit(&gadcsem, GADC_MAX_LOWSPEED_DEVICES, GADC_MAX_LOWSPEED_DEVICES);
	gfxMutexInit(&gadcmutex);
	gtimerInit(&LowSpeedGTimer);
	#if GFX_USE_GEVENT
		gtimerInit(&HighSpeedGTimer);
	#endif
}

static inline void StartADC(bool_t onNoHS) {
	gfxSystemLock();
	if (!(gflags & GADC_GFLG_ISACTIVE) || (onNoHS && !curlsdev))
		FindNextConversionI();
	gfxSystemUnlock();
}

static void BSemSignalCallback(adcsample_t *buffer, void *param) {
	(void) buffer;

	/* Signal the BinarySemaphore parameter */
	gfxSemSignal((gfxSem *)param);
}

#if GFX_USE_GEVENT
	static void HighSpeedGTimerCallback(void *param) {
		(void) param;
		GSourceListener	*psl;
		GEventADC		*pe;

		psl = 0;
		while ((psl = geventGetSourceListener((GSourceHandle)(&HighSpeedGTimer), psl))) {
			if (!(pe = (GEventADC *)geventGetEventBuffer(psl))) {
				// This listener is missing - save this.
				psl->srcflags |= GADC_HSADC_LOSTEVENT;
				continue;
			}

			pe->type = GEVENT_ADC;
			pe->count = hs.lastcount;
			pe->buffer = hs.lastbuffer;
			pe->flags = hs.lastflags | psl->srcflags;
			psl->srcflags = 0;
			geventSendEvent(psl);
		}
	}
#endif

static void LowSpeedGTimerCallback(void *param) {
	(void) param;
	GADCCallbackFunction	fn;
	void					*prm;
	adcsample_t				*buffer;
	struct lsdev			*p;

	#if ADC_ISR_FULL_CODE_BUG
		/* Ensure the ADC is running if it needs to be - Bugfix HACK */
		StartADC(FALSE);
	#endif

	/**
	 * Look for completed low speed timers.
	 * We don't need to take the mutex as we are the only place that things are freed and we
	 * do that atomically.
	 */
	for(p=ls; p < &ls[GADC_MAX_LOWSPEED_DEVICES]; p++) {
		if ((p->flags & (GADC_FLG_ISACTIVE|GADC_FLG_ISDONE)) == (GADC_FLG_ISACTIVE|GADC_FLG_ISDONE)) {
			/* This item is done - perform its callback */
			fn = p->fn;				// Save the callback details
			prm = p->param;
			buffer = p->lld.buffer;
			p->fn = 0;				// Needed to prevent the compiler removing the local variables
			p->param = 0;			// Needed to prevent the compiler removing the local variables
			p->lld.buffer = 0;		// Needed to prevent the compiler removing the local variables
			p->flags = 0;			// The slot is available (indivisible operation)
			gfxSemSignal(&gadcsem);	// Tell everyone
			fn(buffer, prm);		// Perform the callback
		}
	}

}

void gadcHighSpeedInit(uint32_t physdev, uint32_t frequency, adcsample_t *buffer, size_t bufcount, size_t samplesPerEvent)
{
	gadcHighSpeedStop();		/* This does the init for us */

	/* Just save the details and reset everything for now */
	hs.frequency = frequency;
	hs.buffer = buffer;
	hs.bufcount = bufcount;
	hs.samplesPerEvent = samplesPerEvent;
	hs.lastcount = 0;
	hs.lastbuffer = 0;
	hs.lastflags = 0;
	hs.lld.physdev = physdev;
	hs.lld.buffer = buffer;
	hs.lld.count = samplesPerEvent;
	hs.lld.now = FALSE;
	hs.samplesPerConversion = gadc_lld_samples_per_conversion(physdev);
	hs.remaining = bufcount;
	hs.bsem = 0;
	hs.pEvent = 0;
	hs.isrfn = 0;
}

#if GFX_USE_GEVENT
	GSourceHandle gadcHighSpeedGetSource(void) {
		if (!gtimerIsActive(&HighSpeedGTimer))
			gtimerStart(&HighSpeedGTimer, HighSpeedGTimerCallback, NULL, TRUE, TIME_INFINITE);
		hs.flags |= GADC_FLG_GTIMER;
		return (GSourceHandle)&HighSpeedGTimer;
	}
#endif

void gadcHighSpeedSetISRCallback(GADCISRCallbackFunction isrfn) {
	hs.isrfn = isrfn;
}

void gadcHighSpeedSetBSem(gfxSem *pbsem, GEventADC *pEvent) {
	/* Use the system lock to ensure they occur atomically */
	gfxSystemLock();
	hs.pEvent = pEvent;
	hs.bsem = pbsem;
	gfxSystemUnlock();
}

void gadcHighSpeedStart(void) {
	/* If its already going we don't need to do anything */
	if (hs.flags & GADC_FLG_ISACTIVE)
		return;

	gadc_lld_start_timer(hs.lld.physdev, hs.frequency);
	hs.flags = GADC_FLG_ISACTIVE;
	StartADC(FALSE);
}

void gadcHighSpeedStop(void) {
	if (hs.flags & GADC_FLG_ISACTIVE) {
		/* No more from us */
		hs.flags = 0;
		gadc_lld_stop_timer(hs.lld.physdev);
		/*
		 * We have to pass TRUE to StartADC() as we might have the ADC marked as active when it isn't
		 * due to stopping the timer while it was converting.
		 */
		StartADC(TRUE);
	}
}

void gadcLowSpeedGet(uint32_t physdev, adcsample_t *buffer) {
	struct lsdev	*p;
	gfxSem			mysem;

	/* Start the Low Speed Timer */
	gfxSemInit(&mysem, 1, 1);
	gfxMutexEnter(&gadcmutex);
	if (!gtimerIsActive(&LowSpeedGTimer))
		gtimerStart(&LowSpeedGTimer, LowSpeedGTimerCallback, NULL, TRUE, TIME_INFINITE);
	gfxMutexExit(&gadcmutex);

	while(1) {
		/* Wait for an available slot */
		gfxSemWait(&gadcsem, TIME_INFINITE);

		/* Find a slot */
		gfxMutexEnter(&gadcmutex);
		for(p = ls; p < &ls[GADC_MAX_LOWSPEED_DEVICES]; p++) {
			if (!(p->flags & GADC_FLG_ISACTIVE)) {
				p->lld.physdev = physdev;
				p->lld.buffer = buffer;
				p->fn = BSemSignalCallback;
				p->param = &mysem;
				p->flags = GADC_FLG_ISACTIVE;
				gfxMutexExit(&gadcmutex);
				StartADC(FALSE);
				gfxSemWait(&mysem, TIME_INFINITE);
				return;
			}
		}
		gfxMutexExit(&gadcmutex);

		/**
		 *  We should never get here - the count semaphore must be wrong.
		 *  Decrement it and try again.
		 */
	}
}

bool_t gadcLowSpeedStart(uint32_t physdev, adcsample_t *buffer, GADCCallbackFunction fn, void *param) {
	struct lsdev *p;

	/* Start the Low Speed Timer */
	gfxMutexEnter(&gadcmutex);
	if (!gtimerIsActive(&LowSpeedGTimer))
		gtimerStart(&LowSpeedGTimer, LowSpeedGTimerCallback, NULL, TRUE, TIME_INFINITE);

	/* Find a slot */
	for(p = ls; p < &ls[GADC_MAX_LOWSPEED_DEVICES]; p++) {
		if (!(p->flags & GADC_FLG_ISACTIVE)) {
			/* We know we have a slot - this should never wait anyway */
			gfxSemWait(&gadcsem, TIME_IMMEDIATE);
			p->lld.physdev = physdev;
			p->lld.buffer = buffer;
			p->fn = fn;
			p->param = param;
			p->flags = GADC_FLG_ISACTIVE;
			gfxMutexExit(&gadcmutex);
			StartADC(FALSE);
			return TRUE;
		}
	}
	gfxMutexExit(&gadcmutex);
	return FALSE;
}

#endif /* GFX_USE_GADC */
/** @} */

