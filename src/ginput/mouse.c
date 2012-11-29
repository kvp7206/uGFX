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
 * @file    src/ginput/mouse.c
 * @brief   GINPUT mouse/touch code.
 *
 * @addtogroup GINPUT_MOUSE
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gtimer.h"
#include "ginput.h"

#if GINPUT_NEED_MOUSE || defined(__DOXYGEN__)

#include "lld/ginput/mouse.h"

#if GINPUT_MOUSE_NEED_CALIBRATION
	#if !defined(GFX_USE_GDISP) || !GFX_USE_GDISP
		#error "GINPUT: GFX_USE_GDISP must be defined when mouse or touch calibration is required"
	#endif

	#define GINPUT_MOUSE_CALIBRATION_FONT		&fontUI2Double
	#define GINPUT_MOUSE_CALIBRATION_FONT2		&fontUI2Narrow
	#define GINPUT_MOUSE_CALIBRATION_TEXT		"Calibration"
	#define GINPUT_MOUSE_CALIBRATION_ERROR_TEXT	"Failed - Please try again!"
	#define GINPUT_MOUSE_CALIBRATION_SAME_TEXT	"Error: Same Reading - Check Driver!"

	#if GINPUT_MOUSE_MAX_CALIBRATION_ERROR < 0
		#define GINPUT_MOUSE_CALIBRATION_POINTS		3
	#else
		#define GINPUT_MOUSE_CALIBRATION_POINTS		4
	#endif

	typedef struct Calibration_t {
	    float ax;
	    float bx;
	    float cx;
	    float ay;
	    float by;
	    float cy;
	} Calibration;
#endif

typedef struct MousePoint_t {
	coord_t		x, y;
	} MousePoint;

static GTIMER_DECL(MouseTimer);

static struct MouseConfig_t {
	MouseReading					t;
	MousePoint						movepos;
	MousePoint						clickpos;
	systime_t						clicktime;
	uint16_t						last_buttons;
	uint16_t						flags;
			#define FLG_INIT_DONE		0x8000
			#define FLG_CLICK_TIMER		0x0001
			#define FLG_IN_CAL			0x0010
			#define FLG_CAL_OK			0x0020
			#define FLG_CAL_SAVED		0x0040
			#define FLG_CAL_FREE		0x0080
	#if GINPUT_MOUSE_NEED_CALIBRATION
		GMouseCalibrationSaveRoutine	fnsavecal;
		GMouseCalibrationLoadRoutine	fnloadcal;
		Calibration						caldata;
	#endif
	} MouseConfig;

