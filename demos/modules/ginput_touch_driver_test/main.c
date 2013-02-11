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
#include "gfx.h"

static GConsoleObject			gc;
static GButtonObject			gNext;
static GButtonObject			gPrev;
static GListener				gl;

/*------------------------------------------------------------------------*
 * GINPUT Touch Driver Calibrator.                                        *
 *------------------------------------------------------------------------*/
int main(void) {
	GSourceHandle			gs, gsNext, gsPrev;
	GEvent					*pe;
	GEventMouse				*pem;
	GEventGWinButton		*peb;
	coord_t					swidth, sheight;
	GHandle					ghc, ghNext, ghPrev;
	BaseSequentialStream	*gp;
	GEventType				deviceType;
	font_t					font;

	halInit();			// Initialise the Hardware
	chSysInit();		// Initialize the OS
	gdispInit();		// Initialize the display

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();
	ghNext = ghPrev = 0;

	// Create our title
	font = gdispOpenFont("UI2");
	gdispFillStringBox(0, 0, swidth, 20, "Touch Calibration", font, Red, White, justifyLeft);

	// Create our main display window
	ghc = gwinCreateConsole(&gc, 0, 20, swidth, sheight-20, font);
	gwinClear(ghc);
	gp = gwinGetConsoleStream(ghc);

	// Initialize the mouse in our special no calibration mode.
	geventListenerInit(&gl);
	gs = ginputGetMouse(9999);
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	/*
	 * Test: Device Type
	 */

StepDeviceType:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n1. DEVICE TYPE\n\n");

	pem = (GEventMouse *)&gl.event;
	ginputGetMouseStatus(0, pem);
	deviceType = pem->type;

	gwinSetColor(ghc, White);
	chprintf(gp, "This is detected as a %s device\n\n",
		deviceType == GEVENT_MOUSE ? "MOUSE" : (pem->type == GEVENT_TOUCH ? "TOUCH" : "UNKNOWN"));

	if (ghNext)
		chprintf(gp, "Press Next or Back to continue.\n");
	else if (deviceType == GEVENT_MOUSE)
		chprintf(gp, "Click the mouse button to move on to the next test.\n");
	else
		chprintf(gp, "Press and release your finger to move on to the next test.\n");

	while(1) {
		pe = geventEventWait(&gl, TIME_INFINITE);
		if (pe->type == GEVENT_GWIN_BUTTON) {
			peb = (GEventGWinButton *)pe;
			if (peb->button == ghPrev)
				goto StepClickJitter;
			if (peb->button == ghNext)
				break;
		}
		if (pe->type == GEVENT_MOUSE || pe->type == GEVENT_TOUCH) {
			pem = (GEventMouse *)pe;
			if (!ghNext && (pem->meta & GMETA_MOUSE_UP))
				break;
		}
	}

	/*
	 * Test: Mouse raw reading jitter
	 */

StepRawJitter:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n2. GINPUT_MOUSE_READ_CYCLES\n\n");

	gwinSetColor(ghc, White);
	if (deviceType == GEVENT_MOUSE)
		chprintf(gp, "Press and hold the mouse button.\n\n");
	else
		chprintf(gp, "Press and hold on the surface.\n\n");
	chprintf(gp, "Numbers will display in this window.\n"
			"Ensure that values don't jump around very much when your finger is stationary.\n\n"
			"Increasing GINPUT_MOUSE_READ_CYCLES helps reduce jitter but increases CPU usage.\n\n");

	if (ghNext)
		chprintf(gp, "Press Next or Back to continue.\n");
	else if (deviceType == GEVENT_MOUSE)
		chprintf(gp, "Release the mouse button to move on to the next test.\n");
	else
		chprintf(gp, "Release your finger to move on to the next test.\n");

	// For this test turn on ALL mouse movement events
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA|GLISTEN_MOUSENOFILTER);

	while(1) {
		pe = geventEventWait(&gl, TIME_INFINITE);
		if (pe->type == GEVENT_GWIN_BUTTON) {
			peb = (GEventGWinButton *)pe;
			if (peb->button == ghPrev)
				goto StepDeviceType;
			if (peb->button == ghNext)
				break;
		}
		if (pe->type == GEVENT_MOUSE || pe->type == GEVENT_TOUCH) {
			pem = (GEventMouse *)pe;
			if ((pem->current_buttons & GINPUT_MOUSE_BTN_LEFT))
				chprintf(gp, "%u:%u\n", pem->x, pem->y);
			if (!ghNext && (pem->meta & GMETA_MOUSE_UP))
				break;
		}
	}

	// Reset to just changed movements.
	geventAttachSource(&gl, gs, GLISTEN_MOUSEDOWNMOVES|GLISTEN_MOUSEMETA);

	/*
	 * Test: Calibration
	 */

