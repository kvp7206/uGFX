The new GDISP driver is an architecture independent rewrite of the GLCD interface.
This new architecture independence should allow many new low level drivers to be easily added.

GDISP allows low-level driver hardware accelerated drawing routines while providing a software emulation
if the low level driver can not provide it. A basic low level driver now only requires 2 routines to be written.

A glcd.h compatibility file has been included that allow applications written to use the existing GLCD driver to
use the GDISP driver with little or no change.

It is written in the ChibiOS style with ChibiOS style includes and documentation.

It is encapsulated into a "halext" structure with appropriate readme's that allow for easy inclusion in any
ChibiOS project. This structure can be seamlessly added to as new driver types are added and it supports
low level drivers that are neither platform or board specific (although they can be).
