/*	ChibiOS/GFX - Copyright (C) 2012
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
 * @file	src/touchscreen.c
 * @brief	Touchscreen Driver code.
 *
 * @addtogroup TOUCHSCREEN
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "touchscreen.h"

#if GFX_USE_TOUCHSCREEN || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
static struct cal_t *cal;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static coord_t _tsReadRealX(void) {
    int32_t results = 0;
    int16_t i;
    coord_t x;

    for(i = 0; i < TOUCHSCREEN_CONVERSIONS; i++) {
        results += ts_lld_read_x();
    }

    /* Take the average of the readings */
    x = results / TOUCHSCREEN_CONVERSIONS;

    return x;
}

static coord_t _tsReadRealY(void) {
    int32_t results = 0;
    int16_t i;
    coord_t y;

    for(i = 0; i < TOUCHSCREEN_CONVERSIONS; i++) {
        results += ts_lld_read_y();
    }

    /* Take the average of the readings */
    y = results / TOUCHSCREEN_CONVERSIONS;

    return y;
}

static void _tsDrawCross(uint16_t x, uint16_t y) {
	gdispDrawLine(x-15, y, x-2, y, White);
	gdispDrawLine(x+2, y, x+15, y, White);
	gdispDrawLine(x, y-15, x, y-2, White);
	gdispDrawLine(x, y+2, x, y+15, White);
  
	gdispDrawLine(x-15, y+15, x-7, y+15, RGB2COLOR(184,158,131));
	gdispDrawLine(x-15, y+7, x-15, y+15, RGB2COLOR(184,158,131));

	gdispDrawLine(x-15, y-15, x-7, y-15, RGB2COLOR(184,158,131));
	gdispDrawLine(x-15, y-7, x-15, y-15, RGB2COLOR(184,158,131));

	gdispDrawLine(x+7, y+15, x+15, y+15, RGB2COLOR(184,158,131));
	gdispDrawLine(x+15, y+7, x+15, y+15, RGB2COLOR(184,158,131));

	gdispDrawLine(x+7, y-15, x+15, y-15, RGB2COLOR(184,158,131));
	gdispDrawLine(x+15, y-15, x+15, y-7, RGB2COLOR(184,158,131));    
}

static void _tsTransform(coord_t *x, coord_t *y) {
	*x = (coord_t) (cal->ax * (*x) + cal->bx * (*y) + cal->cx);
	*y = (coord_t) (cal->ay * (*x) + cal->by * (*y) + cal->cy);
}

