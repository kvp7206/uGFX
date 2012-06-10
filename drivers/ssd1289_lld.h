#ifndef SSD1289_H
#define SSD1289_H

#include "glcd.h"

#ifndef LCD_USE_SSD1289

uint16_t lld_lcdInit(void);
void lld_lcdSetCursor(uint16_t x, uint16_t y);
void lld_lcdSetOrientation(uint8_t newOrientation);
void lld_lcdSetWindow(x0, y0, x1, y1);
void lld_lcdClear(uint16_t color);
void lld_lcdDrawPixel(uint16_t x, uint16_ty, uint16_t color);
uint16_t lld_lcdGetPixelColor(uint16_t x, uint16_t y);
uint16_t lld_lcdGetHeight(void);
uint16_t lld_lcdGetWidth(void);

#endif
#endif
