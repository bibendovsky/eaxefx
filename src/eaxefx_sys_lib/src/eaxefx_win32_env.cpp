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


#include "eaxefx_env.h"

#include <windows.h>
#include <shlobj.h>

#include "eaxefx_exception.h"
#include "eaxefx_encoding.h"


namespace eaxefx::env
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EnvException :
	public Exception
{
public:
	explicit EnvException(
		const char* message)
		:
		Exception{"EAXEFX_SYS_ENV", message}
	{
	}
}; // EnvException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

fs::Path get_special_folder(
	SpecialFolderType special_folder_type)
{
	auto win32_csidl = -1;

	switch (special_folder_type)
	{
		case SpecialFolderType::app_data:
			win32_csidl = CSIDL_APPDATA;
			break;

		default:
			throw EnvException{"Unknown special folder type."};
	}

	::WCHAR u16_buffer[MAX_PATH + 1];

	const auto win32_result = ::SHGetFolderPathW(
		nullptr,
		win32_csidl,
		nullptr,
		::SHGFP_TYPE_CURRENT,
		u16_buffer
	);

	if (FAILED(win32_result))
	{
		throw EnvException{"Failed to get special folder."};
	}

	const auto u8_buffer = encoding::to_utf8(reinterpret_cast<const char16_t*>(u16_buffer));

	return fs::Path{u8_buffer.c_str(), static_cast<Int>(u8_buffer.size())};
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::env
