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


#include "eaxefx_exception.h"

#include <exception>
#include <string_view>


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Exception::Exception(
	std::string_view context,
	std::string_view message)
{
	make_message(
		context.empty() ? "???" : context,
		message.empty() ? "???" : message
	);
}


const char* Exception::what() const noexcept
{
	return what_.c_str();
}

void Exception::make_message(
	std::string_view context_string_view,
	std::string_view message_string_view)
{
	static constexpr auto left_prefix_string_view = std::string_view{"["};
	static constexpr auto right_prefix_string_view = std::string_view{"] "};

	const auto what_size =
		left_prefix_string_view.size() +
		context_string_view.size() +
		right_prefix_string_view.size() +
		message_string_view.size();

	what_.reserve(what_size);

	what_ += left_prefix_string_view;
	what_ += context_string_view;
	what_ += right_prefix_string_view;
	what_ += message_string_view;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
