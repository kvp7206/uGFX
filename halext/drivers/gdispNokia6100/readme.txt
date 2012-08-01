To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	c) One (only) of:
		#define LCD_USE_GE8
		#define LCD_USE_GE12

2. To your makefile add the following lines:
	include $(CHIBIOS)/os/halext/halext.mk
	include $(CHIBIOS)/os/halext/drivers/gdispSsd1289/gdisp_lld.mk
