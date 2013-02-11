To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: #define GDISP_NEED_MULTITHREAD TRUE
	c) One (only) of:
		#define GDISP_USE_GPIO
		#define GDISP_USE_FSMC
  d) If you want to use DMA (only works with FSMC):
    #define GDISP_USE_DMA
    #define GDISP_DMA_STREAM STM32_DMA2_STREAM6 //You can change the DMA channel according to your needs
    
2. Edit gdisp_lld_panel.h with your panel properties

3. To your makefile add the following lines:
	include $(GFXLIB)/drivers/gdisp/SSD1963/gdisp_lld.mk
  
  
Example FSMC config with DMA:

#define GDISP_SCREEN_WIDTH 480
#define GDISP_SCREEN_HEIGHT 272

#define GDISP_USE_FSMC

#define GDISP_USE_DMA
#define GDISP_DMA_STREAM STM32_DMA2_STREAM6

#if defined(GDISP_USE_GPIO)

    #define GDISP_CMD_PORT GPIOC
    #define GDISP_DATA_PORT GPIOD

    #define GDISP_CS 0
    #define GDISP_RS 1
    #define GDISP_WR 2
    #define GDISP_RD 3
#endif
