/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

/* The handles for our two Windows */
GHandle GW1, GW2;

int main(void) {
    halInit();
    chSysInit();
   	coord_t		i, j;

    /* Initialize and clear the display */
    gdispInit();
    gdispClear(Lime);

    /* Create two windows */
    GW1 = gwinCreateWindow(NULL, 20, 10, 200, 150);
    GW2 = gwinCreateWindow(NULL, 50, 190, 150, 100);

    /* Set fore- and background colors for both windows */
    gwinSetColor(GW1, Black);
    gwinSetBgColor(GW1, White);
    gwinSetColor(GW2, White);
    gwinSetBgColor(GW2, Blue);

    /* Clear both windows - to set background color */
    gwinClear(GW1);
    gwinClear(GW2);

    gwinDrawLine (GW1, 5, 30, 150, 110);
    for(i=5, j=0; i < 200 && j < 150; i+=3, j+=i/20)
        	gwinDrawPixel (GW1, i, j);

    /*  
     * Draw two filled circles at the same coordinate
     * of each window to demonstrate the relative coordinates
     * of windows
     */
    gwinFillCircle(GW1, 20, 20, 15);
    gwinFillCircle(GW2, 20, 20, 15);

    while(TRUE) {
        chThdSleepMilliseconds(500);
    }   
}

