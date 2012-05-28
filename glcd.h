#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240

#define Set_CS			LCD_CMD_PORT->ODR  |= (1 << GPIOD_LCD_CS);
#define Clr_CS			LCD_CMD_PORT->ODR  &= ~(1 << GPIOD_LCD_CS);
#define Set_RS			LCD_CMD_PORT->ODR  |= (1 << GPIOD_LCD_RS);
#define Clr_RS			LCD_CMD_PORT->ODR  &= ~(1 << GPIOD_LCD_RS);
#define Set_WR			LCD_CMD_PORT->ODR  |= (1 << GPIOD_LCD_WR);
#define Clr_WR			LCD_CMD_PORT->ODR  &= ~(1 << GPIOD_LCD_WR);
#define Set_RD			LCD_CMD_PORT->ODR  |= (1 << GPIOD_LCD_RD);
#define Clr_RD			LCD_CMD_PORT->ODR  &= ~(1 << GPIOD_LCD_RD);

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

#define RGB565CONVERT(red, green, blue)\
(uint16_t)( (( red   >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 ))

enum orientation {portrait = 0, landscape = 1};

uint16_t lcdReadReg(uint16_t reg);
void lcdInit(void);
void lcdClear(uint16_t color);
void lcdTest(void);
uint16_t lcdBGR2RGB(uint16_t color);
uint16_t lcdGetPoint(uint16_t x,uint16_t y);
void lcdSetOrientation(uint8_t newOrientation);
void lcdDrawPixel(uint16_t x,uint16_t y,uint16_t point);
void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdDrawRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1, uint8_t filled, uint16_t color);
void lcdDrawCircle(uint8_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color);
void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdFillArea2(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdSetWindows(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong);
void lcdString(uint16_t x, uint16_t y, uint8_t *str,uint16_t color, uint16_t bkcolor);
void lcdChar(unsigned short x,unsigned short y,unsigned char c,unsigned short charcolor,unsigned short bkcolor);

#endif
