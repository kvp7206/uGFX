To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GDISP	TRUE

	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD

	c) If you are not using a known board then create a gdisp_lld_board.h file
		and ensure it is on your include path.
		Use the gdisp_lld_board_example.h or gdisp_lld_board_fsmc.h file as a basis.
		Currently known boards are:
		 	BOARD_FIREBULL_STM32_F103	- GPIO interface: requires GDISP_CMD_PORT and GDISP_DATA_PORT to be defined

	d) The following are optional - define them if you are not using the defaults below:
		#define GDISP_SCREEN_WIDTH	320
		#define GDISP_SCREEN_HEIGHT	240

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/SSD1289/gdisp_lld.mk
