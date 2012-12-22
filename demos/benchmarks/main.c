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
#include "stdlib.h"
#include "string.h"
#include "gfx.h"

#define SCB_DEMCR (*(volatile unsigned *)0xE000EDFC)
#define CPU_RESET_CYCLECOUNTER do { SCB_DEMCR = SCB_DEMCR | 0x01000000; \
DWT_CYCCNT = 0; \
DWT_CTRL = DWT_CTRL | 1 ; } while(0)

static int uitoa(unsigned int value, char * buf, int max) {
    int n = 0;
    int i = 0;
    unsigned int tmp = 0;

    if (NULL == buf)
        return -3;

    if (2 > max)
        return -4;

    i=1;
    tmp = value;
    if (0 > tmp) {
        tmp *= -1;
        i++;
    }
    for (;;) {
        tmp /= 10;
        if (0 >= tmp)
            break;
        i++;
    }
    if (i >= max) {
        buf[0] = '?';
        buf[1] = 0x0;
        return 2;
    }

    n = i;
    tmp = value;
    if (0 > tmp) {
        tmp *= -1;
    }
    buf[i--] = 0x0;
    for (;;) {
        buf[i--] = (tmp % 10) + '0';
        tmp /= 10;
        if (0 >= tmp) {
            break;
        }
    }
    if (-1 != i) {
        buf[i--] = '-';
    }

    return n;
}

void benchmark(void) {
    uint32_t i, pixels, ms, pps;
    char pps_str[25];
	coord_t height, width, rx, ry, rcx, rcy;
    color_t random_color;
	font_t font;

    gdispSetOrientation(GDISP_ROTATE_90);
    gdispClear(Black);

	width = gdispGetWidth();
	height = gdispGetHeight();
    font = gdispOpenFont("UI2 Double");

	gdispDrawStringBox(0, 0, width, 30, "ChibiOS/GFX - Benchmark", font, White, justifyCenter);

	font = gdispOpenFont("UI2");
	gdispDrawStringBox(0, height/2, width, 30, "5000 random rectangles", font, White, justifyCenter);
	
	chThdSleepMilliseconds(3000);
	
	/* seed for the rand() */
	srand(DWT_CYCCNT);
	pixels = 0;

	CPU_RESET_CYCLECOUNTER;

	for (i = 0; i < 5000; i++) {
		random_color = (rand() % 65535);
		rx = (rand() % (width-10));
		ry = (rand() % (height-10));
		rcx = (rand() % ((width-rx)-10))+10;
		rcy = (rand() % ((height-ry)-10))+10;

		gdispFillArea(rx, ry, rcx, rcy, random_color);
		pixels += (rcx+1)*(rcy+1);
	}

	ms = DWT_CYCCNT / 168000;
	pps = (float)pixels/((float)ms/1000.0f);

	memset (pps_str, 0, sizeof(pps_str));
	uitoa(pps, pps_str, sizeof(pps_str));
	strcat(pps_str, " Pixels/s");

	font = gdispOpenFont("UI2 Double");
	gdispClear(Black);
	gdispDrawStringBox(0, 0, width, 30, "ChibiOS/GFX - Benchmark", font, White, justifyCenter);
	gdispDrawStringBox(0, height/2, width, 30, pps_str, font, White, justifyCenter);
	//gdispDrawString(20, height/2, pps_str, font, White);
}

int main(void) {
    halInit();
    chSysInit();
	gdispInit();
	
	benchmark();
    
	while(TRUE) {
        chThdSleepMilliseconds(500);
	}

	return 0;
}
