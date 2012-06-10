#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"
#include "drivers/ssd1289_lld.h"

#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   320

#define PORTRAIT	(lcdGetOrientation() == portrait || lcdGetOrientation() == portraitInv)
#define LANDSCAPE	(lcdGetOrientation() == landscape || lcdGetOrientation() == landscapeInv)

#define Set_CS			palSetPad(LCD_CMD_PORT, LCD_CS);
#define Clr_CS			palClearPad(LCD_CMD_PORT, LCD_CS);
#define Set_RS			palSetPad(LCD_CMD_PORT, LCD_RS);
#define Clr_RS			palClearPad(LCD_CMD_PORT, LCD_RS);
#define Set_WR			palSetPad(LCD_CMD_PORT, LCD_WR);
#define Clr_WR			palClearPad(LCD_CMD_PORT, LCD_WR);
#define Set_RD			palSetPad(LCD_CMD_PORT, LCD_RD);
#define Clr_RD			palClearPad(LCD_CMD_PORT, LCD_RD);

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

void lcdInit(void);
void lcdClear(uint16_t color);
void lcdSetOrientation(uint8_t newOrientation);
void lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t point);
void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdDrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t filled, uint16_t color);
void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t* str, uint16_t fontColor, uint16_t bkColor);
void lcdDrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color);
void lcdDrawString(uint16_t x, uint16_t y, unsigned char *str, uint16_t color, uint16_t bkColor);
void lcdDrawChar(uint16_t x, uint16_t y, unsigned char c, uint16_t charcolor, uint16_t bkColor);
uint16_t lcdGetHeight(void);
uint16_t lcdGetWidth(void);
uint16_t lcdGetOrientation(void);
uint16_t lcdBGR2RGB(uint16_t color);
uint16_t lcdGetPixelColor(uint16_t x, uint16_t y);

#endif
