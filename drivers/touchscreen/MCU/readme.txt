To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_TOUCHSCREEN		TRUE

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/touchscreen/MCU/touchscreen_lld.mk

