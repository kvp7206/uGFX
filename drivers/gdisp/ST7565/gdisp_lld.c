/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GDISP || defined(__DOXYGEN__)


/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"
#include "st7565.h"
#include "gdisp_lld_board.h"


/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
#define GDISP_SCREEN_HEIGHT		64
#endif
#ifndef GDISP_SCREEN_WIDTH
#define GDISP_SCREEN_WIDTH		128
#endif

#define GDISP_INITIAL_CONTRAST		0xFF

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define delay(us)         gfxSleepMicroseconds(us)
#define delay_ms(ms)      gfxSleepMilliseconds(ms)

// The memory buffer for the display
static uint8_t gdisp_buffer[GDISP_SCREEN_HEIGHT * GDISP_SCREEN_WIDTH / 8];

/** Set the display to normal or inverse.
 *  @param[in] value 0 for normal mode, or 1 for inverse mode.
 *  @notapi
 */
static void invert_display(uint8_t i) {
  write_cmd(i ? ST7565_INVERT_DISPLAY : ST7565_POSITIVE_DISPLAY);
}

/** Turn the whole display off.
 *	Sends the display to sleep, but leaves RAM intact.
 *	@notapi
 */
static void display_off(void) {
  write_cmd(ST7565_DISPLAY_OFF);
}

/** Turn the whole display on.
 * 	Wakes up this display following a sleep() call.
 *	@notapi
 */
static void display_on(void) {
  write_cmd(ST7565_DISPLAY_ON);
}

/** Set the display contrast.
 *  @param[in] value The contrast, from 1 to 63.
 *	@notapi
 */
static void set_contrast(uint8_t value) {
  write_cmd(ST7565_CONTRAST);
  write_cmd(value & 0x3F);
}

/** Set the display start line.  This is the line at which the display will start rendering.
 *  @param[in] value A value from 0 to 63 denoting the line to start at.
 *	@notapi
 */
static void set_display_start_line(unsigned char value) {
  write_cmd(ST7565_START_LINE | value);
}

