To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GDISP	TRUE

	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	
	d) If you are not using a known board then create a gdisp_lld_board.h file
		and ensure it is on your include path.
		Use the gdisp_lld_board_example.h file as a basis.
		Currently known boards are:
		 	HY_STM32_100p

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/ILI9325/gdisp_lld.mk