#if GINPUT_MOUSE_NEED_CALIBRATION
	static __inline void _tsDrawCross(const MousePoint *pp) {
		gdispDrawLine(pp->x-15, pp->y, pp->x-2, pp->y, White);
		gdispDrawLine(pp->x+2, pp->y, pp->x+15, pp->y, White);
		gdispDrawLine(pp->x, pp->y-15, pp->x, pp->y-2, White);
		gdispDrawLine(pp->x, pp->y+2, pp->x, pp->y+15, White);

		gdispDrawLine(pp->x-15, pp->y+15, pp->x-7, pp->y+15, RGB2COLOR(184,158,131));
		gdispDrawLine(pp->x-15, pp->y+7, pp->x-15, pp->y+15, RGB2COLOR(184,158,131));

		gdispDrawLine(pp->x-15, pp->y-15, pp->x-7, pp->y-15, RGB2COLOR(184,158,131));
		gdispDrawLine(pp->x-15, pp->y-7, pp->x-15, pp->y-15, RGB2COLOR(184,158,131));

		gdispDrawLine(pp->x+7, pp->y+15, pp->x+15, pp->y+15, RGB2COLOR(184,158,131));
		gdispDrawLine(pp->x+15, pp->y+7, pp->x+15, pp->y+15, RGB2COLOR(184,158,131));

		gdispDrawLine(pp->x+7, pp->y-15, pp->x+15, pp->y-15, RGB2COLOR(184,158,131));
		gdispDrawLine(pp->x+15, pp->y-15, pp->x+15, pp->y-7, RGB2COLOR(184,158,131));
	}

	static __inline void _tsClearCross(const MousePoint *pp) {
		gdispFillArea(pp->x - 15, pp->y - 15, 42, 42, Blue);
	}

	static __inline void _tsTransform(MouseReading *pt, const Calibration *c) {
		pt->x = (coord_t) (c->ax * pt->x + c->bx * pt->y + c->cx);
		pt->y = (coord_t) (c->ay * pt->x + c->by * pt->y + c->cy);
	}

	static __inline void _tsDo3PointCalibration(const MousePoint *cross, const MousePoint *points, Calibration *c) {
		float dx, dx0, dx1, dx2, dy0, dy1, dy2;

		/* Compute all the required determinants */
		dx = ((float)(points[0].x - points[2].x)) * ((float)(points[1].y - points[2].y))
			- ((float)(points[1].x - points[2].x)) * ((float)(points[0].y - points[2].y));

		dx0 = ((float)(cross[0].x - cross[2].x)) * ((float)(points[1].y - points[2].y))
			- ((float)(cross[1].x - cross[2].x)) * ((float)(points[0].y - points[2].y));

		dx1 = ((float)(cross[1].x - cross[2].x)) * ((float)(points[0].x - points[2].x))
			- ((float)(cross[0].x - cross[2].x)) * ((float)(points[1].x - points[2].x));

		dx2 = cross[0].x * ((float)points[1].x * (float)points[2].y - (float)points[2].x * (float)points[1].y) -
			cross[1].x * ((float)points[0].x * (float)points[2].y - (float)points[2].x * (float)points[0].y) +
			cross[2].x * ((float)points[0].x * (float)points[1].y - (float)points[1].x * (float)points[0].y);

		dy0 = ((float)(cross[0].y - cross[2].y))  *  ((float)(points[1].y - points[2].y))
			- ((float)(cross[1].y - cross[2].y))  *  ((float)(points[0].y - points[2].y));

		dy1  = ((float)(cross[1].y - cross[2].y)) * ((float)(points[0].x - points[2].x))
			- ((float)(cross[0].y - cross[2].y)) * ((float)(points[1].x - points[2].x));

		dy2  = cross[0].y * ((float)points[1].x * (float)points[2].y - (float)points[2].x * (float)points[1].y) -
			cross[1].y * ((float)points[0].x * (float)points[2].y - (float)points[2].x * (float)points[0].y) +
			cross[2].y * ((float)points[0].x * (float)points[1].y - (float)points[1].x * (float)points[0].y);

		/* Now, calculate all the required coefficients */
		c->ax = dx0 / dx;
		c->bx = dx1 / dx;
		c->cx = dx2 / dx;

		c->ay = dy0 / dx;
		c->by = dy1 / dx;
		c->cy = dy2 / dx;
	}
#endif

#if GINPUT_MOUSE_READ_CYCLES > 1
	static void get_raw_reading(MouseReading *pt) {
		int32_t x, y, z;
		unsigned i;

		x = y = z = 0;
		for(i = 0; i < GINPUT_MOUSE_READ_CYCLES; i++) {
			ginput_lld_mouse_get_reading(pt);
			x += pt->x;
			y += pt->y;
			z += pt->z;
		}

		/* Take the average of the readings */
		pt->x = x / GINPUT_MOUSE_READ_CYCLES;
		pt->y = y / GINPUT_MOUSE_READ_CYCLES;
		pt->z = z / GINPUT_MOUSE_READ_CYCLES;
	}
#else
	#define get_raw_reading(pt)		ginput_lld_mouse_get_reading(pt)
#endif

static void get_calibrated_reading(MouseReading *pt) {
	#if GINPUT_MOUSE_NEED_CALIBRATION || GDISP_NEED_CONTROL
		coord_t		w, h;
	#endif

	get_raw_reading(pt);

	#if GINPUT_MOUSE_NEED_CALIBRATION || GDISP_NEED_CONTROL
		w = gdispGetWidth();
		h = gdispGetHeight();
	#endif

	#if GINPUT_MOUSE_NEED_CALIBRATION
		_tsTransform(pt, &MouseConfig.caldata);
	#endif

	#if GDISP_NEED_CONTROL
		switch(gdispGetOrientation()) {
			case GDISP_ROTATE_0:
				break;
			case GDISP_ROTATE_90:
				{
					coord_t t = pt->y;
					pt->y = h - 1 - pt->x;
					pt->x = t;
				}
				break;
			case GDISP_ROTATE_180:
				pt->x = w - 1 - pt->x;
				pt->y = h - 1 - pt->y;
				break;
			case GDISP_ROTATE_270:
				{
					coord_t t = pt->x;
					pt->x = w - 1 - pt->y;
					pt->y = t;
				}
				break;
		}
	#endif

	#if GINPUT_MOUSE_NEED_CALIBRATION
		if (pt->x < 0)	pt->x = 0;
		else if (pt->x >= w) pt->x = w-1;
		if (pt->y < 0)	pt->y = 0;
		else if (pt->y >= h) pt->y = h-1;
	#endif
}

