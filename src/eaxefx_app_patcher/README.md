EAX App Patcher
===============

Contents
========

1. Overview
2. Minimum requirements
3. Resolved issues
4. Usage
5. Build requirements
6. Disclaimer


1 - Overview
============

The project fixes known issues of applications which are using EAX.


2 - Minimum requirements
========================

Operating system:
- Windows XP with Service Pack 3


3 - Resolved issues
===================

- Doom 3 (2004) v1.3.1 [visual twitch bug](https://www.pcgamingwiki.com/wiki/Doom_3#Visual_twitch_bug_with_OpenAL.2FEAX).
- Doom 3 (2004) v1.3.1 [channel contribution issue](https://github.com/dhewm/dhewm3/commit/3c01757d27a22a3858737f6b51e85895d2fac887).
- Doom 3 (2004) v1.3.1 [uninitialized reverb properties](https://www.pcgamingwiki.com/wiki/Doom_3#Incorrect_EAX_effect_in_Doom3:_Ressurection_of_evil_with_OpenAL.2FEAX).
- Doom 3 (2004) v1.3.1 error "idSoundCache: error unloading data from OpenAL hardware buffer".
- Quake 4 (2005) v1.4.2 / v1.4.3 (Steam) source occlusion out of range.
- Prey (2006) v1.4 source occlusion out of range.  
  Use retail version of executable from [official patch v1.4](https://community.pcgamingwiki.com/files/file/1063-prey-patches/) for offline patching.
- Beyond Good & Evil (2003) v1.1 (GOG / Ubisoft Connect) invalid reverb parameters for Environment Diffusion and Decay LF Ratio.


4 - Usage
=========

Terminology:
- `DIR` - the directory with application's main executable.

Steps:
1. Backup application's main executable.
2. Extract the release package into some directory.
3. Copy extracted `eaxefx_app_patcher.exe` into the `DIR`.
4. Run `eaxefx_app_patcher.exe` and follow the instructions.


5 - Build requirements
======================

Common:
- CMake v3.8.2

Without custom CRT:
- C89 compatible compiler.

With custom CRT:
- Visual C++ 2017.


6 - Disclaimer
==============

Copyright (c) 2020-2023 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

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
