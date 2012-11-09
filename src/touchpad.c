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
 * @file	src/touchpad.c
 * @brief	Touchpad Driver code.
 *
 * @addtogroup TOUCHPAD
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "touchpad.h"

#if GFX_USE_TOUCHPAD || defined(__DOXYGEN__)

#if TOUCHPAD_STORE_CALIBRATION
extern void tp_store_calibration_lld(struct cal_t *cal);
extern struct cal_t *tp_restore_calibration_lld(void);
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
static struct cal_t *cal;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static coord_t _tpReadRealX(void) {
    int32_t results = 0;
    int16_t i;
    coord_t x;

    for(i = 0; i < CONVERSIONS; i++) {
        results += tp_lld_read_x();
    }

    /* Take the average of the readings */
    x = results / CONVERSIONS;

    return x;
}

static coord_t _tpReadRealY(void) {
    int32_t results = 0;
    int16_t i;
    coord_t y;

    for(i = 0; i < CONVERSIONS; i++) {
        results += tp_lld_read_y();
    }

    /* Take the average of the readings */
    y = results / CONVERSIONS;

    return y;
}

static void _tpDrawCross(uint16_t x, uint16_t y) {
    gdispDrawLine(x-15, y, x-2, y, 0xffff);
    gdispDrawLine(x+2, y, x+15, y, 0xffff);
    gdispDrawLine(x, y-15, x, y-2, 0xffff);
    gdispDrawLine(x, y+2, x, y+15, 0xffff);
  
    gdispDrawLine(x-15, y+15, x-7, y+15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x-15, y+7, x-15, y+15, RGB565CONVERT(184,158,131));

    gdispDrawLine(x-15, y-15, x-7, y-15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x-15, y-7, x-15, y-15, RGB565CONVERT(184,158,131));

    gdispDrawLine(x+7, y+15, x+15, y+15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x+15, y+7, x+15, y+15, RGB565CONVERT(184,158,131));

    gdispDrawLine(x+7, y-15, x+15, y-15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x+15, y-15, x+15, y-7, RGB565CONVERT(184,158,131));    
}

static void _tpTransform(coord_t *x, coord_t *y) {
  *x = (coord_t) (cal->ax * (*x) + cal->bx * (*y) + cal->cx);
  *y = (coord_t) (cal->ay * (*x) + cal->by * (*y) + cal->cy);
}

