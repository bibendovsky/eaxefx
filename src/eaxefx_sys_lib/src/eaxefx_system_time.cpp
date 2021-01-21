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


#include "eaxefx_system_time.h"

#include <charconv>


namespace eaxefx
{


void make_system_time_string(
	const SystemTime& system_time,
	String& system_time_string)
{
	// YYYY-MM-DD HH:mm:ss.sss
	constexpr auto max_string_length = 23;

	system_time_string.resize(max_string_length);

	const auto number_to_string = [](
		int number,
		int max_digit_count,
		char*& chars)
	{
		const auto end_chars = chars + max_digit_count;

		int digit_count;

		if (number > 999)
		{
			digit_count = 4;
		}
		else if (number > 99)
		{
			digit_count = 3;
		}
		else if (number > 9)
		{
			digit_count = 2;
		}
		else
		{
			digit_count = 1;
		}

		const auto zero_count = max_digit_count - digit_count;

		for (auto i = 0; i < zero_count; ++i)
		{
			*chars++ = '0';
		}

		std::to_chars(chars, end_chars, number);

		chars = end_chars;
	};

	auto chars = system_time_string.data();
	number_to_string(system_time.year, 4, chars);
	*chars++ = '-';
	number_to_string(system_time.month, 2, chars);
	*chars++ = '-';
	number_to_string(system_time.day, 2, chars);
	*chars++ = ' ';
	number_to_string(system_time.hour, 2, chars);
	*chars++ = ':';
	number_to_string(system_time.minute, 2, chars);
	*chars++ = ':';
	number_to_string(system_time.second, 2, chars);
	*chars++ = '.';
	number_to_string(system_time.millisecond, 3, chars);

	system_time_string.resize(chars - system_time_string.data());
}


} // eaxefx