StepCalibrate:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n3. GINPUT_MOUSE_CALIBRATION_ERROR\n\n");
	gwinSetColor(ghc, Gray);
	chprintf(gp, "Ensure GINPUT_MOUSE_NEED_CALIBRATION = TRUE and GINPUT_MOUSE_CALIBRATION_ERROR is >= 0\n\n");
	gwinSetColor(ghc, White);
	chprintf(gp, "You will be presented with a number of points to touch.\nPress them in turn.\n\n"
			"If the calibration repeatedly fails, increase GINPUT_MOUSE_CALIBRATION_ERROR and try again.\n\n");

	if (ghNext)
		chprintf(gp, "Press Next to start the calibration.\n");
	else if (deviceType == GEVENT_MOUSE)
		chprintf(gp, "Click the mouse button to start the calibration.\n");
	else
		chprintf(gp, "Press and release your finger to start the calibration.\n");

	while(1) {
		pe = geventEventWait(&gl, TIME_INFINITE);
		if (pe->type == GEVENT_GWIN_BUTTON) {
			peb = (GEventGWinButton *)pe;
			if (peb->button == ghPrev)
				goto StepRawJitter;
			if (peb->button == ghNext)
				break;
		}
		if (pe->type == GEVENT_MOUSE || pe->type == GEVENT_TOUCH) {
			pem = (GEventMouse *)pe;
			if (!ghNext && (pem->meta & GMETA_MOUSE_UP))
				break;
		}
	}

	// Calibrate
	ginputCalibrateMouse(0);

	/* From now on we can use Next and Previous Buttons */
	if (!ghNext) {

		ghNext = gwinCreateButton(&gNext, swidth-50, 0, 50, 20, font, GBTN_NORMAL);
		gwinSetButtonText(ghNext, "Next", FALSE);
		gsNext = gwinGetButtonSource(ghNext);
		geventAttachSource(&gl, gsNext, 0);
		gwinAttachButtonMouseSource(ghNext, gs);

		ghPrev = gwinCreateButton(&gPrev, swidth-100, 0, 50, 20, font, GBTN_NORMAL);
		gwinSetButtonText(ghPrev, "Back", FALSE);
		gsPrev = gwinGetButtonSource(ghPrev);
		geventAttachSource(&gl, gsPrev, 0);
		gwinAttachButtonMouseSource(ghPrev, gs);

#if 0
		{
			GSourceHandle			gsButton1, gsButton2;

			// Attach a couple of hardware toggle buttons to our Next and Back buttons as well.
			//	We can always use the mouse to trigger the buttons if you don't want to use hardware toggles.
			//	This code depends on your hardware. Turn it on only if you have
			//	defined a board definition for your toggle driver. Then change
			//	the next two lines to be correct for your hardware. The values
			//	below are correct for the Win32 toggle driver.
			gsButton1 = ginputGetToggle(GINPUT_TOGGLE_MOMENTARY1);
			gsButton2 = ginputGetToggle(GINPUT_TOGGLE_MOMENTARY2);
			gwinAttachButtonToggleSource(ghNext, gsButton2);
			gwinAttachButtonToggleSource(ghPrev, gsButton1);
		}
#endif
	}

	// Calibration used the whole screen - re-establish our title
	gdispFillStringBox(0, 0, swidth, 20, "Touch Calibration", font, Green, White, justifyLeft);
	gwinButtonDraw(ghNext);
	gwinButtonDraw(ghPrev);

	/*
	 * Test: Mouse movement jitter
	 */