static void MousePoll(void *param) {
	(void) param;
	GSourceListener	*psl;
	GEventMouse		*pe;
	unsigned 		meta;
	uint16_t		tbtns;
	uint32_t		cdiff;
	uint32_t		mdiff;

	// Save the last mouse state
	MouseConfig.last_buttons = MouseConfig.t.buttons;

	// Get the new mouse reading
	get_calibrated_reading(&MouseConfig.t);

	// Calculate out new event meta value and handle CLICK and CXTCLICK
	meta = GMETA_NONE;

	// Calculate the position difference from our movement reference (update the reference if out of range)
	mdiff = (MouseConfig.t.x - MouseConfig.movepos.x) * (MouseConfig.t.x - MouseConfig.movepos.x) +
		(MouseConfig.t.y - MouseConfig.movepos.y) * (MouseConfig.t.y - MouseConfig.movepos.y);
	if (mdiff > GINPUT_MOUSE_MAX_MOVE_JITTER * GINPUT_MOUSE_MAX_MOVE_JITTER) {
		MouseConfig.movepos.x = MouseConfig.t.x;
		MouseConfig.movepos.y = MouseConfig.t.y;
	}
	
	// Check if the click has moved outside the click area and if so cancel the click
	if ((MouseConfig.flags & FLG_CLICK_TIMER)) {
		cdiff = (MouseConfig.t.x - MouseConfig.clickpos.x) * (MouseConfig.t.x - MouseConfig.clickpos.x) +
			(MouseConfig.t.y - MouseConfig.clickpos.y) * (MouseConfig.t.y - MouseConfig.clickpos.y);
		if (cdiff > GINPUT_MOUSE_MAX_CLICK_JITTER * GINPUT_MOUSE_MAX_CLICK_JITTER)
			MouseConfig.flags &= ~FLG_CLICK_TIMER;
	}

	// Mouse down
	tbtns = MouseConfig.t.buttons & ~MouseConfig.last_buttons;
	if ((tbtns & GINPUT_MOUSE_BTN_LEFT))
		meta |= GMETA_MOUSE_DOWN;
	if ((tbtns & (GINPUT_MOUSE_BTN_LEFT|GINPUT_MOUSE_BTN_RIGHT))) {
		MouseConfig.clickpos.x = MouseConfig.t.x;
		MouseConfig.clickpos.y = MouseConfig.t.y;
		MouseConfig.clicktime = chTimeNow();
		MouseConfig.flags |= FLG_CLICK_TIMER;
	}

	// Mouse up
	tbtns = ~MouseConfig.t.buttons & MouseConfig.last_buttons;
	if ((tbtns & GINPUT_MOUSE_BTN_LEFT))
		meta |= GMETA_MOUSE_UP;
	if ((tbtns & (GINPUT_MOUSE_BTN_LEFT|GINPUT_MOUSE_BTN_RIGHT))) {
		if ((MouseConfig.flags & FLG_CLICK_TIMER)) {
			if ((tbtns & GINPUT_MOUSE_BTN_LEFT)
					#if GINPUT_MOUSE_CLICK_TIME != TIME_INFINITE
						&& chTimeNow() - MouseConfig.clicktime < MS2ST(GINPUT_MOUSE_CLICK_TIME)
					#endif
					)
				meta |= GMETA_MOUSE_CLICK;
			else
				meta |= GMETA_MOUSE_CXTCLICK;
			MouseConfig.flags &= ~FLG_CLICK_TIMER;
		}
	}

	// Send the event to the listeners that are interested.
	psl = 0;
	while ((psl = geventGetSourceListener((GSourceHandle)(&MouseConfig), psl))) {
		if (!(pe = (GEventMouse *)geventGetEventBuffer(psl))) {
			// This listener is missing - save the meta events that have happened
			psl->srcflags |= meta;
			continue;
		}

		// If we haven't really moved (and there are no meta events) don't bother sending the event
		if (mdiff <= GINPUT_MOUSE_MAX_MOVE_JITTER * GINPUT_MOUSE_MAX_MOVE_JITTER && !psl->srcflags && !meta && !(psl->listenflags & GLISTEN_MOUSENOFILTER))
			continue;

		// Send the event if we are listening for it
		if (((MouseConfig.t.buttons & GINPUT_MOUSE_BTN_LEFT) && (psl->listenflags & GLISTEN_MOUSEDOWNMOVES))
				|| (!(MouseConfig.t.buttons & GINPUT_MOUSE_BTN_LEFT) && (psl->listenflags & GLISTEN_MOUSEUPMOVES))
				|| (meta && (psl->listenflags & GLISTEN_MOUSEMETA))) {
			pe->type = GINPUT_MOUSE_EVENT_TYPE;
			pe->instance = 0;
			pe->x = MouseConfig.t.x;
			pe->y = MouseConfig.t.y;
			pe->z = MouseConfig.t.z;
			pe->current_buttons = MouseConfig.t.buttons;
			pe->last_buttons = MouseConfig.last_buttons;
			pe->meta = meta;
			if (psl->srcflags) {
				pe->current_buttons |= GINPUT_MISSED_MOUSE_EVENT;
				pe->meta |= psl->srcflags;
				psl->srcflags = 0;
			}
			geventSendEvent(psl);
		}
	}
}

