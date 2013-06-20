Description:

Driver for LCD with 4-wire serial interface (65k colors).

To use this driver:

1. Add in your gfxconf.h:
	a) #define GFX_USE_GDISP	TRUE

	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD

	c) If you are not using a known board then create a gdisp_lld_board.h file
		and ensure it is on your include path.
		Use the gdisp_lld_board_st_stm32f4_discovery.h file as a basis.
		Currently known boards are:
			BOARD_ST_STM32F4_DISCOVERY
		Board configuration assume that you have STM32_PWM_USE_TIM4 set to TRUE in your mcuconf.h.

	d) The following are optional - define them if you are not using the defaults below:
		#define GDISP_SCREEN_WIDTH	240
		#define GDISP_SCREEN_HEIGHT	320

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/HX8347D/gdisp_lld.mk
