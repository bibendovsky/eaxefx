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


#include "eaxefx_string.h"

#include <string_view>
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
		std::string_view message)
		:
		Exception{"EAXEFX_TO_STRING", message}
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
	else if constexpr (
		std::is_same_v<T, int> || std::is_same_v<T, unsigned int> ||
		std::is_same_v<T, long> || std::is_same_v<T, unsigned long>)
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
String to_string_generic(
	T value,
	const char* format_string)
{
	constexpr auto max_length = to_string_max_length<T>();

	auto result = String{};
	result.resize(max_length);

	const auto data = result.data();
	char* data_end = nullptr;

	const auto win32_result = StringCbPrintfExA(
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

	result.resize(data_end - data);

	return result;
}


} // namespace

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

String to_string(
	int value)
{
	static_assert(sizeof(int) == 4);

	return to_string_generic(value, "%d");
}

String to_string(
	unsigned int value)
{
	static_assert(sizeof(unsigned int) == 4);

	return to_string_generic(value, "%u");
}

String to_string(
	float value)
{
	static_assert(sizeof(float) == 4);

	return to_string_generic(value, "%f");
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
