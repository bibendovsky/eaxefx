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


#include "eaxefx_al_loader.h"

#include <cassert>

#include "eaxefx_al_symbols.h"
#include "eaxefx_exception.h"
#include "eaxefx_string.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlLoaderImplException :
	public Exception
{
public:
	explicit AlLoaderImplException(
		const char* message)
		:
		Exception{"AL_LOADER", message}
	{
	}
}; // AlLoaderImplException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlLoaderImpl :
	public AlLoader
{
public:
	AlLoaderImpl(
		SharedLibrary* shared_library);


	AlAlcSymbolsUPtr resolve_alc_symbols() override;

	AlAlSymbolsUPtr resolve_al_symbols() override;

	AlEfxSymbolsUPtr resolve_efx_symbols() override;


private:
	::LPALCGETPROCADDRESS alc_get_proc_address_{};
	::LPALGETPROCADDRESS al_get_proc_address_{};


	[[noreturn]]
	static void fail_resolve_symbol(
		const char* symbol_name);


	void initialize_alc_get_proc_address(
		eaxefx::SharedLibrary* shared_library);

	void initialize_al_get_proc_address(
		eaxefx::SharedLibrary* shared_library);


	template<
		typename T
	>
		void resolve_alc_symbol(
			const char* symbol_name,
			T& symbol)
	{
		symbol = reinterpret_cast<T>(alc_get_proc_address_(nullptr, symbol_name));

		if (!symbol)
		{
			fail_resolve_symbol(symbol_name);
		}
	}

	template<
		typename T
	>
	void resolve_al_symbol(
		const char* symbol_name,
		T& symbol)
	{
		symbol = reinterpret_cast<T>(al_get_proc_address_(symbol_name));

		if (!symbol)
		{
			fail_resolve_symbol(symbol_name);
		}
	}
}; // AlLoaderImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlLoaderImpl::AlLoaderImpl(
	eaxefx::SharedLibrary* shared_library)
{
	if (!shared_library)
	{
		throw AlLoaderImplException{"Null shared library."};
	}

	initialize_alc_get_proc_address(shared_library);
	initialize_al_get_proc_address(shared_library);
}

[[noreturn]]
void AlLoaderImpl::fail_resolve_symbol(
	const char* symbol_name)
{
	assert(symbol_name);

	auto error_message = String{};
	error_message.clear();
	error_message += "Symbol \"";
	error_message += symbol_name;
	error_message += "\" not found.";

	throw AlLoaderImplException{error_message.c_str()};
}

void AlLoaderImpl::initialize_alc_get_proc_address(
	eaxefx::SharedLibrary* shared_library)
{
	assert(shared_library);

	alc_get_proc_address_ = reinterpret_cast<decltype(alc_get_proc_address_)>(
		shared_library->resolve(AlAlcSymbolsNames::alcGetProcAddress)
	);

	if (!alc_get_proc_address_)
	{
		fail_resolve_symbol(AlAlcSymbolsNames::alcGetProcAddress);
	}
}

void AlLoaderImpl::initialize_al_get_proc_address(
	eaxefx::SharedLibrary* shared_library)
{
	assert(shared_library);

	al_get_proc_address_ = reinterpret_cast<decltype(al_get_proc_address_)>(
		shared_library->resolve(AlAlSymbolsNames::alGetProcAddress)
	);

	if (!al_get_proc_address_)
	{
		fail_resolve_symbol(AlAlSymbolsNames::alGetProcAddress);
	}
}

