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


#include "eaxefx_al_api.h"

#include <algorithm>
#include <exception>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string_view>
#include <unordered_map>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"

#include "eaxefx_al_api_context.h"
#include "eaxefx_al_api_utils.h"
#include "eaxefx_al_loader.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eax_api.h"
#include "eaxefx_eaxx.h"
#include "eaxefx_exception.h"
#include "eaxefx_logger.h"
#include "eaxefx_moveable_mutex_lock.h"
#include "eaxefx_mutex.h"
#include "eaxefx_shared_library.h"
#include "eaxefx_string.h"

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

class AlApiImpl :
	public AlApi
{
public:
	AlApiImpl() noexcept;

	~AlApiImpl();


	Logger* get_logger() noexcept override;

	AlLoader* get_al_loader() const noexcept override;

	AlAlcSymbols* get_al_alc_symbols() const noexcept override;

	AlAlSymbols* get_al_al_symbols() const noexcept override;

	void on_thread_detach() noexcept override;

	void on_process_detach() noexcept override;

	MoveableMutexLock get_lock() override;

	AlApiContext& get_current_context() override;

	Eaxx* get_eaxx() const noexcept;


	// =========================================================================
	// ALC v1.1

	::ALCcontext* ALC_APIENTRY alcCreateContext(
		::ALCdevice* device,
		const ::ALCint* attrlist) noexcept;

	::ALCboolean ALC_APIENTRY alcMakeContextCurrent(
		::ALCcontext* context) noexcept;

	void ALC_APIENTRY alcProcessContext(
		::ALCcontext* context) noexcept;

	void ALC_APIENTRY alcSuspendContext(
		::ALCcontext* context) noexcept;

	void ALC_APIENTRY alcDestroyContext(
		::ALCcontext* context) noexcept;

	::ALCcontext* ALC_APIENTRY alcGetCurrentContext() noexcept;

	::ALCdevice* ALC_APIENTRY alcGetContextsDevice(
		::ALCcontext* context) noexcept;

	::ALCdevice* ALC_APIENTRY alcOpenDevice(
		const ::ALCchar* devicename) noexcept;

	::ALCboolean ALC_APIENTRY alcCloseDevice(
		::ALCdevice* device) noexcept;

	::ALCenum ALC_APIENTRY alcGetError(
		::ALCdevice* device) noexcept;

	::ALCboolean ALC_APIENTRY alcIsExtensionPresent(
		::ALCdevice* device,
		const ::ALCchar* extname) noexcept;

	void* ALC_APIENTRY alcGetProcAddress(
		::ALCdevice* device,
		const ::ALCchar* funcname) noexcept;

	::ALCenum ALC_APIENTRY alcGetEnumValue(
		::ALCdevice* device,
		const ::ALCchar* enumname) noexcept;

	const ::ALCchar* ALC_APIENTRY alcGetString(
		::ALCdevice* device,
		::ALCenum param) noexcept;

	void ALC_APIENTRY alcGetIntegerv(
		::ALCdevice* device,
		::ALCenum param,
		::ALCsizei size,
		::ALCint* values) noexcept;

	::ALCdevice* ALC_APIENTRY alcCaptureOpenDevice(
		const ::ALCchar* devicename,
		::ALCuint frequency,
		::ALCenum format,
		::ALCsizei buffersize) noexcept;

	::ALCboolean ALC_APIENTRY alcCaptureCloseDevice(
		::ALCdevice* device) noexcept;

	void ALC_APIENTRY alcCaptureStart(
		::ALCdevice* device) noexcept;

	void ALC_APIENTRY alcCaptureStop(
		::ALCdevice* device) noexcept;

	void ALC_APIENTRY alcCaptureSamples(
		::ALCdevice* device,
		::ALCvoid* buffer,
		::ALCsizei samples) noexcept;

	// ALC v1.1
	// =========================================================================


	// =========================================================================
	// AL v1.1

	void AL_APIENTRY alDopplerFactor(
		::ALfloat value) noexcept;

	void AL_APIENTRY alDopplerVelocity(
		::ALfloat value) noexcept;

	void AL_APIENTRY alSpeedOfSound(
		::ALfloat value) noexcept;

	void AL_APIENTRY alDistanceModel(
		::ALenum distanceModel) noexcept;

	void AL_APIENTRY alEnable(
		::ALenum capability) noexcept;

	void AL_APIENTRY alDisable(
		::ALenum capability) noexcept;

	::ALboolean AL_APIENTRY alIsEnabled(
		::ALenum capability) noexcept;

	const ::ALchar* AL_APIENTRY alGetString(
		::ALenum param) noexcept;

	void AL_APIENTRY alGetBooleanv(
		::ALenum param,
		::ALboolean* values) noexcept;

	void AL_APIENTRY alGetIntegerv(
		::ALenum param,
		::ALint* values) noexcept;

	void AL_APIENTRY alGetFloatv(
		::ALenum param,
		::ALfloat* values) noexcept;

	void AL_APIENTRY alGetDoublev(
		::ALenum param,
		::ALdouble* values) noexcept;

	::ALboolean AL_APIENTRY alGetBoolean(
		::ALenum param) noexcept;

	::ALint AL_APIENTRY alGetInteger(
		::ALenum param) noexcept;

	::ALfloat AL_APIENTRY alGetFloat(
		::ALenum param) noexcept;

	::ALdouble AL_APIENTRY alGetDouble(
		::ALenum param) noexcept;

	::ALenum AL_APIENTRY alGetError() noexcept;

	::ALboolean AL_APIENTRY alIsExtensionPresent(
		const ::ALchar* extname) noexcept;

	void* AL_APIENTRY alGetProcAddress(
		const ::ALchar* fname) noexcept;

	::ALenum AL_APIENTRY alGetEnumValue(
		const ::ALchar* ename) noexcept;

	void AL_APIENTRY alListenerf(
		::ALenum param,
		::ALfloat value) noexcept;

	void AL_APIENTRY alListener3f(
		::ALenum param,
		::ALfloat value1,
		::ALfloat value2,
		::ALfloat value3) noexcept;

	void AL_APIENTRY alListenerfv(
		::ALenum param,
		const ::ALfloat* values) noexcept;

	void AL_APIENTRY alListeneri(
		::ALenum param,
		::ALint value) noexcept;

	void AL_APIENTRY alListener3i(
		::ALenum param,
		::ALint value1,
		::ALint value2,
		::ALint value3) noexcept;

	void AL_APIENTRY alListeneriv(
		::ALenum param,
		const ::ALint* values) noexcept;

	void AL_APIENTRY alGetListenerf(
		::ALenum param,
		::ALfloat* value) noexcept;

	void AL_APIENTRY alGetListener3f(
		::ALenum param,
		::ALfloat* value1,
		::ALfloat* value2,
		::ALfloat* value3) noexcept;

	void AL_APIENTRY alGetListenerfv(
		::ALenum param,
		::ALfloat* values) noexcept;

	void AL_APIENTRY alGetListeneri(
		::ALenum param,
		::ALint* value) noexcept;

	void AL_APIENTRY alGetListener3i(
		::ALenum param,
		::ALint* value1,
		::ALint* value2,
		::ALint* value3) noexcept;

	void AL_APIENTRY alGetListeneriv(
		::ALenum param,
		::ALint* values) noexcept;

	void AL_APIENTRY alGenSources(
		::ALsizei n,
		::ALuint* sources) noexcept;

	void AL_APIENTRY alDeleteSources(
		::ALsizei n,
		const ::ALuint* sources) noexcept;

	::ALboolean AL_APIENTRY alIsSource(
		::ALuint source) noexcept;

	void AL_APIENTRY alSourcef(
		::ALuint source,
		::ALenum param,
		::ALfloat value) noexcept;

	void AL_APIENTRY alSource3f(
		::ALuint source,
		::ALenum param,
		::ALfloat value1,
		::ALfloat value2,
		::ALfloat value3) noexcept;

	void AL_APIENTRY alSourcefv(
		::ALuint source,
		::ALenum param,
		const ::ALfloat* values) noexcept;

	void AL_APIENTRY alSourcei(
		::ALuint source,
		::ALenum param,
		::ALint value) noexcept;

	void AL_APIENTRY alSource3i(
		::ALuint source,
		::ALenum param,
		::ALint value1,
		::ALint value2,
		::ALint value3) noexcept;

	void AL_APIENTRY alSourceiv(
		::ALuint source,
		::ALenum param,
		const ::ALint* values) noexcept;

	void AL_APIENTRY alGetSourcef(
		::ALuint source,
		::ALenum param,
		::ALfloat* value) noexcept;

	void AL_APIENTRY alGetSource3f(
		::ALuint source,
		::ALenum param,
		::ALfloat* value1,
		::ALfloat* value2,
		::ALfloat* value3) noexcept;

	void AL_APIENTRY alGetSourcefv(
		::ALuint source,
		::ALenum param,
		::ALfloat* values) noexcept;

	void AL_APIENTRY alGetSourcei(
		::ALuint source,
		::ALenum param,
		::ALint* value) noexcept;

	void AL_APIENTRY alGetSource3i(
		::ALuint source,
		::ALenum param,
		::ALint* value1,
		::ALint* value2,
		::ALint* value3) noexcept;

	void AL_APIENTRY alGetSourceiv(
		::ALuint source,
		::ALenum param,
		::ALint* values) noexcept;

	void AL_APIENTRY alSourcePlayv(
		::ALsizei n,
		const ::ALuint* sources) noexcept;

	void AL_APIENTRY alSourceStopv(
		::ALsizei n,
		const ::ALuint* sources) noexcept;

	void AL_APIENTRY alSourceRewindv(
		::ALsizei n,
		const ::ALuint* sources) noexcept;

	void AL_APIENTRY alSourcePausev(
		::ALsizei n,
		const ::ALuint* sources) noexcept;

	void AL_APIENTRY alSourcePlay(
		::ALuint source) noexcept;

	void AL_APIENTRY alSourceStop(
		::ALuint source) noexcept;

	void AL_APIENTRY alSourceRewind(
		::ALuint source) noexcept;

	void AL_APIENTRY alSourcePause(
		::ALuint source) noexcept;

	void AL_APIENTRY alSourceQueueBuffers(
		::ALuint source,
		::ALsizei nb,
		const ::ALuint* buffers) noexcept;

	void AL_APIENTRY alSourceUnqueueBuffers(
		::ALuint source,
		::ALsizei nb,
		::ALuint* buffers) noexcept;

	void AL_APIENTRY alGenBuffers(
		::ALsizei n,
		::ALuint* buffers) noexcept;

	void AL_APIENTRY alDeleteBuffers(
		::ALsizei n,
		const ::ALuint* buffers) noexcept;

	::ALboolean AL_APIENTRY alIsBuffer(
		::ALuint buffer) noexcept;

	void AL_APIENTRY alBufferData(
		::ALuint buffer,
		::ALenum format,
		const ::ALvoid* data,
		::ALsizei size,
		::ALsizei freq) noexcept;

	void AL_APIENTRY alBufferf(
		::ALuint buffer,
		::ALenum param,
		::ALfloat value) noexcept;

	void AL_APIENTRY alBuffer3f(
		::ALuint buffer,
		::ALenum param,
		::ALfloat value1,
		::ALfloat value2,
		::ALfloat value3) noexcept;

	void AL_APIENTRY alBufferfv(
		::ALuint buffer,
		::ALenum param,
		const ::ALfloat* values) noexcept;

	void AL_APIENTRY alBufferi(
		::ALuint buffer,
		::ALenum param,
		::ALint value) noexcept;

	void AL_APIENTRY alBuffer3i(
		::ALuint buffer,
		::ALenum param,
		::ALint value1,
		::ALint value2,
		::ALint value3) noexcept;

	void AL_APIENTRY alBufferiv(
		::ALuint buffer,
		::ALenum param,
		const ::ALint* values) noexcept;

	void AL_APIENTRY alGetBufferf(
		::ALuint buffer,
		::ALenum param,
		::ALfloat* value) noexcept;

	void AL_APIENTRY alGetBuffer3f(
		::ALuint buffer,
		::ALenum param,
		::ALfloat* value1,
		::ALfloat* value2,
		::ALfloat* value3) noexcept;

	void AL_APIENTRY alGetBufferfv(
		::ALuint buffer,
		::ALenum param,
		::ALfloat* values) noexcept;

	void AL_APIENTRY alGetBufferi(
		::ALuint buffer,
		::ALenum param,
		::ALint* value) noexcept;

	void AL_APIENTRY alGetBuffer3i(
		::ALuint buffer,
		::ALenum param,
		::ALint* value1,
		::ALint* value2,
		::ALint* value3) noexcept;

	void AL_APIENTRY alGetBufferiv(
		::ALuint buffer,
		::ALenum param,
		::ALint* values) noexcept;

	// AL v1.1
	// =========================================================================


private:
	struct Strings;
	struct ErrorMessages;

	static constexpr auto min_string_buffer_capacity = 4 * 1'024;


	using AlSymbolMap = std::unordered_map<std::string_view, void*>;

	using Contexts = std::list<AlApiContextUPtr>;


	struct Device
	{
		::ALCdevice* al_device{};

		Contexts contexts{};
	}; // Device

	using Devices = std::list<Device>;


	using InitializeFunc = MoveableMutexLock (AlApiImpl::*)();


	using MutexLock = std::scoped_lock<Mutex>;


	InitializeFunc initialize_func_{};
	MutexUPtr mutex_{};
	eaxefx::NullableLogger logger_{};
	eaxefx::SharedLibraryUPtr al_library_{};
	eaxefx::AlLoaderUPtr al_loader_{};
	eaxefx::AlAlcSymbolsUPtr al_alc_symbols_{};
	AlSymbolMap al_alc_symbol_map_{};
	eaxefx::AlAlSymbolsUPtr al_al_symbols_{};
	AlSymbolMap al_al_symbol_map_{};
	Devices devices_{};
	AlApiContext* current_context_{};
	eaxefx::EaxxUPtr eaxx_{};

	String string_buffer_{};



	void apply_patch_collection() noexcept;


	void initialize_al_driver();

	void initialize_al_symbols();


	void initialize_al_alc_symbol_map() noexcept;

	void initialize_al_al_symbol_map() noexcept;

	void initialize_al_wrapper_entries() noexcept;


	MoveableMutexLock initialize_invalid_state();

	MoveableMutexLock initialize_not_initialized();

	MoveableMutexLock initialize_lock_mutex();

	MoveableMutexLock initialize_try_to_initialize();

	MoveableMutexLock initialize();


	Device* find_device(
		::ALCdevice* al_device) noexcept;

	Device& get_device(
		::ALCdevice* al_device);


	AlApiContext& get_context();

	AlApiContext& get_context(
		::ALCcontext* al_context);

	void remove_context(
		const AlApiContext& context);


	void log_devices_info(
		::ALCenum alc_token);

	void log_playback_devices();

	void log_capture_devices();

	void log_devices_info();


	void log_device_extensions(
		::ALCdevice* al_device);

	void log_device_info(
		::ALCdevice* al_device);


	void log_desired_context_attribute_list(
		const ::ALCint* al_attributes);
}; // AlApiImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct AlApiImpl::Strings
{
};

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct AlApiImpl::ErrorMessages
{
	static constexpr auto invalid_state = "Invalid state.";
	static constexpr auto not_initialized = "Not initialized.";
};

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlApiImpl::AlApiImpl() noexcept
{
	try
	{
		mutex_ = make_mutex();

		string_buffer_.reserve(min_string_buffer_capacity);

		initialize_func_ = &AlApiImpl::initialize_try_to_initialize;
	}
	catch (...)
	{
		initialize_func_ = &AlApiImpl::initialize_invalid_state;
	}
}

