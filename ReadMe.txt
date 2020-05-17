# Disclaimer

This project was not initiated by me!
The original sources are from http://www.hdsdr.de/hardware.html.
I just uploaded it on GitHub and added a script to build and modify the DLL on Linux.

If you are (or know) the original developer, please let me know, so i can give kudos to him!

## What's this?

an ExtIO DLL for HDSDR (http://hdsdr.de/) and other WinRad derivatives
allowing control of many Ham radio receivers, which are supported by Hamlib.

## License?

LGPL

## Warranty?

Like explained in Lesser GPL you get no warranty at all.
We are not responsible for anything, even if you damage your hardware.

## Howto install?

download hamlib-win32-1.2.14.zip or later
from http://sourceforge.net/projects/hamlib/files/hamlib/
and unpack it

try controlling your rig with "rigctl.exe" from command line
read FAQ / documentation on
http://hamlib.org

copy the files in hamlib-win32-1.2.14/bin/ into your HDSDR directory

copy ExtIO_HamLib.dll and ExtIO_HamLib.cfg  into your HDSDR directory

open ExtIO_HamLib.cfg with an editor and enter your rig settings
lines beginning directly with a '#' are comments.
take care not to use create empty lines! for testing switch on logging.

you're ready to run HDSDR
lot's of luck!

## Howto compile with Visual C++ 2008 Express Edition:

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

## Howto compile on Linux

get mingw32

´´´
apt install g++-mingw-w64-i686-win32
´´´

Get Hamlib Package for Windows from https://hamlib.github.io/.
Extract it and adapt path in build.sh

Run ´´´build.sh´´´.

## Use HDSDR on Linux with rigctld

See instructions "howto install?" above.

In ExtIO_HamLib.cfg use 127.0.0.1:4532 instead of comX.

Run rigctld as you would normally do.
Like (for KX3):
´´´
rigctld -m 229 -r /dev/ttyUSB0
´´´

Run HDSDR!

In case of problems ExtIO_HamLib.log should contain some information.
