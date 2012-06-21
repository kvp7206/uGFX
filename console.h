#ifndef CONSOLE_H
#define CONSOLE_H

/**
 * @brief   Structure representing a GLCD driver.
 */
typedef struct GLCDConsole GLCDConsole;

/**
 * @brief   @p GLCDConsole specific methods.
 */
#define _glcd_driver_methods                                              \
  _base_asynchronous_channel_methods

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p GLCDConsole virtual methods table.
 */
struct GLCDConsoleVMT {
  _glcd_driver_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   GLCD Console class.
 * @details This class extends @p BaseAsynchronousChannel by adding physical
 *          I/O queues.
 */
struct GLCDConsole {
  /** @brief Virtual Methods Table.*/
  const struct GLCDConsoleVMT *vmt;
  _base_asynchronous_channel_data
  /* WARNING: Do not add any data to this struct above this comment, only below */
  /* text buffer */
  uint8_t *buf;
  /* font */
  const uint8_t *font;
  /* lcd area to use */
  uint16_t x0,y0,x1,y1;
  /* current cursor position, in character coordinates (not pixels) */
  uint16_t cx,cy;
  /* console size in characters */
  uint16_t sx,sy;
  /* foreground and background colour */
  uint16_t bkcolor, color;
  /* font size in pixels */
  uint8_t fx,fy;
  /* buffer index */
  uint16_t bidx, blen, bstrt;
  bool_t full;
};

#ifdef __cplusplus
extern "C" {
#endif

msg_t lcdConsoleInit(GLCDConsole *console, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		const uint8_t *font, uint8_t *buffer, uint16_t bkcolor, uint16_t color);

msg_t lcdConsolePut(GLCDConsole *console, char c);
msg_t lcdConsoleWrite(GLCDConsole *console, uint8_t *bp, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */
