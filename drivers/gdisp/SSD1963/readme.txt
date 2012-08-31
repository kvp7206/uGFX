To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	c) One (only) of:
		#define LCD_USE_GPIO (Work in progress)
		#define LCD_USE_FSMC
	d) Edit gdisp_lld_panel.h with your panel properties

2. To your makefile add the following lines:
	include $(LCDLIB)/drivers/gdisp/SSD1963/gdisp_lld.mk