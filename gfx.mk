# don't re-define GFXLIB if it has been set elsewhere, e.g in Makefile
ifeq ($(GFXLIB),)
	GFXLIB =	$(CHIBIOS)/ext/gfx
endif

GFXSRC +=   $(GFXLIB)/src/gdisp.c \
            $(GFXLIB)/src/gdisp_fonts.c \
			$(GFXLIB)/src/gevent.c \
			$(GFXLIB)/src/gtimer.c \
            $(GFXLIB)/src/gwin.c \
            $(GFXLIB)/src/touchscreen.c \
			$(GFXLIB)/src/graph.c \

GFXINC +=   $(GFXLIB)/include

include $(GFXLIB)/src/gwin/gwin.mk
include $(GFXLIB)/src/ginput/ginput.mk
