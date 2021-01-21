/*

EAX OpenAL Extension

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

*/


#ifndef EAXEFX_SYS_WIN32_CONDITION_VARIABLE_INCLUDED
#define EAXEFX_SYS_WIN32_CONDITION_VARIABLE_INCLUDED


#include <chrono>

#include <windows.h>

#include "eaxefx_shared_library.h"
#include "eaxefx_sys_win32_critical_section.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class SysWin32ConditionVariable
{
public:
	SysWin32ConditionVariable();

	SysWin32ConditionVariable(
		const SysWin32ConditionVariable& rhs) = delete;

	SysWin32ConditionVariable& operator=(
		const SysWin32ConditionVariable& rhs) = delete;

	~SysWin32ConditionVariable();


	bool sleep(
		SysWin32CriticalSection& critical_section,
		std::chrono::milliseconds timeout);

	void wake();


private:
	using InitializeConditionVariableFunc = VOID (WINAPI *)(
		PCONDITION_VARIABLE ConditionVariable);

	using WakeConditionVariableFunc = VOID (WINAPI *)(
		PCONDITION_VARIABLE ConditionVariable);

	using SleepConditionVariableCSFunc = BOOL (WINAPI *)(
		PCONDITION_VARIABLE ConditionVariable,
		PCRITICAL_SECTION CriticalSection,
		DWORD dwMilliseconds);


	SharedLibraryUPtr kernel32_shared_library_{};
	InitializeConditionVariableFunc initialize_condition_variable_func_{};
	WakeConditionVariableFunc wake_condition_variable_func_{};
	SleepConditionVariableCSFunc sleep_condition_variable_cs_func_{};

	CONDITION_VARIABLE condition_variable_;
}; // SysWin32ConditionVariable

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_SYS_WIN32_CONDITION_VARIABLE_INCLUDED
