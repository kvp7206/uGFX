GLCD now uses the GDISP low level drivers and fonts.

To update your make to use this new version:
	Add the low level driver yo want to use to your make file. eg.
		include $(CHIBIOS)/os/halext/drivers/gdispTestStub/gdisp_lld.mk

There some restrictions that GLCD places on your use of new features and on the capabilities
of the low level driver.

They are:
	1/ GLCD requires a driver that supports RGB565 pixel format. This is a
		limitation of the GLCD API. To update the API would create compatability
		issues with existing applications.
	2/ If you want to use the GLCD scroll or the GLCD read-pixel calls then your
		low level driver must support them. If it doesn't these calls will
		fail.
	3/ You cannot reduce the code size like in GDISP by defining macros to
		compile out code that you are not using.
	4/ Some of the new features of GDISP like right or center justified text are not
		supported as there is no equivelant API in GDISP.
	5/ There is no mechanism to send hardware specific commands to the low level driver
		such as commands to control the back-light.

What it does do that GDISP currently doesn't:
	1/ Asynchronous multi-thread support.