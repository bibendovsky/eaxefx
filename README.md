EAX OpenAL Extension
====================


Contents
========

1. Overview  
   1.1 Doom 3 visual twitch fix
2. Supported drivers
3. Expected driver file names
4. Missing features  
   4.1 Bypassed  
   4.2 Not supported
5. Known issues
6. Usage
7. Compiling requirements
8. Third party use
9. Disclaimer


1 - Overview
============

The project enables EAX OpenAL Extension for systems without appropriate hardware.

OpenAL wrapper implements EAX calls with Effects Extension by using underlying OpenAL driver.


1.1 - Doom 3 visual twitch fix
==============================

OpenAL wrapper embedded with runtime patch to fix Doom 3 [visual twitch bug](http://www.pcgamingwiki.com/wiki/Doom_3#Visual_twitch_bug_with_OpenAL.2FEAX).

Included console application `eaxefx_doom3_fix.exe` has the same purpose but offline.

Tested on `Doom 3 v1.3.1`.


2 - Supported drivers
=====================

- `OpenAL Soft`

The release package bundled with `OpenAL Soft` driver.


3 - Expected driver file names
==============================

- `eaxefx_driver.dll`
- `dsoal-aldrv.dll`
- `soft_oal.dll`

The wrapper tries to load one of this driver in the order specified above.


4.1 - Bypassed features
=======================

Context:
- HF Reference (EAX 4.0/5.0)
- Macro FX Factor (EAX 5.0)
- Speaker configuration (EAX 5.0)


4.2 - Not supported
===================

Source:
- Speaker levels (EAX 5.0)

General:
- X-RAM


5 - Known issues
================

- `OpenAL Soft` official binaries `v1.20.x` may cause a crash upon exiting the application.


6 - Usage
=========

Terminology:
- `DIR` - the directory with applications's main executable.

Steps:
1. Backup (if necessary) `OpenAL32.dll` and `soft_oal.dll` in the `DIR`.
2. Extract the release package into some directory.
3. Copy extracted `OpenAL32.dll` into the `DIR`.
4. If the `DIR` does not contain any driver mentioned in the item 4, copy there extracted `soft_oal.dll`.
5. Enable appropriate EAX options in the application and restart it if necessary.


7 - Compiling requirements
==========================

Common:
- Windows build target.
- 32-bit build target.

Minimum:
- CMake v3.8.2
- C++17 compatible compiler.


8 - Third party use
===================

* [OpenAL Soft](http://openal-soft.org/)  
  See file `eaxefx_openal_soft_license.txt` for license information.  


9 - Disclaimer
==============

Copyright (c) 2020-2021 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
