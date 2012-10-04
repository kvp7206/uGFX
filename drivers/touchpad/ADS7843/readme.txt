To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_TOUCHPAD		TRUE

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/touchpadADS7843/touchpad_lld.mk

