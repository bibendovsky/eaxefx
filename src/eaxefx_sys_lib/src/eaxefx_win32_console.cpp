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


#include "eaxefx_console.h"

#include <memory>
#include <vector>

#include <windows.h>


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32Console :
	public Console
{
public:
	Win32Console() noexcept;


	String read_line() override;


	void write(
		std::string_view view) override;

	void write_error(
		std::string_view view) override;


	void flush() override;


private:
	bool has_in_handle_{};
	bool has_err_handle_{};
	bool has_out_handle_{};

	bool need_convert_input_{};
	bool need_convert_output_{};

	::UINT in_code_page_{};
	::UINT out_code_page_{};

	::HANDLE in_handle_{};
	::HANDLE err_handle_{};
	::HANDLE out_handle_{};


	static bool is_handle_valid(
		::HANDLE handle) noexcept;

	static void flush(
		::HANDLE handle) noexcept;


	static String utf8_to_oem(
		std::string_view utf8_view,
		::UINT code_page);

	static String oem_to_utf8(
		std::string_view oem_view,
		::UINT code_page);


	String read_line_internal();

	void write_internal(
		::HANDLE handle,
		std::string_view view);
}; // Win32Console

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Win32Console::Win32Console() noexcept
{
	in_code_page_ = ::GetConsoleCP();
	out_code_page_ = ::GetConsoleOutputCP();

	const auto has_in_code_page = in_code_page_ != 0;
	const auto has_out_code_page = out_code_page_ != 0;

	in_handle_ = ::GetStdHandle(STD_INPUT_HANDLE);
	err_handle_ = ::GetStdHandle(STD_ERROR_HANDLE);
	out_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);

	has_in_handle_ = has_in_code_page && is_handle_valid(in_handle_);
	has_err_handle_ = has_out_code_page && is_handle_valid(err_handle_);
	has_out_handle_ = has_out_code_page && is_handle_valid(out_handle_);

	if (has_in_handle_)
	{
		need_convert_input_ = (in_code_page_ != CP_UTF8);
	}

	if (has_err_handle_ || has_out_handle_)
	{
		need_convert_output_ = (out_code_page_ != CP_UTF8);
	}
}

String Win32Console::read_line()
{
	if (!has_in_handle_)
	{
		return String{};
	}

	return read_line_internal();
}

void Win32Console::write(
	std::string_view view)
{
	if (!has_out_handle_)
	{
		return;
	}

	if (view.empty())
	{
		return;
	}

	write_internal(out_handle_, view);
}

void Win32Console::write_error(
	std::string_view view)
{
	if (!has_err_handle_)
	{
		return;
	}

	if (view.empty())
	{
		return;
	}

	write_internal(err_handle_, view);
}

void Win32Console::flush()
{
	if (has_err_handle_)
	{
		flush(err_handle_);
	}

	if (has_out_handle_)
	{
		flush(out_handle_);
	}
}

bool Win32Console::is_handle_valid(
	::HANDLE handle) noexcept
{
	return handle && handle != INVALID_HANDLE_VALUE;
}

void Win32Console::flush(
	::HANDLE handle) noexcept
{
	::FlushFileBuffers(handle);
}

String Win32Console::utf8_to_oem(
	std::string_view utf8_view,
	::UINT code_page)
{
	// UTF-8 -> ::WCHAR
	//

	const auto wide_size = ::MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_view.data(),
		utf8_view.size(),
		nullptr,
		0
	);

	if (wide_size == 0)
	{
		return String{};
	}

	auto wide_buffer = std::make_unique<::WCHAR[]>(wide_size);

	const auto actual_wide_size = ::MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_view.data(),
		utf8_view.size(),
		wide_buffer.get(),
		wide_size
	);

	if (actual_wide_size != wide_size)
	{
		return String{};
	}


	// ::WCHAR -> OEM
	//

	const auto oem_size = ::WideCharToMultiByte(
		code_page,
		0,
		wide_buffer.get(),
		actual_wide_size,
		nullptr,
		0,
		nullptr,
		nullptr
	);

	if (oem_size == 0)
	{
		return String{};
	}

	auto oem_string = String{};
	oem_string.resize(oem_size);

	const auto actual_oem_size = ::WideCharToMultiByte(
		code_page,
		0,
		wide_buffer.get(),
		actual_wide_size,
		oem_string.data(),
		oem_size,
		nullptr,
		nullptr
	);

	if (actual_oem_size != oem_size)
	{
		return String{};
	}

	return oem_string;
}

String Win32Console::oem_to_utf8(
	std::string_view oem_view,
	::UINT code_page)
{
	// UTF-8 -> ::WCHAR
	//

	const auto wide_size = ::MultiByteToWideChar(
		code_page,
		0,
		oem_view.data(),
		oem_view.size(),
		nullptr,
		0
	);

	if (wide_size == 0)
	{
		return String{};
	}

	auto wide_buffer = std::make_unique<::WCHAR[]>(wide_size);

	const auto actual_wide_size = ::MultiByteToWideChar(
		code_page,
		0,
		oem_view.data(),
		oem_view.size(),
		wide_buffer.get(),
		wide_size
	);

	if (actual_wide_size != wide_size)
	{
		return String{};
	}


	// ::WCHAR -> UTF-8
	//

	const auto utf8_size = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		wide_buffer.get(),
		actual_wide_size,
		nullptr,
		0,
		nullptr,
		nullptr
	);

	if (utf8_size == 0)
	{
		return String{};
	}

	auto utf8_string = String{};
	utf8_string.resize(utf8_size);

	const auto actual_utf8_size = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		wide_buffer.get(),
		actual_wide_size,
		utf8_string.data(),
		utf8_size,
		nullptr,
		nullptr
	);

	if (actual_utf8_size != utf8_size)
	{
		return String{};
	}

	return utf8_string;
}

String Win32Console::read_line_internal()
{
	constexpr auto oem_line_reserve = 1024;

	auto oem_line = String{};
	oem_line.resize(oem_line_reserve);

	auto oem_read_size = ::DWORD{};

	::ReadFile(
		in_handle_,
		oem_line.data(),
		oem_line_reserve,
		&oem_read_size,
		nullptr
	);

	if (oem_read_size == 0)
	{
		return String{};
	}

	oem_line.resize(oem_read_size);

	if (need_convert_input_)
	{
		auto utf8_string = oem_to_utf8(oem_line, in_code_page_);

		if (!utf8_string.empty())
		{
			utf8_string.resize(utf8_string.size() - 2);
		}

		return utf8_string;
	}
	else
	{
		oem_line.resize(oem_read_size - 2);

		return oem_line;
	}
}

void Win32Console::write_internal(
	::HANDLE handle,
	std::string_view view)
{
	if (need_convert_output_)
	{
		const auto oem_string = utf8_to_oem(view, out_code_page_);

		if (!oem_string.empty())
		{
			::WriteFile(handle, oem_string.data(), oem_string.size(), nullptr, nullptr);
		}
	}
	else
	{
		::WriteFile(handle, view.data(), view.size(), nullptr, nullptr);
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

ConsoleUPtr make_console()
{
	return std::make_unique<Win32Console>();
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
