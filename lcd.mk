# LCD files.
LCDSRC = 	${CHIBIOS}/ext/lcd/glcd.c \
			${CHIBIOS}/ext/lcd/fonts.c \
			${CHIBIOS}/ext/lcd/touchpad.c \
			${CHIBIOS}/ext/lcd/graph.c \
			${CHIBIOS}/ext/lcd/gui.c \
			${CHIBIOS}/ext/lcd/drivers/ssd1289_lld.c \
			${CHIBIOS}/ext/lcd/drivers/s6d1121_lld.c \
			${CHIBIOS}/ext/lcd/drivers/ads7843_lld.c

LCDINC = 	${CHIBIOS}/ext/lcd \
			${CHIBIOS}/etc/lcd/drivers
