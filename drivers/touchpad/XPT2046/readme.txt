To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_TOUCHPAD		TRUE

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/touchpad/XPT2046/touchpad_lld.mk

