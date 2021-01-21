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


#include "eaxefx_exception.h"

#include <cassert>

#include <string_view>


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Exception::Exception(
	const char* context,
	const char* message)
{
	assert(context != nullptr);
	assert(message != nullptr);

	const auto context_sv = std::string_view{context};
	const auto message_sv = std::string_view{message};

	static constexpr auto left_prefix = std::string_view{"["};
	static constexpr auto right_prefix = std::string_view{"] "};

	const auto what_size =
		left_prefix.size() +
		context_sv.size() +
		right_prefix.size() +
		message_sv.size() +
		1
	;

	what_ = std::make_unique<char[]>(what_size);
	auto what = what_.get();
	copy_string(left_prefix.data(), left_prefix.size(), what);
	copy_string(context_sv.data(), context_sv.size(), what);
	copy_string(right_prefix.data(), right_prefix.size(), what);
	copy_string(message_sv.data(), message_sv.size(), what);
	*what = '\0';
}

const char* Exception::what() const noexcept
{
	assert(what_ != nullptr);

	return what_.get();
}

void Exception::copy_string(
	const char* src,
	std::size_t src_size,
	char*& dst)
{
	assert(src != nullptr);
	assert(dst != nullptr);

	for (auto i = std::size_t{}; i < src_size; ++i)
	{
		dst[i] = src[i];
	}

	dst += src_size;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
