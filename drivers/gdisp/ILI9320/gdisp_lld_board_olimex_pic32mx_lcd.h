/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/ILI9320/gdisp_lld_board_olimex_pic32mx_lcd.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ILI9325 display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

#ifndef noinline
#define noinline __attribute__((noinline))
#endif

static void gdisp_lld_init_board(void) {
  // RST
  palSetPadMode(IOPORTA, 7, PAL_MODE_OUTPUT);
  palClearPad(IOPORTA, 7);

  // RS
  palSetPadMode(IOPORTA, 10, PAL_MODE_OUTPUT);
  palSetPad(IOPORTA, 10);

  // CS
  palSetPadMode(IOPORTA, 9, PAL_MODE_OUTPUT);
  palClearPad(IOPORTA, 9);

  // Backlight
  palSetPadMode(IOPORTD, 3, PAL_MODE_OUTPUT);
  palSetPad(IOPORTD, 3);

  // PMP setup
  PMMODE = 0;
  PMAEN = 0;
  PMCON = 0;
  PMMODEbits.MODE = 2;
  PMMODEbits.WAITB = 0;
  PMMODEbits.WAITM = 1;
  PMMODEbits.WAITE = 0;
  PMCONbits.CSF = 0;
  PMCONbits.PTRDEN = 1;
  PMCONbits.PTWREN = 1;
  PMMODEbits.MODE16 = 1;
  PMCONbits.PMPEN = 1;

  palClearPad(IOPORTA, 9);
}

#define PmpWaitBusy()   do {} while (PMMODEbits.BUSY)

static noinline void gdisp_lld_reset_pin(bool_t state) {
  if (state)
    palClearPad(IOPORTA, 7);
  else
    palSetPad(IOPORTA, 7);
}

static noinline void gdisp_lld_write_index(uint16_t data) {
  volatile uint16_t dummy;

  PmpWaitBusy();
  palClearPad(IOPORTA, 10);
  PMDIN = data;
  PmpWaitBusy();
  palSetPad(IOPORTA, 10);

  dummy = PMDIN;
  (void)dummy;
}

static noinline void gdisp_lld_write_data(uint16_t data) {
  PMDIN = data;
  PmpWaitBusy();
}

static noinline uint16_t gdisp_lld_read_data(void) {
  PmpWaitBusy();
  return PMDIN;
}

/* if not available, just ignore the argument and return */
static void gdisp_lld_backlight(uint8_t percentage) {
  if (percentage)
    palClearPad(IOPORTD, 3);
  else
    palSetPad(IOPORTD, 3);
}

static inline void acquire_bus(void) {
  /* Nothing to do here since LCD is the only device on that bus */
}

static inline void release_bus(void) {
  /* Nothing to do here since LCD is the only device on that bus */
}
#endif /* GDISP_LLD_BOARD_H */
/** @} */

