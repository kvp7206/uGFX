To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: #define GDISP_NEED_MULTITHREAD TRUE
	c) One (only) of:
		#define LCD_USE_GPIO
		#define LCD_USE_FSMC
  d) If you want to use DMA (only works with FSMC):
    #define LCD_USE_DMA
    #define LCD_DMA_STREAM STM32_DMA2_STREAM6 //You can change the DMA channel according to your needs
    
2. Edit gdisp_lld_panel.h with your panel properties

3. To your makefile add the following lines:
	include $(LCDLIB)/drivers/gdisp/SSD1963/gdisp_lld.mk
  
  
Example FSMC config with DMA:

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#define LCD_USE_FSMC

#define LCD_USE_DMA
#define LCD_DMA_STREAM STM32_DMA2_STREAM6

#if defined(LCD_USE_GPIO)

    #define LCD_CMD_PORT GPIOC
    #define LCD_DATA_PORT GPIOD

    #define LCD_CS 0
    #define LCD_RS 1
    #define LCD_WR 2
    #define LCD_RD 3
#endif