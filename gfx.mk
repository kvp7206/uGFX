# don't re-define GFXLIB if it has been set elsewhere, e.g in Makefile
ifeq ($(GFXLIB),)
	GFXLIB =	$(CHIBIOS)/ext/gfx
endif

GFXINC +=   $(GFXLIB)/include
GFXSRC +=

include $(GFXLIB)/src/gdisp/gdisp.mk
include $(GFXLIB)/src/tdisp/tdisp.mk
include $(GFXLIB)/src/gevent/gevent.mk
include $(GFXLIB)/src/gtimer/gtimer.mk
include $(GFXLIB)/src/gwin/gwin.mk
include $(GFXLIB)/src/ginput/ginput.mk
include $(GFXLIB)/src/gadc/gadc.mk
include $(GFXLIB)/src/gaudin/gaudin.mk
include $(GFXLIB)/src/gaudout/gaudout.mk
include $(GFXLIB)/src/gmisc/gmisc.mk
