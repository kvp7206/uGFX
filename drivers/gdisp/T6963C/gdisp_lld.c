/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/ILI9320/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the ILI9320 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "stdint.h"
#include "string.h"
#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include "stm32f4xx.h"
#include "gdisp_lld_config.h"


#if 1//GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

#include "gdisp_lld_board_ThunderCryer.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/
/* This controller is only ever used with a 240 x 320 display */
#if defined(GDISP_SCREEN_HEIGHT)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_SCREEN_WIDTH		240
#define GDISP_SCREEN_HEIGHT		128

#define GDISP_INITIAL_CONTRAST	50
#define GDISP_INITIAL_BACKLIGHT	100

#define GLCD_NUMBER_OF_LINES            GDISP_SCREEN_HEIGHT
#define GLCD_PIXELS_PER_LINE            GDISP_SCREEN_WIDTH
#define GLCD_FONT_WIDTH                 8

#define GLCD_GRAPHIC_AREA               (GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
#define GLCD_TEXT_AREA                  (GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH)
#define GLCD_GRAPHIC_SIZE               (GLCD_GRAPHIC_AREA * GLCD_NUMBER_OF_LINES)
#define GLCD_TEXT_SIZE                  (GLCD_TEXT_AREA * (GLCD_NUMBER_OF_LINES/8))

#define GLCD_TEXT_HOME                  0
#define GLCD_GRAPHIC_HOME               (GLCD_TEXT_HOME + GLCD_TEXT_SIZE)
#define GLCD_OFFSET_REGISTER            2
#define  GLCD_EXTERNAL_CG_HOME          (GLCD_OFFSET_REGISTER << 11)


#define T6963_SET_CURSOR_POINTER        0x21
#define T6963_SET_OFFSET_REGISTER       0x22
#define T6963_SET_ADDRESS_POINTER       0x24

#define T6963_SET_TEXT_HOME_ADDRESS     0x40
#define T6963_SET_TEXT_AREA             0x41
#define T6963_SET_GRAPHIC_HOME_ADDRESS  0x42
#define T6963_SET_GRAPHIC_AREA          0x43

#define T6963_MODE_SET                  0x80

#define T6963_DISPLAY_MODE              0x90
    #define T6963_CURSOR_BLINK_ON       0x01
    #define T6963_CURSOR_DISPLAY_ON     0x02
    #define T6963_TEXT_DISPLAY_ON       0x04
    #define T6963_GRAPHIC_DISPLAY_ON    0x08


#define T6963_CURSOR_PATTERN_SELECT     0xA0
    #define T6963_CURSOR_1_LINE         0x00
    #define T6963_CURSOR_2_LINE         0x01
    #define T6963_CURSOR_3_LINE         0x02
    #define T6963_CURSOR_4_LINE         0x03
    #define T6963_CURSOR_5_LINE         0x04
    #define T6963_CURSOR_6_LINE         0x05
    #define T6963_CURSOR_7_LINE         0x06
    #define T6963_CURSOR_8_LINE         0x07

#define T6963_SET_DATA_AUTO_WRITE       0xB0
#define T6963_SET_DATA_AUTO_READ        0xB1
#define T6963_AUTO_RESET                0xB2
#define T6963_DATA_WRITE_AND_INCREMENT  0xC0
#define T6963_DATA_READ_AND_INCREMENT   0xC1
#define T6963_DATA_WRITE_AND_DECREMENT  0xC2
#define T6963_DATA_READ_AND_DECREMENT   0xC3
#define T6963_DATA_WRITE_AND_NONVARIALBE 0xC4
#define T6963_DATA_READ_AND_NONVARIABLE 0xC5

#define T6963_SCREEN_PEEK               0xE0
#define T6963_SCREEN_COPY               0xE8

#define T6963_BIT_SET                   0xF8
#define T6963_BIT_RESET                 0xF0

#define GDISP_LLD_THREAD_STACK_SIZE     1024



