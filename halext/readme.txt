To include any of these functions/drivers in your project...

	1/ Specify the path to the LCDLIB. If none defined, default is $(CHIBIOS)/ext/lcd

	2/ In your project Makefile (amongst similiar lines but after the hal line) add the line...
		include $(LCDLIB)/lcd.mk

	3/ In your project Makefile add the makefiles for any specific drivers you want e.g
		include $(LCDLIB)/halext/drivers/touchpad/touchpadXPT2046/touchpad_lld.mk

	4/ In your project halconf.h turn on the support you want eg.
		/**
		 * @brief   Enables the Touchpad subsystem.
		 */
		#if !defined(HAL_USE_TOUCHPAD) || defined(__DOXYGEN__)
			#define HAL_USE_TOUCHPAD           TRUE
		#endif

	5/ Do a make clean.

