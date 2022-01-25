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


#ifndef EAXEFX_AL_API_INCLUDED
#define EAXEFX_AL_API_INCLUDED


#include "eaxefx_al_api_context.h"
#include "eaxefx_al_loader.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_logger.h"
#include "eaxefx_moveable_mutex_lock.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlApi
{
public:
	AlApi() noexcept = default;

	virtual ~AlApi() = default;


	virtual Logger* get_logger() noexcept = 0;

	virtual AlLoader* get_al_loader() const noexcept = 0;

	virtual AlAlcSymbols* get_al_alc_symbols() const noexcept = 0;

	virtual AlAlSymbols* get_al_al_symbols() const noexcept = 0;

	virtual void on_thread_detach() noexcept = 0;

	virtual void on_process_detach() noexcept = 0;

	virtual MoveableMutexLock get_lock() = 0;

	virtual AlApiContext& get_current_context() = 0;

	virtual ALboolean AL_APIENTRY eax_set_buffer_mode(
		ALsizei n,
		const ALuint* buffers,
		ALint value) = 0;

	virtual ALenum AL_APIENTRY eax_get_buffer_mode(
		ALuint buffer,
		ALint* pReserved) = 0;
}; // AlApi

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern AlApi& g_al_api;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_AL_API_INCLUDED