AlAlcSymbolsUPtr AlLoaderImpl::resolve_alc_symbols()
{
	auto al_alc_symbols = std::make_unique<AlAlcSymbols>();

	resolve_alc_symbol(AlAlcSymbolsNames::alcCreateContext, al_alc_symbols->alcCreateContext);
	resolve_alc_symbol(AlAlcSymbolsNames::alcMakeContextCurrent, al_alc_symbols->alcMakeContextCurrent);
	resolve_alc_symbol(AlAlcSymbolsNames::alcProcessContext, al_alc_symbols->alcProcessContext);
	resolve_alc_symbol(AlAlcSymbolsNames::alcSuspendContext, al_alc_symbols->alcSuspendContext);
	resolve_alc_symbol(AlAlcSymbolsNames::alcDestroyContext, al_alc_symbols->alcDestroyContext);
	resolve_alc_symbol(AlAlcSymbolsNames::alcGetCurrentContext, al_alc_symbols->alcGetCurrentContext);
	resolve_alc_symbol(AlAlcSymbolsNames::alcGetContextsDevice, al_alc_symbols->alcGetContextsDevice);
	resolve_alc_symbol(AlAlcSymbolsNames::alcOpenDevice, al_alc_symbols->alcOpenDevice);
	resolve_alc_symbol(AlAlcSymbolsNames::alcCloseDevice, al_alc_symbols->alcCloseDevice);
	resolve_alc_symbol(AlAlcSymbolsNames::alcGetError, al_alc_symbols->alcGetError);
	resolve_alc_symbol(AlAlcSymbolsNames::alcIsExtensionPresent, al_alc_symbols->alcIsExtensionPresent);
	resolve_alc_symbol(AlAlcSymbolsNames::alcGetProcAddress, al_alc_symbols->alcGetProcAddress);
	resolve_alc_symbol(AlAlcSymbolsNames::alcGetEnumValue, al_alc_symbols->alcGetEnumValue);
	resolve_alc_symbol(AlAlcSymbolsNames::alcGetString, al_alc_symbols->alcGetString);
	resolve_alc_symbol(AlAlcSymbolsNames::alcGetIntegerv, al_alc_symbols->alcGetIntegerv);
	resolve_alc_symbol(AlAlcSymbolsNames::alcCaptureOpenDevice, al_alc_symbols->alcCaptureOpenDevice);
	resolve_alc_symbol(AlAlcSymbolsNames::alcCaptureCloseDevice, al_alc_symbols->alcCaptureCloseDevice);
	resolve_alc_symbol(AlAlcSymbolsNames::alcCaptureStart, al_alc_symbols->alcCaptureStart);
	resolve_alc_symbol(AlAlcSymbolsNames::alcCaptureStop, al_alc_symbols->alcCaptureStop);
	resolve_alc_symbol(AlAlcSymbolsNames::alcCaptureSamples, al_alc_symbols->alcCaptureSamples);

	return al_alc_symbols;
}

