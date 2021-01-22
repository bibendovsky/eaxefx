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


#include "eaxefx_string.h"

#include <type_traits>

#include <strsafe.h>

#include "eaxefx_exception.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class ToStringException :
	public Exception
{
public:
	explicit ToStringException(
		const char* message)
		:
		Exception{"WIN32_TO_STRING", message}
	{
	}
}; // ToStringException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

namespace
{


template<
	typename T
>
constexpr int to_string_max_length() noexcept
{
	if (false)
	{
	}
	else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, long>)
	{
		return 11;
	}
	else if constexpr (std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned long>)
	{
		return 10;
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		return 16;
	}
	else
	{
		return 0;
	}
}


template<
	typename T
>
inline void to_string_generic(
	T value,
	const char* format_string,
	String& string)
{
	constexpr auto max_length = to_string_max_length<T>();

	string.resize(max_length);

	const auto data = string.data();
	char* data_end = nullptr;

	const auto win32_result = ::StringCbPrintfExA(
		data,
		max_length + 1,
		&data_end,
		nullptr,
		0,
		format_string,
		value
	);

	if (FAILED(win32_result))
	{
		throw ToStringException{"Failed to convert."};
	}

	string.resize(static_cast<String::size_type>(data_end - data));
}


} // namespace

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void to_string(
	int value,
	String& string)
{
	to_string_generic(value, "%d", string);
}

void to_string(
	unsigned int value,
	String& string)
{
	to_string_generic(value, "%u", string);
}

void to_string(
	float value,
	String& string)
{
	to_string_generic(value, "%f", string);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

String to_string(
	int value)
{
	static_assert(sizeof(int) == 4);

	auto string = String{};
	to_string_generic(value, "%d", string);
	return string;
}

String to_string(
	unsigned int value)
{
	static_assert(sizeof(unsigned int) == 4);

	auto string = String{};
	to_string_generic(value, "%u", string);
	return string;
}

String to_string(
	float value)
{
	static_assert(sizeof(float) == 4);

	auto string = String{};
	to_string_generic(value, "%f", string);
	return string;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
