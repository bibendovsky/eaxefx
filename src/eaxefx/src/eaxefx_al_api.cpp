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


#include <exception>
#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"

#include "eaxefx_al_loader.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eax_api.h"
#include "eaxefx_eaxx.h"
#include "eaxefx_exception.h"
#include "eaxefx_logger.h"
#include "eaxefx_shared_library.h"

#include "eaxefx_patch.h"
#include "eaxefx_patch_collection.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlApiException :
	public Exception
{
public:
	explicit AlApiException(
		const char* message)
		:
		Exception{"AL_API", message}
	{
	}
}; // AlApiException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlApi
{
public:
	AlApi();

	~AlApi();

	bool is_initialized() const noexcept;

	void on_thread_detach() noexcept;

	void on_process_detach() noexcept;

	Eaxx* get_eaxx() const noexcept;

	void* find_function(
		std::string_view name) const noexcept;


private:
	using FunctionMap = std::unordered_map<std::string_view, void*>;


	bool is_initialized_{};
	FunctionMap function_map_{make_function_map()};
	eaxefx::LoggerUPtr logger_{};
	eaxefx::SharedLibraryUPtr al_library_{};
	eaxefx::AlLoaderUPtr al_loader_{};
	eaxefx::EaxxUPtr eaxx_{};


	static FunctionMap make_function_map();

	void apply_patch_collection() noexcept;

	void initialize_al_shared_library();

	void initialize_internal();
}; // AlApi

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlApi::AlApi()
{
	initialize_internal();
}

AlApi::~AlApi()
{
	if (logger_)
	{
		logger_->info("");
		logger_->info(">>>>>>>>>>>>>>>>>>>>>>>>");
		logger_->info("");
	}
}

bool AlApi::is_initialized() const noexcept
{
	return is_initialized_;
}

void AlApi::on_thread_detach() noexcept
{
	if (!is_initialized_)
	{
		return;
	}

	logger_->flush();
}

void AlApi::on_process_detach() noexcept
{
	if (!is_initialized_)
	{
		return;
	}

	logger_->set_immediate_mode();
}

Eaxx* AlApi::get_eaxx() const noexcept
{
	return eaxx_.get();
}

void* AlApi::find_function(
	std::string_view name) const noexcept
{
	const auto it = function_map_.find(name);

	if (it != function_map_.end())
	{
		return it->second;
	}

	return nullptr;
}

