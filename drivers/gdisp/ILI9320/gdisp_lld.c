/*
    ChibiOS/GFX - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    drivers/gdisp/ILI9320/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for the ILI9320 display.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "lld/gdisp/emulation.c"

#if defined(GDISP_USE_CUSTOM_BOARD) && GDISP_USE_CUSTOM_BOARD
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#elif defined(BOARD_OLIMEX_STM32_LCD)
	#include "gdisp_lld_board_olimex_stm32_lcd.h"
#else
	#include "gdisp_lld_board.h"
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/* This controller is only ever used with a 240 x 320 display */
#if defined(GDISP_SCREEN_HEIGHT)
	#undef GDISP_SCREEN_HEIGHT
#endif
#if defined(GDISP_SCREEN_WIDTH)
	#undef GDISP_SCREEN_WIDTH
#endif

#define GDISP_SCREEN_WIDTH		240
#define GDISP_SCREEN_HEIGHT		320

#define GDISP_INITIAL_CONTRAST	50
#define GDISP_INITIAL_BACKLIGHT	100

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
uint32_t DISPLAY_CODE;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/
static __inline void lld_lcdDelay(uint16_t us) {
    chThdSleepMicroseconds(us);
}

static __inline void lld_lcdWriteIndex(uint16_t index) {
	GDISP_LLD(write_index)(index);
}

static __inline void lld_lcdWriteData(uint16_t data) {
	GDISP_LLD(write_data)(data);
}

static __inline void lld_lcdWriteReg(uint16_t lcdReg, uint16_t lcdRegValue) {
	GDISP_LLD(write_index)(lcdReg);
	GDISP_LLD(write_data)(lcdRegValue);
}

static __inline uint16_t lld_lcdReadData(void) {
	/* fix this! */
	//return GDISP_LLD(read_data);
	return GDISP_RAM;
}

static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
    volatile uint16_t dummy;

    GDISP_LLD(write_index)(lcdReg);
    dummy = lld_lcdReadData();
    (void)dummy;

    return lld_lcdReadData();
}

static __inline void lld_lcdWriteStreamStart(void) {
	lld_lcdWriteIndex(0x0022);
}
	
static __inline void lld_lcdWriteStreamStop(void) {

}

static __inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
	uint16_t i;

	for(i = 0; i < size; i++)
		lld_lcdWriteData(buffer[i]);
}

static __inline void lld_lcdReadStreamStart(void) {
	lld_lcdWriteIndex(0x0022);
}

static __inline void lld_lcdReadStreamStop(void) {

}

static __inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
	uint16_t i;
	volatile uint16_t dummy;

	dummy = lld_lcdReadData();
	(void)dummy;

	for(i = 0; i < size; i++)
		buffer[i] = lld_lcdReadData();
}

