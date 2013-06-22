Description:

Driver for LCD with 16-bit 8080 interface (65k colors).

To use this driver:

1. Add in your gfxconf.h:
	a) #define GFX_USE_GDISP	TRUE

	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD

	c) If you are not using a known board then create a gdisp_lld_board.h file
		and ensure it is on your include path. Use the gdisp_lld_board_embest_dmstf4bb.h
		file as a basis.
		Currently known boards are:
			BOARD_EMBEST_DMSTF4BB - FSMC interface
				This board configuration assumes that you have PWM driver enabled
				and STM32_PWM_USE_TIM4 set to TRUE in your mcuconf.h.
		If you use FSMC, define in your board file GDISP_USE_FSMC. If you use DMA,
		define GDISP_USE_DMA.

	d) The following are optional - define them if you are not using the defaults below:
		#define GDISP_SCREEN_WIDTH	320
		#define GDISP_SCREEN_HEIGHT	240

2. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/SSD2119/gdisp_lld.mk
