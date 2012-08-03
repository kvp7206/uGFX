/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GLCDCONF_H
#define GLCDCONF_H

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

/***** LCD CONTROLLER *****/
#define LCD_USE_SSD1289
// #define LCD_USE_S6D1121


/***** LCD INTERFACE *****/
#define LCD_USE_GPIO
// #define LCD_USE_SPI
// #define LCD_USE_FSMC


/***** TOUCHPAD CONTROLLER *****/
// #define TOUCHPAD_USE_ADS7843
#define TOUCHPAD_USE_XPT2046


/***** GUI OPTIONS *****/
#define GUI_USE_ANIMATIONS

#endif

