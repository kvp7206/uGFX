To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	c) All of the following (with appropriate values):
		#define GDISP_SCREEN_WIDTH	640
		#define GDISP_SCREEN_HEIGHT	480


2. To your makefile add the following lines:
	include $(GFXLIB)/gfx.mk
	include $(GFXLIB)/drivers/gdisp/Win32/gdisp_lld.mk

3. Modify your makefile to add -lgdi32 to the DLIBS line. i.e.
	DLIBS = -lws2_32 -lgdi32

