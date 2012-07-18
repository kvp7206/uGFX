LCDLIB =	$(CHIBIOS)/ext/lcd

include 	$(LCDLIB)/drivers/drivers.mk
include 	$(LCDLIB)/gui/gui.mk

LCDSRC = 	$(LCDLIB)/glcd.c \
			$(LCDLIB)/fonts.c \
			$(LCDLIB)/touchpad.c \
			$(LCDLIB)/graph.c \
			$(LCDLIB)/console.c \
			$(LCDLIB)/fastMath.c \
			$(LCD_DRIVERS_SRC) \
			$(LCD_GUI_SRC)

LCDINC = 	$(LCDLIB) \
			$(LCD_DRIVERS_INC) \
			$(LCD_GUI_INC)