/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
static uint8_t FrameBuffer[GLCD_NUMBER_OF_LINES][(GLCD_PIXELS_PER_LINE/8)];
static WORKING_AREA(wa_gdisp_lld_thread, GDISP_LLD_THREAD_STACK_SIZE);
static Mutex gdisp_draw_mtx; /* Mutex declaration */
/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
  * @brief  polled timer supported us delay
  * @param  us micro seconds to wait depents on timer frequency
  */
static inline void gdisp_lld_lcdDelay(uint16_t us) {
  gptPolledDelay(&GPTD14, (us) );
}

/**
  * @brief  reads T6963C status byte
  * @param  None
  * @retval true for display ready
  */
bool gdisp_lld_check_status(void)
{

    uint16_t tmp;
    // set data pins as input
    GLCD_DATA_INPUT;

    GLCD_CTRL_PORT->BSRR.H.clear = (GLCD_RD | GLCD_CE);
    //palClearPort(GLCD_CTRL_PORT, GLCD_RD | GLCD_CE);

    gdisp_lld_lcdDelay(c_iDelayFore);

    tmp = GLCD_DATA_PORT->IDR;
    tmp = ((tmp & GLCD_DATA_PORT_MASK) >> GLCD_DATA_OFFSET);

    //palSetPort(GLCD_CTRL_PORT,  GLCD_RD | GLCD_CE);
    GLCD_CTRL_PORT->BSRR.H.set = (GLCD_RD | GLCD_CE);



    GLCD_DATA_OUTPUT;

    return ((tmp&0x03)==0x03);
}

static inline void gdisp_lld_init_board(void) {
  // init us delay timer
  gptStart(&GPTD14,&gptConf14);
  /* activate Backlight PWM */
  pwmStart(&PWMD9,&pwmconf9);
  pwmEnableChannel(&PWMD9, 0, 0);
}


/**
  * @brief  Reset the Display
  * @param  None
  * @retval None
  */
static inline void gdisp_lld_reset(void) {
  //palClearPort(GLCD_CTRL_PORT, GLCD_RESET | GLCD_CE );
  GLCD_CTRL_PORT->BSRR.H.clear =(GLCD_RESET | GLCD_CE);

  chThdSleepMilliseconds(100);

  //palSetPort(GLCD_CTRL_PORT, GLCD_RESET | GLCD_CE );
  GLCD_CTRL_PORT->BSRR.H.set = (GLCD_RESET | GLCD_CE);
}

/**
  * @brief  writes a command
  * @param  command
  * @retval None
  */
static inline void gdisp_lld_write_command(uint16_t cmd) {
  while(!gdisp_lld_check_status());

  GLCD_DATA_PORT->BSRR.H.clear = GLCD_DATA_PORT_MASK; //Clear Data pins
  GLCD_DATA_PORT->BSRR.H.set = (cmd << (GLCD_DATA_OFFSET)); // lowbyte will stay as is in this write

  //palClearPort(GLCD_CTRL_PORT, GLCD_WR | GLCD_CE);
  GLCD_CTRL_PORT->BSRR.H.clear = (GLCD_WR | GLCD_CE);

  gdisp_lld_lcdDelay(c_iDelayFore);   // time for display to read the data

  //palSetPort(GLCD_CTRL_PORT , GLCD_WR | GLCD_CE);
  GLCD_CTRL_PORT->BSRR.H.set = (GLCD_WR | GLCD_CE);

  gdisp_lld_lcdDelay(c_iDelayAfter);
}

/**
  * @brief  writes data
  * @param  data
  * @retval None
  */
static inline void gdisp_lld_write_data(uint16_t data) {
  while(!gdisp_lld_check_status());

  GLCD_DATA_PORT->BSRR.H.clear = GLCD_DATA_PORT_MASK; //Clear Data pins
  GLCD_DATA_PORT->BSRR.H.set = ((uint16_t)data << GLCD_DATA_OFFSET); // lowbyte will stay as is in this write

  //palClearPort(GLCD_CTRL_PORT, GLCD_CD | GLCD_WR | GLCD_CE);
  GLCD_CTRL_PORT->BSRR.H.clear = (GLCD_CD | GLCD_WR | GLCD_CE);

  gdisp_lld_lcdDelay(c_iDelayFore);

  //palSetPort(GLCD_CTRL_PORT, GLCD_CD | GLCD_WR | GLCD_CE);
  GLCD_CTRL_PORT->BSRR.H.set = (GLCD_CD | GLCD_WR | GLCD_CE);

}

