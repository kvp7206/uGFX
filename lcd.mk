LCDLIB =	$(CHIBIOS)/ext/lcd

include 	$(LCDLIB)/drivers/drivers.mk
include		$(LCDLIB)/glcd/glcd.mk
include		$(LCDLIB)/touchpad/touchpad.mk
include		$(LCDLIB)/graph/graph.mk
include 	$(LCDLIB)/gui/gui.mk

LCDSRC = 	$(LCD_DRIVERS_SRC) \
			$(LCD_GLCD_SRC) \
			$(LCD_TOUCHPAD_SRC) \
			$(LCD_GRAPH_SRC) \
			$(LCD_GUI_SRC)

LCDINC = 	$(LCDLIB) \
			$(LCD_DRIVERS_INC) \
			$(LCD_GLCD_INC) \
			$(LCD_TOUCHPAD_INC) \
			$(LCD_GRAPH_INC) \
			$(LCD_GUI_INC)
