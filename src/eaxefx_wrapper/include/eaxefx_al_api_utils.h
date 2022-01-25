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


#ifndef EAXEFX_AL_API_UTILS_INCLUDED
#define EAXEFX_AL_API_UTILS_INCLUDED


#include "AL/alc.h"

#include "eaxefx_logger.h"
#include "eaxefx_string.h"


namespace eaxefx::al_api
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct Strings
{
	static constexpr auto indent = "    ";
	static constexpr auto indented_none = "    none";
	static constexpr auto space_equals_space = " = ";
	static constexpr auto three_question_marks = "???";
	static constexpr auto equals_line_16 = "================";
	static constexpr auto null = "<null>";

	static constexpr auto major_version = "major version";
	static constexpr auto minor_version = "minor version";
	static constexpr auto mixer_frequency = "mixer frequency";
	static constexpr auto refresh_interval = "refresh interval";
	static constexpr auto is_synchronous = "is synchronous";
	static constexpr auto mono_sources = "mono sources";
	static constexpr auto stereo_sources = "stereo sources";
	static constexpr auto max_auxiliary_sends = "max auxiliary sends";
}; // Strings

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void log_context_attribute_list(
	Logger* logger,
	const ALCint* al_attributes);

void log_extensions(
	Logger* logger,
	const char* al_extensions);

void log_string(
	Logger* logger,
	const char* title,
	const char* string);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::al_api


#endif // !EAXEFX_AL_API_UTILS_INCLUDED

