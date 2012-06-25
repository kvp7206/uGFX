#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"
#include "fonts.h"

#if !defined(LCD_USE_FSMC) && !defined(LCD_USE_GPIO) && !defined(LCD_USE_SPI)
#include "glcdconf.h"
#endif

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

typedef const uint8_t* font_t;

// A few macros
#define lcdGetFontHeight(font)		(font[FONT_TABLE_HEIGHT_IDX])

/**
 * @brief   Structure representing a GLCD driver.
 */
typedef struct GLCDDriver GLCDDriver;

struct GLCDDriver {
};

#ifdef __cplusplus
extern "C" {
#endif

/* Core functions */
void lcdInit(GLCDDriver *);

void lcdClear(uint16_t color);
void lcdSetOrientation(uint8_t newOrientation);
void lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdWriteArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *buffer, size_t n);

/* Drawing functions */
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t point);
void lcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcdDrawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t filled, uint16_t color);
void lcdDrawRectString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, font_t font, uint16_t fontColor, uint16_t bkColor);
void lcdDrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color);

/* Text Rendering Functions */
int lcdDrawChar(uint16_t cx, uint16_t cy, char c, font_t font, uint16_t color, uint16_t bkcolor, bool_t tpText);
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
void lcdVerticalScroll(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t lines);

#ifdef __cplusplus
}
#endif

#endif
