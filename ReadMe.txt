

What's this?
============
an ExtIO DLL for HDSDR (http://hdsdr.de/) and other WinRad derivatives
allowing control of many Ham radio receivers, which are supported by Hamlib.


License?
========
LGPL


Warranty?
=========
Like explained in Lesser GPL you get no warranty at all.
We are not responsible for anything, even if you damage your hardware.


Howto install?
==============
1-
download hamlib-win32-1.2.14.zip or later
from http://sourceforge.net/projects/hamlib/files/hamlib/
and unpack it

2-
try controlling your rig with "rigctl.exe" from command line
read FAQ / documentation on
http://hamlib.org

3-
copy the files in hamlib-win32-1.2.14/bin/ into your HDSDR directory

4-
copy ExtIO_HamLib.dll and ExtIO_HamLib.cfg  into your HDSDR directory

5-
open ExtIO_HamLib.cfg with an editor and enter your rig settings
lines beginning directly with a '#' are comments.
take care not to use create empty lines! for testing switch on logging.

6-
you're ready to run HDSDR
lot's of luck!



Howto compile with Visual C++ 2008 Express Edition:
===================================================

C/C++:

General / Additional Include Directories:
add "<..>\hamlib-win32-1.2.14\include"

Preprocessor Defines:
add "EXTIO_HAMLIB_EXPORTS;_CRT_SECURE_NO_WARNINGS"

Code Generation:
activate Stringpooling (/GF)

Precompiled Headers:
deactivate them


Linker:

Incremental Linking:
deactivate (/INCREMENTAL:NO)

Additional Library Paths:
add "<..>\hamlib-win32-1.2.14\lib\msvc"

Input / Additional Dependencies:
add "libhamlib-2.lib"

Input / Module definitions:
add "ExtIO_HamLib.def"