/**
  * @brief  writes data with increment of the address pointer
  * @param  data
  * @retval None
  */
static inline void gdisp_lld_write_data_inc(uint16_t data) {
  gdisp_lld_write_data(data);
  gdisp_lld_write_command(T6963_DATA_WRITE_AND_INCREMENT);
}

/**
  * @brief  reads data from display
  * @param  None
  * @retval data from display
  */
static inline uint16_t gdisp_lld_read_data(void) {
  uint16_t tmp;
  while(!gdisp_lld_check_status());

  GLCD_DATA_INPUT;

  GLCD_CTRL_PORT->BSRR.H.clear = (GLCD_RD | GLCD_CD | GLCD_CE);

  gdisp_lld_lcdDelay(c_iDelayFore);

  tmp = ((palReadPort(GLCD_DATA_PORT) & GLCD_DATA_PORT_MASK) >> GLCD_DATA_OFFSET);

  GLCD_CTRL_PORT->BSRR.H.set =  GLCD_RD | GLCD_CD | GLCD_CE ;

  GLCD_DATA_OUTPUT;
  return (unsigned char)tmp;
}

/**
  * @brief  sets backlight
  * @param  percent     Backlight level in percent
  */
static inline void gdisp_lld_backlight(uint8_t percent) {
    pwmEnableChannel(&PWMD9, 0, percent );
}

/**
  * @brief  sets the address pointer to a specific address
  * @param  address     new address for the address pointer
  */
static inline void gdisp_lld_set_address_pointer(uint16_t address){
  address += 2; // display offset
  gdisp_lld_write_data(address & 0xFF);
  gdisp_lld_write_data(address >> 8);
  gdisp_lld_write_command(T6963_SET_ADDRESS_POINTER);
}

/**
  * @brief  sets the cursor to a specific coordinate
  * @param  x   new cursor x coordinate
  * @param  y   new cursor y coordinate
  */
static inline void gdisp_lld_set_cursor(uint16_t x, uint16_t y){
  uint16_t address;

  address = GLCD_GRAPHIC_HOME + (x / GLCD_FONT_WIDTH) + (GLCD_GRAPHIC_AREA * y);
  gdisp_lld_set_address_pointer(address);
}

/*
 * draw display farme buffer thread
 */
static msg_t gdisp_lld_thread(void *arg) {
	static int i;
	static systime_t tCycleStart;

	while (TRUE) {
		/* get system time */
		tCycleStart = chTimeNow();

		/*
		 * only draw buffer when not drawing
		 */
		chMtxLock(&gdisp_draw_mtx);

		// Graphics and Text are different mem pools in this Controller
		gdisp_lld_set_address_pointer(GLCD_GRAPHIC_HOME);

		for(i = 0; i < GLCD_GRAPHIC_SIZE; i++)
		{
			/* draw the buffer */
			gdisp_lld_write_data_inc(FrameBuffer[i]);
		}
		/*
		 * release mutex
		 */
		chMtxUnLock();

		/* wait for cyclic run through */
		chThdSleepUntil(MS2ST(40) + tCycleStart);
	}
}

