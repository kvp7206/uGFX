#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240

#define Set_Cs        LCD_PORT->ODR  |= ( 1<<12 );
#define Clr_Cs        LCD_PORT->ODR  &= ~( 1<<12 );
#define Set_Rs        LCD_PORT->ODR  |= ( 1<<13 );
#define Clr_Rs        LCD_PORT->ODR  &= ~( 1<<13 );
#define Set_nWr       LCD_PORT->ODR  |= ( 1<<14 );
#define Clr_nWr       LCD_PORT->ODR  &= ~( 1<<14 );
#define Set_nRd       LCD_PORT->ODR  |= ( 1<<15 );
#define Clr_nRd       LCD_PORT->ODR  &= ~( 1<<15 );

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

void lcdInit(void);
void lcdClear(uint16_t Color);
void lcdTest(void);
uint16_t lcdBGR2RGB(uint16_t color);
uint16_t lcdGetPoint(uint16_t Xpos,uint16_t Ypos);
void lcdDrawPixel(uint16_t Xpos,uint16_t Ypos,uint16_t point);
void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdDrawRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1, uint8_t filled, uint16_t color);
void lcdDrawCircle(uint8_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color);
void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdFillArea2(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdSetWindows(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong);
void lcdString(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor);
void lcdChar(unsigned short Xpos,unsigned short Ypos,unsigned char c,unsigned short charColor,unsigned short bkColor);

#endif