bool_t GDISP_LLD(init)(void) {
	/* Initialise your display */
	GDISP_LLD(init_board)();

	/* Hardware reset */
	GDISP_LLD(setpin_reset)(TRUE);
	lld_lcdDelay(1000);
	GDISP_LLD(setpin_reset)(FALSE);
	lld_lcdDelay(1000);

    DISPLAY_CODE = lld_lcdReadReg(0);
    lld_lcdWriteReg(0x0000, 0x0001); //start Int. osc
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0001, 0x0100); //Set SS bit (shift direction of outputs is from S720 to S1)
    lld_lcdWriteReg(0x0002, 0x0700); //select  the line inversion
    lld_lcdWriteReg(0x0003, 0x1038); //Entry mode(Horizontal : increment,Vertical : increment, AM=1)
    lld_lcdWriteReg(0x0004, 0x0000); //Resize control(No resizing)
    lld_lcdWriteReg(0x0008, 0x0202); //front and back porch 2 lines
    lld_lcdWriteReg(0x0009, 0x0000); //select normal scan
    lld_lcdWriteReg(0x000A, 0x0000); //display control 4
    lld_lcdWriteReg(0x000C, 0x0000); //system interface(2 transfer /pixel), internal sys clock,     
    lld_lcdWriteReg(0x000D, 0x0000); //Frame marker position
    lld_lcdWriteReg(0x000F, 0x0000); //selects clk, enable and sync signal polarity,
    lld_lcdWriteReg(0x0010, 0x0000); //  
    lld_lcdWriteReg(0x0011, 0x0000); //power control 2 reference voltages = 1:1,
    lld_lcdWriteReg(0x0012, 0x0000); //power control 3 VRH
    lld_lcdWriteReg(0x0013, 0x0000); //power control 4 VCOM amplitude
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0010, 0x17B0); //power control 1 BT,AP
    lld_lcdWriteReg(0x0011, 0x0137); //power control 2 DC,VC
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0012, 0x0139); //power control 3 VRH
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0013, 0x1d00); //power control 4 vcom amplitude
    lld_lcdWriteReg(0x0029, 0x0011); //power control 7 VCOMH
    lld_lcdDelay(500);
    lld_lcdWriteReg(0x0030, 0x0007);
    lld_lcdWriteReg(0x0031, 0x0403);
    lld_lcdWriteReg(0x0032, 0x0404);
    lld_lcdWriteReg(0x0035, 0x0002);
    lld_lcdWriteReg(0x0036, 0x0707);
    lld_lcdWriteReg(0x0037, 0x0606);
    lld_lcdWriteReg(0x0038, 0x0106);
    lld_lcdWriteReg(0x0039, 0x0007);
    lld_lcdWriteReg(0x003c, 0x0700);
    lld_lcdWriteReg(0x003d, 0x0707);
    lld_lcdWriteReg(0x0020, 0x0000); //starting Horizontal GRAM Address
    lld_lcdWriteReg(0x0021, 0x0000); //starting Vertical GRAM Address
    lld_lcdWriteReg(0x0050, 0x0000); //Horizontal GRAM Start Position
    lld_lcdWriteReg(0x0051, 0x00EF); //Horizontal GRAM end Position
    lld_lcdWriteReg(0x0052, 0x0000); //Vertical GRAM Start Position
    lld_lcdWriteReg(0x0053, 0x013F); //Vertical GRAM end Position
	switch (DISPLAY_CODE) {   
		case 0x9320:
        	lld_lcdWriteReg(0x0060, 0x2700); //starts scanning from G1, and 320 drive lines
        	break;
      	case 0x9325:
     		lld_lcdWriteReg(0x0060, 0xA700); //starts scanning from G1, and 320 drive lines
			break;
	}

    lld_lcdWriteReg(0x0061, 0x0001); //fixed base display
    lld_lcdWriteReg(0x006a, 0x0000); //no scroll
    lld_lcdWriteReg(0x0090, 0x0010); //set Clocks/Line =16, Internal Operation Clock Frequency=fosc/1,
    lld_lcdWriteReg(0x0092, 0x0000); //set gate output non-overlap period=0
    lld_lcdWriteReg(0x0093, 0x0003); //set Source Output Position=3
    lld_lcdWriteReg(0x0095, 0x0110); //RGB interface(Clocks per line period=16 clocks)
    lld_lcdWriteReg(0x0097, 0x0110); //set Gate Non-overlap Period 0 locksc
    lld_lcdWriteReg(0x0098, 0x0110); //
    lld_lcdWriteReg(0x0007, 0x0173); //display On

	// Turn on the backlight
	GDISP_LLD(set_backlight)(GDISP_INITIAL_BACKLIGHT);
	
    /* Initialise the GDISP structure */
    GDISP.Width = GDISP_SCREEN_WIDTH;
    GDISP.Height = GDISP_SCREEN_HEIGHT;
    GDISP.Orientation = GDISP_ROTATE_0;
    GDISP.Powermode = powerOn;
    GDISP.Backlight = GDISP_INITIAL_BACKLIGHT;
    GDISP.Contrast = GDISP_INITIAL_CONTRAST;
    #if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
	GDISP.clipx0 = 0;
	GDISP.clipy0 = 0;
	GDISP.clipx1 = GDISP.Width;
	GDISP.clipy1 = GDISP.Height;
    #endif

	return TRUE;
}