AlApi::FunctionMap AlApi::make_function_map()
{
	auto function_map = FunctionMap
	{
		{"alDopplerFactor", reinterpret_cast<void*>(::alDopplerFactor)},
		{"alDopplerVelocity", reinterpret_cast<void*>(::alDopplerVelocity)},
		{"alSpeedOfSound", reinterpret_cast<void*>(::alSpeedOfSound)},
		{"alDistanceModel", reinterpret_cast<void*>(::alDistanceModel)},
		{"alEnable", reinterpret_cast<void*>(::alEnable)},
		{"alDisable", reinterpret_cast<void*>(::alDisable)},
		{"alIsEnabled", reinterpret_cast<void*>(::alIsEnabled)},
		{"alGetString", reinterpret_cast<void*>(::alGetString)},
		{"alGetBooleanv", reinterpret_cast<void*>(::alGetBooleanv)},
		{"alGetIntegerv", reinterpret_cast<void*>(::alGetIntegerv)},
		{"alGetFloatv", reinterpret_cast<void*>(::alGetFloatv)},
		{"alGetDoublev", reinterpret_cast<void*>(::alGetDoublev)},
		{"alGetBoolean", reinterpret_cast<void*>(::alGetBoolean)},
		{"alGetInteger", reinterpret_cast<void*>(::alGetInteger)},
		{"alGetFloat", reinterpret_cast<void*>(::alGetFloat)},
		{"alGetDouble", reinterpret_cast<void*>(::alGetDouble)},
		{"alGetError", reinterpret_cast<void*>(::alGetError)},
		{"alIsExtensionPresent", reinterpret_cast<void*>(::alIsExtensionPresent)},
		{"alGetProcAddress", reinterpret_cast<void*>(::alGetProcAddress)},
		{"alGetEnumValue", reinterpret_cast<void*>(::alGetEnumValue)},
		{"alListenerf", reinterpret_cast<void*>(::alListenerf)},
		{"alListener3f", reinterpret_cast<void*>(::alListener3f)},
		{"alListenerfv", reinterpret_cast<void*>(::alListenerfv)},
		{"alListeneri", reinterpret_cast<void*>(::alListeneri)},
		{"alListener3i", reinterpret_cast<void*>(::alListener3i)},
		{"alListeneriv", reinterpret_cast<void*>(::alListeneriv)},
		{"alGetListenerf", reinterpret_cast<void*>(::alGetListenerf)},
		{"alGetListener3f", reinterpret_cast<void*>(::alGetListener3f)},
		{"alGetListenerfv", reinterpret_cast<void*>(::alGetListenerfv)},
		{"alGetListeneri", reinterpret_cast<void*>(::alGetListeneri)},
		{"alGetListener3i", reinterpret_cast<void*>(::alGetListener3i)},
		{"alGetListeneriv", reinterpret_cast<void*>(::alGetListeneriv)},
		{"alGenSources", reinterpret_cast<void*>(::alGenSources)},
		{"alDeleteSources", reinterpret_cast<void*>(::alDeleteSources)},
		{"alIsSource", reinterpret_cast<void*>(::alIsSource)},
		{"alSourcef", reinterpret_cast<void*>(::alSourcef)},
		{"alSource3f", reinterpret_cast<void*>(::alSource3f)},
		{"alSourcefv", reinterpret_cast<void*>(::alSourcefv)},
		{"alSourcei", reinterpret_cast<void*>(::alSourcei)},
		{"alSource3i", reinterpret_cast<void*>(::alSource3i)},
		{"alSourceiv", reinterpret_cast<void*>(::alSourceiv)},
		{"alGetSourcef", reinterpret_cast<void*>(::alGetSourcef)},
		{"alGetSource3f", reinterpret_cast<void*>(::alGetSource3f)},
		{"alGetSourcefv", reinterpret_cast<void*>(::alGetSourcefv)},
		{"alGetSourcei", reinterpret_cast<void*>(::alGetSourcei)},
		{"alGetSource3i", reinterpret_cast<void*>(::alGetSource3i)},
		{"alGetSourceiv", reinterpret_cast<void*>(::alGetSourceiv)},
		{"alSourcePlayv", reinterpret_cast<void*>(::alSourcePlayv)},
		{"alSourceStopv", reinterpret_cast<void*>(::alSourceStopv)},
		{"alSourceRewindv", reinterpret_cast<void*>(::alSourceRewindv)},
		{"alSourcePausev", reinterpret_cast<void*>(::alSourcePausev)},
		{"alSourcePlay", reinterpret_cast<void*>(::alSourcePlay)},
		{"alSourceStop", reinterpret_cast<void*>(::alSourceStop)},
		{"alSourceRewind", reinterpret_cast<void*>(::alSourceRewind)},
		{"alSourcePause", reinterpret_cast<void*>(::alSourcePause)},
		{"alSourceQueueBuffers", reinterpret_cast<void*>(::alSourceQueueBuffers)},
		{"alSourceUnqueueBuffers", reinterpret_cast<void*>(::alSourceUnqueueBuffers)},
		{"alGenBuffers", reinterpret_cast<void*>(::alGenBuffers)},
		{"alDeleteBuffers", reinterpret_cast<void*>(::alDeleteBuffers)},
		{"alIsBuffer", reinterpret_cast<void*>(::alIsBuffer)},
		{"alBufferData", reinterpret_cast<void*>(::alBufferData)},
		{"alBufferf", reinterpret_cast<void*>(::alBufferf)},
		{"alBuffer3f", reinterpret_cast<void*>(::alBuffer3f)},
		{"alBufferfv", reinterpret_cast<void*>(::alBufferfv)},
		{"alBufferi", reinterpret_cast<void*>(::alBufferi)},
		{"alBuffer3i", reinterpret_cast<void*>(::alBuffer3i)},
		{"alBufferiv", reinterpret_cast<void*>(::alBufferiv)},
		{"alGetBufferf", reinterpret_cast<void*>(::alGetBufferf)},
		{"alGetBuffer3f", reinterpret_cast<void*>(::alGetBuffer3f)},
		{"alGetBufferfv", reinterpret_cast<void*>(::alGetBufferfv)},
		{"alGetBufferi", reinterpret_cast<void*>(::alGetBufferi)},
		{"alGetBuffer3i", reinterpret_cast<void*>(::alGetBuffer3i)},
		{"alGetBufferiv", reinterpret_cast<void*>(::alGetBufferiv)},

		{"alcCreateContext", reinterpret_cast<void*>(::alcCreateContext)},
		{"alcMakeContextCurrent", reinterpret_cast<void*>(::alcMakeContextCurrent)},
		{"alcProcessContext", reinterpret_cast<void*>(::alcProcessContext)},
		{"alcSuspendContext", reinterpret_cast<void*>(::alcSuspendContext)},
		{"alcDestroyContext", reinterpret_cast<void*>(::alcDestroyContext)},
		{"alcGetCurrentContext", reinterpret_cast<void*>(::alcGetCurrentContext)},
		{"alcGetContextsDevice", reinterpret_cast<void*>(::alcGetContextsDevice)},
		{"alcOpenDevice", reinterpret_cast<void*>(::alcOpenDevice)},
		{"alcCloseDevice", reinterpret_cast<void*>(::alcCloseDevice)},
		{"alcGetError", reinterpret_cast<void*>(::alcGetError)},
		{"alcIsExtensionPresent", reinterpret_cast<void*>(::alcIsExtensionPresent)},
		{"alcGetProcAddress", reinterpret_cast<void*>(::alcGetProcAddress)},
		{"alcGetEnumValue", reinterpret_cast<void*>(::alcGetEnumValue)},
		{"alcGetString", reinterpret_cast<void*>(::alcGetString)},
		{"alcGetIntegerv", reinterpret_cast<void*>(::alcGetIntegerv)},
		{"alcCaptureOpenDevice", reinterpret_cast<void*>(::alcCaptureOpenDevice)},
		{"alcCaptureCloseDevice", reinterpret_cast<void*>(::alcCaptureCloseDevice)},
		{"alcCaptureStart", reinterpret_cast<void*>(::alcCaptureStart)},
		{"alcCaptureStop", reinterpret_cast<void*>(::alcCaptureStop)},
		{"alcCaptureSamples", reinterpret_cast<void*>(::alcCaptureSamples)},

		{"EAXGet", reinterpret_cast<void*>(::EAXGet)},
		{"EAXSet", reinterpret_cast<void*>(::EAXSet)},
	};

	return function_map;
}

