To use this driver template
	1. Copy this entire directory (including the directory itself)
		into halext/drivers
	2. Rename the directory to match your hardware.
	3. Customise each file in the directory including the .mk file
		and this file. An example for this file is below...

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

2. To your makefile add the following lines:
	include $(CHIBIOS)/os/halext/halext.mk
	include $(CHIBIOS)/os/halext/drivers/gdispXXXXX/gdisp_lld.mk
