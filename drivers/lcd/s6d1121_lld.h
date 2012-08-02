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

#ifndef S6D1121_H
#define S6D1121_H

#include "glcd.h"

#ifdef LCD_USE_S6D1121

// I/O assignments
#define LCD_BL_GPIO			GPIOB
#define LCD_BL_PIN			8

#define LCD_CS_GPIO			GPIOD
#define LCD_CS_PIN			7

#define LCD_RS_GPIO			GPIOD
#define LCD_RS_PIN			11

#define LCD_RST_GPIO		GPIOD
#define LCD_RST_PIN			10

#define LCD_RD_GPIO			GPIOD
#define LCD_RD_PIN			9

#define LCD_WR_GPIO			GPIOD
#define LCD_WR_PIN			8

#define LCD_D0_GPIO			GPIOD
#define LCD_D4_GPIO			GPIOE

#ifdef __cplusplus
extern "C" {
#endif


void lld_lcdInit(void);
void lld_lcdSetCursor(uint16_t x, uint16_t y);
void lld_lcdSetOrientation(uint8_t newOrientation);
void lld_lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lld_lcdClear(uint16_t color);
void lld_lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);
uint16_t lld_lcdGetPixelColor(uint16_t x, uint16_t y);
uint16_t lld_lcdGetOrientation(void);
uint16_t lld_lcdGetHeight(void);
uint16_t lld_lcdGetWidth(void);
void lld_lcdVerticalScroll(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t lines);

#ifdef __cplusplus
}
#endif

#endif
#endif

