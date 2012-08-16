This low level driver is a test stub that doesn't talk to any
real hardware. It is included to allow testing of the compilation
process.

Do not use this driver as a template for new drivers. Use the
	templates/gdispXXXXX directory for that.

To use this driver:

1. Add in your halconf.h:
	a) #define HAL_USE_GDISP	TRUE
	b) Any optional high level driver defines (see gdisp.h)
		you want to compile test eg: GDISP_NEED_MULTITHREAD

2. To your makefile add the following lines:
	include $(CHIBIOS)/os/halext/halext.mk
	include $(CHIBIOS)/os/halext/drivers/gdispTestStub/gdisp_lld.mk