bool_t gdisp_lld_init(void) {
	/* Initialise your display */
	gdisp_lld_init_board();

	gdisp_lld_reset();

	// Graphic home address
	gdisp_lld_write_data(GLCD_GRAPHIC_HOME & 0xFF);
	gdisp_lld_write_data(GLCD_GRAPHIC_HOME >> 8);
	gdisp_lld_write_command(T6963_SET_GRAPHIC_HOME_ADDRESS);

	//graphic line length
	gdisp_lld_write_data(GLCD_GRAPHIC_AREA);
	gdisp_lld_write_data(0x00);
	gdisp_lld_write_command(T6963_SET_GRAPHIC_AREA);

	//text home address
	gdisp_lld_write_data(GLCD_TEXT_HOME & 0xFF);
	gdisp_lld_write_data(GLCD_TEXT_HOME >> 8);
	gdisp_lld_write_command(T6963_SET_TEXT_HOME_ADDRESS);

	//text line length
	gdisp_lld_write_data(GLCD_TEXT_AREA);
	gdisp_lld_write_data(0x00);
	gdisp_lld_write_command(T6963_SET_TEXT_AREA);

	//write offset register  (no effect)
	gdisp_lld_write_data(GLCD_OFFSET_REGISTER);
	gdisp_lld_write_data(0x00);
	gdisp_lld_write_command(T6963_SET_OFFSET_REGISTER);

	// display in XOR Mode
	gdisp_lld_write_command(T6963_MODE_SET | 1);

	//Graphic and no Text mode
	gdisp_lld_write_command(T6963_DISPLAY_MODE | T6963_GRAPHIC_DISPLAY_ON );


	// Turn on the backlight
	gdisp_lld_backlight(GDISP_INITIAL_BACKLIGHT);
	
    /* Initialise the GDISP structure */
    GDISP.Width = GDISP_SCREEN_WIDTH;
    GDISP.Height = GDISP_SCREEN_HEIGHT;
    GDISP.Orientation = GDISP_ROTATE_0;
    GDISP.Powermode = powerOn;
    GDISP.Backlight = GDISP_INITIAL_BACKLIGHT;
    gdisp_lld_backlight(GDISP.Backlight);
    GDISP.Contrast = GDISP_INITIAL_CONTRAST;
    #if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
	GDISP.clipx0 = 0;
	GDISP.clipy0 = 0;
	GDISP.clipx1 = GDISP.Width;
	GDISP.clipy1 = GDISP.Height;
    #endif


	chMtxInit(&gdisp_draw_mtx); /* Mutex initialization before use */

	/* Starting the frame buffer draw thread*/
	(void)chThdCreateStatic(wa_gdisp_lld_thread, sizeof(wa_gdisp_lld_thread),
						  NORMALPRIO, gdisp_lld_thread, NULL);

	return TRUE;
}

void gdisp_lld_draw_pixel(coord_t x, coord_t y, color_t color) {
#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
	if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
#endif
	unsigned char tmp;
	tmp = (GLCD_FONT_WIDTH - 1) - (x % GLCD_FONT_WIDTH);

	chMtxLock(&gdisp_draw_mtx);
	/* lock framebuffer access */
	if(color){ /* reset bit */
		FrameBuffer[(x / GLCD_FONT_WIDTH) + (GLCD_GRAPHIC_AREA * y)] &= ~(1<<tmp);
	}else{ /* set bit */
		FrameBuffer[(x / GLCD_FONT_WIDTH) + (GLCD_GRAPHIC_AREA * y)] |= (1<<tmp);
	}
	/* unlock framebuffer access */
	chMtxUnLock();
}

