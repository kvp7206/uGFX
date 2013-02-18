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
 * @file    src/gadc/gadc.c
 * @brief   GADC sub-system code.
 *
 * @addtogroup GADC
 * @{
 */
#include "ch.h"
#include "hal.h"
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

static SEMAPHORE_DECL(gadcsem, GADC_MAX_LOWSPEED_DEVICES);
static MUTEX_DECL(gadcmutex);
static GTIMER_DECL(LowSpeedGTimer);
#if GFX_USE_GEVENT
	static GTIMER_DECL(HighSpeedGTimer);
#endif

static volatile uint16_t	gflags = 0;
	#define GADC_GFLG_INITDONE	0x0001
	#define GADC_GFLG_ISACTIVE	0x0002

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

	// Other stuff we need to track progress and for signalling
	GadcLldTimerData		lld;
	size_t					samplesPerConversion;
	size_t					remaining;
	BinarySemaphore			*bsem;
	GEventADC				*pEvent;
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
static __inline void FindNextConversionI(void) {
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
			if (hs.bsem)
				chBSemSignalI(hs.bsem);

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

static __inline void DoInit(void) {
	if (!(gflags & GADC_GFLG_INITDONE)) {
		gflags |= GADC_GFLG_INITDONE;
		gadc_lld_init();
	}
}

static __inline void StartADC(bool_t onNoHS) {
	chSysLock();
	if (!(gflags & GADC_GFLG_ISACTIVE) || (onNoHS && !curlsdev))
		FindNextConversionI();
	chSysUnlock();
}

static void BSemSignalCallback(adcsample_t *buffer, void *param) {
	(void) buffer;

	/* Signal the BinarySemaphore parameter */
	chBSemSignal((BinarySemaphore *)param);
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
			chSemSignal(&gadcsem);	// Tell everyone
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
}

#if GFX_USE_GEVENT
	GSourceHandle gadcHighSpeedGetSource(void) {
		DoInit();
		if (!gtimerIsActive(&HighSpeedGTimer))
			gtimerStart(&HighSpeedGTimer, HighSpeedGTimerCallback, NULL, TRUE, TIME_INFINITE);
		hs.flags |= GADC_FLG_GTIMER;
		return (GSourceHandle)&HighSpeedGTimer;
	}
#endif

void gadcHighSpeedSetBSem(BinarySemaphore *pbsem, GEventADC *pEvent) {
	DoInit();

	/* Use the system lock to ensure they occur atomically */
	chSysLock();
	hs.pEvent = pEvent;
	hs.bsem = pbsem;
	chSysUnlock();
}

void gadcHighSpeedStart(void) {
	DoInit();

	/* If its already going we don't need to do anything */
	if (hs.flags & GADC_FLG_ISACTIVE)
		return;

	gadc_lld_start_timer(hs.lld.physdev, hs.frequency);
	hs.flags = GADC_FLG_ISACTIVE;
	StartADC(FALSE);
}

void gadcHighSpeedStop(void) {
	DoInit();

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
	struct lsdev *p;
	BSEMAPHORE_DECL(mysem, TRUE);

	/* Start the Low Speed Timer */
	chMtxLock(&gadcmutex);
	if (!gtimerIsActive(&LowSpeedGTimer))
		gtimerStart(&LowSpeedGTimer, LowSpeedGTimerCallback, NULL, TRUE, TIME_INFINITE);
	chMtxUnlock();

	while(1) {
		/* Wait for an available slot */
		chSemWait(&gadcsem);

		/* Find a slot */
		chMtxLock(&gadcmutex);
		for(p = ls; p < &ls[GADC_MAX_LOWSPEED_DEVICES]; p++) {
			if (!(p->flags & GADC_FLG_ISACTIVE)) {
				p->lld.physdev = physdev;
				p->lld.buffer = buffer;
				p->fn = BSemSignalCallback;
				p->param = &mysem;
				p->flags = GADC_FLG_ISACTIVE;
				chMtxUnlock();
				StartADC(FALSE);
				chBSemWait(&mysem);
				return;
			}
		}
		chMtxUnlock();

		/**
		 *  We should never get here - the count semaphore must be wrong.
		 *  Decrement it and try again.
		 */
	}
}

bool_t gadcLowSpeedStart(uint32_t physdev, adcsample_t *buffer, GADCCallbackFunction fn, void *param) {
	struct lsdev *p;

	DoInit();

	/* Start the Low Speed Timer */
	chMtxLock(&gadcmutex);
	if (!gtimerIsActive(&LowSpeedGTimer))
		gtimerStart(&LowSpeedGTimer, LowSpeedGTimerCallback, NULL, TRUE, TIME_INFINITE);

	/* Find a slot */
	for(p = ls; p < &ls[GADC_MAX_LOWSPEED_DEVICES]; p++) {
		if (!(p->flags & GADC_FLG_ISACTIVE)) {
			/* We know we have a slot - this should never wait anyway */
			chSemWaitTimeout(&gadcsem, TIME_IMMEDIATE);
			p->lld.physdev = physdev;
			p->lld.buffer = buffer;
			p->fn = fn;
			p->param = param;
			p->flags = GADC_FLG_ISACTIVE;
			chMtxUnlock();
			StartADC(FALSE);
			return TRUE;
		}
	}
	chMtxUnlock();
	return FALSE;
}

#endif /* GFX_USE_GADC */
/** @} */

