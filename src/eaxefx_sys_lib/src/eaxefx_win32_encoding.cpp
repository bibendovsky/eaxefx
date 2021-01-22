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


#include "eaxefx_encoding.h"

#include <windows.h>

#include "eaxefx_exception.h"


namespace eaxefx::encoding
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32EncodingToUtf16Exception :
	public Exception
{
public:
	explicit Win32EncodingToUtf16Exception(
		const char* message)
		:
		Exception{"WIN32_ENCODING_TO_UTF16", message}
	{
	}
}; // Win32EncodingToUtf16Exception

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Win32EncodingToUtf8Exception :
	public Exception
{
public:
	explicit Win32EncodingToUtf8Exception(
		const char* message)
		:
		Exception{"WIN32_ENCODING_TO_UTF8", message}
	{
	}
}; // Win32EncodingToUtf8Exception

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

U16String to_utf16_internal(
	const char* utf8_string,
	int utf8_length)
{
	if (!utf8_string)
	{
		throw Win32EncodingToUtf16Exception{"Null string."};
	}

	if (utf8_length < 0)
	{
		utf8_length = static_cast<int>(String::traits_type::length(utf8_string));
	}

	if (utf8_length == 0)
	{
		return U16String{};
	}

	const auto u16_size = ::MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_string,
		utf8_length,
		nullptr,
		0
	);

	if (u16_size <= 0)
	{
		throw Win32EncodingToUtf16Exception{"Failed to get max length."};
	}

	auto u16_string = U16String{};
	u16_string.resize(u16_size);

	const auto u16_result = ::MultiByteToWideChar(
		CP_UTF8,
		0,
		utf8_string,
		utf8_length,
		reinterpret_cast<::LPWSTR>(u16_string.data()),
		u16_size
	);

	if (u16_result != u16_size)
	{
		throw Win32EncodingToUtf16Exception{"Failed to convert."};
	}

	return u16_string;
}

U16String to_utf16(
	const String& utf8_string)
{
	return to_utf16_internal(utf8_string.c_str(), static_cast<int>(utf8_string.size()));
}

U16String to_utf16(
	const char* utf8_string)
{
	return to_utf16_internal(utf8_string, -1);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

String to_utf8_internal(
	const char16_t* utf16_string,
	int utf16_length)
{
	if (!utf16_string)
	{
		throw Win32EncodingToUtf8Exception{"Null string."};
	}

	if (utf16_length < 0)
	{
		utf16_length = static_cast<int>(U16String::traits_type::length(utf16_string));
	}

	if (utf16_string == 0)
	{
		return String{};
	}

	const auto u8_size = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		reinterpret_cast<::LPCWSTR>(utf16_string),
		utf16_length,
		nullptr,
		0,
		nullptr,
		nullptr
	);

	if (u8_size <= 0)
	{
		throw Win32EncodingToUtf8Exception{"Failed to get max length."};
	}

	auto u8_string = String{};
	u8_string.resize(u8_size);

	const auto u8_result = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		reinterpret_cast<::LPCWSTR>(utf16_string),
		utf16_length,
		u8_string.data(),
		u8_size,
		nullptr,
		nullptr
	);

	if (u8_result != u8_size)
	{
		throw Win32EncodingToUtf8Exception{"Failed to convert."};
	}

	return u8_string;
}

String to_utf8(
	const char16_t* utf16_string)
{
	return to_utf8_internal(utf16_string, -1);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::encoding
