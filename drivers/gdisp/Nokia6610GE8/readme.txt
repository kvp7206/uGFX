This driver is for the Nokia6610 Epson (GE8) controller

To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	c) If you are not using a known board then create a gdisp_lld_board.h file
		and ensure it is on your include path.
		Use the gdisp_lld_board_example.h file as a basis.
		Currently known boards are:
		 	Olimex SAM7-EX256

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/Nokia6610GE8/gdisp_lld.mk
