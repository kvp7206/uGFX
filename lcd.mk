LCDLIB =	$(CHIBIOS)/ext/lcd

include 	$(LCDLIB)/drivers/drivers.mk

LCDSRC = 	$(LCDLIB)/glcd.c \
			$(LCDLIB)/fonts.c \
			$(LCDLIB)/touchpad.c \
			$(LCDLIB)/graph.c \
			$(LCDLIB)/gui.c \
			$(LCDLIB)/console.c \
			$(LCD_DRIVERS_SRC)

LCDINC = 	$(LCDLIB) \
			$(LCD_DRIVERS_INC)
