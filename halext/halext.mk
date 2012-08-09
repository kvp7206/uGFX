# List of all the ChibiOS/RT META files, there is no need to remove the files
# from this list, you can disable parts of the kernel by editing halconf.h.

HALSRC +=	$(LCDLIB)/halext/src/gdisp.c \
			$(LCDLIB)/halext/src/gdisp_fonts.c \
			$(LCDLIB)/halext/src/gdisp_emulation.c \
			$(LCDLIB)/halext/src/touchpad.c

# Required include directories
HALINC +=	$(LCDLIB)/halext/include