/* Mouse Functions */
GSourceHandle ginputGetMouse(uint16_t instance) {
	#if GINPUT_MOUSE_NEED_CALIBRATION
		Calibration		*pc;
	#endif

	// We only support a single mouse instance currently
	//	Instance 9999 is the same as instance 0 except that it installs
	//	a special "raw" calibration if there isn't one we can load.
	if (instance && instance != 9999)
		return 0;

	// Do we need to initialise the mouse subsystem?
	if (!(MouseConfig.flags & FLG_INIT_DONE)) {
		ginput_lld_mouse_init();

		#if GINPUT_MOUSE_NEED_CALIBRATION
			#if GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE
				if (!MouseConfig.fnloadcal) {
					MouseConfig.fnloadcal = ginput_lld_mouse_calibration_load;
					MouseConfig.flags &= ~FLG_CAL_FREE;
				}
				if (!MouseConfig.fnsavecal)
					MouseConfig.fnsavecal = ginput_lld_mouse_calibration_save;
			#endif
			if (MouseConfig.fnloadcal && (pc = (Calibration *)MouseConfig.fnloadcal(instance))) {
				MouseConfig.caldata = pc[0];
				MouseConfig.flags |= (FLG_CAL_OK|FLG_CAL_SAVED);
				if ((MouseConfig.flags & FLG_CAL_FREE))
					chHeapFree((void *)pc);
			} else if (instance == 9999) {
				MouseConfig.caldata.ax = 1;
				MouseConfig.caldata.bx = 0;
				MouseConfig.caldata.cx = 0;
				MouseConfig.caldata.ay = 0;
				MouseConfig.caldata.by = 1;
				MouseConfig.caldata.cy = 0;
				MouseConfig.flags |= (FLG_CAL_OK|FLG_CAL_SAVED);
			} else
				ginputCalibrateMouse(instance);
		#endif

		// Get the first reading
		MouseConfig.last_buttons = 0;
		get_calibrated_reading(&MouseConfig.t);

		// Mark init as done and start the Poll timer
		MouseConfig.flags |= FLG_INIT_DONE;
		gtimerStart(&MouseTimer, MousePoll, 0, TRUE, GINPUT_MOUSE_POLL_PERIOD);
	}

	// Return our structure as the handle
	return (GSourceHandle)&MouseConfig;
}

/* Get the current mouse position and button status.
 *	Unlike a listener event, this status cannot record meta events such as "CLICK"
 *	Returns FALSE on error (eg invalid instance)
 */
