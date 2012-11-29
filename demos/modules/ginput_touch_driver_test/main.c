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

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "gdisp.h"
#include "ginput.h"
#include "gwin.h"

static GConsoleObject			gc;
static GListener				gl;

/*------------------------------------------------------------------------*
 * GINPUT Touch Driver Calibrator.                                        *
 *------------------------------------------------------------------------*/
int main(void) {
	GSourceHandle			gs;
	GEventMouse				*pem;
	coord_t					swidth, sheight;
	GHandle					ghc;
	BaseSequentialStream	*gp;
	unsigned				testnum;

	halInit();			// Initialise the Hardware
	chSysInit();		// Initialize the OS
	gdispInit();		// Initialize the display

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();
	testnum = 0;

	// Create our title
	gdispFillStringBox(0, 0, swidth, 20, "Touch Calibration", &fontUI2, Red, White, justifyCenter);

	// Create our main display window
	ghc = gwinCreateConsole(&gc, 0, 20, swidth, sheight-20, &fontUI2);
	gwinClear(ghc);
	gp = gwinGetConsoleStream(ghc);

	// Initialize the mouse in our special no calibration mode.
	geventListenerInit(&gl);
	gs = ginputGetMouse(9999);
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	/*
	 * Test: Device Type
	 */

	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n%u. DEVICE TYPE\n\n", ++testnum);

	pem = (GEventMouse *)&gl.event;
	ginputGetMouseStatus(0, pem);

	gwinSetColor(ghc, White);
	chprintf(gp, "This is detected as a %s device\n\n",
		pem->type == GEVENT_MOUSE ? "MOUSE" : (pem->type == GEVENT_TOUCH ? "TOUCH" : "UNKNOWN"));

	chprintf(gp, "Press and release your finger (or mouse button) to move on to the next test.\n");

	do {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (pem->type != GEVENT_MOUSE && pem->type != GEVENT_TOUCH)		// Safety Check
			continue;

	} while (!(pem->meta & GMETA_MOUSE_UP));

	/*
	 * Test: Mouse raw reading jitter
	 */

	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n%u. GINPUT_MOUSE_READ_CYCLES\n\n", ++testnum);

	gwinSetColor(ghc, White);
	chprintf(gp, "Press on the surface (or press and hold the mouse button).\n\n");
	chprintf(gp, "Numbers will display in this window.\n"
			"Ensure that values don't jump around very much when your finger is stationary.\n\n"
			"Increasing GINPUT_MOUSE_READ_CYCLES helps reduce jitter but increases CPU usage.\n\n"
			"Releasing your finger (or mouse button) will move on to the next test.\n\n");

	// For this test turn on ALL mouse movement events
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA|GLISTEN_MOUSENOFILTER);

	do {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (pem->type != GEVENT_MOUSE && pem->type != GEVENT_TOUCH)		// Safety Check
			continue;

		if ((pem->current_buttons & GINPUT_MOUSE_BTN_LEFT))
			chprintf(gp, "%u:%u\n", pem->x, pem->y);
	} while (!(pem->meta & GMETA_MOUSE_UP));

	// Reset to just changed movements.
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	/*
	 * Test: Calibration
	 */

	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n%u. GINPUT_MOUSE_CALIBRATION_ERROR\n\n", ++testnum);
	gwinSetColor(ghc, Gray);
	chprintf(gp, "Ensure GINPUT_MOUSE_NEED_CALIBRATION = TRUE and GINPUT_MOUSE_CALIBRATION_ERROR is >= 0\n\n");
	gwinSetColor(ghc, White);
	chprintf(gp, "When you press and release the surface, calibration will start.\n");
	chprintf(gp, "You will be presented with a number of points to touch.\nPress them in turn.\n\n"
			"If the calibration repeatedly fails increase GINPUT_MOUSE_CALIBRATION_ERROR and try again.\n");

	do {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (pem->type != GEVENT_MOUSE && pem->type != GEVENT_TOUCH)		// Safety Check
			continue;

	} while (!(pem->meta & GMETA_MOUSE_UP));

	// Calibrate
	ginputCalibrateMouse(0);

	// Calibration uses the whole screen - re-establish our title
	gdispFillStringBox(0, 0, swidth, 20, "Touch Calibration", &fontUI2, Green, White, justifyCenter);

	/*
	 * Test: Mouse movement jitter
	 */

	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n%u. GINPUT_MOUSE_MOVE_JITTER\n\n", ++testnum);

	gwinSetColor(ghc, White);
	chprintf(gp, "Press firmly on the surface (or press and hold the mouse button) and move around as if to draw.\n\n");
	chprintf(gp, "Dots will display in this window. Ensure that when you stop moving your finger that "
			"new dots stop displaying.\nNew dots should only display when your finger is moving.\n\n"
			"Adjust GINPUT_MOUSE_MOVE_JITTER to the smallest value that this reliably works for.\n\n"
			"Releasing your finger (or mouse button) will move on to the next test.\n\n");

	do {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (pem->type != GEVENT_MOUSE && pem->type != GEVENT_TOUCH)		// Safety Check
			continue;

		if ((pem->current_buttons & GINPUT_MOUSE_BTN_LEFT))
			chprintf(gp, ".");
	} while (!(pem->meta & GMETA_MOUSE_UP));

	/*
	 * Test: Polling frequency
	 */

	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n%u. GINPUT_MOUSE_POLL_PERIOD\n\n", ++testnum);

	gwinSetColor(ghc, White);
	chprintf(gp, "Press firmly on the surface (or press and hold the mouse button) and move around as if to draw.\n\n");
	chprintf(gp, "A green line will follow your finger.\n"
			"Adjust GINPUT_MOUSE_POLL_PERIOD to the highest value that provides a line without "
			"gaps that are too big.\nDecreasing the value increases CPU usage.\n"
			"About 25 (millisecs) normally produces good results."
			"This test can be ignored for interrupt driven drivers.\n\n"
			"Releasing your finger (or mouse button) will move on to the next test.\n");

	do {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (pem->type != GEVENT_MOUSE && pem->type != GEVENT_TOUCH)		// Safety Check
			continue;

		if ((pem->current_buttons & GINPUT_MOUSE_BTN_LEFT))
			gdispDrawPixel(pem->x, pem->y, Green);
	} while (!(pem->meta & GMETA_MOUSE_UP));

	
	/*
	 * Test: Click Jitter
	 */

	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n%u. GINPUT_MOUSE_MAX_CLICK_JITTER\n\n", ++testnum);

	gwinSetColor(ghc, White);
	chprintf(gp, "Press and release the touch surface to \"click\".\nTry both short and long presses.\n");
	chprintf(gp, "For a mouse click with the left and right buttons.\n\n");
	chprintf(gp, "Dots will display in this window. A yellow dash is a left (or short) click. "
			"A red x is a right (or long) click.\n\n"
			"Adjust GINPUT_MOUSE_CLICK_JITTER to the smallest value that this reliably works for.\n"
			"Adjust GINPUT_MOUSE_CLICK_TIME to adjust distinguishing short vs long presses.\n"
			"TIME_INFINITE means there are no long presses (although a right mouse button will still work).\n\n"
			"Note: moving your finger (mouse) during a click cancels it."
			"This test does not end.\n\n");

	while(1) {
		pem = (GEventMouse *)geventEventWait(&gl, TIME_INFINITE);
		if (pem->type != GEVENT_MOUSE && pem->type != GEVENT_TOUCH)		// Safety Check
			continue;
			
		if ((pem->meta & GMETA_MOUSE_CLICK)) {
			gwinSetColor(ghc, Yellow);
			chprintf(gp, "-");
		}
		if ((pem->meta & GMETA_MOUSE_CXTCLICK)) {
			gwinSetColor(ghc, Red);
			chprintf(gp, "x");
		}
	}
}
