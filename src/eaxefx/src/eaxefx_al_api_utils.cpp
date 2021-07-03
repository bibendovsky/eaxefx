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


#include "eaxefx_al_api_utils.h"

#include <cassert>

#include <algorithm>
#include <exception>

#include "AL/al.h"
#include "AL/efx.h"


namespace eaxefx::al_api
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void log_context_attribute_list(
	Logger* logger,
	const ::ALCint* al_attributes)
{
	assert(logger);

	if (!al_attributes || (*al_attributes == 0))
	{
		logger->info(Strings::indented_none);
		return;
	}

	auto string_buffer = String{};
	string_buffer.reserve(64);

	while (true)
	{
		const auto al_name = (*al_attributes++);

		if (al_name == AL_NONE)
		{
			break;
		}

		const auto al_value = (*al_attributes++);

		string_buffer.clear();
		string_buffer += Strings::indent;

		switch (al_name)
		{
			case ALC_MAJOR_VERSION:
				string_buffer += Strings::major_version;
				break;

			case ALC_MINOR_VERSION:
				string_buffer += Strings::minor_version;
				break;

			case ALC_FREQUENCY:
				string_buffer += Strings::mixer_frequency;
				break;

			case ALC_REFRESH:
				string_buffer += Strings::refresh_interval;
				break;

			case ALC_SYNC:
				string_buffer += Strings::is_synchronous;
				break;

			case ALC_MONO_SOURCES:
				string_buffer += Strings::mono_sources;
				break;

			case ALC_STEREO_SOURCES:
				string_buffer += Strings::stereo_sources;
				break;

			case ALC_MAX_AUXILIARY_SENDS:
				string_buffer += Strings::max_auxiliary_sends;
				break;

			default:
				string_buffer += Strings::three_question_marks;
				string_buffer += " (";
				string_buffer += to_string(al_name);
				string_buffer += ')';
				break;
		}

		string_buffer += Strings::space_equals_space;
		string_buffer += to_string(al_value);

		logger->info(string_buffer.c_str());
	}
}

void log_extensions(
	Logger* logger,
	const char* al_extensions)
{
	assert(logger);

	if (!al_extensions || (*al_extensions) == '\0')
	{
		logger->info(Strings::indented_none);
		return;
	}

	const auto extensions_size = String::traits_type::length(al_extensions);

	auto extensions = String{};
	extensions.reserve(extensions_size + 1);
	extensions.assign(al_extensions, extensions_size);
	extensions.append('\0');
	std::replace(extensions.begin(), extensions.end(), ' ', '\0');

	auto extension = extensions.c_str();

	auto extension_string = String{};
	extension_string.reserve(64);

	while ((*extension) != '\0')
	{
		const auto extension_size = String::traits_type::length(extension);

		extension_string.clear();
		extension_string += Strings::indent;
		extension_string.append(extension, extension_size);

		logger->info(extension_string.c_str());

		extension += extension_size + 1;
	}
}

void log_string(
	Logger* logger,
	const char* title,
	const char* string)
{
	assert(logger);
	assert(title);

	auto message = String{};
	message.reserve(128);

	message += title;
	message += ": ";
	message += (string ? string : Strings::null);

	logger->info(message.c_str());
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx::al_api
