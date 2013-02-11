This driver enables you to have two underlying drivers handling different hardware.
A choice is made at run-time of which driver to call based on which driver succeeds
to initialise first (init returns TRUE).

To use this driver:

1. Add in your halconf.h:
	a) #define GFX_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h) eg: GDISP_NEED_MULTITHREAD
	c) Define these:
		#define GDISP_VMT_NAME1(x)	x##YourDriver1
		#define GDISP_VMT_NAME2(x)	x##YourDriver2
		Note YourDriver1 & 2 are the basenames of the directories containing the driver.
		Note that both drivers must be the same pixel format which is
			GDISP_PIXELFORMAT_RGB565 by default. Alter gdispVMT/gdisp_lld_config.h if your
			pixel format is different on both drivers.
	d) Any driver specific defines. If both drivers use the same defines then they must
		accept the same values for the define.

2. To your makefile add the following lines:
	include $(CHIBIOS)/os/halext/halext.mk
	include $(CHIBIOS)/os/halext/drivers/gdispVMT/gdisp_lld.mk
	
