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


#include "eaxefx_encoding.h"

#include <string_view>

#include <windows.h>

#include "eaxefx_exception.h"


namespace eaxefx::encoding
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EncodingToUtf16Exception :
	public Exception
{
public:
	explicit EncodingToUtf16Exception(
		std::string_view message)
		:
		Exception{"ENCODING_TO_UTF16", message}
	{
	}
}; // EncodingToUtf16Exception

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

std::u16string to_utf16(
	const std::string& utf8_string)
{
	if (utf8_string.empty())
	{
		return std::u16string{};
	}

	const auto u16_size = MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		utf8_string.c_str(),
		utf8_string.size(),
		nullptr,
		0
	);

	if (u16_size <= 0)
	{
		throw EncodingToUtf16Exception{"Failed to get max length."};
	}

	auto u16_string = std::u16string{};
	u16_string.resize(u16_size);

	const auto u16_result = MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		utf8_string.c_str(),
		utf8_string.size(),
		reinterpret_cast<LPWSTR>(u16_string.data()),
		u16_size
	);

	if (u16_result != u16_size)
	{
		throw EncodingToUtf16Exception{"Failed to convert."};
	}

	return u16_string;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::encoding
