# List the required driver.
LCDSRC += ${CHIBIOS}/os/halext/drivers/gdispVMT/gdisp_lld.c \
			${CHIBIOS}/os/halext/drivers/gdispVMT/gdisp_lld_driver1.c \
			${CHIBIOS}/os/halext/drivers/gdispVMT/gdisp_lld_driver2.c

# Required include directories
LCDINC += ${CHIBIOS}/os/halext/drivers/gdispVMT