static void gdisp_lld_display(void) {
  uint8_t p;
  set_display_start_line(0);

  for (p = 0; p < 8; p++) {
    write_cmd(ST7565_PAGE | p);
    write_cmd(ST7565_COLUMN_MSB | 0);
    write_cmd(ST7565_COLUMN_LSB | 0);
    write_cmd(ST7565_RMW);
    write_data(&gdisp_buffer[p * GDISP_SCREEN_WIDTH], GDISP_SCREEN_WIDTH);
  }
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */
/*
 The following 2 routines are required.
 All other routines are optional.
 */

/**
 * @brief   Low level GDISP driver initialization.
 *
 * @notapi
 */
bool_t gdisp_lld_init(void) {
  // Initialize your display
  init_board();

  // Hardware reset.
  setpin_reset(TRUE);
  delay_ms(10);
  setpin_reset(FALSE);
  delay_ms(1);

  write_cmd(ST7565_LCD_BIAS_7);
  write_cmd(ST7565_ADC_NORMAL);
  write_cmd(ST7565_COM_SCAN_INC);
  set_display_start_line(0);

  set_contrast(32);
  write_cmd(ST7565_RESISTOR_RATIO | 0x3);

  // turn on voltage converter (VC=1, VR=0, VF=0)
  write_cmd(ST7565_POWER_CONTROL | 0x04);
  delay_ms(50);
  // turn on voltage regulator (VC=1, VR=1, VF=0)
  write_cmd(ST7565_POWER_CONTROL | 0x06);
  delay_ms(50);
  // turn on voltage follower (VC=1, VR=1, VF=1)
  write_cmd(ST7565_POWER_CONTROL | 0x07);
  delay_ms(50);

  display_on();
  write_cmd(ST7565_ALLON_NORMAL);
  invert_display(0);// Disable Inversion of display.

  write_cmd(ST7565_RMW);
  gdisp_lld_display();

  // Initialize the GDISP structure
  GDISP.Width = GDISP_SCREEN_WIDTH;
  GDISP.Height = GDISP_SCREEN_HEIGHT;
  GDISP.Orientation = GDISP_ROTATE_0;
  GDISP.Powermode = powerOn;
  GDISP.Contrast = 50;
#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
  GDISP.clipx0 = 0;
  GDISP.clipy0 = 0;
  GDISP.clipx1 = GDISP.Width;
  GDISP.clipy1 = GDISP.Height;
#endif
  return TRUE;
}

/**
 * @brief   Draws a pixel on the display.
 *
 * @param[in] x        X location of the pixel
 * @param[in] y        Y location of the pixel
 * @param[in] color    The color of the pixel
 *
 * @notapi
 */
void gdisp_lld_draw_pixel(coord_t x, coord_t y, color_t color) {
#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
  if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
#endif

  if (color == 1)
  gdisp_buffer[x+ (y/8)*GDISP_SCREEN_WIDTH] |= (1<<y%8);
  else
  gdisp_buffer[x+ (y/8)*GDISP_SCREEN_WIDTH] &= ~(1<<y%8);
}

/* ---- Optional Routines ---- */
/*
 All the below routines are optional.
 Defining them will increase speed but everything
 will work if they are not defined.
 If you are not using a routine - turn it off using
 the appropriate GDISP_HARDWARE_XXXX macro.
 Don't bother coding for obvious similar routines if
 there is no performance penalty as the emulation software
 makes a good job of using similar routines.
 eg. If gfillarea() is defined there is little
 point in defining clear() unless the
 performance bonus is significant.
 For good performance it is suggested to implement
 fillarea() and blitarea().
 */

#if (GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL) || defined(__DOXYGEN__)
/**
 * @brief   Driver Control
 * @details Unsupported control codes are ignored.
 * @note    The value parameter should always be typecast to (void *).
 * @note    There are some predefined and some specific to the low level driver.
 * @note    GDISP_CONTROL_POWER         - Takes a gdisp_powermode_t
 *          GDISP_CONTROL_ORIENTATION   - Takes a gdisp_orientation_t
 *          GDISP_CONTROL_BACKLIGHT 	- Takes an int from 0 to 100. For a driver
 *                                        that only supports off/on anything other
 *                                        than zero is on.
 *          GDISP_CONTROL_CONTRAST      - Takes an int from 0 to 100.
 *          GDISP_CONTROL_LLD           - Low level driver control constants start at
 *                                        this value.
 *
 * @param[in] what		What to do.
 * @param[in] value 	The value to use (always cast to a void *).
 *
 * @notapi
 */
void gdisp_lld_control(unsigned what, void *value) {
  switch(what) {
    case GDISP_CONTROL_POWER:
      if (GDISP.Powermode == (gdisp_powermode_t)value)
        return;

      switch((gdisp_powermode_t)value) {
        case powerOff:
          display_off();
          break;
        case powerSleep:
          display_off();
          break;
        case powerDeepSleep:
          display_off();
          break;
        case powerOn:
          display_on();
          break;
        default:
          return;
      }
      GDISP.Powermode = (gdisp_powermode_t)value;
      return;

    case GDISP_CONTROL_BACKLIGHT:
      set_backlight((uint8_t)(size_t)value);
      return;

    case GDISP_CONTROL_CONTRAST:
      if ((unsigned)value > 100) value = (void*)100;
      if (GDISP.Contrast == (uint8_t)((float)((size_t)value) * 63.0/100.0))
        return;
      set_contrast((uint8_t)((float)((size_t)value) * 63.0/100.0) );
      GDISP.Contrast = (unsigned)value;
      return;

    case GDISP_CONTROL_LLD_FLUSH:
      gdisp_lld_display();
      return;
  }
}
#endif // GDISP_NEED_CONTROL

#endif // GFX_USE_GDISP
/** @} */