bool_t ginputGetMouseStatus(uint16_t instance, GEventMouse *pe) {
	if (instance || (MouseConfig.flags & (FLG_INIT_DONE|FLG_IN_CAL)) != FLG_INIT_DONE)
		return FALSE;

	pe->type = GINPUT_MOUSE_EVENT_TYPE;
	pe->instance = instance;
	pe->x = MouseConfig.t.x;
	pe->y = MouseConfig.t.y;
	pe->z = MouseConfig.t.z;
	pe->current_buttons = MouseConfig.t.buttons;
	pe->last_buttons = MouseConfig.last_buttons;
	if (pe->current_buttons & ~pe->last_buttons & GINPUT_MOUSE_BTN_LEFT)
		pe->meta = GMETA_MOUSE_DOWN;
	else if (~pe->current_buttons & pe->last_buttons & GINPUT_MOUSE_BTN_LEFT)
		pe->meta = GMETA_MOUSE_UP;
	else
		pe->meta = GMETA_NONE;
	return TRUE;
}

/* Run a mouse calibration.
 *	Returns FALSE if the driver doesn't support it or if the handle is invalid.
 */
bool_t ginputCalibrateMouse(uint16_t instance) {
	#if !GINPUT_MOUSE_NEED_CALIBRATION
		(void) instance;
		
		return FALSE;
	#else

		const coord_t height  =  gdispGetHeight();
		const coord_t width  =  gdispGetWidth();
		const MousePoint cross[]  =  {{(width / 4), (height / 4)},
										{(width - (width / 4)) , (height / 4)},
										{(width - (width / 4)) , (height - (height / 4))},
										{(width / 2), (height / 2)}}; /* Check point */
		MousePoint points[GINPUT_MOUSE_CALIBRATION_POINTS];
		const MousePoint	*pc;
		MousePoint *pt;
		int32_t px, py;
		unsigned i, j;
		#if GINPUT_MOUSE_MAX_CALIBRATION_ERROR >= 0
			unsigned	err;
		#endif

		if (instance || (MouseConfig.flags & FLG_IN_CAL))
			return FALSE;

		MouseConfig.flags |= FLG_IN_CAL;
		gtimerStop(&MouseTimer);
		MouseConfig.flags &= ~(FLG_CAL_OK|FLG_CAL_SAVED);

		#if GDISP_NEED_CONTROL
			gdispSetOrientation(GDISP_ROTATE_0);
		#endif

		#if GDISP_NEED_CLIP
			gdispSetClip(0, 0, width, height);
		#endif

		#if GINPUT_MOUSE_MAX_CALIBRATION_ERROR >= 0
			while(1) {
		#endif
				gdispClear(Blue);

				gdispFillStringBox(0, 5, width, 30, GINPUT_MOUSE_CALIBRATION_TEXT, GINPUT_MOUSE_CALIBRATION_FONT,  White, Blue, justifyCenter);

				for(i = 0, pt = points, pc = cross; i < GINPUT_MOUSE_CALIBRATION_POINTS; i++, pt++, pc++) {
					_tsDrawCross(pc);

					do {

						/* Wait for the mouse to be pressed */
						while(get_raw_reading(&MouseConfig.t), !(MouseConfig.t.buttons & GINPUT_MOUSE_BTN_LEFT))
							chThdSleepMilliseconds(20);

						/* Average all the samples while the mouse is down */
						for(px = py = 0, j = 0;
								chThdSleepMilliseconds(20),			/* Settling time between readings */
								get_raw_reading(&MouseConfig.t),
								(MouseConfig.t.buttons & GINPUT_MOUSE_BTN_LEFT);
								j++) {
							px += MouseConfig.t.x;
							py += MouseConfig.t.y;
						}

					} while(!j);

					pt->x = px / j;
					pt->y = py / j;

					_tsClearCross(pc);

					if (i >= 1 && pt->x == (pt-1)->x && pt->y == (pt-1)->y) {
						gdispFillStringBox(0, 35, width, 40, GINPUT_MOUSE_CALIBRATION_SAME_TEXT, GINPUT_MOUSE_CALIBRATION_FONT2,  Red, Yellow, justifyCenter);
						chThdSleepMilliseconds(5000);
						gdispFillArea(0, 35, width, 40, Blue);
					}

				}

				/* Apply 3 point calibration algorithm */
				_tsDo3PointCalibration(cross, points, &MouseConfig.caldata);

				 /* Verification of correctness of calibration (optional) :
				 *  See if the 4th point (Middle of the screen) coincides with the calibrated
				 *  result. If point is within +/- Squareroot(ERROR) pixel margin, then successful calibration
				 *  Else, start from the beginning.
				 */
		#if GINPUT_MOUSE_MAX_CALIBRATION_ERROR >= 0
				/* Transform the co-ordinates */
				MouseConfig.t.x = points[3].x;
				MouseConfig.t.y = points[3].y;
				_tsTransform(&MouseConfig.t, &MouseConfig.caldata);

				/* Calculate the delta */
				err = (MouseConfig.t.x - cross[3].x) * (MouseConfig.t.x - cross[3].x) +
					(MouseConfig.t.y - cross[3].y) * (MouseConfig.t.y - cross[3].y);

				if (err <= GINPUT_MOUSE_MAX_CALIBRATION_ERROR * GINPUT_MOUSE_MAX_CALIBRATION_ERROR)
					break;

				gdispFillStringBox(0, 35, width, 40, GINPUT_MOUSE_CALIBRATION_ERROR_TEXT, GINPUT_MOUSE_CALIBRATION_FONT2,  Red, Yellow, justifyCenter);
				chThdSleepMilliseconds(5000);
			}
		#endif

		// Restart everything
		MouseConfig.flags |= FLG_CAL_OK;
		MouseConfig.last_buttons = 0;
		get_calibrated_reading(&MouseConfig.t);
		MouseConfig.flags &= ~FLG_IN_CAL;
		if ((MouseConfig.flags & FLG_INIT_DONE))
			gtimerStart(&MouseTimer, MousePoll, 0, TRUE, GINPUT_MOUSE_POLL_PERIOD);
		
		// Save the calibration data (if possible)
		if (MouseConfig.fnsavecal) {
			MouseConfig.fnsavecal(instance, (const uint8_t *)&MouseConfig.caldata, sizeof(MouseConfig.caldata));
			MouseConfig.flags |= FLG_CAL_SAVED;
		}
		return TRUE;
	#endif
}

