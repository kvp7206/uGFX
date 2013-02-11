To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/S6D1121/gdisp_lld.mk

