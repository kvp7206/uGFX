#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"
#include "fonts.h"
#include "ssd1289_lld.h"
#include "s6d1121_lld.h"

#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   320

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

// For text rendering only
extern uint16_t bgcolor, fgcolor;
extern uint16_t cx, cy;
extern const uint8_t* font;

// A few macros
#define lcdGotoXY(x,y)				{ cx=x; cy=y; }
#define lcdGetCurFontHeight()		(font[FONT_TABLE_HEIGHT_IDX])

void lcdInit(void);

void lcdClear(uint16_t color);
void lcdSetOrientation(uint8_t newOrientation);
void lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t point);
void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdDrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t filled, uint16_t color);
void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, uint16_t fontColor, uint16_t bkColor);
void lcdDrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color);

void lcdSetFontTransparency(uint8_t transparency);
void lcdSetFont(const uint8_t *newFont);
void lcdDrawChar(char c);
void lcdPutString(const char *str);
void lcdDrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bkcolor);
void lcdLineBreak(void);

uint16_t lcdMeasureChar(char c);
uint16_t lcdMeasureString(const char* str);

uint16_t lcdGetHeight(void);
uint16_t lcdGetWidth(void);
uint16_t lcdGetOrientation(void);

uint16_t lcdBGR2RGB(uint16_t color);
uint16_t lcdGetPixelColor(uint16_t x, uint16_t y);

#endif