void AlApi::apply_patch_collection() noexcept
try
{
	const auto patch_collection = make_patch_collection();

	for (const auto& patch : patch_collection)
	{
		const auto process_patcher = make_process_patcher(patch);

		const auto status = process_patcher->get_status();

		switch (status)
		{
			case PatchStatus::unpatched:
				{
					const auto message = std::string{} + "Try to patch " + patch.name + ".";
					logger_->info(message.c_str());
					process_patcher->apply();
				}

				break;

			case PatchStatus::patched:
				{
					const auto message = std::string{} + patch.name + " already patched.";
					logger_->info(message.c_str());
				}

				break;

			default:
				break;
		}
	}
}
catch (const std::exception& ex)
{
	logger_->error("Failed to patch.");
	logger_->write(ex);
}

void AlApi::initialize_al_shared_library()
{
	logger_->info("Load AL driver.");

	using KnownNames = std::vector<const char*>;

	const auto known_names = KnownNames
	{
		"eaxefx_driver.dll",
		"dsoal-aldrv.dll",
		"soft_oal.dll",
	};

	for (const auto& known_name : known_names)
	{
		try
		{
			const auto message = "Try to load a driver \"" + String{known_name} + "\".";
			logger_->info(message.c_str());
			al_library_ = make_shared_library(known_name);

			return;
		}
		catch (const std::exception& ex)
		{
			logger_->error(ex);
		}
	}

	throw AlApiException{"Failed to load any suitable driver."};
}

