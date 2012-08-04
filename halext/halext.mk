# List of all the ChibiOS/RT META files, there is no need to remove the files
# from this list, you can disable parts of the kernel by editing halconf.h.
HALSRC += ${CHIBIOS}/os/halext/src/gdisp.c \
			${CHIBIOS}/os/halext/src/gdisp_fonts.c \
			${CHIBIOS}/os/halext/src/gdisp_emulation.c

# Required include directories
HALINC += ${CHIBIOS}/os/halext/include
