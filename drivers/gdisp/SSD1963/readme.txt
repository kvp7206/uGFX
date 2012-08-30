To use this driver template
	1. Copy this entire directory (including the directory itself)
		into halext/drivers
	2. Rename the directory to match your hardware.
	3. Customise each file in the directory including the .mk file
		and this file. An example for this file is below...
	4. Keep any board specific code in a file you create called
		gdisp_lld_board_yourboardname.h and adjust gdisp.c to match.
		This enables someone porting to a new board to add another
		suitable boad definition without worrying about the rest of
		the driver. See the gdispNokia6610 driver as an example.

------------------------------------------------------------------
To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	c) One (only) of:
		#define LCD_USE_GPIO
		#define LCD_USE_SPI
		#define LCD_USE_FSMC
	d) All of the following (with appropriate values):
		#define SCREEN_WIDTH	128
		#define SCREEN_HEIGHT	128
	e) If you are not using a known board then create a gdisp_lld_board.h file
		and ensure it is on your include path.
		Use the gdisp_lld_board_example.h file as a basis.
		Currently known boards are:
		 	XXXXXXXXX


2. To your makefile add the following lines:
	include $(CHIBIOS)/os/halext/halext.mk
	include $(CHIBIOS)/os/halext/drivers/gdispXXXXX/gdisp_lld.mk
