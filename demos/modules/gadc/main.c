/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * This demo demonstrates the use of the GADC module using it read both a microphone,
 * an analogue dial wheel and a temperature sensor.
 * The microphone gets read at high frequency to display a very simple oscilloscope.
 * The dial and temperature gets read at a low frequency to just print when
 * it changes value.
 *
 * It also demonstrates how to write your own custom GWIN window type.
 */
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "gfx.h"

/* Include our custom gwin oscilloscope */
#include "gwinosc.h"

/*
 * Match these to your hardware
 * If you don't have a DIAL device or a TEMP device - just don't define it.
 */
#define MY_MIC_DEVICE		GADC_PHYSDEV_MICROPHONE
#define MY_DIAL_DEVICE		GADC_PHYSDEV_DIAL
#define MY_TEMP_DEVICE		GADC_PHYSDEV_TEMPERATURE
#define MY_DIAL_JITTER		1
#define MY_TEMP_JITTER		3

/* Specify our timing parameters */
#define	MY_MIC_FREQUENCY	4000			/* 4khz */
#define MY_LS_DELAY			200				/* 200ms (5 times per second) for the dial and temperature */

/* The desired size for our scope window */
#define SCOPE_CX			64
#define SCOPE_CY			64

/* Data */
static GScopeObject			gScopeWindow;
static GConsoleObject		gTextWindow;
static GTimer				lsTimer;

#ifdef MY_DIAL_DEVICE
	static adcsample_t			dialvalue;
	static adcsample_t			lastdial = -(MY_DIAL_JITTER+1);

	/**
	 * We have got a dial reading - handle it
	 */
	static void GotDialReading(adcsample_t *buffer, void *param) {
		(void) buffer;

		/* Buffer should always point to "dialvalue" anyway */

		/* Remove jitter from the value */
		if ((dialvalue > lastdial && dialvalue - lastdial > MY_DIAL_JITTER)
				|| (lastdial > dialvalue && lastdial - dialvalue > MY_DIAL_JITTER)) {

			/* Write the value */
			chprintf((BaseSequentialStream *)param, "DIAL: %u\n", dialvalue);

			/* Save for next time */
			lastdial = dialvalue;
		}
	}
#endif

#ifdef MY_TEMP_DEVICE
	static adcsample_t			tempvalue;
	static adcsample_t			lasttemp = -(MY_TEMP_JITTER+1);

	/**
	 * We have got a temperature reading - handle it
	 */
	static void GotTempReading(adcsample_t *buffer, void *param) {
		(void) buffer;

		/* Buffer should always point to "tempvalue" anyway */

		/* Remove jitter from the value */
		if ((tempvalue > lasttemp && tempvalue - lasttemp > MY_TEMP_JITTER)
				|| (lasttemp > tempvalue && lasttemp - tempvalue > MY_TEMP_JITTER)) {

			/* Write the value */
			chprintf((BaseSequentialStream *)param, "TEMP: %u\n", tempvalue);

			/* Save for next time */
			lasttemp = tempvalue;
		}
	}
#endif

#if defined(MY_DIAL_DEVICE) || defined(MY_TEMP_DEVICE)
	/**
	 * Start a read of the dial and temperature
	 */
	static void LowSpeedTimer(void *param) {
		/* We are not checking for an error here - but who cares, this is just a demo */
		#ifdef MY_DIAL_DEVICE
			gadcLowSpeedStart(MY_DIAL_DEVICE, &dialvalue, GotDialReading, param);
		#endif
		#ifdef MY_TEMP_DEVICE
			gadcLowSpeedStart(MY_TEMP_DEVICE, &tempvalue, GotTempReading, param);
		#endif
	}
#endif

/*
 * Application entry point.
 */
int main(void) {
	GHandle					ghScope;
	coord_t					swidth, sheight;
	#if defined(MY_DIAL_DEVICE) || defined(MY_TEMP_DEVICE)
		GHandle					ghText;
		BaseSequentialStream	*gsText;
		font_t					font;
	#endif

	halInit();
	chSysInit();
	gdispInit();
	gdispClear(Black);

	/* Get the screen dimensions */
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	#if defined(MY_DIAL_DEVICE) || defined(MY_TEMP_DEVICE)
		/* Set up the console window we use for dial readings */
		font = gdispOpenFont("UI2");
		ghText = gwinCreateConsole(&gTextWindow, 0, 0, swidth-SCOPE_CX, sheight, font);
		gwinSetBgColor(ghText, Black);
		gwinSetColor(ghText, Yellow);
		gwinClear(ghText);
		gsText = gwinGetConsoleStream(ghText);

		/* Start our timer for reading the dial */
		gtimerInit(&lsTimer);
		gtimerStart(&lsTimer, LowSpeedTimer, gsText, TRUE, MY_LS_DELAY);
	#endif

	/* Set up the scope window in the top right on the screen */
	ghScope = gwinCreateScope(&gScopeWindow, swidth-SCOPE_CX, 0, SCOPE_CX, SCOPE_CY, MY_MIC_DEVICE, MY_MIC_FREQUENCY);
	gwinSetBgColor(ghScope, White);
	gwinSetColor(ghScope, Red);
	gwinClear(ghScope);

	/* Just keep displaying the scope traces */
	while (TRUE) {
		/**
		 * The function below internally performs a wait thus giving the timer thread a
		 * chance to run.
		 */
		gwinWaitForScopeTrace(ghScope);
	}
}
