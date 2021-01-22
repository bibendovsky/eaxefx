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

#include <limits>
#include <string_view>

#include "eaxefx_exception.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class StoiException :
	public Exception
{
public:
	explicit StoiException(
		const char* message)
		:
		Exception{"STOI", message}
	{
	}
}; // StoiException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int stoi(
	String string)
{
	auto string_view = std::string_view{string.data(), string.size()};

	if (string_view.empty() || string_view.size() > 11)
	{
		throw StoiException{"String size out of range."};
	}

	auto is_negative = false;

	switch (string_view.front())
	{
		case '-':
			is_negative = true;
			string_view.remove_prefix(1);
			break;

		case '+':
			string_view.remove_prefix(1);
			break;

		default:
			break;
	}

	auto value = 0LL;

	for (const auto digit_char : string_view)
	{
		const auto digit = digit_char - '0';

		if (digit < 0 || digit > 9)
		{
			throw StoiException{"Invalid digit."};
		}

		value += (value * 10) + digit;
	}

	if (is_negative)
	{
		value = -value;
	}

	static constexpr auto min_value = std::numeric_limits<int>::min();
	static constexpr auto max_value = std::numeric_limits<int>::max();

	if (value < min_value || value > max_value)
	{
		throw StoiException{"Value out of range."};
	}

	return static_cast<int>(value);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