static void _tsDo3PointCalibration(const coord_t (*cross)[2], coord_t (*points)[2], cal_t *c) {
	float dx, dx0, dx1, dx2, dy0, dy1, dy2;

	/* Compute all the required determinants */
	dx = ((float)(points[0][0] - points[2][0])) * ((float)(points[1][1] - points[2][1]))
		- ((float)(points[1][0] - points[2][0])) * ((float)(points[0][1] - points[2][1]));

	dx0 = ((float)(cross[0][0] - cross[2][0])) * ((float)(points[1][1] - points[2][1]))
		- ((float)(cross[1][0] - cross[2][0])) * ((float)(points[0][1] - points[2][1]));

	dx1 = ((float)(points[0][0] - points[2][0])) * ((float)(cross[1][0] - cross[2][0]))
		- ((float)(points[1][0] - points[2][0])) * ((float)(cross[0][0] - cross[2][0]));

	dx2 = cross[0][0] * ((float)points[1][0] * (float)points[2][1] - (float)points[2][0] * (float)points[1][1]) -
		cross[1][0] * ((float)points[0][0] * (float)points[2][1] - (float)points[2][0] * (float)points[0][1]) +
		cross[2][0] * ((float)points[0][0] * (float)points[1][1] - (float)points[1][0] * (float)points[0][1]);

	dy0 = ((float)(cross[0][1] - cross[2][1]))  *  ((float)(points[1][1] - points[2][1]))
		- ((float)(cross[1][1] - cross[2][1]))  *  ((float)(points[0][1] - points[2][1]));

	dy1  = ((float)(points[0][0] - points[2][0]))  *  ((float)(cross[1][1] - cross[2][1]))
		- ((float)(points[1][0] - points[2][0]))  *  ((float)(cross[0][1] - cross[2][1]));

	dy2  = cross[0][1] * ((float)points[1][0] * (float)points[2][1] - (float)points[2][0] * (float)points[1][1]) -
		cross[1][1] * ((float)points[0][0] * (float)points[2][1] - (float)points[2][0] * (float)points[0][1]) +
		cross[2][1] * ((float)points[0][0] * (float)points[1][1] - (float)points[1][0] * (float)points[0][1]);

	/* Now, calculate all the required coefficients */
	c->ax = dx0 / dx;
	c->bx = dx1 / dx;
	c->cx = dx2 / dx;

	c->ay = dy0 / dx;
	c->by = dy1 / dx;
	c->cy = dy2 / dx;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief	Touchscreen Driver initialization.
 * @note	Calling this function automatically invokes a tsCalibration()
 *
 * @param[in] ts	The touchscreen driver struct
 *
 * @api
 */
void tsInit(const TouchscreenDriver *ts) {
	/* Initialise Mutex */
	//MUTEX_INIT

	/* Initialise driver */
	//MUTEX_ENTER
	ts_lld_init(ts);
	//MUTEX_EXIT

	#if TOUCHSCREEN_STORE_CALIBRATION
		cal = ts_restore_calibration_lld();
		if(cal != NULL)
			return;		// All done
	#endif

	cal = (struct cal_t*)chHeapAlloc(NULL, sizeof(struct cal_t));
	if(cal == NULL)
		return;

	tsCalibrate();
}

/**
 * @brief   Get the X-Coordinate, relative to screen zero point.
 *
 * @return  The X position in pixels.
 *
 * @api
 */
coord_t tsReadX(void) {
	coord_t x, y;

#if TOUCHSCREEN_XY_INVERTED
	x = _tsReadRealY();
	y = _tsReadRealX();
#else
	x = _tsReadRealX();
	y = _tsReadRealY();
#endif

	_tsTransform(&x, &y);

    switch(gdispGetOrientation()) {
        case GDISP_ROTATE_0:
            return x;
        case GDISP_ROTATE_90:
            return y;
        case GDISP_ROTATE_180:
            return gdispGetWidth() - x - 1;
        case GDISP_ROTATE_270:
            return gdispGetWidth() - y - 1;
    }

    return 0;
}

/**
 * @brief   Get the X-Coordinate, relative to screen zero point.
 *
 * @return  The Y position in pixels.
 *
 * @api
 */
coord_t tsReadY(void) {
    coord_t x, y;

#if TOUCHSCREEN_XY_INVERTED
    x = _tsReadRealY();
    y = _tsReadRealX();
#else
    x = _tsReadRealX();
    y = _tsReadRealY();
#endif

    _tsTransform(&x, &y);

    switch(gdispGetOrientation()) {
        case GDISP_ROTATE_0:
            return y;
        case GDISP_ROTATE_90:
            return gdispGetHeight() - x - 1;
        case GDISP_ROTATE_180:
            return gdispGetHeight() - y - 1;
        case GDISP_ROTATE_270:
            return x;
    }

    return 0;
}

/**
 * @brief	Get the pressure.
 *
 * @return	The pressure.
 *
 * @api
 */
#if TOUCHSCREEN_HAS_PRESSURE || defined(__DOXYGEN__)
	uint16_t tsReadZ(void) {
		/* ToDo */
		return (ts_lld_read_z());
	}
#endif

/**
 * @brief	Returns if touchscreen is pressed or not
 *
 * @return	TRUE if pressed, FALSE otherwise
 *
 * @api
 */
#if TOUCHSCREEN_HAS_PRESSED || defined(__DOXYGEN__)
	bool_t tsPressed(void) {
		return ts_lld_pressed();
	}
#endif

/* Define maximum no. of times to sample the calibration point */
#define MAX_CAL_SAMPLES     10

/**
 * @brief	Function to calibrate touchscreen
 * @details   This function interactively performs calibration of the touchscreen
 *          using 3-point calibration algorithm. Optionally, it also verifies
 *          the accuracy of the calibration coefficients obtained if the symbol
 *          TOUCHSCREEN_VERIFY_CALIBRATION is defined in the configuration.
 *
 * @note	You don't have to call this function manually. It gets invoked by tsInit()
 *
 * @api
 */
void tsCalibrate(void) {
	const uint16_t height  =  gdispGetHeight();
	const uint16_t width  =  gdispGetWidth();
	const coord_t cross[][2]  =  {{(width / 4), (height / 4)},
                                    {(width - (width / 4)) , (height / 4)},
                                    {(width - (width / 4)) , (height - (height / 4))},
                                    {(width / 2), (height / 2)}}; /* Check point */
	coord_t points[4][2];
	int32_t px, py;
	uint8_t i, j;

	#if GDISP_NEED_CONTROL
	gdispSetOrientation(GDISP_ROTATE_0);
	#endif

	gdispClear(Blue);

	gdispFillStringBox(0, 5, gdispGetWidth(), 30, "Calibration", &fontUI2Double,  White, Blue, justifyCenter);

#if TOUCHSCREEN_VERIFY_CALIBRATION
calibrate:
	for(i = 0; i < 4; i++) {
#else
	for(i = 0; i < 3; i++) {
#endif
		_tsDrawCross(cross[i][0], cross[i][1]);

		while(!tsPressed())
			chThdSleepMilliseconds(2);        /* Be nice to other threads*/
	
		chThdSleepMilliseconds(20);         /* Allow screen to settle */

		/* Take a little more samples per point and their average
		 * for precise calibration */
		px = py = 0;
		j = 0;

		while(j < MAX_CAL_SAMPLES) {
			if(tsPressed()) {	/* We have valid pointer data */    
				#if TOUCHSCREEN_XY_INVERTED
				py += _tsReadRealX();
				px += _tsReadRealY();
				#else
				px += _tsReadRealX();
				py += _tsReadRealY();
				#endif

				j++;
			}
		}
	
		points[i][0] = px / j;
		points[i][1] = py / j;

		chThdSleepMilliseconds(100);

		while(tsPressed())
			chThdSleepMilliseconds(2);	/* Be nice to other threads*/

		gdispFillArea(cross[i][0] - 15, cross[i][1] - 15, 42, 42, Blue);
	}

	/* Apply 3 point calibration algorithm */
	_tsDo3PointCalibration(cross, points, cal);

	#if TOUCHSCREEN_VERIFY_CALIBRATION
	 /* Verification of correctness of calibration (optional) :
	 *  See if the 4th point (Middle of the screen) coincides with the calibrated
	 *  result. If point is with +/- 2 pixel margin, then successful calibration
	 *  Else, start from the beginning.
	 */

	/* Transform the co-ordinates */
	_tpTransform(&points[3][0], &points[3][1]);

	/* Calculate the delta */
	px = (points[3][0] - cross[3][0]) * (points[3][0] - cross[3][0]) +
		(points[3][1] - cross[3][1]) * (points[3][1] - cross[3][1]);

	if(px > 4)
		goto calibrate;
	#endif

	/* If enabled, serialize the calibration values for storage */
	#if TOUCHSCREEN_STORE_CALIBRATION
	ts_store_calibration_lld(cal);
	#endif
}

#endif /* GFX_USE_TOUCHSCREEN */
/** @} */