static void lld_lcdSetCursor(uint16_t x, uint16_t y) {
    uint32_t addr;

    addr = y * 0x100 + x;

	switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
    		lld_lcdWriteReg(0x0020, addr & 0xff);   /* low addr */
    		lld_lcdWriteReg(0x0021, (addr >> 8) & 0x1ff); /* high addr */
			break;

		case GDISP_ROTATE_90:
            lld_lcdWriteReg(0x0020, (addr >> 8) & 0x1ff);   /* low addr */
            lld_lcdWriteReg(0x0021, addr & 0xff); /* high addr */
			break;

		case GDISP_ROTATE_180:
			break;

		case GDISP_ROTATE_270:
			break;
	}
}

static void lld_lcdSetViewPort(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy) {
	switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
			lld_lcdWriteReg(0x0050, x);
			lld_lcdWriteReg(0x0051, x + cx - 1);
			lld_lcdWriteReg(0x0052, y);
			lld_lcdWriteReg(0x0053, y + cy - 1);
			break;

		case GDISP_ROTATE_90:
			lld_lcdWriteReg(0x0050, y);
			lld_lcdWriteReg(0x0051, y + cy - 1);
			lld_lcdWriteReg(0x0052, x);
			lld_lcdWriteReg(0x0053, x + cx - 1);
			break;

		case GDISP_ROTATE_180:
			break;

		case GDISP_ROTATE_270:
			break;

	}

	lld_lcdSetCursor(x, y);
}

static __inline void lld_lcdResetViewPort(void) { 
    switch(GDISP.Orientation) {
        case GDISP_ROTATE_0:
        case GDISP_ROTATE_180:
            lld_lcdSetViewPort(0, 0, GDISP_SCREEN_WIDTH, GDISP_SCREEN_HEIGHT);
            break;
        case GDISP_ROTATE_90:
        case GDISP_ROTATE_270:
           	lld_lcdSetViewPort(0, 0, GDISP_SCREEN_HEIGHT, GDISP_SCREEN_WIDTH);
            break;
    }
}

void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
    #if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
        if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
    #endif
    lld_lcdSetCursor(x, y);
    lld_lcdWriteReg(0x0022, color);
}

#if GDISP_HARDWARE_CLEARS || defined(__DOXYGEN__)
	void GDISP_LLD(clear)(color_t color) {
	    unsigned i;

	    lld_lcdSetCursor(0, 0);
	    lld_lcdWriteStreamStart();

	    for(i = 0; i < GDISP_SCREEN_WIDTH * GDISP_SCREEN_HEIGHT; i++)
	    	lld_lcdWriteData(color);

	    lld_lcdWriteStreamStop();
	}
