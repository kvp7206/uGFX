# don't re-define LCDLIB if it has been set elsewhere, e.g in Makefile
ifeq ($(LCDLIB),)
	LCDLIB =	$(CHIBIOS)/ext/lcd
endif

LCDSRC +=   $(LCDLIB)/src/gdisp.c \
            $(LCDLIB)/src/gdisp_fonts.c \
            $(LCDLIB)/src/touchpad.c \
			$(LCDLIB)/src/console.c

LCDINC +=   $(LCDLIB)/include
