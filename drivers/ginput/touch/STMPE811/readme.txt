The STMPE811 driver comes with two different #defines to perfectly fit
your application:


STMPE811_NO_GPIO_IRQPIN
This Macro is meant to be set in your board file. When you set this macro to
TRUE, the GINPUT module will not use the IRQ lane which might be connected
to a GPIO pin to recognize interrupts by the STMPE811 controller. This
costs a few more I2C calls.
When the interrupt IRQ pin is connected to a GPIO of your MCU, set this
macro to FALSE.


STMP811_SLOW_CPU
If you have a slow CPU and you need to take care of your resources, you can
set this macro TRUE. This will save some IRQs and therefore a few I2C calls.
The disadvantage is a little higher response time.
If you don't want to draw continious lines on your display, it's recommended
to set this to TRUE anyways.

