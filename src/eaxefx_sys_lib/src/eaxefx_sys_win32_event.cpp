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


#include "eaxefx_sys_win32_event.h"

#include "eaxefx_exception.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32EventException :
	public Exception
{
public:
	explicit Win32EventException(
		const char* message)
		:
		Exception{"SYS_WIN32_EVENT", message}
	{
	}
}; // Win32EventException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

SysWin32Event::SysWin32Event()
	:
	handle_{make_win32_event()}
{
}

SysWin32Event::~SysWin32Event()
{
	CloseHandle(handle_);
}

void SysWin32Event::set(
	bool value)
{
	BOOL win32_result;

	if (value)
	{
		win32_result = SetEvent(handle_);
	}
	else
	{
		win32_result = ResetEvent(handle_);
	}

	if (!win32_result)
	{
		throw Win32EventException("Failed to set a value.");
	}
}

void SysWin32Event::wait()
{
	wait_for_ms_internal(INFINITE);
}

bool SysWin32Event::wait_for(
	std::chrono::milliseconds timeout_ms)
{
	DWORD win32_timeout_ms;

	if (timeout_ms.count() >= INFINITE)
	{
		win32_timeout_ms = INFINITE;
	}
	else
	{
		win32_timeout_ms = static_cast<DWORD>(timeout_ms.count());
	}

	return wait_for_ms_internal(win32_timeout_ms);
}

HANDLE SysWin32Event::make_win32_event()
{
	const auto handle = CreateEventW(nullptr, TRUE, FALSE, nullptr);

	if (handle == nullptr)
	{
		throw Win32EventException("Failed to create.");
	}

	return handle;
}

bool SysWin32Event::wait_for_ms_internal(
	DWORD timeout_ms)
{
	const auto win32_result = WaitForSingleObject(handle_, static_cast<DWORD>(timeout_ms));

	switch (win32_result)
	{
		case WAIT_OBJECT_0:
			return true;

		case WAIT_TIMEOUT:
			return false;

		default:
			throw Win32EventException("Wait failed.");
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
