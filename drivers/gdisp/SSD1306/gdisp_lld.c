/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

#include "gfx.h"

#include "SSD1306.h"

#if GFX_USE_GDISP || defined(__DOXYGEN__)

/* Include the emulation code for things we don't support */
#include "gdisp/lld/emulation.c"

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

// Include wiring specific header
#include "gdisp_lld_board_example_i2c.h"

// Some common routines and macros
#define delay(us)					gfxSleepMicroseconds(us)
#define delayms(ms)					gfxSleepMilliseconds(ms)

// The memory buffer for the display
static uint8_t gdisp_buffer[GDISP_SCREEN_HEIGHT * GDISP_SCREEN_WIDTH / 8];

/** Set the display to normal or inverse.
 *  @param[in] value 0 for normal mode, or 1 for inverse mode.
 *  @notapi
 */
static void invert_display(uint8_t i) {
	write_cmd(i ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

/** Turn the whole display off.
 *	Sends the display to sleep, but leaves RAM intact.
 *	@notapi
 */
static void display_off(){
	write_cmd(SSD1306_DISPLAYOFF);
}

/** Turn the whole display on.
 * 	Wakes up this display following a sleep() call.
 *	@notapi
 */
static void display_on()	{
	write_cmd(SSD1306_DISPLAYON);
}

/** Set the vertical shift by COM.
 * 	@param[in] value The number of rows to shift, from 0 - 63.
 *	@notapi
*/
static void set_display_offset(unsigned char value) {
	write_cmd(SSD1306_SETDISPLAYOFFSET);
	write_cmd(value & 0x3F);
}

/** Set the display contrast.
 *  @param[in] value The contrast, from 1 to 256.
 *	@notapi
 */
static void set_contrast(unsigned char value) {
	write_cmd(SSD1306_SETCONTRAST);
	write_cmd(value);
}

/** Set the display start line.  This is the line at which the display will start rendering.
 *  @param[in] value A value from 0 to 63 denoting the line to start at.
 *	@notapi
 */
static void set_display_start_line(unsigned char value) {
	write_cmd(SSD1306_SETSTARTLINE | value);
}

/** Set the segment remap state.  This allows the module to be addressed as if flipped horizontally.
 * NOTE: Changing this setting has no effect on data already in the module's GDDRAM.
 * @param[in] value 0 = column address 0 = segment 0 (the default), 1 = column address 127 = segment 0 (flipped).
 *	@notapi
 */
static void set_segment_remap(unsigned char value) {
	write_cmd(value ? SSD1306_SEGREMAP+1 : SSD1306_SEGREMAP);
}

/** Set the multiplex ratio.
 *  @param[in] value MUX will be set to (value+1). Valid values range from 15 to 63 - MUX 16 to 64.
 *	@notapi
 */
static void set_multiplex_ratio(unsigned char value) {
	write_cmd(SSD1306_SETMULTIPLEX);
	write_cmd(value & 0x3F);
}

/** Set COM output scan direction.  If the display is active, this will immediately vertically
 * flip the display.
 * @param[in] value 0 = Scan from COM0 (default), 1 = reversed (scan from COM[N-1]).
 *	@notapi
 */
static void set_com_output_scan_direction(unsigned char value) {
	write_cmd(value ? SSD1306_COMSCANDEC : SSD1306_COMSCANINC);
}

static void set_com_pins_hardware_configuration(unsigned char sequential, unsigned char lr_remap)	{
	write_cmd(SSD1306_SETCOMPINS);
	write_cmd(0x02 | ((sequential & 1) << 4) | ((lr_remap & 1) << 5));
}

/** Flip display content horizontally.
 * 	NOTE: This only flips display content, but doesn't turn the char writing around.
 * 		  You have to unmirror everything manually.
 * 	@param[in] value 0 = column address 0 = segment 0 (the default), 1 = column address 127 = segment 0 (flipped).
 *	@notapi
 */
static void flip_display(unsigned char enable) {
	if( enable && GDISP.Orientation == GDISP_ROTATE_0) {
		set_com_output_scan_direction(0);
		set_segment_remap(0);
		GDISP.Orientation = GDISP_ROTATE_0;
	}
	if( !enable && GDISP.Orientation == GDISP_ROTATE_180) {
		set_com_output_scan_direction(1);
		set_segment_remap(1);
		GDISP.Orientation = GDISP_ROTATE_180;
	}
	else
		return;
}

/** Perform a "no operation".
 *	@notapi
 */
static void nop() {
	write_cmd(0xE3);
}

/** Page Addressing Mode: Set the column start address register for
 * 	page addressing mode.
 *	@param[in] address The address (full byte).
 *	@notapi
 */
static void set_start_address_pam(unsigned char address)
{
	// "Set Lower Column Start Address for Page Addressing Mode"
	write_cmd(address & 0x0F);

	// "Set Higher Column Start Address for Page Addressing Mode"
	write_cmd((address << 4) & 0x0F);
}

/** Set memory addressing mode to the given value.
 *	@param[in] mode 0 for Horizontal addressing mode,\n 1 for Vertical addressing mode,\n or 2 for Page addressing mode (PAM).  2 is the default.
 *	@notapi
 */
static void set_memory_addressing_mode(unsigned char mode)
{
	write_cmd(SSD1306_MEMORYMODE);
	write_cmd(mode & 0x3);
}

/** Set column address range for horizontal/vertical addressing mode.
 *	 @param[in] start Column start address, 0 - 127.
 *	 @param[in] end Column end address, 0 - 127.
 *	@notapi
 */
static void set_column_address_hvam(unsigned char start, unsigned char end)
{
	write_cmd(SSD1306_HV_COLUMN_ADDRESS);
	write_cmd(start & 0x7F);
	write_cmd(end & 0x7F);
}

/** Set page start and end address for horizontal/vertical addressing mode.
 * 	@param[in] start The start page, 0 - 7.
 *	@param[in] end The end page, 0 - 7.
 *	@notapi
 */
static void set_page_address_hvam(unsigned char start, unsigned char end)
{
	write_cmd(SSD1306_HV_PAGE_ADDRESS);
	write_cmd(start & 0x07);
	write_cmd(end & 0x07);
}

/** Set the GDDRAM page start address for page addressing mode.
 *	@param[in] address The start page, 0 - 7.
 *	@notapi
 */
static void set_page_start_pam(unsigned char address)
{
	write_cmd(SSD1306_PAM_PAGE_START | (address & 0x07));
}

/** Set the display clock divide ratio and the oscillator frequency.
 * 	@param[in] ratio The divide ratio, default is 0.
 *	@param[in] frequency The oscillator frequency, 0 - 127. Default is 8.
 *	@notapi
 */
static void set_display_clock_ratio_and_frequency(unsigned char ratio, unsigned char frequency)
{
	write_cmd(SSD1306_SETDISPLAYCLOCKDIV);
	write_cmd((ratio & 0x0F) | ((frequency & 0x0F) << 4));
}

/** Set the precharge period.
 * 	@param[in] phase1 Phase 1 period in DCLK clocks.  1 - 15, default is 2.
 *	@param[in] phase2 Phase 2 period in DCLK clocks.  1 - 15, default is 2.
 *	@notapi
 */
static void set_precharge_period(unsigned char phase1, unsigned char phase2)
{
	write_cmd(SSD1306_SETPRECHARGE);
	write_cmd((phase1 & 0x0F) | ((phase2 & 0x0F ) << 4));
}

/** Set the Vcomh deselect level.
 *	@param[in] level @p 0 = 0.65 x Vcc, @p 1 = 0.77 x Vcc (default), @p 2 = 0.83 x Vcc.
 *	@notapi
 */
static void set_vcomh_deselect_level(unsigned char level)
{
	write_cmd(SSD1306_SETVCOMDETECT);
	write_cmd((level & 0x03) << 4);
}

/** Enable/disable charge pump.
 *	@param[in] enable 0 to disable, 1 to enable the internal charge pump.
 *	@notapi
 */
static void set_charge_pump(unsigned char enable)
{
	write_cmd(SSD1306_ENABLE_CHARGE_PUMP);
	write_cmd(enable ? 0x14 : 0x10);
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
	delayms(1);
	setpin_reset(FALSE);
	delayms(10);
	setpin_reset(TRUE);

	// Get the bus for the following initialization commands.
	acquire_bus();

	display_off();
	set_display_clock_ratio_and_frequency(0, 8);
	#if GDISP_SCREEN_HEIGHT == 64
		set_multiplex_ratio(0x3F);			// 1/64 duty
	#endif
	#if GDISP_SCREEN_HEIGHT == 32
		set_multiplex_ratio(0x1F); 			// 1/32 duty
	#endif
	set_precharge_period(0xF, 0x01);		//
	set_display_offset(0);					//
	set_display_start_line(0);					//
	set_charge_pump(1);						// Enable internal charge pump.
	set_memory_addressing_mode(0); 			// horizontal addressing mode; across then down //act like ks0108 (horizontal addressing mode)
	set_segment_remap(1);					//
	set_com_output_scan_direction(1);		//
	#if GDISP_SCREEN_HEIGHT == 64
		set_com_pins_hardware_configuration(1, 0);
	#endif
	#if GDISP_SCREEN_HEIGHT == 32
		set_com_pins_hardware_configuration(0, 1);
	#endif
	set_contrast(GDISP_INITIAL_CONTRAST);	// Set initial contrast.
	set_vcomh_deselect_level(1);			//
	display_on();							// Turn on OLED panel.
	invert_display(0);						// Disable Inversion of display.
	set_column_address_hvam(0, 127);		//
	set_page_address_hvam(0, 7);			//

	release_bus();

	gdisp_lld_display();

    // Initialize the GDISP structure
	GDISP.Width = GDISP_SCREEN_WIDTH;
	GDISP.Height = GDISP_SCREEN_HEIGHT;
	GDISP.Orientation = GDISP_ROTATE_0;
	GDISP.Powermode = powerOn;
	GDISP.Contrast = GDISP_INITIAL_CONTRAST;
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

	if (color == SSD1306_WHITE)
		gdisp_buffer[x+ (y/8)*GDISP_SCREEN_WIDTH] |=  (1<<y%8);
	else
		gdisp_buffer[x+ (y/8)*GDISP_SCREEN_WIDTH] &= ~(1<<y%8);
}

void gdisp_lld_display() {
	set_display_start_line(0);

	/* We're sending half a line in one X-mission.*/
	uint8_t command[GDISP_SCREEN_WIDTH/2],
			cmdLength = sizeof(command)/sizeof(command[0]),
			parts = GDISP_SCREEN_WIDTH/cmdLength;

	for(int i=0; i<GDISP_SCREEN_HEIGHT/8; i++){
		for(int j = 0; j<parts; j++){
			memmove(command, &gdisp_buffer[i*GDISP_SCREEN_WIDTH + j*cmdLength], cmdLength);
			write_data(command, cmdLength);
		}
	}
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

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @note    Optional.
	 * @note    If x,y + cx,cy is off the screen, the result is undefined.
	 * @note    If lines is >= cy, it is equivalent to a area fill with bgcolor.
	 *
	 * @param[in] x, y     The start of the area to be scrolled
	 * @param[in] cx, cy   The size of the area to be scrolled
	 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor  The color to fill the newly exposed area.
	 *
	 * @notapi
	 */
	void gdisp_lld_vertical_scroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		/* See datasheet table T10-1 for this*/
		uint8_t fHeight = (uint8_t)gdispGetFontMetric(gwinGetDefaultFont(), fontLineSpacing);
		set_multiplex_ratio(GDISP_SCREEN_HEIGHT - fHeight+1);
		set_display_offset(fHeight-2);

		/* Scrolling animation.*/
		for(int i=0; i<fHeight; i++){
			set_display_start_line(i);
			gfxSleepMilliseconds(10);
		}

		/* Shift buffer up a font line.*/
		for (int i = 0; i < GDISP_SCREEN_WIDTH*(GDISP_SCREEN_HEIGHT/8-1); i++) {
			gdisp_buffer[i]  = gdisp_buffer[i+GDISP_SCREEN_WIDTH*(fHeight/8)] >> fHeight % 8;
			gdisp_buffer[i] |= gdisp_buffer[i+GDISP_SCREEN_WIDTH*(fHeight/8 + 1)] << (8 - fHeight%8);
		}

		/* Clear last page.*/
		memset( &gdisp_buffer[GDISP_SCREEN_HEIGHT*GDISP_SCREEN_WIDTH/8 - GDISP_SCREEN_WIDTH*2], SSD1306_BLACK, GDISP_SCREEN_WIDTH*2);

		/* Update display.*/
		gdisp_lld_display();
	}

	/**
	 * @warning Implementation only fully supports left and right...some command issues here.
	 * Activate a scroll for rows start through stop.
	 * Hint, the display is 16 rows tall. To scroll the whole display, run:
	 * @code
	 * display.scrollright(0x00, 0x0F)
	 * @endcode
	 * @param[in] start The start of the area to be scrolled
	 * @param[in] stop	The size of the area to be scrolled
	 * @param[in] dir	direction of scrolling
	 * 					[left, right, up, down, up_right, up_left, down_left, down_right]
	 * @note    Optional. *
	 *
	 * @notapi
	 */
	void gdisp_lld_start_scroll(uint8_t dir, uint8_t start, uint8_t stop, uint8_t interval){
//		if(dir == GDISP_SCROLL_RIGHT || GDISP_SCROLL_LEFT || GDISP_SCROLL_UP) {
//			switch (dir) {
//				case GDISP_SCROLL_RIGHT:
//					write_cmd(SSD1306_SCROLL_HORIZONTAL_RIGHT);
//					break;
//				case GDISP_SCROLL_LEFT:
//					write_cmd(SSD1306_SCROLL_HORIZONTAL_LEFT);
//					break;
//			}
//			write_cmd(0X00);			// Dummy byte.
//			write_cmd(start & 0x07);	// Define start page address.
//			switch (interval) {			// Set time interval between each scroll step (5 frames)
//			        case   2: write_cmd(0x07); break; // 111b
//			        case   3: write_cmd(0x04); break; // 100b
//			        case   4: write_cmd(0x05); break; // 101b
//			        case   5: write_cmd(0x00); break; // 000b
//			        case  25: write_cmd(0x06); break; // 110b
//			        case  64: write_cmd(0x01); break; // 001b
//			        case 128: write_cmd(0x02); break; // 010b
//			        case 256: write_cmd(0x03); break; // 011b
//			        default:
//			            // default to 2 frame interval
//			            write_cmd(0x07); break;
//			    }
//			write_cmd(stop & 0x07);		// Define stop page address
//			write_cmd(0X01);			// Set vertical scrolling offset as no row.
//			write_cmd(0XFF);			// Undocumented but needed.
//			write_cmd(SSD1306_SCROLL_ACTIVATE);
//		}
//		else if(dir == GDISP_SCROLL_UP || GDISP_SCROLL_DOWN) {
//			switch (dir) {
//				case GDISP_SCROLL_UP:
//					gdisp_lld_set_vertical_scroll_area(0x00, GDISP_SCREEN_HEIGHT);
//					write_cmd(SSD1306_SCROLL_VERTICAL_AND_HORIZONTAL_RIGHT);
//					break;
//
//				case GDISP_SCROLL_DOWN:
//					gdisp_lld_set_vertical_scroll_area(0x00, GDISP_SCREEN_HEIGHT);
//					write_cmd(SSD1306_SCROLL_VERTICAL_AND_HORIZONTAL_LEFT);
//					break;
//			}
//			write_cmd(0X00);		// Dummy byte.
//			write_cmd(start);		// Define start page address.
//			write_cmd(0X00);		// Set time interval between each scroll step (5 frames)
//			write_cmd(stop);		// Define stop page address
//			write_cmd(0X01);		// Set vertical scrolling offset as no row.
//			write_cmd(SSD1306_SCROLL_ACTIVATE);
//			gdisp_lld_set_vertical_scroll_area(0x00, GDISP_SCREEN_HEIGHT-10);
//			write_cmd(SSD1306_SCROLL_VERTICAL_AND_HORIZONTAL_RIGHT);
//			write_cmd(0X00);		// Dummy byte.
//			write_cmd(start);		// Define start page address.
//			write_cmd(0X00);		// Set time interval between each scroll step (5 frames)
//			write_cmd(stop);		// Define stop page address
//			write_cmd(0X03);		// Set vertical scrolling offset as no row.
//			write_cmd(SSD1306_SCROLL_ACTIVATE);
//		}
	}

	/**
	 * Sets vertical scroll area of display.
	 * @param[in] start The start of the area to be scrolled [y coordinate]
	 * @param[in] stop	The size of the area to be scrolled [y coordinate]
	 * @note    Optional. *
	 *
	 * @notapi
	 */
	void gdisp_lld_set_vertical_scroll_area(uint8_t start, uint8_t stop){
		write_cmd(SSD1306_SCROLL_SET_VERTICAL_SCROLL_AREA);
		write_cmd(start);
		write_cmd(stop);
	}

	/** Deactivate the continuous scroll set up with start_horizontal_scroll() or
	 *  start_vertical_and_horizontal_scroll().
	 *	@see set_horizontal_scroll, set_vertical_and_horizontal_scroll
	 * 	@notapi
	 */
	void gdisp_lld_stop_scroll(void){
		write_cmd(SSD1306_SCROLL_DEACTIVATE);
	}
#endif	// GDISP_NEED_SCROLL

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
	    #if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
	        if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
	        if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
	        if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	        if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
	        if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
	    #endif

		for(int i=x; i<x+cx; i++) {
			for(int j=y; j<y+cy; j++) {
				gdisp_lld_draw_pixel(i,j,color);
			}
		}
	}
#endif 	// GDISP_HARDWARE_FILLS

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
			case powerSleep:
				display_off();
			case powerDeepSleep:
				display_off();
			case powerOn:
				display_on();
			default:
				return;
			}
			GDISP.Powermode = (gdisp_powermode_t)value;
			return;
		case GDISP_CONTROL_ORIENTATION:
				if (GDISP.Orientation == (gdisp_orientation_t)value)
					return;
				switch((gdisp_orientation_t)value) {
				case GDISP_ROTATE_0:
					flip_display(0);
					GDISP.Height = GDISP_SCREEN_HEIGHT;
					GDISP.Width = GDISP_SCREEN_WIDTH;
					break;
				case GDISP_ROTATE_180:
					flip_display(1);
					GDISP.Height = GDISP_SCREEN_HEIGHT;
					GDISP.Width = GDISP_SCREEN_WIDTH;
					break;
				default:
					return;
				}
				#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
					GDISP.clipx0 = 0;
					GDISP.clipy0 = 0;
					GDISP.clipx1 = GDISP.Width;
					GDISP.clipy1 = GDISP.Height;
				#endif
				GDISP.Orientation = (gdisp_orientation_t)value;
				return;
		case GDISP_CONTROL_CONTRAST:
				if ((unsigned)value > 100)
					value = (void *)100;
				if (GDISP.Contrast == (uint8_t)((float)((uint8_t)value) * 256.0/100.0) )
					return;
				set_contrast((uint8_t)((float)((uint8_t)value) * 256.0/100.0) );
				GDISP.Contrast = (unsigned)value;
				return;
		}
	}
#endif // GDISP_NEED_CONTROL

/**
 * Let the display blink several times by means of invert and invert back.
 * @param	num		number of blink cycles to do
 * @param	speed	milliseconds to wait between toggling inversion
 * @param	wait	milliseconds to wait before start of all blink cycles and after finishing blink cycles
 * @notapi
 */
 void gdisp_lld_display_blink(uint8_t num, uint16_t speed, uint16_t wait){
	uint8_t	 inv = 0;

	gfxSleepMilliseconds(wait);
	for(int i=0; i<2*num; i++) {
		inv ^= 1;
		invert_display(inv);
		gfxSleepMilliseconds(speed);
	}
	gfxSleepMilliseconds(wait);
}

#endif // GFX_USE_GDISP
/** @} */