AlApiImpl::~AlApiImpl()
{
	logger_.info("");
	logger_.info(">>>>>>>>>>>>>>>>>>>>>>>>");
	logger_.info("");
	logger_.flush();
}

Logger* AlApiImpl::get_logger() noexcept
{
	return &logger_;
}

AlLoader* AlApiImpl::get_al_loader() const noexcept
{
	return al_loader_.get();
}

AlAlcSymbols* AlApiImpl::get_al_alc_symbols() const noexcept
{
	return al_alc_symbols_.get();
}

AlAlSymbols* AlApiImpl::get_al_al_symbols() const noexcept
{
	return al_al_symbols_.get();
}

void AlApiImpl::on_thread_detach() noexcept
{
	logger_.set_immediate_mode();
}

void AlApiImpl::on_process_detach() noexcept
{
	logger_.set_immediate_mode();
}

MoveableMutexLock AlApiImpl::get_lock()
{
	return initialize();
}

AlApiContext& AlApiImpl::get_current_context()
{
	return get_context();
}

Eaxx* AlApiImpl::get_eaxx() const noexcept
{
	return eaxx_.get();
}

// ==========================================================================
// ALC v1.1

::ALCcontext* ALC_APIENTRY AlApiImpl::alcCreateContext(
	::ALCdevice* device,
	const ::ALCint* attrlist) noexcept
