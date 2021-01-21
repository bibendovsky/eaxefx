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


#ifndef EAXEFX_LOGGER_INCLUDED
#define EAXEFX_LOGGER_INCLUDED


#include <exception>
#include <memory>

#include "eaxefx_console.h"
#include "eaxefx_string.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

enum class LoggerMessageType
{
	info,
	warning,
	error,
}; // LoggerMessageType

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class Logger
{
public:
	Logger() = default;

	virtual ~Logger() = default;


	virtual void flush() noexcept = 0;

	virtual void set_immediate_mode() noexcept = 0;


	virtual void write(
		LoggerMessageType message_type,
		const String& message) noexcept = 0;

	virtual void write(
		const std::exception& exception) noexcept = 0;


	void info(
		const String& message) noexcept;

	void warning(
		const String& message) noexcept;

	void error(
		const String& message) noexcept;


	void error(
		const std::exception& exception) noexcept;

	void error(
		const std::exception& exception,
		const String& message) noexcept;
}; // Logger

using LoggerUPtr = std::unique_ptr<Logger>;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct LoggerParam
{
	bool skip_message_prefix{};
	Console* console{};
	String path{};
}; // LoggerParam


LoggerUPtr make_logger(
	const LoggerParam& param);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_LOGGER_INCLUDED