/* Set the routines to save and fetch calibration data.
 * This function should be called before first calling ginputGetMouse() for a particular instance
 *	as the gdispGetMouse() routine may attempt to fetch calibration data and perform a startup calibration if there is no way to get it.
 *	If this is called after gdispGetMouse() has been called and the driver requires calibration storage, it will immediately save the data is has already obtained.
 * The 'requireFree' parameter indicates if the fetch buffer must be free()'d to deallocate the buffer provided by the Fetch routine.
 */
void ginputSetMouseCalibrationRoutines(uint16_t instance, GMouseCalibrationSaveRoutine fnsave, GMouseCalibrationLoadRoutine fnload, bool_t requireFree) {
	#if GINPUT_MOUSE_NEED_CALIBRATION
		if (instance)
			return;

		MouseConfig.fnloadcal = fnload;
		MouseConfig.fnsavecal = fnsave;
		if (requireFree)
			MouseConfig.flags |= FLG_CAL_FREE;
		else
			MouseConfig.flags &= ~FLG_CAL_FREE;
		#if GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE
			if (!MouseConfig.fnloadcal) {
				MouseConfig.fnloadcal = ginput_lld_mouse_calibration_load;
				MouseConfig.flags &= ~FLG_CAL_FREE;
			}
			if (!MouseConfig.fnsavecal)
				MouseConfig.fnsavecal = ginput_lld_mouse_calibration_save;
		#endif
		if (MouseConfig.fnsavecal && (MouseConfig.flags & (FLG_CAL_OK|FLG_CAL_SAVED)) == FLG_CAL_OK) {
			MouseConfig.fnsavecal(instance, (const uint8_t *)&MouseConfig.caldata, sizeof(MouseConfig.caldata));
			MouseConfig.flags |= FLG_CAL_SAVED;
		}
	#else
		(void)instance, (void)fnsave, (void)fnload, (void)requireFree;
	#endif
}

/* Test if a particular mouse instance requires routines to save its calibration data. */
bool_t ginputRequireMouseCalibrationStorage(uint16_t instance) {
	if (instance)
		return FALSE;

	#if GINPUT_MOUSE_NEED_CALIBRATION && !GINPUT_MOUSE_LLD_CALIBRATION_LOADSAVE
		return TRUE;
	#else
		return FALSE;
	#endif
}

/* Wake up the mouse driver from an interrupt service routine (there may be new readings available) */
void ginputMouseWakeup(void) {
	gtimerJab(&MouseTimer);
}

/* Wake up the mouse driver from an interrupt service routine (there may be new readings available) */
void ginputMouseWakeupI(void) {
	gtimerJabI(&MouseTimer);
}

#endif /* GINPUT_NEED_MOUSE */
/** @} */