AlAlSymbolsUPtr AlLoaderImpl::resolve_al_symbols()
{
	auto al_al_symbols = std::make_unique<AlAlSymbols>();

	resolve_al_symbol(AlAlSymbolsNames::alDopplerFactor, al_al_symbols->alDopplerFactor);
	resolve_al_symbol(AlAlSymbolsNames::alDopplerVelocity, al_al_symbols->alDopplerVelocity);
	resolve_al_symbol(AlAlSymbolsNames::alSpeedOfSound, al_al_symbols->alSpeedOfSound);
	resolve_al_symbol(AlAlSymbolsNames::alDistanceModel, al_al_symbols->alDistanceModel);
	resolve_al_symbol(AlAlSymbolsNames::alEnable, al_al_symbols->alEnable);
	resolve_al_symbol(AlAlSymbolsNames::alDisable, al_al_symbols->alDisable);
	resolve_al_symbol(AlAlSymbolsNames::alIsEnabled, al_al_symbols->alIsEnabled);
	resolve_al_symbol(AlAlSymbolsNames::alGetString, al_al_symbols->alGetString);
	resolve_al_symbol(AlAlSymbolsNames::alGetBooleanv, al_al_symbols->alGetBooleanv);
	resolve_al_symbol(AlAlSymbolsNames::alGetIntegerv, al_al_symbols->alGetIntegerv);
	resolve_al_symbol(AlAlSymbolsNames::alGetFloatv, al_al_symbols->alGetFloatv);
	resolve_al_symbol(AlAlSymbolsNames::alGetDoublev, al_al_symbols->alGetDoublev);
	resolve_al_symbol(AlAlSymbolsNames::alGetBoolean, al_al_symbols->alGetBoolean);
	resolve_al_symbol(AlAlSymbolsNames::alGetInteger, al_al_symbols->alGetInteger);
	resolve_al_symbol(AlAlSymbolsNames::alGetFloat, al_al_symbols->alGetFloat);
	resolve_al_symbol(AlAlSymbolsNames::alGetDouble, al_al_symbols->alGetDouble);
	resolve_al_symbol(AlAlSymbolsNames::alGetError, al_al_symbols->alGetError);
	resolve_al_symbol(AlAlSymbolsNames::alIsExtensionPresent, al_al_symbols->alIsExtensionPresent);
	resolve_al_symbol(AlAlSymbolsNames::alGetProcAddress, al_al_symbols->alGetProcAddress);
	resolve_al_symbol(AlAlSymbolsNames::alGetEnumValue, al_al_symbols->alGetEnumValue);
	resolve_al_symbol(AlAlSymbolsNames::alListenerf, al_al_symbols->alListenerf);
	resolve_al_symbol(AlAlSymbolsNames::alListener3f, al_al_symbols->alListener3f);
	resolve_al_symbol(AlAlSymbolsNames::alListenerfv, al_al_symbols->alListenerfv);
	resolve_al_symbol(AlAlSymbolsNames::alListeneri, al_al_symbols->alListeneri);
	resolve_al_symbol(AlAlSymbolsNames::alListener3i, al_al_symbols->alListener3i);
	resolve_al_symbol(AlAlSymbolsNames::alListeneriv, al_al_symbols->alListeneriv);
	resolve_al_symbol(AlAlSymbolsNames::alGetListenerf, al_al_symbols->alGetListenerf);
	resolve_al_symbol(AlAlSymbolsNames::alGetListener3f, al_al_symbols->alGetListener3f);
	resolve_al_symbol(AlAlSymbolsNames::alGetListenerfv, al_al_symbols->alGetListenerfv);
	resolve_al_symbol(AlAlSymbolsNames::alGetListeneri, al_al_symbols->alGetListeneri);
	resolve_al_symbol(AlAlSymbolsNames::alGetListener3i, al_al_symbols->alGetListener3i);
	resolve_al_symbol(AlAlSymbolsNames::alGetListeneriv, al_al_symbols->alGetListeneriv);
	resolve_al_symbol(AlAlSymbolsNames::alGenSources, al_al_symbols->alGenSources);
	resolve_al_symbol(AlAlSymbolsNames::alDeleteSources, al_al_symbols->alDeleteSources);
	resolve_al_symbol(AlAlSymbolsNames::alIsSource, al_al_symbols->alIsSource);
	resolve_al_symbol(AlAlSymbolsNames::alSourcef, al_al_symbols->alSourcef);
	resolve_al_symbol(AlAlSymbolsNames::alSource3f, al_al_symbols->alSource3f);
	resolve_al_symbol(AlAlSymbolsNames::alSourcefv, al_al_symbols->alSourcefv);
	resolve_al_symbol(AlAlSymbolsNames::alSourcei, al_al_symbols->alSourcei);
	resolve_al_symbol(AlAlSymbolsNames::alSource3i, al_al_symbols->alSource3i);
	resolve_al_symbol(AlAlSymbolsNames::alSourceiv, al_al_symbols->alSourceiv);
	resolve_al_symbol(AlAlSymbolsNames::alGetSourcef, al_al_symbols->alGetSourcef);
	resolve_al_symbol(AlAlSymbolsNames::alGetSource3f, al_al_symbols->alGetSource3f);
	resolve_al_symbol(AlAlSymbolsNames::alGetSourcefv, al_al_symbols->alGetSourcefv);
	resolve_al_symbol(AlAlSymbolsNames::alGetSourcei, al_al_symbols->alGetSourcei);
	resolve_al_symbol(AlAlSymbolsNames::alGetSource3i, al_al_symbols->alGetSource3i);
	resolve_al_symbol(AlAlSymbolsNames::alGetSourceiv, al_al_symbols->alGetSourceiv);
	resolve_al_symbol(AlAlSymbolsNames::alSourcePlayv, al_al_symbols->alSourcePlayv);
	resolve_al_symbol(AlAlSymbolsNames::alSourceStopv, al_al_symbols->alSourceStopv);
	resolve_al_symbol(AlAlSymbolsNames::alSourceRewindv, al_al_symbols->alSourceRewindv);
	resolve_al_symbol(AlAlSymbolsNames::alSourcePausev, al_al_symbols->alSourcePausev);
	resolve_al_symbol(AlAlSymbolsNames::alSourcePlay, al_al_symbols->alSourcePlay);
	resolve_al_symbol(AlAlSymbolsNames::alSourceStop, al_al_symbols->alSourceStop);
	resolve_al_symbol(AlAlSymbolsNames::alSourceRewind, al_al_symbols->alSourceRewind);
	resolve_al_symbol(AlAlSymbolsNames::alSourcePause, al_al_symbols->alSourcePause);
	resolve_al_symbol(AlAlSymbolsNames::alSourceQueueBuffers, al_al_symbols->alSourceQueueBuffers);
	resolve_al_symbol(AlAlSymbolsNames::alSourceUnqueueBuffers, al_al_symbols->alSourceUnqueueBuffers);
	resolve_al_symbol(AlAlSymbolsNames::alGenBuffers, al_al_symbols->alGenBuffers);
	resolve_al_symbol(AlAlSymbolsNames::alDeleteBuffers, al_al_symbols->alDeleteBuffers);
	resolve_al_symbol(AlAlSymbolsNames::alIsBuffer, al_al_symbols->alIsBuffer);
	resolve_al_symbol(AlAlSymbolsNames::alBufferData, al_al_symbols->alBufferData);
	resolve_al_symbol(AlAlSymbolsNames::alBufferf, al_al_symbols->alBufferf);
	resolve_al_symbol(AlAlSymbolsNames::alBuffer3f, al_al_symbols->alBuffer3f);
	resolve_al_symbol(AlAlSymbolsNames::alBufferfv, al_al_symbols->alBufferfv);
	resolve_al_symbol(AlAlSymbolsNames::alBufferi, al_al_symbols->alBufferi);
	resolve_al_symbol(AlAlSymbolsNames::alBuffer3i, al_al_symbols->alBuffer3i);
	resolve_al_symbol(AlAlSymbolsNames::alBufferiv, al_al_symbols->alBufferiv);
	resolve_al_symbol(AlAlSymbolsNames::alGetBufferf, al_al_symbols->alGetBufferf);
	resolve_al_symbol(AlAlSymbolsNames::alGetBuffer3f, al_al_symbols->alGetBuffer3f);
	resolve_al_symbol(AlAlSymbolsNames::alGetBufferfv, al_al_symbols->alGetBufferfv);
	resolve_al_symbol(AlAlSymbolsNames::alGetBufferi, al_al_symbols->alGetBufferi);
	resolve_al_symbol(AlAlSymbolsNames::alGetBuffer3i, al_al_symbols->alGetBuffer3i);
	resolve_al_symbol(AlAlSymbolsNames::alGetBufferiv, al_al_symbols->alGetBufferiv);

	return al_al_symbols;
}

