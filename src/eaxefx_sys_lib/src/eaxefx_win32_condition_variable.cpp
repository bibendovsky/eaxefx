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


#include "eaxefx_win32_condition_variable.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Win32ConditionVariable::Win32ConditionVariable()
{
	InitializeConditionVariable(&condition_variable_);
}

Win32ConditionVariable::~Win32ConditionVariable() = default;

bool Win32ConditionVariable::sleep(
	Win32CriticalSection& critical_section,
	std::chrono::milliseconds timeout)
{
	DWORD timeout_ms;

	if (timeout.count() >= INFINITE)
	{
		timeout_ms = INFINITE;
	}
	else
	{
		timeout_ms = static_cast<DWORD>(timeout.count());
	}

	const auto win32_result = SleepConditionVariableCS(
		&condition_variable_,
		&critical_section.get(),
		timeout_ms);

	return win32_result != 0;
}

void Win32ConditionVariable::wake()
{
	WakeConditionVariable(&condition_variable_);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



} // eaxefx
