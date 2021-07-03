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


#include "eaxefx_fs.h"

#include "eaxefx_platform.h"

#include <cassert>

#if EAXEFX_WIN32
#include <windows.h>

#include <memory>
#endif // EAXEFX_WIN32

#include "eaxefx_core_types.h"
#include "eaxefx_exception.h"

#if EAXEFX_WIN32
#include "eaxefx_encoding.h"
#endif // EAXEFX_WIN32

namespace eaxefx::fs
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

namespace
{


struct ErrorMessages
{
	static constexpr auto get_working_dir = "Failed to get working directory.";
	static constexpr auto set_working_dir = "Failed to set working directory.";
	static constexpr auto get_absolute_path = "Failed to get absolute path.";
	static constexpr auto create_dir = "Failed to create directory.";
	static constexpr auto create_dirs = "Failed to create directory recursively.";
	static constexpr auto rename = "Failed to rename.";
	static constexpr auto remove = "Failed to remove.";
}; // ErrorMessages


} // namespace

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

namespace
{


class FileSystemException :
	public Exception
{
public:
	explicit FileSystemException(
		const char* message)
		:
		Exception{"EAXEFX_SYS_FS", message}
	{
	}
}; // FileSystemException


} // namespace

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Path get_working_directory()
{
#if EAXEFX_WIN32
	// With trailing "nul".
	const auto max_utf16_size = ::GetCurrentDirectoryW(0, nullptr);

	if (max_utf16_size == 0)
	{
		throw FileSystemException{ErrorMessages::get_working_dir};
	}

	auto utf16_buffer = std::make_unique<::WCHAR[]>(max_utf16_size);

	// Without trailing "nul".
	const auto win32_utf16_size = ::GetCurrentDirectoryW(max_utf16_size, utf16_buffer.get());

	if (win32_utf16_size != (max_utf16_size - 1))
	{
		throw FileSystemException{ErrorMessages::get_working_dir};
	}

	const auto u8_string = encoding::to_utf8(reinterpret_cast<const char16_t*>(utf16_buffer.get()));

	return Path{u8_string.c_str(), static_cast<Int>(u8_string.size())};
#endif // EAXEFX_WIN32
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void create_directory(
	const char* path)
{
#if EAXEFX_WIN32
	const auto u16_path = encoding::to_utf16(path);

	const auto win32_result = ::CreateDirectoryW(reinterpret_cast<::LPCWSTR>(u16_path.c_str()), nullptr);

	if (win32_result == 0)
	{
		const auto last_error = ::GetLastError();

		if (last_error != ERROR_ALREADY_EXISTS)
		{
			throw FileSystemException{ErrorMessages::create_dir};
		}
	}
#endif // EAXEFX_WIN32
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::fs