AlEfxSymbolsUPtr AlLoaderImpl::resolve_efx_symbols()
{
	auto al_efx_symbols = std::make_unique<AlEfxSymbols>();

	resolve_al_symbol(AlEfxSymbolsNames::alGenEffects, al_efx_symbols->alGenEffects);
	resolve_al_symbol(AlEfxSymbolsNames::alDeleteEffects, al_efx_symbols->alDeleteEffects);
	resolve_al_symbol(AlEfxSymbolsNames::alIsEffect, al_efx_symbols->alIsEffect);
	resolve_al_symbol(AlEfxSymbolsNames::alEffecti, al_efx_symbols->alEffecti);
	resolve_al_symbol(AlEfxSymbolsNames::alEffectiv, al_efx_symbols->alEffectiv);
	resolve_al_symbol(AlEfxSymbolsNames::alEffectf, al_efx_symbols->alEffectf);
	resolve_al_symbol(AlEfxSymbolsNames::alEffectfv, al_efx_symbols->alEffectfv);
	resolve_al_symbol(AlEfxSymbolsNames::alGetEffecti, al_efx_symbols->alGetEffecti);
	resolve_al_symbol(AlEfxSymbolsNames::alGetEffectiv, al_efx_symbols->alGetEffectiv);
	resolve_al_symbol(AlEfxSymbolsNames::alGetEffectf, al_efx_symbols->alGetEffectf);
	resolve_al_symbol(AlEfxSymbolsNames::alGetEffectfv, al_efx_symbols->alGetEffectfv);
	resolve_al_symbol(AlEfxSymbolsNames::alGenFilters, al_efx_symbols->alGenFilters);
	resolve_al_symbol(AlEfxSymbolsNames::alDeleteFilters, al_efx_symbols->alDeleteFilters);
	resolve_al_symbol(AlEfxSymbolsNames::alIsFilter, al_efx_symbols->alIsFilter);
	resolve_al_symbol(AlEfxSymbolsNames::alFilteri, al_efx_symbols->alFilteri);
	resolve_al_symbol(AlEfxSymbolsNames::alFilteriv, al_efx_symbols->alFilteriv);
	resolve_al_symbol(AlEfxSymbolsNames::alFilterf, al_efx_symbols->alFilterf);
	resolve_al_symbol(AlEfxSymbolsNames::alFilterfv, al_efx_symbols->alFilterfv);
	resolve_al_symbol(AlEfxSymbolsNames::alGetFilteri, al_efx_symbols->alGetFilteri);
	resolve_al_symbol(AlEfxSymbolsNames::alGetFilteriv, al_efx_symbols->alGetFilteriv);
	resolve_al_symbol(AlEfxSymbolsNames::alGetFilterf, al_efx_symbols->alGetFilterf);
	resolve_al_symbol(AlEfxSymbolsNames::alGetFilterfv, al_efx_symbols->alGetFilterfv);
	resolve_al_symbol(AlEfxSymbolsNames::alGenAuxiliaryEffectSlots, al_efx_symbols->alGenAuxiliaryEffectSlots);
	resolve_al_symbol(AlEfxSymbolsNames::alDeleteAuxiliaryEffectSlots, al_efx_symbols->alDeleteAuxiliaryEffectSlots);
	resolve_al_symbol(AlEfxSymbolsNames::alIsAuxiliaryEffectSlot, al_efx_symbols->alIsAuxiliaryEffectSlot);
	resolve_al_symbol(AlEfxSymbolsNames::alAuxiliaryEffectSloti, al_efx_symbols->alAuxiliaryEffectSloti);
	resolve_al_symbol(AlEfxSymbolsNames::alAuxiliaryEffectSlotiv, al_efx_symbols->alAuxiliaryEffectSlotiv);
	resolve_al_symbol(AlEfxSymbolsNames::alAuxiliaryEffectSlotf, al_efx_symbols->alAuxiliaryEffectSlotf);
	resolve_al_symbol(AlEfxSymbolsNames::alAuxiliaryEffectSlotfv, al_efx_symbols->alAuxiliaryEffectSlotfv);
	resolve_al_symbol(AlEfxSymbolsNames::alGetAuxiliaryEffectSloti, al_efx_symbols->alGetAuxiliaryEffectSloti);
	resolve_al_symbol(AlEfxSymbolsNames::alGetAuxiliaryEffectSlotiv, al_efx_symbols->alGetAuxiliaryEffectSlotiv);
	resolve_al_symbol(AlEfxSymbolsNames::alGetAuxiliaryEffectSlotf, al_efx_symbols->alGetAuxiliaryEffectSlotf);
	resolve_al_symbol(AlEfxSymbolsNames::alGetAuxiliaryEffectSlotfv, al_efx_symbols->alGetAuxiliaryEffectSlotfv);

	return al_efx_symbols;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlLoaderUPtr make_al_loader(
	SharedLibrary* shared_library)
{
	return std::make_unique<AlLoaderImpl>(shared_library);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