static void _tpDo3PointCalibration(const coord_t (*cross)[2], coord_t (*points)[2], cal_t *c) {
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
 * @brief	Touchpad Driver initialization.
 * @note    This function is NOT currently implicitly invoked by @p halInit().
 *          It must be called manually.
 *
 * @param[in] tp	The touchpad driver struct
 *
 * @api
 */
void tpInit(const TOUCHPADDriver *tp) {
	cal = (struct cal_t*)chHeapAlloc(NULL, sizeof(struct cal_t));
	if(cal == NULL)
		return;

	/* Initialise Mutex */
	//MUTEX_INIT

	/* Initialise driver */
	//MUTEX_ENTER
	tp_lld_init(tp);
	//MUTEX_EXIT

	#if TOUCHPAD_STORE_CALIBRATION
		cal = tp_restore_calibration_lld();
		if(cal == NULL) {
			cal = (struct cal_t*)chHeapAlloc(NULL, sizeof(struct cal_t));
			tpCalibrate();
		}
	#endif
}

/**
 * @brief   Get the X-Coordinate, relative to screen zero point.
 *
 * @return  The X position in pixels.
 *
 * @api
 */
coord_t tpReadX(void) {
    coord_t x, y;

#if TOUCHPAD_XY_INVERTED == TRUE
    x = _tpReadRealY();
    y = _tpReadRealX();
#else
    x = _tpReadRealX();
    y = _tpReadRealY();
#endif

    _tpTransform(&x, &y);

    switch(gdispGetOrientation()) {
        case GDISP_ROTATE_0:
            return x;
        case GDISP_ROTATE_90:
            return y;
        case GDISP_ROTATE_180:
            return GDISP_SCREEN_WIDTH - x - 1;
        case GDISP_ROTATE_270:
            return GDISP_SCREEN_HEIGHT - y - 1;
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
coord_t tpReadY(void) {
    coord_t x, y;

#if TOUCHPAD_XY_INVERTED == TRUE
    x = _tpReadRealY();
    y = _tpReadRealX();
#else
    x = _tpReadRealX();
    y = _tpReadRealY();
#endif

    _tpTransform(&x, &y);

    switch(gdispGetOrientation()) {
        case GDISP_ROTATE_0:
            return y;
        case GDISP_ROTATE_90:
            return GDISP_SCREEN_WIDTH - x - 1;
        case GDISP_ROTATE_180:
            return GDISP_SCREEN_HEIGHT - y - 1;
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
#if TOUCHPAD_HAS_PRESSURE || defined(__DOXYGEN__)
	uint16_t tpReadZ(void) {
		/* ToDo */
		return (tp_lld_read_z());
	}
#endif

/**
 * @brief	Returns if touchpad is pressed or not
 *
 * @return	TRUE if pressed, FALSE otherwise
 *
 * @api
 */
#if TOUCHPAD_HAS_IRQ || defined(__DOXYGEN__)
	bool_t tpIRQ(void) {
		return tp_lld_irq();
	}
#endif

/* Define maximum no. of times to sample the calibration point */
#define MAX_CAL_SAMPLES     10

/**
 * @brief	Function to calibrate touchscreen
 * @details   This function interactively performs calibration of the touchscreen
 *          using 3-point calibration algorithm. Optionally, it also verifies
 *          the accuracy of the calibration coefficients obtained if the symbol
 *          TOUCHPAD_VERIFY_CALIBRATION is defined in the configuration.
 *
 * @api
 */
void tpCalibrate(void) {
    const uint16_t height  =  gdispGetHeight();
    const uint16_t width  =  gdispGetWidth();
    const coord_t cross[][2]  =  {{(width / 4), (height / 4)},
                                    {(width - (width / 4)) , (height / 4)},
                                    {(width - (width / 4)) , (height - (height / 4))},
                                    {(width / 2), (height / 2)}}; /* Check point */
    coord_t points[4][2];
    int32_t px, py;
    uint8_t i, j;

    gdispSetOrientation(GDISP_ROTATE_0);
    gdispClear(Blue);

    gdispFillStringBox(0, 5, gdispGetWidth(), 30, "Calibration", &fontUI2Double,  White, Blue, justifyCenter);

#if defined(TOUCHPAD_VERIFY_CALIBRATION)
calibrate:
    for(i = 0; i < 4; i++) {
#else
    for(i = 0; i < 3; i++) {
#endif
      _tpDrawCross(cross[i][0], cross[i][1]);

      while(!tpIRQ())
        chThdSleepMilliseconds(2);        /* Be nice to other threads*/

      chThdSleepMilliseconds(20);         /* Allow screen to settle */

      /* Take a little more samples per point and their average
       * for precise calibration */
      px = py = 0;

      j = 0;
      while (j < MAX_CAL_SAMPLES) {
        if (tpIRQ()) {
          /* We have valid pointer data */
          px += _tpReadRealX();
          py += _tpReadRealY();

          j++;
        }
      }

      points[i][0] = px / j;
      points[i][1] = py / j;

      chThdSleepMilliseconds(100);

      while(tpIRQ())
        chThdSleepMilliseconds(2);        /* Be nice to other threads*/

      gdispFillArea(cross[i][0] - 15, cross[i][1] - 15, 42, 42, Blue);
    }

    /* Apply 3 point calibration algorithm */
    _tpDo3PointCalibration(cross, points, cal);

#if defined(TOUCHPAD_VERIFY_CALIBRATION)
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

    if (px > 4)
      goto calibrate;
#endif

    /* If enabled, serialize the calibration values for storage */
      #if TOUCHPAD_STORE_CALIBRATION
        tp_store_calibration_lld(cal);
      #endif
}

#endif /* GFX_USE_TOUCHPAD */
/** @} */

