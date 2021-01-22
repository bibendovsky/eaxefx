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


#include "eaxefx_sys_win32_condition_variable.h"

#include "eaxefx_exception.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class SysWin32ConditionVariableException :
	public Exception
{
public:
	explicit SysWin32ConditionVariableException(
		const char* message)
		:
		Exception{"SYS_WIN32_CONDITION_VARIABLE", message}
	{
	}
}; // SysWin32ConditionVariableException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

SysWin32ConditionVariable::SysWin32ConditionVariable()
	:
	kernel32_shared_library_{make_shared_library("kernel32")},
	initialize_condition_variable_func_{
		reinterpret_cast<InitializeConditionVariableFunc>(
			kernel32_shared_library_->resolve("InitializeConditionVariable"))},
	wake_condition_variable_func_{
		reinterpret_cast<WakeConditionVariableFunc>(
			kernel32_shared_library_->resolve("WakeConditionVariable"))},
	sleep_condition_variable_cs_func_{
		reinterpret_cast<SleepConditionVariableCSFunc>(
			kernel32_shared_library_->resolve("SleepConditionVariableCS"))}
{
	if (!initialize_condition_variable_func_ ||
		!wake_condition_variable_func_ ||
		!sleep_condition_variable_cs_func_)
	{
		throw SysWin32ConditionVariableException{"Missing symbols."};
	}

	initialize_condition_variable_func_(&condition_variable_);
}

SysWin32ConditionVariable::~SysWin32ConditionVariable() = default;

bool SysWin32ConditionVariable::sleep(
	SysWin32CriticalSection& critical_section,
	std::chrono::milliseconds timeout)
{
	::DWORD timeout_ms;

	if (timeout.count() >= INFINITE)
	{
		timeout_ms = INFINITE;
	}
	else
	{
		timeout_ms = static_cast<::DWORD>(timeout.count());
	}

	const auto win32_result = sleep_condition_variable_cs_func_(
		&condition_variable_,
		&critical_section.get(),
		timeout_ms);

	return win32_result != 0;
}

void SysWin32ConditionVariable::wake()
{
	wake_condition_variable_func_(&condition_variable_);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



} // eaxefx
