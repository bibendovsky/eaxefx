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


#include "eaxefx_file.h"

#include <windows.h>

#include "eaxefx_encoding.h"
#include "eaxefx_exception.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32FileException :
	public Exception
{
public:
	explicit Win32FileException(
		const char* message)
		:
		Exception{"WIN32_FILE", message}
	{
	}
}; // Win32FileException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32File :
	public File
{
public:
	Win32File(
		const char* path,
		FileOpenMode open_mode);

	~Win32File() override;


	void set_position(
		int position) override;

	void move_to_the_end() override;


	int read(
		void* buffer,
		int size) override;

	int write(
		const void* buffer,
		int size) override;


private:
	bool is_readable_{};
	bool is_writable_{};
	::HANDLE handle_{};


	bool is_handle_valid() noexcept;

	void ensure_is_open();
}; // Win32File

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Win32File::Win32File(
	const char* path,
	FileOpenMode open_mode)
{
	if (!path || path[0] == '\0')
	{
		throw Win32FileException{"Null or empty path."};
	}

	constexpr auto mode_error_message = "Unsupported open mode.";

	const auto is_readable = (open_mode & file_open_mode_read) != 0;
	const auto is_writable = (open_mode & file_open_mode_write) != 0;
	const auto is_truncate = (open_mode & file_open_mode_truncate) != 0;

	auto win32_desired_access = ::DWORD{};

	if (is_readable)
	{
		win32_desired_access |= GENERIC_READ;
	}

	if (is_writable)
	{
		win32_desired_access |= GENERIC_WRITE;
	}

	if (win32_desired_access == 0)
	{
		throw Win32FileException{mode_error_message};
	}


	auto win32_creation_disposition = ::DWORD{};

	if (is_truncate)
	{
		if (!is_writable)
		{
			throw Win32FileException{mode_error_message};
		}

		win32_creation_disposition = CREATE_ALWAYS;
	}
	else
	{
		if (is_readable)
		{
			win32_creation_disposition = OPEN_EXISTING;
		}
		else if (is_writable)
		{
			win32_creation_disposition = OPEN_ALWAYS;
		}
		else
		{
			throw Win32FileException{mode_error_message};
		}
	}

	const auto utf16_path = encoding::to_utf16(path);

	handle_ = ::CreateFileW(
		reinterpret_cast<::LPCWSTR>(utf16_path.c_str()),
		win32_desired_access,
		0,
		nullptr,
		win32_creation_disposition,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (!is_handle_valid())
	{
		throw Win32FileException{"Failed to open file."};
	}

	is_readable_ = is_readable;
	is_writable_ = is_writable;
}

Win32File::~Win32File()
{
	if (is_handle_valid())
	{
		::CloseHandle(handle_);
	}
}

void Win32File::set_position(
	int position)
{
	ensure_is_open();

	::LARGE_INTEGER win32_position;
	win32_position.QuadPart = position;

	const auto win32_result = ::SetFilePointerEx(
		handle_,
		win32_position,
		nullptr,
		FILE_BEGIN
	);

	if (!win32_result)
	{
		throw Win32FileException{"Failed to set position."};
	}
}

void Win32File::move_to_the_end()
{
	ensure_is_open();

	auto win32_position = ::LARGE_INTEGER{};

	const auto win32_result = ::SetFilePointerEx(
		handle_,
		win32_position,
		nullptr,
		FILE_END
	);

	if (!win32_result)
	{
		throw Win32FileException{"Failed to set position to the end."};
	}
}

int Win32File::read(
	void* buffer,
	int size)
{
	ensure_is_open();

	if (!is_readable_)
	{
		throw Win32FileException{"Not readable."};
	}

	if (!buffer || size <= 0)
	{
		return 0;
	}

	::DWORD read_size;

	const auto win32_result = ::ReadFile(
		handle_,
		buffer,
		static_cast<::DWORD>(size),
		&read_size,
		nullptr
	);

	if (!win32_result)
	{
		throw Win32FileException{"I/O read error."};
	}

	return static_cast<int>(read_size);
}

int Win32File::write(
	const void* buffer,
	int size)
{
	ensure_is_open();

	if (!is_writable_)
	{
		throw Win32FileException{"Not writable."};
	}

	if (!buffer || size <= 0)
	{
		return 0;
	}

	::DWORD written_size;

	const auto win32_result = ::WriteFile(
		handle_,
		buffer,
		static_cast<::DWORD>(size),
		&written_size,
		nullptr
	);

	if (!win32_result)
	{
		throw Win32FileException{"I/O write error."};
	}

	return static_cast<int>(written_size);
}

bool Win32File::is_handle_valid() noexcept
{
	return handle_ && handle_ != INVALID_HANDLE_VALUE;
}

void Win32File::ensure_is_open()
{
	if (!is_handle_valid())
	{
		throw Win32FileException{"Not open."};
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

FileUPtr make_file(
	const char* path,
	FileOpenMode open_mode)
{
	return std::make_unique<Win32File>(path, open_mode);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
