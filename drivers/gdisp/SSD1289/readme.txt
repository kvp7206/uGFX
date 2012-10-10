To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_GDISP	TRUE

	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD

	c) One (only) of:
		#define GDISP_USE_GPIO
		#define GDISP_USE_SPI
		#define GDISP_USE_FSMC

	d) All of the following (with appropriate values):
		#define GDISP_SCREEN_WIDTH	320
		#define GDISP_SCREEN_HEIGHT	240

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/SSD1289/gdisp_lld.mk