#if GDISP_HARDWARE_CLEARS || defined(__DOXYGEN__)
void gdisp_lld_clear(color_t color) {
	chMtxLock(&gdisp_draw_mtx);
	/* lock framebuffer access */
	if(color){
		memset(FrameBuffer,0,sizeof(FrameBuffer));
	}else{
		memset(FrameBuffer,0xFF,sizeof(FrameBuffer));
	}
	/* unlock framebuffer access */
	chMtxUnLock();
}
#endif

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		chMtxLock(&gdisp_draw_mtx);
		/* lock framebuffer access */

		// TODO: unsuported,

		/* unlock framebuffer access */
		chMtxUnLock();

	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	void gdisp_lld_blit_area_ex(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		coord_t endx, endy;
		unsigned lg;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();

		endx = srcx + cx;
		endy = y + cy;
		lg = srccx - cx;
		buffer += srcx + srcy * srccx;
		for(; y < endy; y++, buffer += lg)
			for(x=srcx; x < endx; x++)
				lld_lcdWriteData(*buffer++);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	color_t gdisp_lld_get_pixel_color(coord_t x, coord_t y) {
		color_t color;

#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
#endif

		unsigned char tmp;
		tmp = (GLCD_FONT_WIDTH - 1) - (x % GLCD_FONT_WIDTH);

		chMtxLock(&gdisp_draw_mtx);
		/* lock framebuffer access */
		color = FrameBuffer[(x / GLCD_FONT_WIDTH) + (GLCD_GRAPHIC_AREA * y)] & (1<<tmp);

		/* unlock framebuffer access */
		chMtxUnLock();

		if(color){
			color = Black;
		}else{
			color = White;
		}

		return color;
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	void gdisp_lld_vertical_scroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		static color_t buf[((GDISP_SCREEN_HEIGHT > GDISP_SCREEN_WIDTH ) ? GDISP_SCREEN_HEIGHT : GDISP_SCREEN_WIDTH)];
		coord_t row0, row1;
		unsigned i, gap, abslines;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		abslines = lines < 0 ? -lines : lines;

		if (abslines >= cy) {
			abslines = cy;
			gap = 0;
		} else {
			gap = cy - abslines;
			for(i = 0; i < gap; i++) {
				if(lines > 0) {
					row0 = y + i + lines;
					row1 = y + i;
				} else {
					row0 = (y - i - 1) + lines;
					row1 = (y - i - 1);
				}

				/* read row0 into the buffer and then write at row1*/
				lld_lcdSetViewPort(x, row0, cx, 1);
				lld_lcdReadStreamStart();
				lld_lcdReadStream(buf, cx);
				lld_lcdReadStreamStop();

				lld_lcdSetViewPort(x, row1, cx, 1);
				lld_lcdWriteStreamStart();
				lld_lcdWriteStream(buf, cx);
				lld_lcdWriteStreamStop();
			}
		}

		/* fill the remaining gap */
		lld_lcdSetViewPort(x, lines > 0 ? (y+gap) : y, cx, abslines);
		lld_lcdWriteStreamStart();
		gap = cx*abslines;
		for(i = 0; i < gap; i++) lld_lcdWriteData(bgcolor);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL) || defined(__DOXYGEN__)
	void gdisp_lld_control(unsigned what, void *value) {
	  switch(what) {
	  case GDISP_CONTROL_POWER:
	    if(GDISP.Powermode == (gdisp_powermode_t)value)
	      return;
	    switch((gdisp_powermode_t)value) {
	    case powerOff:

	      gdisp_lld_backlight(0);
	      break;

	    case powerOn:
	      //*************Power On sequence ******************//

	      gdisp_lld_backlight(GDISP.Backlight);

	      if(GDISP.Powermode != powerSleep || GDISP.Powermode != powerDeepSleep)
	        gdisp_lld_init();
	      else
	        gdisp_lld_write_command(T6963_DISPLAY_MODE | T6963_GRAPHIC_DISPLAY_ON);
	      break;

	    case powerSleep:
	      //deactivate graphic display
	      gdisp_lld_write_command(T6963_DISPLAY_MODE);
	      gdisp_lld_backlight(0);
	      break;

	    case powerDeepSleep:
	      //deactivate graphic display
	      gdisp_lld_write_command(T6963_DISPLAY_MODE);
	      gdisp_lld_backlight(0);
	      break;

	    default:
	      return;
	    }
	    GDISP.Powermode = (gdisp_powermode_t)value;
	    return;


	    case GDISP_CONTROL_BACKLIGHT:
	      if((unsigned)value > 100) value = (void *)100;
	      gdisp_lld_backlight((unsigned)value);
	      GDISP.Backlight = (unsigned)value;
	      break;

	    default:
	      return;
	  }
	}

#endif

#endif /* GFX_USE_GDISP */
/** @} */