try
{
	logger_.info("");
	logger_.info(al_api::Strings::equals_line_16);

	string_buffer_.clear();
	string_buffer_ += "Create context on device ";
	string_buffer_ += to_string_hex(device);
	string_buffer_ += '.';
	logger_.info(string_buffer_.c_str());

	log_desired_context_attribute_list(attrlist);

	const auto mt_lock = initialize();

	auto& our_device = get_device(device);

	auto al_api_context = make_al_api_context();

	auto context_init_param = AlApiContextInitParam{};
	context_init_param.al_device = device;
	context_init_param.al_context_attributes = attrlist;

	al_api_context->alc_create_context(context_init_param);

	our_device.contexts.emplace_back(std::move(al_api_context));

	const auto al_context = our_device.contexts.back()->get_al_context();

	return al_context;
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

::ALCboolean ALC_APIENTRY AlApiImpl::alcMakeContextCurrent(
	::ALCcontext* context) noexcept
try
{
	logger_.info("");
	logger_.info(al_api::Strings::equals_line_16);

	string_buffer_.clear();
	string_buffer_ += "Make context ";
	string_buffer_ += to_string_hex(context);
	string_buffer_ += " current.";
	logger_.info(string_buffer_.c_str());

	const auto mt_lock = initialize();

	if (context)
	{
		auto& our_context = get_context(context);
		our_context.alc_make_current();
		current_context_ = &our_context;
	}
	else
	{
		const auto alc_result = al_alc_symbols_->alcMakeContextCurrent(nullptr);
		current_context_ = nullptr;

		if (alc_result == ALC_FALSE)
		{
			throw AlApiException{"AL failed to unset current context."};
		}
	}

	return ALC_TRUE;
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return ALC_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return ALC_FALSE;
}

void ALC_APIENTRY AlApiImpl::alcProcessContext(
	::ALCcontext* context) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcProcessContext(context);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void ALC_APIENTRY AlApiImpl::alcSuspendContext(
	::ALCcontext* context) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcSuspendContext(context);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void ALC_APIENTRY AlApiImpl::alcDestroyContext(
	::ALCcontext* context) noexcept
try
{
	logger_.info("");
	logger_.info(al_api::Strings::equals_line_16);

	string_buffer_.clear();
	string_buffer_ += "Destroy context ";
	string_buffer_ += to_string_hex(context);
	string_buffer_ += '.';
	logger_.info(string_buffer_.c_str());

	const auto mt_lock = initialize();

	auto& our_context = get_context(context);

	if (&our_context == current_context_)
	{
		logger_.warning("Destroying current context!");
		logger_.flush();
	}

	current_context_ = nullptr;

	our_context.alc_destroy();
	remove_context(our_context);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

::ALCcontext* ALC_APIENTRY AlApiImpl::alcGetCurrentContext() noexcept
try
{
	const auto mt_lock = initialize();

	if (!current_context_)
	{
		return nullptr;
	}

	return current_context_->get_al_context();
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

::ALCdevice* ALC_APIENTRY AlApiImpl::alcGetContextsDevice(
	::ALCcontext* context) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcGetContextsDevice(context);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

::ALCdevice* ALC_APIENTRY AlApiImpl::alcOpenDevice(
	const ::ALCchar* devicename) noexcept
try
{
	const auto mt_lock = initialize();

	logger_.info("");
	logger_.info(al_api::Strings::equals_line_16);

	if (!devicename)
	{
		logger_.info("Open default device.");
	}
	else
	{
		string_buffer_.clear();
		string_buffer_ += "Open device \"";
		string_buffer_ += devicename;
		string_buffer_ += "\".";
		logger_.info(string_buffer_.c_str());
	}

	const auto al_device = al_alc_symbols_->alcOpenDevice(devicename);

	if (!al_device)
	{
		logger_.error("AL failed to open the device.");
		return nullptr;
	}

	string_buffer_.clear();
	string_buffer_ += "Instance: ";
	string_buffer_ += to_string_hex(al_device);
	logger_.info(string_buffer_.c_str());

	log_device_info(al_device);

	devices_.emplace_back(Device{});
	auto& device = devices_.back();

	device.al_device = al_device;

	return al_device;
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

::ALCboolean ALC_APIENTRY AlApiImpl::alcCloseDevice(
	::ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	logger_.info("");
	logger_.info(al_api::Strings::equals_line_16);

	string_buffer_.clear();
	string_buffer_ += "Close device ";
	string_buffer_ += to_string_hex(device);
	string_buffer_ += '.';
	logger_.info(string_buffer_.c_str());

	devices_.remove_if(
		[al_device = device](
			const Device& device)
		{
			return device.al_device == al_device;
		}
	);

	const auto al_result = al_alc_symbols_->alcCloseDevice(device);

	if (al_result == ALC_FALSE)
	{
		throw AlApiException{"AL failed to close the device."};
	}

	return al_result;
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return ALC_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return ALC_FALSE;
}

::ALCenum ALC_APIENTRY AlApiImpl::alcGetError(
	::ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	const auto alc_result = al_alc_symbols_->alcGetError(device);

	return alc_result;
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return ALC_INVALID_DEVICE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return ALC_INVALID_DEVICE;
}

::ALCboolean ALC_APIENTRY AlApiImpl::alcIsExtensionPresent(
	::ALCdevice* device,
	const ::ALCchar* extname) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcIsExtensionPresent(device, extname);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_FALSE;
}

void* ALC_APIENTRY AlApiImpl::alcGetProcAddress(
	::ALCdevice* device,
	const ::ALCchar* funcname) noexcept
try
{
	const auto mt_lock = initialize();

	if (funcname && (*funcname) != '\0')
	{
		const auto symbol_name = std::string_view{funcname};
		const auto alc_symbol_it = al_alc_symbol_map_.find(symbol_name);

		if (alc_symbol_it != al_alc_symbol_map_.cend())
		{
			return alc_symbol_it->second;
		}
	}

	return al_alc_symbols_->alcGetProcAddress(device, funcname);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

::ALCenum ALC_APIENTRY AlApiImpl::alcGetEnumValue(
	::ALCdevice* device,
	const ::ALCchar* enumname) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcGetEnumValue(device, enumname);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_NONE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_NONE;
}

const ::ALCchar* ALC_APIENTRY AlApiImpl::alcGetString(
	::ALCdevice* device,
	::ALCenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcGetString(device, param);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

void ALC_APIENTRY AlApiImpl::alcGetIntegerv(
	::ALCdevice* device,
	::ALCenum param,
	::ALCsizei size,
	::ALCint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcGetIntegerv(device, param, size, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

::ALCdevice* ALC_APIENTRY AlApiImpl::alcCaptureOpenDevice(
	const ::ALCchar* devicename,
	::ALCuint frequency,
	::ALCenum format,
	::ALCsizei buffersize) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcCaptureOpenDevice(devicename, frequency, format, buffersize);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

::ALCboolean ALC_APIENTRY AlApiImpl::alcCaptureCloseDevice(
	::ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcCaptureCloseDevice(device);;
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return ALC_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return ALC_FALSE;
}

void ALC_APIENTRY AlApiImpl::alcCaptureStart(
	::ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcCaptureStart(device);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void ALC_APIENTRY AlApiImpl::alcCaptureStop(
	::ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcCaptureStop(device);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void ALC_APIENTRY AlApiImpl::alcCaptureSamples(
	::ALCdevice* device,
	::ALCvoid* buffer,
	::ALCsizei samples) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcCaptureSamples(device, buffer, samples);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

// ALC v1.1
// ==========================================================================

// =========================================================================
// AL v1.1

void AL_APIENTRY AlApiImpl::alDopplerFactor(
	::ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDopplerFactor(value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alDopplerVelocity(
	::ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDopplerVelocity(value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSpeedOfSound(
	::ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSpeedOfSound(value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alDistanceModel(
	::ALenum distanceModel) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDistanceModel(distanceModel);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alEnable(
	::ALenum capability) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alEnable(capability);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alDisable(
	::ALenum capability) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDisable(capability);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

::ALboolean AL_APIENTRY AlApiImpl::alIsEnabled(
	::ALenum capability) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alIsEnabled(capability);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_FALSE;
}

const ::ALchar* AL_APIENTRY AlApiImpl::alGetString(
	::ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	if (current_context_)
	{
		const auto al_string = current_context_->al_get_string(param);

		if (al_string)
		{
			return al_string;
		}
	}

	return al_al_symbols_->alGetString(param);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

void AL_APIENTRY AlApiImpl::alGetBooleanv(
	::ALenum param,
	::ALboolean* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBooleanv(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetIntegerv(
	::ALenum param,
	::ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetIntegerv(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetFloatv(
	::ALenum param,
	::ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetFloatv(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetDoublev(
	::ALenum param,
	::ALdouble* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetDoublev(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

::ALboolean AL_APIENTRY AlApiImpl::alGetBoolean(
	::ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetBoolean(param);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_FALSE;
}

::ALint AL_APIENTRY AlApiImpl::alGetInteger(
	::ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetInteger(param);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return 0;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return 0;
}

::ALfloat AL_APIENTRY AlApiImpl::alGetFloat(
	::ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetFloat(param);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return 0.0F;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return 0.0F;
}

::ALdouble AL_APIENTRY AlApiImpl::alGetDouble(
	::ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetDouble(param);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return 0.0;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return 0.0;
}

::ALenum AL_APIENTRY AlApiImpl::alGetError() noexcept
try
{
	const auto mt_lock = initialize();

	const auto al_result = al_al_symbols_->alGetError();
	return al_result;
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_INVALID_OPERATION;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_INVALID_OPERATION;
}

::ALboolean AL_APIENTRY AlApiImpl::alIsExtensionPresent(
	const ::ALchar* extname) noexcept
try
{
	const auto mt_lock = initialize();

	if (current_context_)
	{
		const auto is_present = current_context_->al_is_extension_present(extname);

		if (is_present)
		{
			return true;
		}
	}

	return al_al_symbols_->alIsExtensionPresent(extname);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_FALSE;
}

void* AL_APIENTRY AlApiImpl::alGetProcAddress(
	const ::ALchar* fname) noexcept
try
{
	if (!fname || (*fname) == '\0')
	{
		return nullptr;
	}

	const auto mt_lock = initialize();

	const auto symbol_name = std::string_view{fname};
	const auto al_symbol_it = al_al_symbol_map_.find(symbol_name);

	if (al_symbol_it != al_al_symbol_map_.cend())
	{
		return al_symbol_it->second;
	}

	if (current_context_)
	{
		const auto symbol = current_context_->al_get_proc_address(symbol_name);

		if (symbol)
		{
			return symbol;
		}
	}

	return al_al_symbols_->alGetProcAddress(fname);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return nullptr;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return nullptr;
}

::ALenum AL_APIENTRY AlApiImpl::alGetEnumValue(
	const ::ALchar* ename) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetEnumValue(ename);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_NONE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_NONE;
}

void AL_APIENTRY AlApiImpl::alListenerf(
	::ALenum param,
	::ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListenerf(param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alListener3f(
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListener3f(param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alListenerfv(
	::ALenum param,
	const ::ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListenerfv(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alListeneri(
	::ALenum param,
	::ALint value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListeneri(param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alListener3i(
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListener3i(param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alListeneriv(
	::ALenum param,
	const ::ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListeneriv(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetListenerf(
	::ALenum param,
	::ALfloat* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListenerf(param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetListener3f(
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListener3f(param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetListenerfv(
	::ALenum param,
	::ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListenerfv(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetListeneri(
	::ALenum param,
	::ALint* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListeneri(param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetListener3i(
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListener3i(param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetListeneriv(
	::ALenum param,
	::ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListeneriv(param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGenSources(
	::ALsizei n,
	::ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGenSources(n, sources);

	auto& context = get_current_context();
	context.al_gen_sources(n, sources);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alDeleteSources(
	::ALsizei n,
	const ::ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDeleteSources(n, sources);
	auto& context = get_current_context();
	context.al_delete_sources(n, sources);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

::ALboolean AL_APIENTRY AlApiImpl::alIsSource(
	::ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alIsSource(source);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_FALSE;
}

void AL_APIENTRY AlApiImpl::alSourcef(
	::ALuint source,
	::ALenum param,
	::ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcef(source, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSource3f(
	::ALuint source,
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSource3f(source, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourcefv(
	::ALuint source,
	::ALenum param,
	const ::ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcefv(source, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourcei(
	::ALuint source,
	::ALenum param,
	::ALint value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcei(source, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSource3i(
	::ALuint source,
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSource3i(source, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourceiv(
	::ALuint source,
	::ALenum param,
	const ::ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceiv(source, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetSourcef(
	::ALuint source,
	::ALenum param,
	::ALfloat* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourcef(source, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetSource3f(
	::ALuint source,
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSource3f(source, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetSourcefv(
	::ALuint source,
	::ALenum param,
	::ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourcefv(source, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetSourcei(
	::ALuint source,
	::ALenum param,
	::ALint* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourcei(source, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetSource3i(
	::ALuint source,
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSource3i(source, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetSourceiv(
	::ALuint source,
	::ALenum param,
	::ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourceiv(source, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourcePlayv(
	::ALsizei n,
	const ::ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePlayv(n, sources);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourceStopv(
	::ALsizei n,
	const ::ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceStopv(n, sources);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourceRewindv(
	::ALsizei n,
	const ::ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceRewindv(n, sources);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourcePausev(
	::ALsizei n,
	const ::ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePausev(n, sources);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourcePlay(
	::ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePlay(source);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourceStop(
	::ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceStop(source);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourceRewind(
	::ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceRewind(source);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourcePause(
	::ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePause(source);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourceQueueBuffers(
	::ALuint source,
	::ALsizei nb,
	const ::ALuint* buffers) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceQueueBuffers(source, nb, buffers);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alSourceUnqueueBuffers(
	::ALuint source,
	::ALsizei nb,
	::ALuint* buffers) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceUnqueueBuffers(source, nb, buffers);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGenBuffers(
	::ALsizei n,
	::ALuint* buffers) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGenBuffers(n, buffers);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alDeleteBuffers(
	::ALsizei n,
	const ::ALuint* buffers) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDeleteBuffers(n, buffers);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

::ALboolean AL_APIENTRY AlApiImpl::alIsBuffer(
	::ALuint buffer) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alIsBuffer(buffer);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
	return AL_FALSE;
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
	return AL_FALSE;
}

void AL_APIENTRY AlApiImpl::alBufferData(
	::ALuint buffer,
	::ALenum format,
	const ::ALvoid* data,
	::ALsizei size,
	::ALsizei freq) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferData(buffer, format, data, size, freq);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alBufferf(
	::ALuint buffer,
	::ALenum param,
	::ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferf(buffer, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alBuffer3f(
	::ALuint buffer,
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBuffer3f(buffer, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alBufferfv(
	::ALuint buffer,
	::ALenum param,
	const ::ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferfv(buffer, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alBufferi(
	::ALuint buffer,
	::ALenum param,
	::ALint value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferi(buffer, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alBuffer3i(
	::ALuint buffer,
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBuffer3i(buffer, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alBufferiv(
	::ALuint buffer,
	::ALenum param,
	const ::ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferiv(buffer, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetBufferf(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferf(buffer, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetBuffer3f(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBuffer3f(buffer, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetBufferfv(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferfv(buffer, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetBufferi(
	::ALuint buffer,
	::ALenum param,
	::ALint* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferi(buffer, param, value);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetBuffer3i(
	::ALuint buffer,
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBuffer3i(buffer, param, value1, value2, value3);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

void AL_APIENTRY AlApiImpl::alGetBufferiv(
	::ALuint buffer,
	::ALenum param,
	::ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferiv(buffer, param, values);
}
catch (const std::exception& ex)
{
	logger_.error(ex.what());
}
catch (...)
{
	logger_.error(al_api::ErrorMessages::generic_exception);
}

// AL v1.1
// =========================================================================

void AlApiImpl::apply_patch_collection() noexcept
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
					logger_.info(message.c_str());
					process_patcher->apply();
				}

				break;

			case PatchStatus::patched:
				{
					const auto message = std::string{} + patch.name + " already patched.";
					logger_.info(message.c_str());
				}

				break;

			default:
				break;
		}
	}
}
catch (const std::exception& ex)
{
	logger_.error("Failed to patch.");
	logger_.error(ex.what());
}

void AlApiImpl::initialize_al_driver()
{
	logger_.info("Load AL driver.");

	constexpr const char* const known_names[] =
	{
		"eaxefx_driver.dll",
		"dsoal-aldrv.dll",
		"soft_oal.dll",
	};

	for (const auto known_name : known_names)
	{
		try
		{
			const auto message = "Try to load a driver \"" + String{known_name} + "\".";
			logger_.info(message.c_str());
			al_library_ = make_shared_library(known_name);

			return;
		}
		catch (const std::exception& ex)
		{
			logger_.error(ex.what());
		}
	}

	throw AlApiException{"Failed to load any suitable driver."};
}

void AlApiImpl::initialize_al_symbols()
{
	logger_.info("Create AL symbol loader.");
	al_loader_ = make_al_loader(al_library_.get());

	logger_.info("Load ALC v1.1 symbols.");
	al_alc_symbols_ = al_loader_->load_alc_symbols();

	logger_.info("Load AL v1.1 symbols.");
	al_al_symbols_ = al_loader_->load_al_symbols();
}

void AlApiImpl::initialize_al_alc_symbol_map() noexcept
{
	constexpr auto capacity = sizeof(AlAlcSymbols) / sizeof(void*);

	al_alc_symbol_map_.clear();
	al_alc_symbol_map_.reserve(capacity);

	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcCreateContext, reinterpret_cast<void*>(::alcCreateContext));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcMakeContextCurrent, reinterpret_cast<void*>(::alcMakeContextCurrent));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcProcessContext, reinterpret_cast<void*>(::alcProcessContext));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcSuspendContext, reinterpret_cast<void*>(::alcSuspendContext));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcDestroyContext, reinterpret_cast<void*>(::alcDestroyContext));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcGetCurrentContext, reinterpret_cast<void*>(::alcGetCurrentContext));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcGetContextsDevice, reinterpret_cast<void*>(::alcGetContextsDevice));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcOpenDevice, reinterpret_cast<void*>(::alcOpenDevice));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcCloseDevice, reinterpret_cast<void*>(::alcCloseDevice));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcGetError, reinterpret_cast<void*>(::alcGetError));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcIsExtensionPresent, reinterpret_cast<void*>(::alcIsExtensionPresent));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcGetProcAddress, reinterpret_cast<void*>(::alcGetProcAddress));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcGetEnumValue, reinterpret_cast<void*>(::alcGetEnumValue));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcGetString, reinterpret_cast<void*>(::alcGetString));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcGetIntegerv, reinterpret_cast<void*>(::alcGetIntegerv));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcCaptureOpenDevice, reinterpret_cast<void*>(::alcCaptureOpenDevice));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcCaptureCloseDevice, reinterpret_cast<void*>(::alcCaptureCloseDevice));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcCaptureStart, reinterpret_cast<void*>(::alcCaptureStart));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcCaptureStop, reinterpret_cast<void*>(::alcCaptureStop));
	al_alc_symbol_map_.emplace(AlAlcSymbolsNames::alcCaptureSamples, reinterpret_cast<void*>(::alcCaptureSamples));
}

void AlApiImpl::initialize_al_al_symbol_map() noexcept
{
	constexpr auto capacity = sizeof(AlAlSymbols) / sizeof(void*);

	al_al_symbol_map_.clear();
	al_al_symbol_map_.reserve(capacity);

	al_al_symbol_map_.emplace(AlAlSymbolsNames::alDopplerFactor, reinterpret_cast<void*>(::alDopplerFactor));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alDopplerVelocity, reinterpret_cast<void*>(::alDopplerVelocity));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSpeedOfSound, reinterpret_cast<void*>(::alSpeedOfSound));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alDistanceModel, reinterpret_cast<void*>(::alDistanceModel));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alEnable, reinterpret_cast<void*>(::alEnable));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alDisable, reinterpret_cast<void*>(::alDisable));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alIsEnabled, reinterpret_cast<void*>(::alIsEnabled));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetString, reinterpret_cast<void*>(::alGetString));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBooleanv, reinterpret_cast<void*>(::alGetBooleanv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetIntegerv, reinterpret_cast<void*>(::alGetIntegerv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetFloatv, reinterpret_cast<void*>(::alGetFloatv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetDoublev, reinterpret_cast<void*>(::alGetDoublev));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBoolean, reinterpret_cast<void*>(::alGetBoolean));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetInteger, reinterpret_cast<void*>(::alGetInteger));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetFloat, reinterpret_cast<void*>(::alGetFloat));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetDouble, reinterpret_cast<void*>(::alGetDouble));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetError, reinterpret_cast<void*>(::alGetError));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alIsExtensionPresent, reinterpret_cast<void*>(::alIsExtensionPresent));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetProcAddress, reinterpret_cast<void*>(::alGetProcAddress));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetEnumValue, reinterpret_cast<void*>(::alGetEnumValue));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alListenerf, reinterpret_cast<void*>(::alListenerf));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alListener3f, reinterpret_cast<void*>(::alListener3f));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alListenerfv, reinterpret_cast<void*>(::alListenerfv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alListeneri, reinterpret_cast<void*>(::alListeneri));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alListener3i, reinterpret_cast<void*>(::alListener3i));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alListeneriv, reinterpret_cast<void*>(::alListeneriv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetListenerf, reinterpret_cast<void*>(::alGetListenerf));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetListener3f, reinterpret_cast<void*>(::alGetListener3f));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetListenerfv, reinterpret_cast<void*>(::alGetListenerfv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetListeneri, reinterpret_cast<void*>(::alGetListeneri));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetListener3i, reinterpret_cast<void*>(::alGetListener3i));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetListeneriv, reinterpret_cast<void*>(::alGetListeneriv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGenSources, reinterpret_cast<void*>(::alGenSources));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alDeleteSources, reinterpret_cast<void*>(::alDeleteSources));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alIsSource, reinterpret_cast<void*>(::alIsSource));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourcef, reinterpret_cast<void*>(::alSourcef));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSource3f, reinterpret_cast<void*>(::alSource3f));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourcefv, reinterpret_cast<void*>(::alSourcefv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourcei, reinterpret_cast<void*>(::alSourcei));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSource3i, reinterpret_cast<void*>(::alSource3i));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourceiv, reinterpret_cast<void*>(::alSourceiv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetSourcef, reinterpret_cast<void*>(::alGetSourcef));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetSource3f, reinterpret_cast<void*>(::alGetSource3f));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetSourcefv, reinterpret_cast<void*>(::alGetSourcefv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetSourcei, reinterpret_cast<void*>(::alGetSourcei));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetSource3i, reinterpret_cast<void*>(::alGetSource3i));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetSourceiv, reinterpret_cast<void*>(::alGetSourceiv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourcePlayv, reinterpret_cast<void*>(::alSourcePlayv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourceStopv, reinterpret_cast<void*>(::alSourceStopv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourceRewindv, reinterpret_cast<void*>(::alSourceRewindv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourcePausev, reinterpret_cast<void*>(::alSourcePausev));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourcePlay, reinterpret_cast<void*>(::alSourcePlay));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourceStop, reinterpret_cast<void*>(::alSourceStop));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourceRewind, reinterpret_cast<void*>(::alSourceRewind));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourcePause, reinterpret_cast<void*>(::alSourcePause));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourceQueueBuffers, reinterpret_cast<void*>(::alSourceQueueBuffers));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alSourceUnqueueBuffers, reinterpret_cast<void*>(::alSourceUnqueueBuffers));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGenBuffers, reinterpret_cast<void*>(::alGenBuffers));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alDeleteBuffers, reinterpret_cast<void*>(::alDeleteBuffers));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alIsBuffer, reinterpret_cast<void*>(::alIsBuffer));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alBufferData, reinterpret_cast<void*>(::alBufferData));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alBufferf, reinterpret_cast<void*>(::alBufferf));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alBuffer3f, reinterpret_cast<void*>(::alBuffer3f));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alBufferfv, reinterpret_cast<void*>(::alBufferfv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alBufferi, reinterpret_cast<void*>(::alBufferi));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alBuffer3i, reinterpret_cast<void*>(::alBuffer3i));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alBufferiv, reinterpret_cast<void*>(::alBufferiv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBufferf, reinterpret_cast<void*>(::alGetBufferf));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBuffer3f, reinterpret_cast<void*>(::alGetBuffer3f));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBufferfv, reinterpret_cast<void*>(::alGetBufferfv));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBufferi, reinterpret_cast<void*>(::alGetBufferi));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBuffer3i, reinterpret_cast<void*>(::alGetBuffer3i));
	al_al_symbol_map_.emplace(AlAlSymbolsNames::alGetBufferiv, reinterpret_cast<void*>(::alGetBufferiv));
}

void AlApiImpl::initialize_al_wrapper_entries() noexcept
{
	initialize_al_alc_symbol_map();
	initialize_al_al_symbol_map();
}

MoveableMutexLock AlApiImpl::initialize_invalid_state()
{
	throw AlApiException{ErrorMessages::invalid_state};
}

MoveableMutexLock AlApiImpl::initialize_not_initialized()
{
	throw AlApiException{ErrorMessages::not_initialized};
}

MoveableMutexLock AlApiImpl::initialize_lock_mutex()
{
	assert(mutex_);
	return MoveableMutexLock{*mutex_};
}

MoveableMutexLock AlApiImpl::initialize_try_to_initialize()
try
{
	assert(mutex_);
	auto mt_lock = MoveableMutexLock{*mutex_};

	auto logger_param = LoggerParam{};
	logger_param.file_path = "eaxefx_log.txt";
	logger_.make(logger_param);

	logger_.info("");
	logger_.info("<<<<<<<<<<<<<<<<<<<<<<<<");
	logger_.info("EAXEFX v" EAXEFX_VERSION);
	logger_.info("<<<<<<<<<<<<<<<<<<<<<<<<");
	logger_.info("");

	initialize_al_driver();
	initialize_al_symbols();
	initialize_al_wrapper_entries();

	apply_patch_collection();

	log_devices_info();

	initialize_func_ = &AlApiImpl::initialize_lock_mutex;

	return mt_lock;
}
catch (...)
{
	initialize_func_ = &AlApiImpl::initialize_not_initialized;
	throw;
}

MoveableMutexLock AlApiImpl::initialize()
{
	assert(initialize_func_);
	return (this->*initialize_func_)();
}

AlApiImpl::Device* AlApiImpl::find_device(
	::ALCdevice* al_device) noexcept
{
	for (auto& device : devices_)
	{
		if (device.al_device == al_device)
		{
			return &device;
		}
	}

	return nullptr;
}

AlApiImpl::Device& AlApiImpl::get_device(
	::ALCdevice* al_device)
{
	auto device = find_device(al_device);

	if (!device)
	{
		throw AlApiException{"Device not found."};
	}

	return *device;
}

AlApiContext& AlApiImpl::get_context()
{
	if (!current_context_)
	{
		throw AlApiException{"No current context."};
	}

	return *current_context_;
}

AlApiContext& AlApiImpl::get_context(
	::ALCcontext* al_context)
{
	for (auto& device : devices_)
	{
		for (auto& context : device.contexts)
		{
			if (context->get_al_context() == al_context)
			{
				return *context;
			}
		}
	}

	throw AlApiException{"Unregistered context."};
}

void AlApiImpl::remove_context(
	const AlApiContext& context)
{
	auto is_removed = false;

	for (auto& device : devices_)
	{
		auto& contexts = device.contexts;

		const auto old_context_count = contexts.size();

		contexts.remove_if(
			[&context_to_remove = context](
				const AlApiContextUPtr& context)
			{
				return context.get() == &context_to_remove;
			}
		);

		const auto new_context_count = contexts.size();

		if (old_context_count != new_context_count)
		{
			is_removed = true;
			break;
		}
	}

	if (!is_removed)
	{
		throw AlApiException{"Unregistered context."};
	}
}

void AlApiImpl::log_devices_info(
	::ALCenum alc_token)
{
	const auto alc_devices_names = al_alc_symbols_->alcGetString(nullptr, alc_token);

	if (!alc_devices_names || (*alc_devices_names) == '\0')
	{
		logger_.info(al_api::Strings::indented_none);
		return;
	}

	auto buffer = String{};
	buffer.reserve(128);

	auto al_device_name = alc_devices_names;

	while ((*al_device_name) != '\0')
	{
		const auto device_name_size = String::traits_type::length(al_device_name);

		buffer.clear();
		buffer += "    \"";
		buffer.append(al_device_name, device_name_size);
		buffer += "\"";

		logger_.info(buffer.c_str());

		al_device_name += device_name_size + 1;
	}
}

void AlApiImpl::log_playback_devices()
{
	logger_.info("");
	logger_.info("Playback devices:");
	log_devices_info(ALC_DEVICE_SPECIFIER);
}

void AlApiImpl::log_capture_devices()
{
	logger_.info("");
	logger_.info("Capture devices:");
	log_devices_info(ALC_CAPTURE_DEVICE_SPECIFIER);
}

void AlApiImpl::log_devices_info()
{
	log_playback_devices();
	log_capture_devices();
}

void AlApiImpl::log_device_extensions(
	::ALCdevice* al_device)
{
	const auto al_device_extensions = al_alc_symbols_->alcGetString(al_device, ALC_EXTENSIONS);

	logger_.info("");
	logger_.info("Extensions:");
	al_api::log_extensions(&logger_, al_device_extensions);
}

void AlApiImpl::log_device_info(
	::ALCdevice* al_device)
{
	log_device_extensions(al_device);
}

void AlApiImpl::log_desired_context_attribute_list(
	const ::ALCint* al_attributes)
{
	logger_.info("");
	logger_.info("Input attributes:");
	al_api::log_context_attribute_list(&logger_, al_attributes);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlApiImpl g_al_api_impl{};
AlApi& g_al_api = g_al_api_impl;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void on_thread_detach() noexcept
{
	g_al_api_impl.on_thread_detach();
}

void on_process_detach() noexcept
{
	g_al_api_impl.on_process_detach();
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern "C"
{


// ==========================================================================
// ALC v1.1

ALC_API ::ALCcontext* ALC_APIENTRY alcCreateContext(
	::ALCdevice* device,
	const ::ALCint* attrlist)
{
	return eaxefx::g_al_api_impl.alcCreateContext(device, attrlist);
}

ALC_API ::ALCboolean ALC_APIENTRY alcMakeContextCurrent(
	::ALCcontext* context)
{
	return eaxefx::g_al_api_impl.alcMakeContextCurrent(context);
}

ALC_API void ALC_APIENTRY alcProcessContext(
	::ALCcontext* context)
{
	eaxefx::g_al_api_impl.alcProcessContext(context);
}

ALC_API void ALC_APIENTRY alcSuspendContext(
	::ALCcontext* context)
{
	eaxefx::g_al_api_impl.alcSuspendContext(context);
}

ALC_API void ALC_APIENTRY alcDestroyContext(
	::ALCcontext* context)
{
	eaxefx::g_al_api_impl.alcDestroyContext(context);
}

ALC_API ::ALCcontext* ALC_APIENTRY alcGetCurrentContext()
{
	return eaxefx::g_al_api_impl.alcGetCurrentContext();
}

ALC_API ::ALCdevice* ALC_APIENTRY alcGetContextsDevice(
	::ALCcontext* context)
{
	return eaxefx::g_al_api_impl.alcGetContextsDevice(context);
}

ALC_API ::ALCdevice* ALC_APIENTRY alcOpenDevice(
	const ::ALCchar* devicename)
{
	return eaxefx::g_al_api_impl.alcOpenDevice(devicename);
}

ALC_API ::ALCboolean ALC_APIENTRY alcCloseDevice(
	::ALCdevice* device)
{
	return eaxefx::g_al_api_impl.alcCloseDevice(device);
}

ALC_API ::ALCenum ALC_APIENTRY alcGetError(
	::ALCdevice* device)
{
	return eaxefx::g_al_api_impl.alcGetError(device);
}

ALC_API ::ALCboolean ALC_APIENTRY alcIsExtensionPresent(
	::ALCdevice* device,
	const ::ALCchar* extname)
{
	return eaxefx::g_al_api_impl.alcIsExtensionPresent(device, extname);
}

ALC_API void* ALC_APIENTRY alcGetProcAddress(
	::ALCdevice* device,
	const ::ALCchar* funcname)
{
	return eaxefx::g_al_api_impl.alcGetProcAddress(device, funcname);
}

ALC_API ::ALCenum ALC_APIENTRY alcGetEnumValue(
	::ALCdevice* device,
	const ::ALCchar* enumname)
{
	return eaxefx::g_al_api_impl.alcGetEnumValue(device, enumname);
}

ALC_API const ::ALCchar* ALC_APIENTRY alcGetString(
	::ALCdevice* device,
	::ALCenum param)
{
	return eaxefx::g_al_api_impl.alcGetString(device, param);
}

ALC_API void ALC_APIENTRY alcGetIntegerv(
	::ALCdevice* device,
	::ALCenum param,
	::ALCsizei size,
	::ALCint* values)
{
	eaxefx::g_al_api_impl.alcGetIntegerv(device, param, size, values);
}

ALC_API ::ALCdevice* ALC_APIENTRY alcCaptureOpenDevice(
	const ::ALCchar* devicename,
	::ALCuint frequency,
	::ALCenum format,
	::ALCsizei buffersize)
{
	return eaxefx::g_al_api_impl.alcCaptureOpenDevice(devicename, frequency, format, buffersize);
}

ALC_API ::ALCboolean ALC_APIENTRY alcCaptureCloseDevice(
	::ALCdevice* device)
{
	return eaxefx::g_al_api_impl.alcCaptureCloseDevice(device);
}

ALC_API void ALC_APIENTRY alcCaptureStart(
	::ALCdevice* device)
{
	eaxefx::g_al_api_impl.alcCaptureStart(device);
}

ALC_API void ALC_APIENTRY alcCaptureStop(::ALCdevice* device)
{
	eaxefx::g_al_api_impl.alcCaptureStop(device);
}

ALC_API void ALC_APIENTRY alcCaptureSamples(
	::ALCdevice* device,
	::ALCvoid* buffer,
	::ALCsizei samples)
{
	eaxefx::g_al_api_impl.alcCaptureSamples(device, buffer, samples);
}

// ALC v1.1
// ==========================================================================


// ==========================================================================
// AL v1.1

AL_API void AL_APIENTRY alDopplerFactor(
	::ALfloat value)
{
	eaxefx::g_al_api_impl.alDopplerFactor(value);
}

AL_API void AL_APIENTRY alDopplerVelocity(
	::ALfloat value)
{
	eaxefx::g_al_api_impl.alDopplerVelocity(value);
}

AL_API void AL_APIENTRY alSpeedOfSound(
	::ALfloat value)
{
	eaxefx::g_al_api_impl.alSpeedOfSound(value);
}

AL_API void AL_APIENTRY alDistanceModel(
	::ALenum distanceModel)
{
	eaxefx::g_al_api_impl.alDistanceModel(distanceModel);
}

AL_API void AL_APIENTRY alEnable(
	::ALenum capability)
{
	eaxefx::g_al_api_impl.alEnable(capability);
}

AL_API void AL_APIENTRY alDisable(
	::ALenum capability)
{
	eaxefx::g_al_api_impl.alDisable(capability);
}

AL_API ::ALboolean AL_APIENTRY alIsEnabled(
	::ALenum capability)
{
	return eaxefx::g_al_api_impl.alIsEnabled(capability);
}

AL_API const ::ALchar* AL_APIENTRY alGetString(
	::ALenum param)
{
	return eaxefx::g_al_api_impl.alGetString(param);
}

AL_API void AL_APIENTRY alGetBooleanv(
	::ALenum param,
	::ALboolean* values)
{
	eaxefx::g_al_api_impl.alGetBooleanv(param, values);
}

AL_API void AL_APIENTRY alGetIntegerv(
	::ALenum param,
	::ALint* values)
{
	eaxefx::g_al_api_impl.alGetIntegerv(param, values);
}

AL_API void AL_APIENTRY alGetFloatv(
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetFloatv(param, values);
}

AL_API void AL_APIENTRY alGetDoublev(
	::ALenum param,
	::ALdouble* values)
{
	eaxefx::g_al_api_impl.alGetDoublev(param, values);
}

AL_API ::ALboolean AL_APIENTRY alGetBoolean(
	::ALenum param)
{
	return eaxefx::g_al_api_impl.alGetBoolean(param);
}

AL_API ::ALint AL_APIENTRY alGetInteger(
	::ALenum param)
{
	return eaxefx::g_al_api_impl.alGetInteger(param);
}

AL_API ::ALfloat AL_APIENTRY alGetFloat(
	::ALenum param)
{
	return eaxefx::g_al_api_impl.alGetFloat(param);
}

AL_API ::ALdouble AL_APIENTRY alGetDouble(
	::ALenum param)
{
	return eaxefx::g_al_api_impl.alGetDouble(param);
}

AL_API ::ALenum AL_APIENTRY alGetError()
{
	return eaxefx::g_al_api_impl.alGetError();
}

AL_API ::ALboolean AL_APIENTRY alIsExtensionPresent(
	const ::ALchar* extname)
{
	return eaxefx::g_al_api_impl.alIsExtensionPresent(extname);
}

AL_API void* AL_APIENTRY alGetProcAddress(
	const ::ALchar* fname)
{
	return eaxefx::g_al_api_impl.alGetProcAddress(fname);
}

AL_API ::ALenum AL_APIENTRY alGetEnumValue(
	const ::ALchar* ename)
{
	return eaxefx::g_al_api_impl.alGetEnumValue(ename);
}

AL_API void AL_APIENTRY alListenerf(
	::ALenum param,
	::ALfloat value)
{
	eaxefx::g_al_api_impl.alListenerf(param, value);
}

AL_API void AL_APIENTRY alListener3f(
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3)
{
	eaxefx::g_al_api_impl.alListener3f(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alListenerfv(
	::ALenum param,
	const ::ALfloat* values)
{
	eaxefx::g_al_api_impl.alListenerfv(param, values);
}

AL_API void AL_APIENTRY alListeneri(
	::ALenum param,
	::ALint value)
{
	eaxefx::g_al_api_impl.alListeneri(param, value);
}

AL_API void AL_APIENTRY alListener3i(
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3)
{
	eaxefx::g_al_api_impl.alListener3i(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alListeneriv(
	::ALenum param,
	const ::ALint* values)
{
	eaxefx::g_al_api_impl.alListeneriv(param, values);
}

AL_API void AL_APIENTRY alGetListenerf(
	::ALenum param,
	::ALfloat* value)
{
	eaxefx::g_al_api_impl.alGetListenerf(param, value);
}

AL_API void AL_APIENTRY alGetListener3f(
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3)
{
	eaxefx::g_al_api_impl.alGetListener3f(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetListenerfv(
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetListenerfv(param, values);
}

AL_API void AL_APIENTRY alGetListeneri(
	::ALenum param,
	::ALint* value)
{
	eaxefx::g_al_api_impl.alGetListeneri(param, value);
}

AL_API void AL_APIENTRY alGetListener3i(
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3)
{
	eaxefx::g_al_api_impl.alGetListener3i(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetListeneriv(
	::ALenum param,
	::ALint* values)
{
	eaxefx::g_al_api_impl.alGetListeneriv(param, values);
}

AL_API void AL_APIENTRY alGenSources(
	::ALsizei n,
	::ALuint* sources)
{
	eaxefx::g_al_api_impl.alGenSources(n, sources);
}

AL_API void AL_APIENTRY alDeleteSources(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::g_al_api_impl.alDeleteSources(n, sources);
}

AL_API ::ALboolean AL_APIENTRY alIsSource(
	::ALuint source)
{
	return eaxefx::g_al_api_impl.alIsSource(source);
}

AL_API void AL_APIENTRY alSourcef(
	::ALuint source,
	::ALenum param,
	::ALfloat value)
{
	eaxefx::g_al_api_impl.alSourcef(source, param, value);
}

AL_API void AL_APIENTRY alSource3f(
	::ALuint source,
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3)
{
	eaxefx::g_al_api_impl.alSource3f(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourcefv(
	::ALuint source,
	::ALenum param,
	const ::ALfloat* values)
{
	eaxefx::g_al_api_impl.alSourcefv(source, param, values);
}

AL_API void AL_APIENTRY alSourcei(
	::ALuint source,
	::ALenum param,
	::ALint value)
{
	eaxefx::g_al_api_impl.alSourcei(source, param, value);
}

AL_API void AL_APIENTRY alSource3i(
	::ALuint source,
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3)
{
	eaxefx::g_al_api_impl.alSource3i(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourceiv(
	::ALuint source,
	::ALenum param,
	const ::ALint* values)
{
	eaxefx::g_al_api_impl.alSourceiv(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcef(
	::ALuint source,
	::ALenum param,
	::ALfloat* value)
{
	eaxefx::g_al_api_impl.alGetSourcef(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3f(
	::ALuint source,
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3)
{
	eaxefx::g_al_api_impl.alGetSource3f(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourcefv(
	::ALuint source,
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetSourcefv(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcei(
	::ALuint source,
	::ALenum param,
	::ALint* value)
{
	eaxefx::g_al_api_impl.alGetSourcei(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3i(
	::ALuint source,
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3)
{
	eaxefx::g_al_api_impl.alGetSource3i(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourceiv(
	::ALuint source,
	::ALenum param,
	::ALint* values)
{
	eaxefx::g_al_api_impl.alGetSourceiv(source, param, values);
}

AL_API void AL_APIENTRY alSourcePlayv(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourcePlayv(n, sources);
}

AL_API void AL_APIENTRY alSourceStopv(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourceStopv(n, sources);
}

AL_API void AL_APIENTRY alSourceRewindv(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourceRewindv(n, sources);
}

AL_API void AL_APIENTRY alSourcePausev(
	::ALsizei n,
	const ::ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourcePausev(n, sources);
}

AL_API void AL_APIENTRY alSourcePlay(
	::ALuint source)
{
	eaxefx::g_al_api_impl.alSourcePlay(source);
}

AL_API void AL_APIENTRY alSourceStop(
	::ALuint source)
{
	eaxefx::g_al_api_impl.alSourceStop(source);
}

AL_API void AL_APIENTRY alSourceRewind(
	::ALuint source)
{
	eaxefx::g_al_api_impl.alSourceRewind(source);
}

AL_API void AL_APIENTRY alSourcePause(
	::ALuint source)
{
	eaxefx::g_al_api_impl.alSourcePause(source);
}

AL_API void AL_APIENTRY alSourceQueueBuffers(
	::ALuint source,
	::ALsizei nb,
	const ::ALuint* buffers)
{
	eaxefx::g_al_api_impl.alSourceQueueBuffers(source, nb, buffers);
}

AL_API void AL_APIENTRY alSourceUnqueueBuffers(
	::ALuint source,
	::ALsizei nb,
	::ALuint* buffers)
{
	eaxefx::g_al_api_impl.alSourceUnqueueBuffers(source, nb, buffers);
}

AL_API void AL_APIENTRY alGenBuffers(
	::ALsizei n,
	::ALuint* buffers)
{
	eaxefx::g_al_api_impl.alGenBuffers(n, buffers);
}

AL_API void AL_APIENTRY alDeleteBuffers(
	::ALsizei n,
	const ::ALuint* buffers)
{
	eaxefx::g_al_api_impl.alDeleteBuffers(n, buffers);
}

AL_API ::ALboolean AL_APIENTRY alIsBuffer(
	::ALuint buffer)
{
	return eaxefx::g_al_api_impl.alIsBuffer(buffer);
}

AL_API void AL_APIENTRY alBufferData(
	::ALuint buffer,
	::ALenum format,
	const ::ALvoid* data,
	::ALsizei size,
	::ALsizei freq)
{
	eaxefx::g_al_api_impl.alBufferData(buffer, format, data, size, freq);
}

AL_API void AL_APIENTRY alBufferf(
	::ALuint buffer,
	::ALenum param,
	::ALfloat value)
{
	eaxefx::g_al_api_impl.alBufferf(buffer, param, value);
}

AL_API void AL_APIENTRY alBuffer3f(
	::ALuint buffer,
	::ALenum param,
	::ALfloat value1,
	::ALfloat value2,
	::ALfloat value3)
{
	eaxefx::g_al_api_impl.alBuffer3f(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alBufferfv(
	::ALuint buffer,
	::ALenum param,
	const ::ALfloat* values)
{
	eaxefx::g_al_api_impl.alBufferfv(buffer, param, values);
}

AL_API void AL_APIENTRY alBufferi(
	::ALuint buffer,
	::ALenum param,
	::ALint value)
{
	eaxefx::g_al_api_impl.alBufferi(buffer, param, value);
}

AL_API void AL_APIENTRY alBuffer3i(
	::ALuint buffer,
	::ALenum param,
	::ALint value1,
	::ALint value2,
	::ALint value3)
{
	eaxefx::g_al_api_impl.alBuffer3i(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alBufferiv(
	::ALuint buffer,
	::ALenum param,
	const ::ALint* values)
{
	eaxefx::g_al_api_impl.alBufferiv(buffer, param, values);
}

AL_API void AL_APIENTRY alGetBufferf(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* value)
{
	eaxefx::g_al_api_impl.alGetBufferf(buffer, param, value);
}

AL_API void AL_APIENTRY alGetBuffer3f(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* value1,
	::ALfloat* value2,
	::ALfloat* value3)
{
	eaxefx::g_al_api_impl.alGetBuffer3f(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetBufferfv(
	::ALuint buffer,
	::ALenum param,
	::ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetBufferfv(buffer, param, values);
}

AL_API void AL_APIENTRY alGetBufferi(
	::ALuint buffer,
	::ALenum param,
	::ALint* value)
{
	eaxefx::g_al_api_impl.alGetBufferi(buffer, param, value);
}

AL_API void AL_APIENTRY alGetBuffer3i(
	::ALuint buffer,
	::ALenum param,
	::ALint* value1,
	::ALint* value2,
	::ALint* value3)
{
	eaxefx::g_al_api_impl.alGetBuffer3i(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetBufferiv(
	::ALuint buffer,
	::ALenum param,
	::ALint* values)
{
	eaxefx::g_al_api_impl.alGetBufferiv(buffer, param, values);
}

// AL v1.1
// ==========================================================================


} // extern "C"


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
