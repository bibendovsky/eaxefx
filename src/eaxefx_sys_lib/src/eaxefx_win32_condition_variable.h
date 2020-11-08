/*

EAX OpenAL Extension

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

*/


#ifndef EAXEFX_WIN32_CONDITION_VARIABLE_INCLUDED
#define EAXEFX_WIN32_CONDITION_VARIABLE_INCLUDED


#include <chrono>

#include <windows.h>

#include "eaxefx_win32_critical_section.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32ConditionVariable
{
public:
	Win32ConditionVariable();

	Win32ConditionVariable(
		const Win32ConditionVariable& rhs) = delete;

	Win32ConditionVariable& operator=(
		const Win32ConditionVariable& rhs) = delete;

	~Win32ConditionVariable();


	bool sleep(
		Win32CriticalSection& critical_section,
		std::chrono::milliseconds timeout);

	void wake();


private:
	CONDITION_VARIABLE condition_variable_;
}; // Win32ConditionVariable

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



} // eaxefx


#endif // !EAXEFX_WIN32_CONDITION_VARIABLE_INCLUDED
