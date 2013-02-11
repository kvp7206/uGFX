To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GINPUT			TRUE
	b) #define GINPUT_NEED_MOUSE		TRUE

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/ginput/touch/MCU/ginput_lld.mk