void AlApi::initialize_internal()
try
{
	auto logger_param = LoggerParam{};
	logger_param.skip_message_prefix = false;
	logger_param.console = nullptr;
	logger_param.path = "eaxefx_log.txt";

	logger_ = make_logger(logger_param);
	logger_->info("");
	logger_->info("<<<<<<<<<<<<<<<<<<<<<<<<");
	logger_->info("EAXEFX v" EAXEFX_VERSION);
	logger_->info("------------------------");
	logger_->info("");

	initialize_al_shared_library();

	logger_->info("Load AL symbols (AL 1.1; ALC 1.1; EFX 1.0).");
	al_loader_ = make_al_loader(al_library_.get());

	logger_->info("Create EAXX.");
	eaxx_ = make_eaxx(logger_.get(), al_loader_.get());

	apply_patch_collection();

	is_initialized_ = true;
}
catch (const std::exception& ex)
{
	if (logger_)
	{
		logger_->write(ex);
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

namespace
{


bool is_process_initialized_{};

using AlApiUPtr = std::unique_ptr<AlApi>;
AlApiUPtr al_api_{};


} // namespace


AlApi& get_al_api()
{
	if (!al_api_)
	{
		is_process_initialized_ = true;
		al_api_ = std::make_unique<AlApi>();
	}

	return *al_api_;
}

void on_thread_detach() noexcept
{
	if (is_process_initialized_)
	{
		auto& al_api = get_al_api();
		al_api.on_thread_detach();
	}
}

void on_process_attach() noexcept
{
	is_process_initialized_ = false;
}

void on_process_detach() noexcept
{
	if (is_process_initialized_)
	{
		auto& al_api = get_al_api();
		al_api.on_process_detach();
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern "C"
{


//
// AL 1.1
//

AL_API void AL_APIENTRY alDopplerFactor(
	::ALfloat value)
{
	eaxefx::alDopplerFactor_(value);
}

AL_API void AL_APIENTRY alDopplerVelocity(
	::ALfloat value)
{
	eaxefx::alDopplerVelocity_(value);
}

AL_API void AL_APIENTRY alSpeedOfSound(
	::ALfloat value)
{
	eaxefx::alSpeedOfSound_(value);
}

AL_API void AL_APIENTRY alDistanceModel(
	::ALenum distanceModel)
{
	eaxefx::alDistanceModel_(distanceModel);
}

AL_API void AL_APIENTRY alEnable(
	::ALenum capability)
{
	eaxefx::alEnable_(capability);
}

AL_API void AL_APIENTRY alDisable(
	::ALenum capability)
{
	eaxefx::alDisable_(capability);
}

AL_API ::ALboolean AL_APIENTRY alIsEnabled(
	::ALenum capability)
{
	return eaxefx::alIsEnabled_(capability);
}

AL_API const ::ALchar* AL_APIENTRY alGetString(
	::ALenum param)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->alGetString(param);
	}
	else
	{
		return nullptr;
	}
}

AL_API void AL_APIENTRY alGetBooleanv(
	::ALenum param,
	::ALboolean* values)
{
	eaxefx::alGetBooleanv_(param, values);
}

AL_API void AL_APIENTRY alGetIntegerv(
	::ALenum param,
	::ALint* values)
{
	eaxefx::alGetIntegerv_(param, values);
}

AL_API void AL_APIENTRY alGetFloatv(
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::alGetFloatv_(param, values);
}

AL_API void AL_APIENTRY alGetDoublev(
	::ALenum param,
	::ALdouble* values)
{
	eaxefx::alGetDoublev_(param, values);
}

AL_API ::ALboolean AL_APIENTRY alGetBoolean(
	::ALenum param)
{
	return eaxefx::alGetBoolean_(param);
}

AL_API ::ALint AL_APIENTRY alGetInteger(
	::ALenum param)
{
	return eaxefx::alGetInteger_(param);
}

AL_API ::ALfloat AL_APIENTRY alGetFloat(
	::ALenum param)
{
	return eaxefx::alGetFloat_(param);
}

AL_API ::ALdouble AL_APIENTRY alGetDouble(
	::ALenum param)
{
	return eaxefx::alGetDouble_(param);
}

AL_API ::ALenum AL_APIENTRY alGetError()
{
	return eaxefx::alGetError_();
}

AL_API ::ALboolean AL_APIENTRY alIsExtensionPresent(
	const ::ALchar* extname)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->alIsExtensionPresent(extname);
	}
	else
	{
		return AL_FALSE;
	}
}

AL_API void* AL_APIENTRY alGetProcAddress(
	const ::ALchar* fname)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		if (
			const auto address = al_api.find_function(fname);
			address)
		{
			return address;
		}
		else
		{
			return eaxefx::alGetProcAddress_(fname);
		}
	}

	return nullptr;
}

AL_API ::ALenum AL_APIENTRY alGetEnumValue(
	const ::ALchar* ename)
{
	return eaxefx::alGetEnumValue_(ename);
}

AL_API void AL_APIENTRY alListenerf(
	::ALenum param,
	::ALfloat value)
{
	eaxefx::alListenerf_(param, value);
}

