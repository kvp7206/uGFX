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

#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"
#include "fonts.h"
#include "fastMath.h"

#if !defined(LCD_USE_FSMC) && !defined(LCD_USE_GPIO) && !defined(LCD_USE_SPI)
#include "glcdconf.h"
#endif

#include "ssd1289_lld.h"
#include "s6d1121_lld.h"

#define PORTRAIT	(lcdGetOrientation() == portrait || lcdGetOrientation() == portraitInv)
#define LANDSCAPE	(lcdGetOrientation() == landscape || lcdGetOrientation() == landscapeInv)

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define RGB565CONVERT(red, green, blue) \
(uint16_t)( (( red   >> 3 ) << 11 ) | (( green >> 2 ) << 5  ) | ( blue  >> 3 ))

enum orientation {portrait, landscape, portraitInv, landscapeInv};
enum filled {frame, filled};
enum transparency {solid, transparent};
enum powermode {powerOff, powerOn, sleepOn, sleepOff};

typedef const uint8_t* font_t;

// A few macros
#define lcdGetFontHeight(font)		(font[FONT_TABLE_HEIGHT_IDX])

/**
 * @brief   Structure representing a GLCD driver.
 */
typedef struct GLCDDriver GLCDDriver;

struct GLCDDriver {
};

enum glcd_result {	GLCD_DONE,
					GLCD_FAILED,
					GLCD_PROGRESS,
				};

typedef enum glcd_result glcd_result_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Core functions */
void lcdInit(GLCDDriver *GLCDD1);
glcd_result_t lcdClear(uint16_t color);
glcd_result_t lcdSetOrientation(uint8_t newOrientation);
glcd_result_t lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
glcd_result_t lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
glcd_result_t lcdWriteArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *buffer, size_t n);
glcd_result_t lcdSetPowerMode(uint8_t powerMode);

/* Drawing functions */
glcd_result_t lcdDrawPixel(uint16_t x, uint16_t y, uint16_t point);
void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdDrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t filled, uint16_t color);
void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, font_t font, uint16_t fontColor, uint16_t bkColor);
void lcdDrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color);
void lcdDrawEllipse(uint16_t x, uint16_t y, uint16_t a, uint16_t b, uint8_t filled, uint16_t color);

/* Text Rendering Functions */
uint16_t lcdDrawChar(uint16_t cx, uint16_t cy, char c, font_t font, uint16_t color, uint16_t bkcolor, bool_t tpText);
void lcdDrawString(uint16_t x, uint16_t y, const char *str, font_t font, uint16_t color, uint16_t bkcolor, bool_t tpText);

/* Character measuring functions */
uint16_t lcdMeasureChar(char c, font_t font);
uint16_t lcdMeasureString(const char* str, font_t font);

/* Size and orientation related */
uint16_t lcdGetHeight(void);
uint16_t lcdGetWidth(void);
uint16_t lcdGetOrientation(void);

/* BGR->RGB and pixel readback */
uint16_t lcdBGR2RGB(uint16_t color);
uint16_t lcdGetPixelColor(uint16_t x, uint16_t y);

/* Scrolling function */
glcd_result_t lcdVerticalScroll(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t lines);

#ifdef __cplusplus
}
#endif

#endif
