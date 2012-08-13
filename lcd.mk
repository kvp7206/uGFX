# don't re-define LCDLIB if it has been set elsewhere, e.g in Makefile
ifeq ($(LCDLIB),)
	LCDLIB =	$(CHIBIOS)/ext/lcd
endif

include $(LCDLIB)/console/console.mk

LCDSRC +=   $(LCDLIB)/src/gdisp.c \
            $(LCDLIB)/src/gdisp_fonts.c \
            $(LCDLIB)/src/touchpad.c 

LCDINC +=   $(LCDLIB)/include
