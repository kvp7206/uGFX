# List the required driver.
LCDSRC += 	$(LCDLIB)/drivers/gdisp/gdispVMT/gdisp_lld.c \
			$(LCDLIB)/os/halext/drivers/gdisp/gdispVMT/gdisp_lld_driver1.c \
			$(LCDLIB)/os/halext/drivers/gdisp/gdispVMT/gdisp_lld_driver2.c

# Required include directories
LCDINC += 	$(LCDLIB)/drivers/gdisp/gdispVMT
