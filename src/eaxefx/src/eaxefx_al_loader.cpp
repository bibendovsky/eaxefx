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

#include "eaxefx_al_symbols.h"
#include "eaxefx_exception.h"


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

	~AlLoaderImpl();


private:
	eaxefx::SharedLibrary* shared_library_{};
	bool has_missing_symbol_{};


	template<typename T>
	void resolve_symbol(
		const char* symbol_name,
		T& symbol) noexcept
	{
		symbol = reinterpret_cast<T>(shared_library_->resolve(symbol_name));

		if (!symbol)
		{
			has_missing_symbol_ = true;
		}
	}

	void resolve_symbols();

	void clear_symbols() noexcept;
}; // AlLoaderImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlLoaderImpl::AlLoaderImpl(
	eaxefx::SharedLibrary* shared_library)
	:
	shared_library_{shared_library}
{
	if (!shared_library_)
	{
		throw AlLoaderImplException("Null shared library.");
	}

	resolve_symbols();
}

AlLoaderImpl::~AlLoaderImpl()
{
	clear_symbols();
}

void AlLoaderImpl::resolve_symbols()
{
	has_missing_symbol_ = false;

	//
	// AL 1.1
	//

	resolve_symbol("alDopplerFactor", alDopplerFactor_);
	resolve_symbol("alDopplerVelocity", alDopplerVelocity_);
	resolve_symbol("alSpeedOfSound", alSpeedOfSound_);
	resolve_symbol("alDistanceModel", alDistanceModel_);
	resolve_symbol("alEnable", alEnable_);
	resolve_symbol("alDisable", alDisable_);
	resolve_symbol("alIsEnabled", alIsEnabled_);
	resolve_symbol("alGetString", alGetString_);
	resolve_symbol("alGetBooleanv", alGetBooleanv_);
	resolve_symbol("alGetIntegerv", alGetIntegerv_);
	resolve_symbol("alGetFloatv", alGetFloatv_);
	resolve_symbol("alGetDoublev", alGetDoublev_);
	resolve_symbol("alGetBoolean", alGetBoolean_);
	resolve_symbol("alGetInteger", alGetInteger_);
	resolve_symbol("alGetFloat", alGetFloat_);
	resolve_symbol("alGetDouble", alGetDouble_);
	resolve_symbol("alGetError", alGetError_);
	resolve_symbol("alIsExtensionPresent", alIsExtensionPresent_);
	resolve_symbol("alGetProcAddress", alGetProcAddress_);
	resolve_symbol("alGetEnumValue", alGetEnumValue_);
	resolve_symbol("alListenerf", alListenerf_);
	resolve_symbol("alListener3f", alListener3f_);
	resolve_symbol("alListenerfv", alListenerfv_);
	resolve_symbol("alListeneri", alListeneri_);
	resolve_symbol("alListener3i", alListener3i_);
	resolve_symbol("alListeneriv", alListeneriv_);
	resolve_symbol("alGetListenerf", alGetListenerf_);
	resolve_symbol("alGetListener3f", alGetListener3f_);
	resolve_symbol("alGetListenerfv", alGetListenerfv_);
	resolve_symbol("alGetListeneri", alGetListeneri_);
	resolve_symbol("alGetListener3i", alGetListener3i_);
	resolve_symbol("alGetListeneriv", alGetListeneriv_);
	resolve_symbol("alGenSources", alGenSources_);
	resolve_symbol("alDeleteSources", alDeleteSources_);
	resolve_symbol("alIsSource", alIsSource_);
	resolve_symbol("alSourcef", alSourcef_);
	resolve_symbol("alSource3f", alSource3f_);
	resolve_symbol("alSourcefv", alSourcefv_);
	resolve_symbol("alSourcei", alSourcei_);
	resolve_symbol("alSource3i", alSource3i_);
	resolve_symbol("alSourceiv", alSourceiv_);
	resolve_symbol("alGetSourcef", alGetSourcef_);
	resolve_symbol("alGetSource3f", alGetSource3f_);
	resolve_symbol("alGetSourcefv", alGetSourcefv_);
	resolve_symbol("alGetSourcei", alGetSourcei_);
	resolve_symbol("alGetSource3i", alGetSource3i_);
	resolve_symbol("alGetSourceiv", alGetSourceiv_);
	resolve_symbol("alSourcePlayv", alSourcePlayv_);
	resolve_symbol("alSourceStopv", alSourceStopv_);
	resolve_symbol("alSourceRewindv", alSourceRewindv_);
	resolve_symbol("alSourcePausev", alSourcePausev_);
	resolve_symbol("alSourcePlay", alSourcePlay_);
	resolve_symbol("alSourceStop", alSourceStop_);
	resolve_symbol("alSourceRewind", alSourceRewind_);
	resolve_symbol("alSourcePause", alSourcePause_);
	resolve_symbol("alSourceQueueBuffers", alSourceQueueBuffers_);
	resolve_symbol("alSourceUnqueueBuffers", alSourceUnqueueBuffers_);
	resolve_symbol("alGenBuffers", alGenBuffers_);
	resolve_symbol("alDeleteBuffers", alDeleteBuffers_);
	resolve_symbol("alIsBuffer", alIsBuffer_);
	resolve_symbol("alBufferData", alBufferData_);
	resolve_symbol("alBufferf", alBufferf_);
	resolve_symbol("alBuffer3f", alBuffer3f_);
	resolve_symbol("alBufferfv", alBufferfv_);
	resolve_symbol("alBufferi", alBufferi_);
	resolve_symbol("alBuffer3i", alBuffer3i_);
	resolve_symbol("alBufferiv", alBufferiv_);
	resolve_symbol("alGetBufferf", alGetBufferf_);
	resolve_symbol("alGetBuffer3f", alGetBuffer3f_);
	resolve_symbol("alGetBufferfv", alGetBufferfv_);
	resolve_symbol("alGetBufferi", alGetBufferi_);
	resolve_symbol("alGetBuffer3i", alGetBuffer3i_);
	resolve_symbol("alGetBufferiv", alGetBufferiv_);


	//
	// ALC 1.1
	//

	resolve_symbol("alcCreateContext", alcCreateContext_);
	resolve_symbol("alcMakeContextCurrent", alcMakeContextCurrent_);
	resolve_symbol("alcProcessContext", alcProcessContext_);
	resolve_symbol("alcSuspendContext", alcSuspendContext_);
	resolve_symbol("alcDestroyContext", alcDestroyContext_);
	resolve_symbol("alcGetCurrentContext", alcGetCurrentContext_);
	resolve_symbol("alcGetContextsDevice", alcGetContextsDevice_);
	resolve_symbol("alcOpenDevice", alcOpenDevice_);
	resolve_symbol("alcCloseDevice", alcCloseDevice_);
	resolve_symbol("alcGetError", alcGetError_);
	resolve_symbol("alcIsExtensionPresent", alcIsExtensionPresent_);
	resolve_symbol("alcGetProcAddress", alcGetProcAddress_);
	resolve_symbol("alcGetEnumValue", alcGetEnumValue_);
	resolve_symbol("alcGetString", alcGetString_);
	resolve_symbol("alcGetIntegerv", alcGetIntegerv_);
	resolve_symbol("alcCaptureOpenDevice", alcCaptureOpenDevice_);
	resolve_symbol("alcCaptureCloseDevice", alcCaptureCloseDevice_);
	resolve_symbol("alcCaptureStart", alcCaptureStart_);
	resolve_symbol("alcCaptureStop", alcCaptureStop_);
	resolve_symbol("alcCaptureSamples", alcCaptureSamples_);


	//
	// EFX 1.0
	//

	resolve_symbol("alGenEffects", alGenEffects_);
	resolve_symbol("alDeleteEffects", alDeleteEffects_);
	resolve_symbol("alIsEffect", alIsEffect_);
	resolve_symbol("alEffecti", alEffecti_);
	resolve_symbol("alEffectiv", alEffectiv_);
	resolve_symbol("alEffectf", alEffectf_);
	resolve_symbol("alEffectfv", alEffectfv_);
	resolve_symbol("alGetEffecti", alGetEffecti_);
	resolve_symbol("alGetEffectiv", alGetEffectiv_);
	resolve_symbol("alGetEffectf", alGetEffectf_);
	resolve_symbol("alGetEffectfv", alGetEffectfv_);
	resolve_symbol("alGenFilters", alGenFilters_);
	resolve_symbol("alDeleteFilters", alDeleteFilters_);
	resolve_symbol("alIsFilter", alIsFilter_);
	resolve_symbol("alFilteri", alFilteri_);
	resolve_symbol("alFilteriv", alFilteriv_);
	resolve_symbol("alFilterf", alFilterf_);
	resolve_symbol("alFilterfv", alFilterfv_);
	resolve_symbol("alGetFilteri", alGetFilteri_);
	resolve_symbol("alGetFilteriv", alGetFilteriv_);
	resolve_symbol("alGetFilterf", alGetFilterf_);
	resolve_symbol("alGetFilterfv", alGetFilterfv_);
	resolve_symbol("alGenAuxiliaryEffectSlots", alGenAuxiliaryEffectSlots_);
	resolve_symbol("alDeleteAuxiliaryEffectSlots", alDeleteAuxiliaryEffectSlots_);
	resolve_symbol("alIsAuxiliaryEffectSlot", alIsAuxiliaryEffectSlot_);
	resolve_symbol("alAuxiliaryEffectSloti", alAuxiliaryEffectSloti_);
	resolve_symbol("alAuxiliaryEffectSlotiv", alAuxiliaryEffectSlotiv_);
	resolve_symbol("alAuxiliaryEffectSlotf", alAuxiliaryEffectSlotf_);
	resolve_symbol("alAuxiliaryEffectSlotfv", alAuxiliaryEffectSlotfv_);
	resolve_symbol("alGetAuxiliaryEffectSloti", alGetAuxiliaryEffectSloti_);
	resolve_symbol("alGetAuxiliaryEffectSlotiv", alGetAuxiliaryEffectSlotiv_);
	resolve_symbol("alGetAuxiliaryEffectSlotf", alGetAuxiliaryEffectSlotf_);
	resolve_symbol("alGetAuxiliaryEffectSlotfv", alGetAuxiliaryEffectSlotfv_);


	if (has_missing_symbol_)
	{
		throw AlLoaderImplException("Failed to resolve all AL symbols.");
	}
}

