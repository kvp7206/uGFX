To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_TOUCHPAD		TRUE

2. To your makefile add the following lines:
	include $(LCDLIB)/lcd.mk
	include $(LCDLIB)/halext/drivers/touchpad/touchpadXPT2046/touchpad_lld.mk

