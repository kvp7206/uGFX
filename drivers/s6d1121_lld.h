#ifndef S6D1121_H
#define S6D1121_H
#include "glcd.h"

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

