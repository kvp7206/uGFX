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
#include "gdisp.h"
#include "touchpad.h"

static const SPIConfig spicfg = { 
    NULL,			// no callback
    GPIOC,			// CS PORT
    6,				// CS PIN
    SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

TOUCHPADDriver TOUCHPADD1 = { 
    &SPID1,			// SPI driver
    &spicfg,		// SPI config
    GPIO,			// IRQ PORT
    4,				// IRQ PIN
    TRUE			// start SPI
};

int main(void) {
	halInit();
	chSysInit();

	gdispInit();

	tpInit(&TOUCHPADD1);
	tpCalibrate();

	gdispClear(Black);	

	while (TRUE) {
		gdispDrawPixel(tpReadX(), tpReadY(), Green);
	}
}

