EAX OpenAL Extension


Contents
========

1. Overview  
   1.1 Doom 3 visual twitch fix
2. Disclaimer
3. Supported drivers
4. Expected driver file names
5. Supported games
6. Known issues
7. Usage
8. Compiling requirements
9. Third party use


1 - Overview
============

The project enables EAX OpenAL extension for systems without appropriate hardware.

OpenAL wrapper implements EAX calls with EFX by using underlying OpenAL driver.


1.1 - Doom 3 visual twitch fix
==============================

OpenAL wrapper embedded with runtime patch to fix Doom 3 [visual twitch bug](http://www.pcgamingwiki.com/wiki/Doom_3#Visual_twitch_bug_with_OpenAL.2FEAX).

Included console application `eaxefx_doom3_fix.exe` has the same purpose but offline.

Tested on Doom3 v1.3.1.


2 - Disclaimer
==============

Copyright (c) 2020 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

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


3 - Supported drivers
=====================

- `OpenAL Soft` (bundled; see item 6 for notes)


4 - Expected driver file names
==============================

- `soft_oal.dll`
- `eaxefx_driver.dll`
- `dsoal-aldrv.dll`


5 - Supported games
===================

- Doom 3
- Quake 4
- Prey (2006)


6 - Known issues
================

- `OpenAL Soft` official binaries `v1.20.0` or higher may cause crashes upon exiting the game.  
  To resolve the issue bundled binary was built with Visual C++.


7 - Usage
=========

- Place all DLL files into the game's directory overwriting existing ones.
- Enable appropriate EAX options in the game.
- Restart the game if necessary.


8 - Compiling requirements
==========================

- Windows build target.
- 32-bit build target.
- CMake v3.8.2
- C++17 compatible compiler.


9 - Third party use
===================

* [OpenAL Soft](http://openal-soft.org/)  
  See file `eaxefx_openal_soft_license.txt` for license information.  
