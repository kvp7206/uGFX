To include any of these functions/drivers in your project...

	1/ Place this halext directory into the $(CHIBIOS)/os directory
	2/ In your project Makefile (amongst similiar lines but after the hal line) add the line...
		include $(CHIBIOS)/os/halext/halext.mk
	3/ In your project Makefile add the makefiles for any specific drivers you want e.g
		include $(CHIBIOS)/os/halext/drivers/gdispNokia6610/gdisp_lld.mk
	4/ In your project halconf.h turn on the support you want eg.
		/**
		 * @brief   Enables the GDISP subsystem.
		 */
		#if !defined(HAL_USE_GDISP) || defined(__DOXYGEN__)
			#define HAL_USE_GDISP           TRUE
			/* Any driver specific defines required go here */
			#define GDISP_NEED_MULTITHREAD	TRUE
		#endif
	5/ Do a make clean.

