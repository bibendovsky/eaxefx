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


#include "eaxefx_utils.h"

#include <cassert>

#include <exception>

#include "eaxefx_common_strings.h"


namespace eaxefx::utils
{


namespace
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct ErrorMessages
{
	static constexpr auto null_current_exception = "Null current exception.";
	static constexpr auto generic_exception = "Generic exception.";
}; // ErrorMessages

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // namespace


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void log_exception(
	Logger* logger,
	const char* message) noexcept
{
	if (!logger)
	{
		assert(!"Null logger.");
		return;
	}

	const auto exception_ptr = std::current_exception();

	logger->error(common::Strings::_);
	logger->error(common::Strings::less_than_signs_8);

	if (message)
	{
		logger->error(message);
	}

	if (exception_ptr)
	{
		try
		{
			std::rethrow_exception(exception_ptr);
		}
		catch (const std::exception& ex)
		{
			const auto ex_message = ex.what();
			logger->error(ex_message);
		}
		catch (...)
		{
			logger->error(ErrorMessages::generic_exception);
		}
	}
	else
	{
		logger->error(ErrorMessages::null_current_exception);
	}

	logger->error(common::Strings::greater_than_signs_8);
	logger->error(common::Strings::_);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::utils
