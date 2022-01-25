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

#ifndef EAXEFX_AL_API_CONTEXT_INCLUDED
#define EAXEFX_AL_API_CONTEXT_INCLUDED


#include "AL/alc.h"

#include <memory>
#include <string_view>

#include "eaxefx_al_loader.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eaxx.h"
#include "eaxefx_logger.h"
#include "eaxefx_mutex.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct AlApiContextInitParam
{
	ALCdevice* al_device{};
	const ALCint* al_context_attributes{};
}; // AlApiContextInitParam

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlApiContext
{
public:
	AlApiContext() noexcept = default;

	virtual ~AlApiContext() = default;


	virtual void alc_create_context(
		const AlApiContextInitParam& param) = 0;

	virtual void alc_make_current() = 0;

	virtual void alc_destroy() = 0;

	virtual void* al_get_proc_address(
		std::string_view symbol_name) const noexcept = 0;

	virtual bool al_is_extension_present(
		const char* extension_name) const noexcept = 0;

	virtual const char* al_get_string(
		ALenum param) const noexcept = 0;

	virtual void al_gen_sources(
		ALsizei n,
		ALuint* sources) = 0;

	virtual void al_delete_sources(
		ALsizei n,
		const ALuint* sources) = 0;


	virtual ALCcontext* get_al_context() const noexcept = 0;

	virtual Eaxx& get_eaxx() = 0;
}; // AlApiContext

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using AlApiContextUPtr = std::unique_ptr<AlApiContext>;

AlApiContextUPtr make_al_api_context();

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_AL_API_CONTEXT_INCLUDED