StepJitter:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n4. GINPUT_MOUSE_MOVE_JITTER\n\n");

	gwinSetColor(ghc, White);
	if (deviceType == GEVENT_MOUSE)
		chprintf(gp, "Press and hold the mouse button and move around as if to draw.\n\n");
	else
		chprintf(gp, "Press firmly on the surface and move around as if to draw.\n\n");

	chprintf(gp, "Dots will display in this window. Ensure that when you stop moving your finger that "
			"new dots stop displaying.\nNew dots should only display when your finger is moving.\n\n"
			"Adjust GINPUT_MOUSE_MOVE_JITTER to the smallest value that this reliably works for.\n\n");
	chprintf(gp, "Press Next or Back to continue.\n\n");

	while(1) {
		pe = geventEventWait(&gl, TIME_INFINITE);
		if (pe->type == GEVENT_GWIN_BUTTON) {
			peb = (GEventGWinButton *)pe;
			if (peb->button == ghPrev)
				goto StepCalibrate;
			if (peb->button == ghNext)
				break;
		}
		if (pe->type == GEVENT_MOUSE || pe->type == GEVENT_TOUCH) {
			pem = (GEventMouse *)pe;
			if ((pem->current_buttons & GINPUT_MOUSE_BTN_LEFT))
				chprintf(gp, ".");
		}
	}

	/*
	 * Test: Polling frequency
	 */

StepPolling:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n5. GINPUT_MOUSE_POLL_PERIOD\n\n");

	gwinSetColor(ghc, White);
	chprintf(gp, "Press firmly on the surface (or press and hold the mouse button) and move around as if to draw.\n\n");
	chprintf(gp, "A green line will follow your finger.\n"
			"Adjust GINPUT_MOUSE_POLL_PERIOD to the highest value that provides a line without "
			"gaps that are too big.\nDecreasing the value increases CPU usage.\n"
			"About 25 (millisecs) normally produces good results."
			"This test can be ignored for interrupt driven drivers.\n\n");
	chprintf(gp, "Press Next or Back to continue.\n\n");

	while(1) {
		pe = geventEventWait(&gl, TIME_INFINITE);
		if (pe->type == GEVENT_GWIN_BUTTON) {
			peb = (GEventGWinButton *)pe;
			if (peb->button == ghPrev)
				goto StepJitter;
			if (peb->button == ghNext)
				break;
		}
		if (pe->type == GEVENT_MOUSE || pe->type == GEVENT_TOUCH) {
			pem = (GEventMouse *)pe;
			if ((pem->current_buttons & GINPUT_MOUSE_BTN_LEFT))
				gdispDrawPixel(pem->x, pem->y, Green);
		}
	}
	
	/*
	 * Test: Click Jitter
	 */

StepClickJitter:
	gwinClear(ghc);
	gwinSetColor(ghc, Yellow);
	chprintf(gp, "\n6. GINPUT_MOUSE_MAX_CLICK_JITTER\n\n");

	gwinSetColor(ghc, White);
	chprintf(gp, "Press and release the touch surface to \"click\".\nTry both short and long presses.\n");
	chprintf(gp, "For a mouse click with the left and right buttons.\n\n");
	chprintf(gp, "Dots will display in this window. A yellow dash is a left (or short) click. "
			"A red x is a right (or long) click.\n\n"
			"Adjust GINPUT_MOUSE_CLICK_JITTER to the smallest value that this reliably works for.\n"
			"Adjust GINPUT_MOUSE_CLICK_TIME to adjust distinguishing short vs long presses.\n"
			"TIME_INFINITE means there are no long presses (although a right mouse button will still work).\n\n"
			"Note: moving your finger (mouse) during a click cancels it.\n\n");
	chprintf(gp, "This is the last test but you can press Next or Back to continue.\n\n");

	while(1) {
		pe = geventEventWait(&gl, TIME_INFINITE);
		if (pe->type == GEVENT_GWIN_BUTTON) {
			peb = (GEventGWinButton *)pe;
			if (peb->button == ghPrev)
				goto StepPolling;
			if (peb->button == ghNext)
				break;
		}
		if (pe->type == GEVENT_MOUSE || pe->type == GEVENT_TOUCH) {
			pem = (GEventMouse *)pe;
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

	// Can't let this really exit
	goto StepDeviceType;
}
