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


#include "eaxefx_platform.h"

#include <windows.h>


namespace eaxefx::platform
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

bool win32_is_vista_or_higher_internal() noexcept
{
	constexpr auto win32_type_mask = ::DWORD{VER_MAJORVERSION | VER_MINORVERSION};

	auto os_version_info_w = ::OSVERSIONINFOEXW{};
	os_version_info_w.dwOSVersionInfoSize = static_cast<::DWORD>(sizeof(::OSVERSIONINFOEXW));
	os_version_info_w.dwMajorVersion = 6;
	os_version_info_w.dwMinorVersion = 0;

	const auto win32_condition_mask = ::VerSetConditionMask(0, win32_type_mask, VER_GREATER_EQUAL);

	const auto win32_result = ::VerifyVersionInfoW(
		&os_version_info_w,
		win32_type_mask,
		win32_condition_mask
	);

	return win32_result != FALSE;
}


bool is_vista_or_higher() noexcept
{
	static auto result = win32_is_vista_or_higher_internal();

	return result;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::platform