#endif

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		unsigned i, area;

		area = cx*cy;
		lld_lcdSetViewPort(x, y, cx, cy);
		lld_lcdWriteStreamStart();
		for(i = 0; i < area; i++)
			lld_lcdWriteData(color);
		lld_lcdWriteStreamStop();
		lld_lcdResetViewPort();
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	void GDISP_LLD(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
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
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		color_t color;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif

		lld_lcdSetCursor(x, y);
		lld_lcdWriteStreamStart();

		color = lld_lcdReadData();
		color = lld_lcdReadData();

		lld_lcdWriteStreamStop();

		return color;
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
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
	void GDISP_LLD(control)(unsigned what, void *value) {
		switch(what) {
			case GDISP_CONTROL_POWER:
				if(GDISP.Powermode == (gdisp_powermode_t)value)
					return;
				switch((gdisp_powermode_t)value) {
					case powerOff:
						lld_lcdWriteReg(0x0007, 0x0000);
						lld_lcdWriteReg(0x0010, 0x0000);
						lld_lcdWriteReg(0x0011, 0x0000);
						lld_lcdWriteReg(0x0012, 0x0000);
						lld_lcdWriteReg(0x0013, 0x0000);
						GDISP_LLD(set_backlight)(0);
						break;
			
					case powerOn:
						//*************Power On sequence ******************//
						lld_lcdWriteReg(0x0010, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
						lld_lcdWriteReg(0x0011, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
						lld_lcdWriteReg(0x0012, 0x0000); /* VREG1OUT voltage */
						lld_lcdWriteReg(0x0013, 0x0000); /* VDV[4:0] for VCOM amplitude */
						lld_lcdDelay(2000);            /* Dis-charge capacitor power voltage */
						lld_lcdWriteReg(0x0010, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
						lld_lcdWriteReg(0x0011, 0x0147); /* DC1[2:0], DC0[2:0], VC[2:0] */
						lld_lcdDelay(500);
						lld_lcdWriteReg(0x0012, 0x013C); /* VREG1OUT voltage */
						lld_lcdDelay(500);
						lld_lcdWriteReg(0x0013, 0x0E00); /* VDV[4:0] for VCOM amplitude */
						lld_lcdWriteReg(0x0029, 0x0009); /* VCM[4:0] for VCOMH */
						lld_lcdDelay(500);
						lld_lcdWriteReg(0x0007, 0x0173); /* 262K color and display ON */	
						GDISP_LLD(set_backlight)(GDISP.Backlight);
						if(GDISP.Powermode != powerSleep || GDISP.Powermode != powerDeepSleep)
							GDISP_LLD(init)();
						break;
	
					case powerSleep:
	             		lld_lcdWriteReg(0x0007, 0x0000); /* display OFF */
	                   	lld_lcdWriteReg(0x0010, 0x0000); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
	                  	lld_lcdWriteReg(0x0011, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
	                  	lld_lcdWriteReg(0x0012, 0x0000); /* VREG1OUT voltage */
	                  	lld_lcdWriteReg(0x0013, 0x0000); /* VDV[4:0] for VCOM amplitude */
	                  	lld_lcdDelay(2000); /* Dis-charge capacitor power voltage */
	                   	lld_lcdWriteReg(0x0010, 0x0002); /* SAP, BT[3:0], APE, AP, DSTB, SLP */				
						GDISP_LLD(set_backlight)(0);
						break;

					case powerDeepSleep:
					    lld_lcdWriteReg(0x0007, 0x0000); /* display OFF */
					    lld_lcdWriteReg(0x0010, 0x0000); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
					    lld_lcdWriteReg(0x0011, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
	   					lld_lcdWriteReg(0x0012, 0x0000); /* VREG1OUT voltage */
	   					lld_lcdWriteReg(0x0013, 0x0000); /* VDV[4:0] for VCOM amplitude */
	   					lld_lcdDelay(2000); /* Dis-charge capacitor power voltage */
	  					lld_lcdWriteReg(0x0010, 0x0004); /* SAP, BT[3:0], APE, AP, DSTB, SLP */
						GDISP_LLD(set_backlight)(0);
						break;

					default:
						return;
				}
				GDISP.Powermode = (gdisp_powermode_t)value;
				return;

			case GDISP_CONTROL_ORIENTATION:
				if(GDISP.Orientation == (gdisp_orientation_t)value)
					return;
				switch((gdisp_orientation_t)value) {
					case GDISP_ROTATE_0:
						lld_lcdWriteReg(0x0001, 0x0100);
						lld_lcdWriteReg(0x0003, 0x1038);
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;

					case GDISP_ROTATE_90:
						lld_lcdWriteReg(0x0001, 0x0000);
						lld_lcdWriteReg(0x0003, 0x1030);
						GDISP.Height = GDISP_SCREEN_WIDTH;
						GDISP.Width = GDISP_SCREEN_HEIGHT;
						break;
			
					case GDISP_ROTATE_180:
						/* ToDo */
						GDISP.Height = GDISP_SCREEN_HEIGHT;
						GDISP.Width = GDISP_SCREEN_WIDTH;
						break;
		
					case GDISP_ROTATE_270:
						/* ToDo */
						GDISP.Height = GDISP_SCREEN_WIDTH;
						GDISP.Width = GDISP_SCREEN_HEIGHT;
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

			case GDISP_CONTROL_BACKLIGHT:
				if((unsigned)value > 100) value = (void *)100;
				GDISP_LLD(set_backlight)((unsigned)value);
				GDISP.Backlight = (unsigned)value;
				break;
			
			default:
				return;
		}
	}

#endif

#endif /* GFX_USE_GDISP */
/** @} */