AL_API void AL_APIENTRY alListener3f(
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3)
{
	eaxefx::alListener3f_(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alListenerfv(
	::ALenum param,
	const ::ALfloat* values)
{
	eaxefx::alListenerfv_(param, values);
}

AL_API void AL_APIENTRY alListeneri(
	::ALenum param,
	::ALint value)
{
	eaxefx::alListeneri_(param, value);
}

AL_API void AL_APIENTRY alListener3i(
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3)
{
	eaxefx::alListener3i_(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alListeneriv(
	::ALenum param,
	const ::ALint* values)
{
	eaxefx::alListeneriv_(param, values);
}

AL_API void AL_APIENTRY alGetListenerf(
	::ALenum param,
	::ALfloat* value)
{
	eaxefx::alGetListenerf_(param, value);
}

AL_API void AL_APIENTRY alGetListener3f(
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3)
{
	eaxefx::alGetListener3f_(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetListenerfv(
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::alGetListenerfv_(param, values);
}

AL_API void AL_APIENTRY alGetListeneri(
	::ALenum param,
	::ALint* value)
{
	eaxefx::alGetListeneri_(param, value);
}

AL_API void AL_APIENTRY alGetListener3i(
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3)
{
	eaxefx::alGetListener3i_(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetListeneriv(
	::ALenum param,
	::ALint* values)
{
	eaxefx::alGetListeneriv_(param, values);
}

AL_API void AL_APIENTRY alGenSources(
	::ALsizei n,
	::ALuint* sources)
{
	const auto& al_api = eaxefx::get_al_api();

	if (n > 0 && sources)
	{
		std::uninitialized_fill_n(sources, n, 0);
	}

	if (al_api.is_initialized())
	{
		al_api.get_eaxx()->alGenSources(n, sources);
	}
}

AL_API void AL_APIENTRY alDeleteSources(
	::ALsizei n,
	const ::ALuint* sources)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		al_api.get_eaxx()->alDeleteSources(n, sources);
	}
}

AL_API ::ALboolean AL_APIENTRY alIsSource(
	::ALuint source)
{
	return eaxefx::alIsSource_(source);
}

AL_API void AL_APIENTRY alSourcef(
	::ALuint source,
	::ALenum param,
	::ALfloat value)
{
	eaxefx::alSourcef_(source, param, value);
}

AL_API void AL_APIENTRY alSource3f(
	::ALuint source,
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3)
{
	eaxefx::alSource3f_(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourcefv(
	::ALuint source,
	::ALenum param,
	const ::ALfloat* values)
{
	eaxefx::alSourcefv_(source, param, values);
}

AL_API void AL_APIENTRY alSourcei(
	::ALuint source,
	::ALenum param,
	::ALint value)
{
	eaxefx::alSourcei_(source, param, value);
}

AL_API void AL_APIENTRY alSource3i(
	::ALuint source,
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3)
{
	eaxefx::alSource3i_(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourceiv(
	::ALuint source,
	::ALenum param,
	const ::ALint* values)
{
	eaxefx::alSourceiv_(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcef(
	::ALuint source,
	::ALenum param,
	::ALfloat* value)
{
	eaxefx::alGetSourcef_(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3f(
	::ALuint source,
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3)
{
	eaxefx::alGetSource3f_(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourcefv(
	::ALuint source,
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::alGetSourcefv_(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcei(
	::ALuint source,
	::ALenum param,
	::ALint* value)
{
	eaxefx::alGetSourcei_(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3i(
	::ALuint source,
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3)
{
	eaxefx::alGetSource3i_(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourceiv(
	::ALuint source,
	::ALenum param,
	::ALint* values)
{
	eaxefx::alGetSourceiv_(source, param, values);
}

AL_API void AL_APIENTRY alSourcePlayv(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::alSourcePlayv_(n, sources);
}

AL_API void AL_APIENTRY alSourceStopv(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::alSourceStopv_(n, sources);
}

AL_API void AL_APIENTRY alSourceRewindv(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::alSourceRewindv_(n, sources);
}

AL_API void AL_APIENTRY alSourcePausev(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::alSourcePausev_(n, sources);
}

AL_API void AL_APIENTRY alSourcePlay(
	::ALuint source)
{
	eaxefx::alSourcePlay_(source);
}

AL_API void AL_APIENTRY alSourceStop(
	::ALuint source)
{
	eaxefx::alSourceStop_(source);
}

AL_API void AL_APIENTRY alSourceRewind(
	::ALuint source)
{
	eaxefx::alSourceRewind_(source);
}

AL_API void AL_APIENTRY alSourcePause(
	::ALuint source)
{
	eaxefx::alSourcePause_(source);
}

AL_API void AL_APIENTRY alSourceQueueBuffers(
	::ALuint source,
	::ALsizei nb,
	const ::ALuint* buffers)
{
	eaxefx::alSourceQueueBuffers_(source, nb, buffers);
}

AL_API void AL_APIENTRY alSourceUnqueueBuffers(
	::ALuint source,
	::ALsizei nb,
	::ALuint* buffers)
{
	eaxefx::alSourceUnqueueBuffers_(source, nb, buffers);
}

AL_API void AL_APIENTRY alGenBuffers(
	::ALsizei n,
	::ALuint* buffers)
{
	eaxefx::alGenBuffers_(n, buffers);
}

AL_API void AL_APIENTRY alDeleteBuffers(
	::ALsizei n,
	const ::ALuint* buffers)
{
	eaxefx::alDeleteBuffers_(n, buffers);
}

AL_API ::ALboolean AL_APIENTRY alIsBuffer(
	::ALuint buffer)
{
	return eaxefx::alIsBuffer_(buffer);
}

AL_API void AL_APIENTRY alBufferData(
	::ALuint buffer,
	::ALenum format,
	const ::ALvoid* data,
	::ALsizei size,
	::ALsizei freq)
{
	eaxefx::alBufferData_(buffer, format, data, size, freq);
}

AL_API void AL_APIENTRY alBufferf(
	::ALuint buffer,
	::ALenum param,
	::ALfloat value)
{
	eaxefx::alBufferf_(buffer, param, value);
}

AL_API void AL_APIENTRY alBuffer3f(
	::ALuint buffer,
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3)
{
	eaxefx::alBuffer3f_(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alBufferfv(
	::ALuint buffer,
	::ALenum param,
	const ::ALfloat* values)
{
	eaxefx::alBufferfv_(buffer, param, values);
}

AL_API void AL_APIENTRY alBufferi(
	::ALuint buffer,
	::ALenum param,
	::ALint value)
{
	eaxefx::alBufferi_(buffer, param, value);
}

AL_API void AL_APIENTRY alBuffer3i(
	::ALuint buffer,
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3)
{
	eaxefx::alBuffer3i_(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alBufferiv(
	::ALuint buffer,
	::ALenum param,
	const ::ALint* values)
{
	eaxefx::alBufferiv_(buffer, param, values);
}

AL_API void AL_APIENTRY alGetBufferf(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* value)
{
	eaxefx::alGetBufferf_(buffer, param, value);
}

AL_API void AL_APIENTRY alGetBuffer3f(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3)
{
	eaxefx::alGetBuffer3f_(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetBufferfv(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::alGetBufferfv_(buffer, param, values);
}

AL_API void AL_APIENTRY alGetBufferi(
	::ALuint buffer,
	::ALenum param,
	::ALint* value)
{
	eaxefx::alGetBufferi_(buffer, param, value);
}

AL_API void AL_APIENTRY alGetBuffer3i(
	::ALuint buffer,
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3)
{
	eaxefx::alGetBuffer3i_(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetBufferiv(
	::ALuint buffer,
	::ALenum param,
	::ALint* values)
{
	eaxefx::alGetBufferiv_(buffer, param, values);
}


//
// ALC 1.1
//

ALC_API ::ALCcontext* ALC_APIENTRY alcCreateContext(
	::ALCdevice* device,
	const ::ALCint* attrlist)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->alcCreateContext(device, attrlist);
	}
	else
	{
		return nullptr;
	}
}

ALC_API ::ALCboolean ALC_APIENTRY alcMakeContextCurrent(
	::ALCcontext* context)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->alcMakeContextCurrent(context);
	}
	else
	{
		return ALC_FALSE;
	}
}

ALC_API void ALC_APIENTRY alcProcessContext(
	::ALCcontext* context)
{
	eaxefx::alcProcessContext_(context);
}

ALC_API void ALC_APIENTRY alcSuspendContext(
	::ALCcontext* context)
{
	eaxefx::alcSuspendContext_(context);
}

ALC_API void ALC_APIENTRY alcDestroyContext(
	::ALCcontext* context)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		al_api.get_eaxx()->alcDestroyContext(context);
	}
}

ALC_API ::ALCcontext* ALC_APIENTRY alcGetCurrentContext()
{
	return eaxefx::alcGetCurrentContext_();
}

ALC_API ::ALCdevice* ALC_APIENTRY alcGetContextsDevice(
	::ALCcontext* context)
{
	return eaxefx::alcGetContextsDevice_(context);
}

ALC_API ::ALCdevice* ALC_APIENTRY alcOpenDevice(
	const ::ALCchar* devicename)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->alcOpenDevice(devicename);
	}
	else
	{
		return nullptr;
	}
}

ALC_API ::ALCboolean ALC_APIENTRY alcCloseDevice(
	::ALCdevice* device)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->alcCloseDevice(device);
	}
	else
	{
		return ALC_FALSE;
	}
}

ALC_API ::ALCenum ALC_APIENTRY alcGetError(
	::ALCdevice* device)
{
	return eaxefx::alcGetError_(device);
}

ALC_API ::ALCboolean ALC_APIENTRY alcIsExtensionPresent(
	::ALCdevice* device,
	const ::ALCchar* extname)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return eaxefx::alcIsExtensionPresent_(device, extname);
	}
	else
	{
		return AL_FALSE;
	}
}

ALC_API void* ALC_APIENTRY alcGetProcAddress(
	::ALCdevice* device,
	const ::ALCchar* funcname)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		if (
			const auto address = al_api.find_function(funcname);
			address)
		{
			return address;
		}
		else
		{
			return eaxefx::alcGetProcAddress_(device, funcname);
		}
	}

	return nullptr;
}

ALC_API ::ALCenum ALC_APIENTRY alcGetEnumValue(
	::ALCdevice* device,
	const ::ALCchar* enumname)
{
	return eaxefx::alcGetEnumValue_(device, enumname);
}

ALC_API const ::ALCchar* ALC_APIENTRY alcGetString(
	::ALCdevice* device,
	::ALCenum param)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return eaxefx::alcGetString_(device, param);
	}
	else
	{
		return nullptr;
	}
}

ALC_API void ALC_APIENTRY alcGetIntegerv(
	::ALCdevice* device,
	::ALCenum param,
	::ALCsizei size,
	::ALCint* values)
{
	eaxefx::alcGetIntegerv_(device, param, size, values);
}

ALC_API ::ALCdevice* ALC_APIENTRY alcCaptureOpenDevice(
	const ::ALCchar* devicename,
	::ALCuint frequency,
	::ALCenum format,
	::ALCsizei buffersize)
{
	return eaxefx::alcCaptureOpenDevice_(devicename, frequency, format, buffersize);
}

ALC_API ::ALCboolean ALC_APIENTRY alcCaptureCloseDevice(
	::ALCdevice* device)
{
	return eaxefx::alcCaptureCloseDevice_(device);
}

ALC_API void ALC_APIENTRY alcCaptureStart(
	::ALCdevice* device)
{
	eaxefx::alcCaptureStart_(device);
}

ALC_API void ALC_APIENTRY alcCaptureStop(::ALCdevice* device)
{
	eaxefx::alcCaptureStop_(device);
}

ALC_API void ALC_APIENTRY alcCaptureSamples(
	::ALCdevice* device,
	::ALCvoid* buffer,
	::ALCsizei samples)
{
	eaxefx::alcCaptureSamples_(device, buffer, samples);
}


//
// EAX
//

AL_API ::ALenum AL_APIENTRY EAXSet(
	const ::GUID* property_set_guid,
	::ALuint property_id,
	::ALuint property_al_name,
	::ALvoid* property_buffer,
	::ALuint property_size)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->EAXSet(
			property_set_guid,
			property_id,
			property_al_name,
			property_buffer,
			property_size
		);
	}
	else
	{
		return AL_INVALID_OPERATION;
	}
}

AL_API ::ALenum AL_APIENTRY EAXGet(
	const ::GUID* property_set_guid,
	::ALuint property_id,
	::ALuint property_al_name,
	::ALvoid* property_buffer,
	::ALuint property_size)
{
	const auto& al_api = eaxefx::get_al_api();

	if (al_api.is_initialized())
	{
		return al_api.get_eaxx()->EAXGet(
			property_set_guid,
			property_id,
			property_al_name,
			property_buffer,
			property_size
		);
	}
	else
	{
		return AL_INVALID_OPERATION;
	}
}


} // extern "C"

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
