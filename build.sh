#!/bin/bash

i686-w64-mingw32-g++ -c -I/home/ixyd/Downloads/hamlib-w32-3.3/include -DEXTIO_HAMLIB_EXPORTS ExtIO_HamLib.cpp -fPIC
i686-w64-mingw32-g++ -shared -o ExtIO_HamLib.dll ExtIO_HamLib.o -Wno-undef --enable-runtime-pseudo-reloc -Wl,--subsystem,windows,--add-stdcall-alias,--out-implib,libexample_dll.a /home/ixyd/Downloads/hamlib-w32-3.3/lib/gcc/libhamlib.dll.a