void AlLoaderImpl::clear_symbols() noexcept
{
	//
	// AL 1.1
	//

	alDopplerFactor_ = nullptr;
	alDopplerVelocity_ = nullptr;
	alSpeedOfSound_ = nullptr;
	alDistanceModel_ = nullptr;
	alEnable_ = nullptr;
	alDisable_ = nullptr;
	alIsEnabled_ = nullptr;
	alGetString_ = nullptr;
	alGetBooleanv_ = nullptr;
	alGetIntegerv_ = nullptr;
	alGetFloatv_ = nullptr;
	alGetDoublev_ = nullptr;
	alGetBoolean_ = nullptr;
	alGetInteger_ = nullptr;
	alGetFloat_ = nullptr;
	alGetDouble_ = nullptr;
	alGetError_ = nullptr;
	alIsExtensionPresent_ = nullptr;
	alGetProcAddress_ = nullptr;
	alGetEnumValue_ = nullptr;
	alListenerf_ = nullptr;
	alListener3f_ = nullptr;
	alListenerfv_ = nullptr;
	alListeneri_ = nullptr;
	alListener3i_ = nullptr;
	alListeneriv_ = nullptr;
	alGetListenerf_ = nullptr;
	alGetListener3f_ = nullptr;
	alGetListenerfv_ = nullptr;
	alGetListeneri_ = nullptr;
	alGetListener3i_ = nullptr;
	alGetListeneriv_ = nullptr;
	alGenSources_ = nullptr;
	alDeleteSources_ = nullptr;
	alIsSource_ = nullptr;
	alSourcef_ = nullptr;
	alSource3f_ = nullptr;
	alSourcefv_ = nullptr;
	alSourcei_ = nullptr;
	alSource3i_ = nullptr;
	alSourceiv_ = nullptr;
	alGetSourcef_ = nullptr;
	alGetSource3f_ = nullptr;
	alGetSourcefv_ = nullptr;
	alGetSourcei_ = nullptr;
	alGetSource3i_ = nullptr;
	alGetSourceiv_ = nullptr;
	alSourcePlayv_ = nullptr;
	alSourceStopv_ = nullptr;
	alSourceRewindv_ = nullptr;
	alSourcePausev_ = nullptr;
	alSourcePlay_ = nullptr;
	alSourceStop_ = nullptr;
	alSourceRewind_ = nullptr;
	alSourcePause_ = nullptr;
	alSourceQueueBuffers_ = nullptr;
	alSourceUnqueueBuffers_ = nullptr;
	alGenBuffers_ = nullptr;
	alDeleteBuffers_ = nullptr;
	alIsBuffer_ = nullptr;
	alBufferData_ = nullptr;
	alBufferf_ = nullptr;
	alBuffer3f_ = nullptr;
	alBufferfv_ = nullptr;
	alBufferi_ = nullptr;
	alBuffer3i_ = nullptr;
	alBufferiv_ = nullptr;
	alGetBufferf_ = nullptr;
	alGetBuffer3f_ = nullptr;
	alGetBufferfv_ = nullptr;
	alGetBufferi_ = nullptr;
	alGetBuffer3i_ = nullptr;
	alGetBufferiv_ = nullptr;


	//
	// ALC 1.1
	//

	alcCreateContext_ = nullptr;
	alcMakeContextCurrent_ = nullptr;
	alcProcessContext_ = nullptr;
	alcSuspendContext_ = nullptr;
	alcDestroyContext_ = nullptr;
	alcGetCurrentContext_ = nullptr;
	alcGetContextsDevice_ = nullptr;
	alcOpenDevice_ = nullptr;
	alcCloseDevice_ = nullptr;
	alcGetError_ = nullptr;
	alcIsExtensionPresent_ = nullptr;
	alcGetProcAddress_ = nullptr;
	alcGetEnumValue_ = nullptr;
	alcGetString_ = nullptr;
	alcGetIntegerv_ = nullptr;
	alcCaptureOpenDevice_ = nullptr;
	alcCaptureCloseDevice_ = nullptr;
	alcCaptureStart_ = nullptr;
	alcCaptureStop_ = nullptr;
	alcCaptureSamples_ = nullptr;


	//
	// EFX 1.0
	//

	alGenEffects_ = nullptr;
	alDeleteEffects_ = nullptr;
	alIsEffect_ = nullptr;
	alEffecti_ = nullptr;
	alEffectiv_ = nullptr;
	alEffectf_ = nullptr;
	alEffectfv_ = nullptr;
	alGetEffecti_ = nullptr;
	alGetEffectiv_ = nullptr;
	alGetEffectf_ = nullptr;
	alGetEffectfv_ = nullptr;
	alGenFilters_ = nullptr;
	alDeleteFilters_ = nullptr;
	alIsFilter_ = nullptr;
	alFilteri_ = nullptr;
	alFilteriv_ = nullptr;
	alFilterf_ = nullptr;
	alFilterfv_ = nullptr;
	alGetFilteri_ = nullptr;
	alGetFilteriv_ = nullptr;
	alGetFilterf_ = nullptr;
	alGetFilterfv_ = nullptr;
	alGenAuxiliaryEffectSlots_ = nullptr;
	alDeleteAuxiliaryEffectSlots_ = nullptr;
	alIsAuxiliaryEffectSlot_ = nullptr;
	alAuxiliaryEffectSloti_ = nullptr;
	alAuxiliaryEffectSlotiv_ = nullptr;
	alAuxiliaryEffectSlotf_ = nullptr;
	alAuxiliaryEffectSlotfv_ = nullptr;
	alGetAuxiliaryEffectSloti_ = nullptr;
	alGetAuxiliaryEffectSlotiv_ = nullptr;
	alGetAuxiliaryEffectSlotf_ = nullptr;
	alGetAuxiliaryEffectSlotfv_ = nullptr;
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
