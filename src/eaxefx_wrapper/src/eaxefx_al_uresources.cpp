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


#include "eaxefx_al_uresources.h"

#include <cassert>

#include "AL/alc.h"

#include "eaxefx_al_api.h"
#include "eaxefx_al_symbols.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void AlcDeviceDeleter::operator()(
	ALCdevice* alc_device) const noexcept
{
	const auto al_alc_symbols = g_al_api.get_al_alc_symbols();

	if (al_alc_symbols)
	{
		static_cast<void>(al_alc_symbols->alcCloseDevice(alc_device));
	}
	else
	{
		assert(!"Null ALC symbols.");
	}
}

void AlcContextDeleter::operator()(
	ALCcontext* alc_context) const noexcept
{
	const auto al_alc_symbols = g_al_api.get_al_alc_symbols();

	if (al_alc_symbols)
	{
		static_cast<void>(al_alc_symbols->alcDestroyContext(alc_context));
	}
	else
	{
		assert(!"Null ALC symbols.");
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
