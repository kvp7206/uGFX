#ifndef SSD1289_H
#define SSD1289_H

#include "glcd.h"
#include "glcdconf.h"

#ifdef LCD_USE_SSD1289

#define Set_CS		palSetPad(LCD_CMD_PORT, LCD_CS);
#define Clr_CS		palClearPad(LCD_CMD_PORT, LCD_CS);
#define Set_RS		palSetPad(LCD_CMD_PORT, LCD_RS);
#define Clr_RS		palClearPad(LCD_CMD_PORT, LCD_RS);
#define Set_WR		palSetPad(LCD_CMD_PORT, LCD_WR);
#define Clr_WR		palClearPad(LCD_CMD_PORT, LCD_WR);
#define Set_RD		palSetPad(LCD_CMD_PORT, LCD_RD);
#define Clr_RD		palClearPad(LCD_CMD_PORT, LCD_RD);

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

#endif
#endif

