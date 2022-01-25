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

#include <cstddef>

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
#include "eaxefx_env.h"
#include "eaxefx_exception.h"
#include "eaxefx_fs.h"
#include "eaxefx_fs_path.h"
#include "eaxefx_logger.h"
#include "eaxefx_moveable_mutex_lock.h"
#include "eaxefx_mutex.h"
#include "eaxefx_shared_library.h"
#include "eaxefx_span.h"
#include "eaxefx_string.h"
#include "eaxefx_utils.h"

#include "eaxefx_patch.h"
#include "eaxefx_patch_collection.h"


namespace eaxefx
{


namespace
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

constexpr auto x_ram_max_size = static_cast<ALsizei>(64 * 1'024 * 1'024);

constexpr auto x_ram_ram_size_enum = static_cast<ALenum>(0x20001);
constexpr auto x_ram_ram_free_enum = static_cast<ALenum>(0x20002);
constexpr auto x_ram_al_storage_automatic_enum = static_cast<ALenum>(0x20003);
constexpr auto x_ram_al_storage_hardware_enum = static_cast<ALenum>(0x20004);
constexpr auto x_ram_al_storage_accessible_enum = static_cast<ALenum>(0x20005);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

ALboolean AL_APIENTRY EAXSetBufferMode(
	ALsizei n,
	const ALuint* buffers,
	ALint value)
try
{
	const auto mutex_lock = g_al_api.get_lock();

	return g_al_api.eax_set_buffer_mode(n, buffers, value);
}
catch (...)
{
	utils::log_exception(g_al_api.get_logger(), AlXRamSymbolsNames::EAXSetBufferMode);
	return AL_FALSE;
}

ALenum AL_APIENTRY EAXGetBufferMode(
	ALuint buffer,
	ALint* pReserved)
try
{
	const auto mutex_lock = g_al_api.get_lock();

	return g_al_api.eax_get_buffer_mode(buffer, pReserved);
}
catch (...)
{
	utils::log_exception(g_al_api.get_logger(), AlXRamSymbolsNames::EAXGetBufferMode);
	return x_ram_al_storage_automatic_enum;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // namespace


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

class AlApiImpl final :
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

	ALboolean AL_APIENTRY eax_set_buffer_mode(
		ALsizei n,
		const ALuint* buffers,
		ALint value);

	ALenum AL_APIENTRY eax_get_buffer_mode(
		ALuint buffer,
		ALint* pReserved);

	Eaxx* get_eaxx() const noexcept;


	// =========================================================================
	// ALC v1.1

	ALCcontext* ALC_APIENTRY alc_create_context(
		ALCdevice* device,
		const ALCint* attrlist) noexcept;

	ALCboolean ALC_APIENTRY alc_make_context_current(
		ALCcontext* context) noexcept;

	void ALC_APIENTRY alc_process_context(
		ALCcontext* context) noexcept;

	void ALC_APIENTRY alc_suspend_context(
		ALCcontext* context) noexcept;

	void ALC_APIENTRY alc_destroy_context(
		ALCcontext* context) noexcept;

	ALCcontext* ALC_APIENTRY alc_get_current_context() noexcept;

	ALCdevice* ALC_APIENTRY alc_get_contexts_device(
		ALCcontext* context) noexcept;

	ALCdevice* ALC_APIENTRY alc_open_device(
		const ALCchar* devicename) noexcept;

	ALCboolean ALC_APIENTRY alc_close_device(
		ALCdevice* device) noexcept;

	ALCenum ALC_APIENTRY alc_get_error(
		ALCdevice* device) noexcept;

	ALCboolean ALC_APIENTRY alc_is_extension_present(
		ALCdevice* device,
		const ALCchar* extname) noexcept;

	void* ALC_APIENTRY alc_get_proc_address(
		ALCdevice* device,
		const ALCchar* funcname) noexcept;

	ALCenum ALC_APIENTRY alc_get_enum_value(
		ALCdevice* device,
		const ALCchar* enumname) noexcept;

	const ALCchar* ALC_APIENTRY alc_get_string(
		ALCdevice* device,
		ALCenum param) noexcept;

	void ALC_APIENTRY alc_get_integer_v(
		ALCdevice* device,
		ALCenum param,
		ALCsizei size,
		ALCint* values) noexcept;

	ALCdevice* ALC_APIENTRY alc_capture_open_device(
		const ALCchar* devicename,
		ALCuint frequency,
		ALCenum format,
		ALCsizei buffersize) noexcept;

	ALCboolean ALC_APIENTRY alc_capture_close_device(
		ALCdevice* device) noexcept;

	void ALC_APIENTRY alc_capture_start(
		ALCdevice* device) noexcept;

	void ALC_APIENTRY alc_capture_stop(
		ALCdevice* device) noexcept;

	void ALC_APIENTRY alc_capture_samples(
		ALCdevice* device,
		ALCvoid* buffer,
		ALCsizei samples) noexcept;

	// ALC v1.1
	// =========================================================================


	// =========================================================================
	// AL v1.1

	void AL_APIENTRY alDopplerFactor(
		ALfloat value) noexcept;

	void AL_APIENTRY alDopplerVelocity(
		ALfloat value) noexcept;

	void AL_APIENTRY alSpeedOfSound(
		ALfloat value) noexcept;

	void AL_APIENTRY alDistanceModel(
		ALenum distanceModel) noexcept;

	void AL_APIENTRY alEnable(
		ALenum capability) noexcept;

	void AL_APIENTRY alDisable(
		ALenum capability) noexcept;

	ALboolean AL_APIENTRY alIsEnabled(
		ALenum capability) noexcept;

	const ALchar* AL_APIENTRY alGetString(
		ALenum param) noexcept;

	void AL_APIENTRY alGetBooleanv(
		ALenum param,
		ALboolean* values) noexcept;

	void AL_APIENTRY alGetIntegerv(
		ALenum param,
		ALint* values) noexcept;

	void AL_APIENTRY alGetFloatv(
		ALenum param,
		ALfloat* values) noexcept;

	void AL_APIENTRY alGetDoublev(
		ALenum param,
		ALdouble* values) noexcept;

	ALboolean AL_APIENTRY alGetBoolean(
		ALenum param) noexcept;

	ALint AL_APIENTRY alGetInteger(
		ALenum param) noexcept;

	ALfloat AL_APIENTRY alGetFloat(
		ALenum param) noexcept;

	ALdouble AL_APIENTRY alGetDouble(
		ALenum param) noexcept;

	ALenum AL_APIENTRY alGetError() noexcept;

	ALboolean AL_APIENTRY alIsExtensionPresent(
		const ALchar* extname) noexcept;

	void* AL_APIENTRY alGetProcAddress(
		const ALchar* fname) noexcept;

	ALenum AL_APIENTRY alGetEnumValue(
		const ALchar* ename) noexcept;

	void AL_APIENTRY alListenerf(
		ALenum param,
		ALfloat value) noexcept;

	void AL_APIENTRY alListener3f(
		ALenum param,
		ALfloat value1,
		ALfloat value2,
		ALfloat value3) noexcept;

	void AL_APIENTRY alListenerfv(
		ALenum param,
		const ALfloat* values) noexcept;

	void AL_APIENTRY alListeneri(
		ALenum param,
		ALint value) noexcept;

	void AL_APIENTRY alListener3i(
		ALenum param,
		ALint value1,
		ALint value2,
		ALint value3) noexcept;

	void AL_APIENTRY alListeneriv(
		ALenum param,
		const ALint* values) noexcept;

	void AL_APIENTRY alGetListenerf(
		ALenum param,
		ALfloat* value) noexcept;

	void AL_APIENTRY alGetListener3f(
		ALenum param,
		ALfloat* value1,
		ALfloat* value2,
		ALfloat* value3) noexcept;

	void AL_APIENTRY alGetListenerfv(
		ALenum param,
		ALfloat* values) noexcept;

	void AL_APIENTRY alGetListeneri(
		ALenum param,
		ALint* value) noexcept;

	void AL_APIENTRY alGetListener3i(
		ALenum param,
		ALint* value1,
		ALint* value2,
		ALint* value3) noexcept;

	void AL_APIENTRY alGetListeneriv(
		ALenum param,
		ALint* values) noexcept;

	void AL_APIENTRY alGenSources(
		ALsizei n,
		ALuint* sources) noexcept;

	void AL_APIENTRY alDeleteSources(
		ALsizei n,
		const ALuint* sources) noexcept;

	ALboolean AL_APIENTRY alIsSource(
		ALuint source) noexcept;

	void AL_APIENTRY alSourcef(
		ALuint source,
		ALenum param,
		ALfloat value) noexcept;

	void AL_APIENTRY alSource3f(
		ALuint source,
		ALenum param,
		ALfloat value1,
		ALfloat value2,
		ALfloat value3) noexcept;

	void AL_APIENTRY alSourcefv(
		ALuint source,
		ALenum param,
		const ALfloat* values) noexcept;

	void AL_APIENTRY alSourcei(
		ALuint source,
		ALenum param,
		ALint value) noexcept;

	void AL_APIENTRY alSource3i(
		ALuint source,
		ALenum param,
		ALint value1,
		ALint value2,
		ALint value3) noexcept;

	void AL_APIENTRY alSourceiv(
		ALuint source,
		ALenum param,
		const ALint* values) noexcept;

	void AL_APIENTRY alGetSourcef(
		ALuint source,
		ALenum param,
		ALfloat* value) noexcept;

	void AL_APIENTRY alGetSource3f(
		ALuint source,
		ALenum param,
		ALfloat* value1,
		ALfloat* value2,
		ALfloat* value3) noexcept;

	void AL_APIENTRY alGetSourcefv(
		ALuint source,
		ALenum param,
		ALfloat* values) noexcept;

	void AL_APIENTRY alGetSourcei(
		ALuint source,
		ALenum param,
		ALint* value) noexcept;

	void AL_APIENTRY alGetSource3i(
		ALuint source,
		ALenum param,
		ALint* value1,
		ALint* value2,
		ALint* value3) noexcept;

	void AL_APIENTRY alGetSourceiv(
		ALuint source,
		ALenum param,
		ALint* values) noexcept;

	void AL_APIENTRY alSourcePlayv(
		ALsizei n,
		const ALuint* sources) noexcept;

	void AL_APIENTRY alSourceStopv(
		ALsizei n,
		const ALuint* sources) noexcept;

	void AL_APIENTRY alSourceRewindv(
		ALsizei n,
		const ALuint* sources) noexcept;

	void AL_APIENTRY alSourcePausev(
		ALsizei n,
		const ALuint* sources) noexcept;

	void AL_APIENTRY alSourcePlay(
		ALuint source) noexcept;

	void AL_APIENTRY alSourceStop(
		ALuint source) noexcept;

	void AL_APIENTRY alSourceRewind(
		ALuint source) noexcept;

	void AL_APIENTRY alSourcePause(
		ALuint source) noexcept;

	void AL_APIENTRY alSourceQueueBuffers(
		ALuint source,
		ALsizei nb,
		const ALuint* buffers) noexcept;

	void AL_APIENTRY alSourceUnqueueBuffers(
		ALuint source,
		ALsizei nb,
		ALuint* buffers) noexcept;

	void AL_APIENTRY alGenBuffers(
		ALsizei n,
		ALuint* buffers) noexcept;

	void AL_APIENTRY alDeleteBuffers(
		ALsizei n,
		const ALuint* buffers) noexcept;

	ALboolean AL_APIENTRY alIsBuffer(
		ALuint buffer) noexcept;

	void AL_APIENTRY alBufferData(
		ALuint buffer,
		ALenum format,
		const ALvoid* data,
		ALsizei size,
		ALsizei freq) noexcept;

	void AL_APIENTRY alBufferf(
		ALuint buffer,
		ALenum param,
		ALfloat value) noexcept;

	void AL_APIENTRY alBuffer3f(
		ALuint buffer,
		ALenum param,
		ALfloat value1,
		ALfloat value2,
		ALfloat value3) noexcept;

	void AL_APIENTRY alBufferfv(
		ALuint buffer,
		ALenum param,
		const ALfloat* values) noexcept;

	void AL_APIENTRY alBufferi(
		ALuint buffer,
		ALenum param,
		ALint value) noexcept;

	void AL_APIENTRY alBuffer3i(
		ALuint buffer,
		ALenum param,
		ALint value1,
		ALint value2,
		ALint value3) noexcept;

	void AL_APIENTRY alBufferiv(
		ALuint buffer,
		ALenum param,
		const ALint* values) noexcept;

	void AL_APIENTRY alGetBufferf(
		ALuint buffer,
		ALenum param,
		ALfloat* value) noexcept;

	void AL_APIENTRY alGetBuffer3f(
		ALuint buffer,
		ALenum param,
		ALfloat* value1,
		ALfloat* value2,
		ALfloat* value3) noexcept;

	void AL_APIENTRY alGetBufferfv(
		ALuint buffer,
		ALenum param,
		ALfloat* values) noexcept;

	void AL_APIENTRY alGetBufferi(
		ALuint buffer,
		ALenum param,
		ALint* value) noexcept;

	void AL_APIENTRY alGetBuffer3i(
		ALuint buffer,
		ALenum param,
		ALint* value1,
		ALint* value2,
		ALint* value3) noexcept;

	void AL_APIENTRY alGetBufferiv(
		ALuint buffer,
		ALenum param,
		ALint* values) noexcept;

	// AL v1.1
	// =========================================================================


private:
	struct Strings;
	struct ErrorMessages;

	static constexpr auto min_string_buffer_capacity = 4 * 1'024;

	using AlSymbolMap = std::unordered_map<std::string_view, void*>;

	struct Buffer
	{
		ALsizei size{};
		ALenum x_ram_mode{};
		bool x_ram_is_hardware{};
		bool x_ram_is_dirty{};
	}; // Buffer

	using BufferMap = std::unordered_map<ALuint, Buffer>;

	using Contexts = std::list<AlApiContextUPtr>;


	struct Device
	{
		ALCdevice* al_device{};
		ALsizei x_ram_free_size{};

		String special_name{};
		BufferMap buffers{};
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


	[[noreturn]]
	static void fail(
		const char* message);


	void apply_patch_collection() noexcept;


	void initialize_al_driver();

	void initialize_al_symbols();


	void initialize_al_alc_symbol_map() noexcept;

	void initialize_al_al_symbol_map() noexcept;

	void initialize_al_wrapper_entries() noexcept;


	void initialize_logger();


	MoveableMutexLock initialize_invalid_state();

	MoveableMutexLock initialize_not_initialized();

	MoveableMutexLock initialize_lock_mutex();

	MoveableMutexLock initialize_try_to_initialize();

	MoveableMutexLock initialize();


	Device* find_device(
		ALCdevice* al_device) noexcept;

	Device& get_device(
		ALCdevice* al_device);

	Device& get_current_device();

	Buffer& get_buffer(
		Device& device,
		ALuint al_buffer_name);

	Buffer& get_current_buffer(
		ALuint al_buffer_name);


	AlApiContext& get_context();

	AlApiContext& get_context(
		ALCcontext* al_context);

	void remove_context(
		const AlApiContext& context);


	void log_devices_info(
		ALCenum alc_token);

	void log_playback_devices();

	void log_capture_devices();

	void log_devices_info();


	void log_device_extensions(
		ALCdevice* al_device);

	void log_device_info(
		ALCdevice* al_device);


	void log_desired_context_attribute_list(
		const ALCint* al_attributes);
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
	logger_.flush();
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

ALboolean AL_APIENTRY AlApiImpl::eax_set_buffer_mode(
	ALsizei n,
	const ALuint* buffers,
	ALint value)
{
	if (n < 0)
	{
		fail("Buffer count out of range.");
	}

	const auto al_buffer_ids = make_span(buffers, n);

	if (al_buffer_ids.empty())
	{
		return AL_TRUE;
	}

	if (!al_buffer_ids.data())
	{
		fail("Null buffers.");
	}

	switch (value)
	{
		case x_ram_al_storage_automatic_enum:
		case x_ram_al_storage_hardware_enum:
		case x_ram_al_storage_accessible_enum:
			break;

		default:
			fail("Unsupported X-RAM mode.");
	}

	auto& device = get_current_device();

	for (const auto al_buffer_id : al_buffer_ids)
	{
		if (al_buffer_id == AL_NONE)
		{
			continue;
		}

		const auto& buffer = get_buffer(device, al_buffer_id);

		if (buffer.x_ram_is_dirty)
		{
			fail("Buffer has audio data.");
		}
	}

	for (const auto al_buffer_id : al_buffer_ids)
	{
		if (al_buffer_id == AL_NONE)
		{
			continue;
		}

		auto& buffer = get_buffer(device, al_buffer_id);
		buffer.x_ram_mode = value;
	}

	return AL_TRUE;
}

ALenum AL_APIENTRY AlApiImpl::eax_get_buffer_mode(
	ALuint buffer,
    ALint* pReserved)
{
	if (buffer == AL_NONE)
	{
		fail("Null AL buffer name.");
	}

	if (pReserved)
	{
		fail("Non-null reserved value.");
	}

	const auto& our_buffer = get_current_buffer(buffer);

	return our_buffer.x_ram_mode;
}

Eaxx* AlApiImpl::get_eaxx() const noexcept
{
	return eaxx_.get();
}

// ==========================================================================
// ALC v1.1

ALCcontext* ALC_APIENTRY AlApiImpl::alc_create_context(
	ALCdevice* device,
	const ALCint* attrlist) noexcept
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
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcCreateContext);
	return nullptr;
}

ALCboolean ALC_APIENTRY AlApiImpl::alc_make_context_current(
	ALCcontext* context) noexcept
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
			fail("AL failed to unset current context.");
		}
	}

	return ALC_TRUE;
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcMakeContextCurrent);
	return ALC_FALSE;
}

void ALC_APIENTRY AlApiImpl::alc_process_context(
	ALCcontext* context) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcProcessContext(context);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcProcessContext);
}

void ALC_APIENTRY AlApiImpl::alc_suspend_context(
	ALCcontext* context) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcSuspendContext(context);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcSuspendContext);
}

void ALC_APIENTRY AlApiImpl::alc_destroy_context(
	ALCcontext* context) noexcept
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
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcDestroyContext);
}

ALCcontext* ALC_APIENTRY AlApiImpl::alc_get_current_context() noexcept
try
{
	const auto mt_lock = initialize();

	if (!current_context_)
	{
		return nullptr;
	}

	return current_context_->get_al_context();
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcGetCurrentContext);
	return nullptr;
}

ALCdevice* ALC_APIENTRY AlApiImpl::alc_get_contexts_device(
	ALCcontext* context) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcGetContextsDevice(context);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcGetContextsDevice);
	return nullptr;
}

ALCdevice* ALC_APIENTRY AlApiImpl::alc_open_device(
	const ALCchar* devicename) noexcept
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

	auto special_device_name = String{};

	auto al_device = al_alc_symbols_->alcOpenDevice(devicename);

	if (!al_device && devicename)
	{
		constexpr auto generic_hardware_sv = std::string_view{"Generic Hardware"};
		constexpr auto generic_software_sv = std::string_view{"Generic Software"};
		constexpr auto direct_sound_3d_sv = std::string_view{"DirectSound3D"};

		const auto device_name_sv = std::string_view{devicename};

		auto is_special_name = false;

		if (false)
		{
		}
		else if (device_name_sv == generic_hardware_sv)
		{
			is_special_name = true;
		}
		else if (device_name_sv == generic_software_sv)
		{
			is_special_name = true;
		}
		else if (device_name_sv == direct_sound_3d_sv)
		{
			is_special_name = true;
		}

		if (is_special_name)
		{
			special_device_name.assign(device_name_sv.data(), device_name_sv.size());
		}

		al_device = al_alc_symbols_->alcOpenDevice(nullptr);
	}

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

	device.special_name = special_device_name;
	device.al_device = al_device;
	device.x_ram_free_size = x_ram_max_size;

	return al_device;
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcOpenDevice);
	return nullptr;
}

ALCboolean ALC_APIENTRY AlApiImpl::alc_close_device(
	ALCdevice* device) noexcept
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
		fail("AL failed to close the device.");
	}

	return al_result;
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcCloseDevice);
	return ALC_FALSE;
}

ALCenum ALC_APIENTRY AlApiImpl::alc_get_error(
	ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	const auto alc_result = al_alc_symbols_->alcGetError(device);

	return alc_result;
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcGetError);
	return ALC_INVALID_DEVICE;
}

ALCboolean ALC_APIENTRY AlApiImpl::alc_is_extension_present(
	ALCdevice* device,
	const ALCchar* extname) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcIsExtensionPresent(device, extname);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcIsExtensionPresent);
	return AL_FALSE;
}

void* ALC_APIENTRY AlApiImpl::alc_get_proc_address(
	ALCdevice* device,
	const ALCchar* funcname) noexcept
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
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcGetProcAddress);
	return nullptr;
}

ALCenum ALC_APIENTRY AlApiImpl::alc_get_enum_value(
	ALCdevice* device,
	const ALCchar* enumname) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcGetEnumValue(device, enumname);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcGetEnumValue);
	return AL_NONE;
}

const ALCchar* ALC_APIENTRY AlApiImpl::alc_get_string(
	ALCdevice* device,
	ALCenum param) noexcept
try
{
	const auto mt_lock = initialize();

	if (device && param == ALC_DEVICE_SPECIFIER)
	{
		const auto& our_device = get_device(device);

		if (!our_device.special_name.empty())
		{
			return our_device.special_name.c_str();
		}
	}

	return al_alc_symbols_->alcGetString(device, param);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcGetString);
	return nullptr;
}

void ALC_APIENTRY AlApiImpl::alc_get_integer_v(
	ALCdevice* device,
	ALCenum param,
	ALCsizei size,
	ALCint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcGetIntegerv(device, param, size, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcGetIntegerv);
}

ALCdevice* ALC_APIENTRY AlApiImpl::alc_capture_open_device(
	const ALCchar* devicename,
	ALCuint frequency,
	ALCenum format,
	ALCsizei buffersize) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcCaptureOpenDevice(devicename, frequency, format, buffersize);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcCaptureOpenDevice);
	return nullptr;
}

ALCboolean ALC_APIENTRY AlApiImpl::alc_capture_close_device(
	ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	return al_alc_symbols_->alcCaptureCloseDevice(device);;
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcCaptureCloseDevice);
	return ALC_FALSE;
}

void ALC_APIENTRY AlApiImpl::alc_capture_start(
	ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcCaptureStart(device);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcCaptureStart);
}

void ALC_APIENTRY AlApiImpl::alc_capture_stop(
	ALCdevice* device) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcCaptureStop(device);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcCaptureStop);
}

void ALC_APIENTRY AlApiImpl::alc_capture_samples(
	ALCdevice* device,
	ALCvoid* buffer,
	ALCsizei samples) noexcept
try
{
	const auto mt_lock = initialize();

	al_alc_symbols_->alcCaptureSamples(device, buffer, samples);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlcSymbolsNames::alcCaptureSamples);
}

// ALC v1.1
// ==========================================================================

// =========================================================================
// AL v1.1

void AL_APIENTRY AlApiImpl::alDopplerFactor(
	ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDopplerFactor(value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alDopplerFactor);
}

void AL_APIENTRY AlApiImpl::alDopplerVelocity(
	ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDopplerVelocity(value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alDopplerVelocity);
}

void AL_APIENTRY AlApiImpl::alSpeedOfSound(
	ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSpeedOfSound(value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSpeedOfSound);
}

void AL_APIENTRY AlApiImpl::alDistanceModel(
	ALenum distanceModel) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDistanceModel(distanceModel);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alDistanceModel);
}

void AL_APIENTRY AlApiImpl::alEnable(
	ALenum capability) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alEnable(capability);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alEnable);
}

void AL_APIENTRY AlApiImpl::alDisable(
	ALenum capability) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alDisable(capability);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alDisable);
}

ALboolean AL_APIENTRY AlApiImpl::alIsEnabled(
	ALenum capability) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alIsEnabled(capability);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alIsEnabled);
	return AL_FALSE;
}

const ALchar* AL_APIENTRY AlApiImpl::alGetString(
	ALenum param) noexcept
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
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetString);
	return nullptr;
}

void AL_APIENTRY AlApiImpl::alGetBooleanv(
	ALenum param,
	ALboolean* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBooleanv(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBooleanv);
}

void AL_APIENTRY AlApiImpl::alGetIntegerv(
	ALenum param,
	ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetIntegerv(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetIntegerv);
}

void AL_APIENTRY AlApiImpl::alGetFloatv(
	ALenum param,
	ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetFloatv(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetFloatv);
}

void AL_APIENTRY AlApiImpl::alGetDoublev(
	ALenum param,
	ALdouble* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetDoublev(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetDoublev);
}

ALboolean AL_APIENTRY AlApiImpl::alGetBoolean(
	ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetBoolean(param);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBoolean);
	return AL_FALSE;
}

ALint AL_APIENTRY AlApiImpl::alGetInteger(
	ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	switch (param)
	{
		case x_ram_ram_size_enum:
			return x_ram_max_size;

		case x_ram_ram_free_enum:
			{
				const auto& device = get_current_device();
				return device.x_ram_free_size;
			}

		default:
			return al_al_symbols_->alGetInteger(param);
	}
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetInteger);
	return 0;
}

ALfloat AL_APIENTRY AlApiImpl::alGetFloat(
	ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetFloat(param);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetFloat);
	return 0.0F;
}

ALdouble AL_APIENTRY AlApiImpl::alGetDouble(
	ALenum param) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alGetDouble(param);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetDouble);
	return 0.0;
}

ALenum AL_APIENTRY AlApiImpl::alGetError() noexcept
try
{
	const auto mt_lock = initialize();

	const auto al_result = al_al_symbols_->alGetError();
	return al_result;
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetError);
	return AL_INVALID_OPERATION;
}

ALboolean AL_APIENTRY AlApiImpl::alIsExtensionPresent(
	const ALchar* extname) noexcept
try
{
	const auto mt_lock = initialize();

	if (current_context_)
	{
		if (extname && extname[0] != '\0')
		{
			constexpr auto x_ram_view = std::string_view{"EAX-RAM"};

			const auto extname_view = std::string_view{extname};

			if (extname_view == x_ram_view)
			{
				return ALC_TRUE;
			}
		}

		const auto is_present = current_context_->al_is_extension_present(extname);

		if (is_present)
		{
			return true;
		}
	}

	return al_al_symbols_->alIsExtensionPresent(extname);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alIsExtensionPresent);
	return AL_FALSE;
}

void* AL_APIENTRY AlApiImpl::alGetProcAddress(
	const ALchar* fname) noexcept
try
{
	if (!fname || (*fname) == '\0')
	{
		return nullptr;
	}

	const auto mt_lock = initialize();

	constexpr auto x_ram_eax_set_buffer_mode_view = std::string_view{AlXRamSymbolsNames::EAXSetBufferMode};
	constexpr auto x_ram_eax_get_buffer_mode_view = std::string_view{AlXRamSymbolsNames::EAXGetBufferMode};

	const auto symbol_name = std::string_view{fname};

	if (false)
	{
	}
	else if (symbol_name == x_ram_eax_set_buffer_mode_view)
	{
		return reinterpret_cast<void*>(eaxefx::EAXSetBufferMode);
	}
	else if (symbol_name == x_ram_eax_get_buffer_mode_view)
	{
		return reinterpret_cast<void*>(eaxefx::EAXGetBufferMode);
	}

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
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetProcAddress);
	return nullptr;
}

ALenum AL_APIENTRY AlApiImpl::alGetEnumValue(
	const ALchar* ename) noexcept
try
{
	if (!ename || ename[0] == '\0')
	{
		fail("Null or empty enum name.");
	}

	constexpr auto x_ram_ram_size_view = std::string_view{"AL_EAX_RAM_SIZE"};
	constexpr auto x_ram_ram_free_view = std::string_view{"AL_EAX_RAM_FREE"};
	constexpr auto x_ram_al_storage_automatic_view = std::string_view{"AL_STORAGE_AUTOMATIC"};
	constexpr auto x_ram_al_storage_hardware_view = std::string_view{"AL_STORAGE_HARDWARE"};
	constexpr auto x_ram_al_storage_accessible_view = std::string_view{"AL_STORAGE_ACCESSIBLE"};

	const auto mt_lock = initialize();

	const auto ename_view = std::string_view{ename};

	if (false)
	{
	}
	else if (ename_view == x_ram_ram_size_view)
	{
		return x_ram_ram_size_enum;
	}
	else if (ename_view == x_ram_ram_free_view)
	{
		return x_ram_ram_free_enum;
	}
	else if (ename_view == x_ram_al_storage_automatic_view)
	{
		return x_ram_al_storage_automatic_enum;
	}
	else if (ename_view == x_ram_al_storage_hardware_view)
	{
		return x_ram_al_storage_hardware_enum;
	}
	else if (ename_view == x_ram_al_storage_accessible_view)
	{
		return x_ram_al_storage_accessible_enum;
	}
	else
	{
		return al_al_symbols_->alGetEnumValue(ename);
	}
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetEnumValue);
	return 0;
}

void AL_APIENTRY AlApiImpl::alListenerf(
	ALenum param,
	ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListenerf(param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alListenerf);
}

void AL_APIENTRY AlApiImpl::alListener3f(
	ALenum param,
	ALfloat value1,
	ALfloat value2,
	ALfloat value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListener3f(param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alListener3f);
}

void AL_APIENTRY AlApiImpl::alListenerfv(
	ALenum param,
	const ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListenerfv(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alListenerfv);
}

void AL_APIENTRY AlApiImpl::alListeneri(
	ALenum param,
	ALint value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListeneri(param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alListeneri);
}

void AL_APIENTRY AlApiImpl::alListener3i(
	ALenum param,
	ALint value1,
	ALint value2,
	ALint value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListener3i(param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alListener3i);
}

void AL_APIENTRY AlApiImpl::alListeneriv(
	ALenum param,
	const ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alListeneriv(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alListeneriv);
}

void AL_APIENTRY AlApiImpl::alGetListenerf(
	ALenum param,
	ALfloat* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListenerf(param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetListenerf);
}

void AL_APIENTRY AlApiImpl::alGetListener3f(
	ALenum param,
	ALfloat* value1,
	ALfloat* value2,
	ALfloat* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListener3f(param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetListener3f);
}

void AL_APIENTRY AlApiImpl::alGetListenerfv(
	ALenum param,
	ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListenerfv(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetListenerfv);
}

void AL_APIENTRY AlApiImpl::alGetListeneri(
	ALenum param,
	ALint* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListeneri(param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetListeneri);
}

void AL_APIENTRY AlApiImpl::alGetListener3i(
	ALenum param,
	ALint* value1,
	ALint* value2,
	ALint* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListener3i(param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetListener3i);
}

void AL_APIENTRY AlApiImpl::alGetListeneriv(
	ALenum param,
	ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetListeneriv(param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetListeneriv);
}

void AL_APIENTRY AlApiImpl::alGenSources(
	ALsizei n,
	ALuint* sources) noexcept
try
{
	const auto al_source_ids = make_span(sources, n);

	if (al_source_ids.empty())
	{
		return;
	}

	const auto mt_lock = initialize();

	static_cast<void>(al_al_symbols_->alGetError());
	al_al_symbols_->alGenSources(n, sources);
	const auto al_result = al_al_symbols_->alGetError();

	if (al_result != AL_NO_ERROR)
	{
		al_al_symbols_->alGenSources(n, sources);
		return;
	}

	auto& context = get_current_context();
	context.al_gen_sources(al_source_ids);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGenSources);
}

void AL_APIENTRY AlApiImpl::alDeleteSources(
	ALsizei n,
	const ALuint* sources) noexcept
try
{
	const auto al_source_ids = make_span(sources, n);

	if (al_source_ids.empty())
	{
		return;
	}

	const auto mt_lock = initialize();

	static_cast<void>(al_al_symbols_->alGetError());
	al_al_symbols_->alDeleteSources(n, sources);
	const auto al_result = al_al_symbols_->alGetError();

	if (al_result != AL_NO_ERROR)
	{
		al_al_symbols_->alDeleteSources(n, sources);
		return;
	}

	auto& context = get_current_context();
	context.al_delete_sources(al_source_ids);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alDeleteSources);
}

ALboolean AL_APIENTRY AlApiImpl::alIsSource(
	ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alIsSource(source);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alIsSource);
	return AL_FALSE;
}

void AL_APIENTRY AlApiImpl::alSourcef(
	ALuint source,
	ALenum param,
	ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcef(source, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourcef);
}

void AL_APIENTRY AlApiImpl::alSource3f(
	ALuint source,
	ALenum param,
	ALfloat value1,
	ALfloat value2,
	ALfloat value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSource3f(source, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSource3f);
}

void AL_APIENTRY AlApiImpl::alSourcefv(
	ALuint source,
	ALenum param,
	const ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcefv(source, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourcefv);
}

void AL_APIENTRY AlApiImpl::alSourcei(
	ALuint source,
	ALenum param,
	ALint value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcei(source, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourcei);
}

void AL_APIENTRY AlApiImpl::alSource3i(
	ALuint source,
	ALenum param,
	ALint value1,
	ALint value2,
	ALint value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSource3i(source, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSource3i);
}

void AL_APIENTRY AlApiImpl::alSourceiv(
	ALuint source,
	ALenum param,
	const ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceiv(source, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourceiv);
}

void AL_APIENTRY AlApiImpl::alGetSourcef(
	ALuint source,
	ALenum param,
	ALfloat* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourcef(source, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetSourcef);
}

void AL_APIENTRY AlApiImpl::alGetSource3f(
	ALuint source,
	ALenum param,
	ALfloat* value1,
	ALfloat* value2,
	ALfloat* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSource3f(source, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetSource3f);
}

void AL_APIENTRY AlApiImpl::alGetSourcefv(
	ALuint source,
	ALenum param,
	ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourcefv(source, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetSourcefv);
}

void AL_APIENTRY AlApiImpl::alGetSourcei(
	ALuint source,
	ALenum param,
	ALint* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourcei(source, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetSourcei);
}

void AL_APIENTRY AlApiImpl::alGetSource3i(
	ALuint source,
	ALenum param,
	ALint* value1,
	ALint* value2,
	ALint* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSource3i(source, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetSource3i);
}

void AL_APIENTRY AlApiImpl::alGetSourceiv(
	ALuint source,
	ALenum param,
	ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetSourceiv(source, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetSourceiv);
}

void AL_APIENTRY AlApiImpl::alSourcePlayv(
	ALsizei n,
	const ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePlayv(n, sources);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourcePlayv);
}

void AL_APIENTRY AlApiImpl::alSourceStopv(
	ALsizei n,
	const ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceStopv(n, sources);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourceStopv);
}

void AL_APIENTRY AlApiImpl::alSourceRewindv(
	ALsizei n,
	const ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceRewindv(n, sources);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourceRewindv);
}

void AL_APIENTRY AlApiImpl::alSourcePausev(
	ALsizei n,
	const ALuint* sources) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePausev(n, sources);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourcePausev);
}

void AL_APIENTRY AlApiImpl::alSourcePlay(
	ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePlay(source);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourcePlay);
}

void AL_APIENTRY AlApiImpl::alSourceStop(
	ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceStop(source);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourceStop);
}

void AL_APIENTRY AlApiImpl::alSourceRewind(
	ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceRewind(source);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourceRewind);
}

void AL_APIENTRY AlApiImpl::alSourcePause(
	ALuint source) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourcePause(source);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourcePause);
}

void AL_APIENTRY AlApiImpl::alSourceQueueBuffers(
	ALuint source,
	ALsizei nb,
	const ALuint* buffers) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceQueueBuffers(source, nb, buffers);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourceQueueBuffers);
}

void AL_APIENTRY AlApiImpl::alSourceUnqueueBuffers(
	ALuint source,
	ALsizei nb,
	ALuint* buffers) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alSourceUnqueueBuffers(source, nb, buffers);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alSourceUnqueueBuffers);
}

void AL_APIENTRY AlApiImpl::alGenBuffers(
	ALsizei n,
	ALuint* buffers) noexcept
try
{
	const auto al_buffer_ids = make_span(buffers, n);

	if (al_buffer_ids.empty())
	{
		return;
	}

	const auto mt_lock = initialize();

	static_cast<void>(al_al_symbols_->alGetError());
	al_al_symbols_->alGenBuffers(n, buffers);
	const auto al_result = al_al_symbols_->alGetError();

	if (al_result != AL_NO_ERROR)
	{
		al_al_symbols_->alGenBuffers(n, buffers);
		return;
	}

	auto& device = get_current_device();
	auto& our_buffers = device.buffers;

	auto our_buffer = Buffer{};
	our_buffer.x_ram_mode = x_ram_al_storage_automatic_enum;

	for (const auto al_buffer_id : al_buffer_ids)
	{
		our_buffers.emplace(al_buffer_id, our_buffer);
	}
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGenBuffers);
}

void AL_APIENTRY AlApiImpl::alDeleteBuffers(
	ALsizei n,
	const ALuint* buffers) noexcept
try
{
	const auto al_buffer_ids = make_span(buffers, n);

	if (al_buffer_ids.empty())
	{
		return;
	}

	const auto mt_lock = initialize();

	static_cast<void>(al_al_symbols_->alGetError());
	al_al_symbols_->alDeleteBuffers(n, buffers);
	const auto al_result = al_al_symbols_->alGetError();

	if (al_result != AL_NO_ERROR)
	{
		al_al_symbols_->alDeleteBuffers(n, buffers);
		return;
	}

	auto& device = get_current_device();
	auto& our_buffers = device.buffers;

	for (const auto al_buffer_id : al_buffer_ids)
	{
		if (al_buffer_id == AL_NONE)
		{
			continue;
		}

		const auto buffer_it = our_buffers.find(al_buffer_id);

		if (buffer_it == our_buffers.cend())
		{
			fail("Unregistered buffer.");
		}

		const auto& our_buffer = buffer_it->second;

		if (our_buffer.x_ram_is_hardware)
		{
			device.x_ram_free_size += our_buffer.size;
			assert(device.x_ram_free_size >= 0 && device.x_ram_free_size <= x_ram_max_size);
		}

		our_buffers.erase(buffer_it);
	}
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alDeleteBuffers);
}

ALboolean AL_APIENTRY AlApiImpl::alIsBuffer(
	ALuint buffer) noexcept
try
{
	const auto mt_lock = initialize();

	return al_al_symbols_->alIsBuffer(buffer);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alIsBuffer);
	return AL_FALSE;
}

void AL_APIENTRY AlApiImpl::alBufferData(
	ALuint buffer,
	ALenum format,
	const ALvoid* data,
	ALsizei size,
	ALsizei freq) noexcept
try
{
	const auto mt_lock = initialize();

	auto& device = get_current_device();
	auto& our_buffer = get_buffer(device, buffer);


	// At-first, prepare information.
	//
	auto x_ram_is_hardware = our_buffer.x_ram_is_hardware;
	auto x_ram_size_delta = decltype(size){};

	switch (our_buffer.x_ram_mode)
	{
		case x_ram_al_storage_automatic_enum:
			if (!our_buffer.x_ram_is_dirty)
			{
				// Never used before.

				if (device.x_ram_free_size >= size)
				{
					// Have enough X-RAM memory.

					x_ram_is_hardware = true;
					x_ram_size_delta = -size;
				}
			}
			else
			{
				// Used at least once.
				// From now on, use only system memory.

				x_ram_is_hardware = false;

				if (our_buffer.x_ram_is_hardware)
				{
					// First allocated was in X-RAM.
					// Free that block.

					x_ram_size_delta = size;
				}
			}

			break;

		case x_ram_al_storage_hardware_enum:
			if (device.x_ram_free_size >= size)
			{
				// Have enough X-RAM memory.

				x_ram_is_hardware = true;
				x_ram_size_delta = our_buffer.size - size;
			}
			else
			{
				// No free X-RAM memory - no buffer.

				static_cast<void>(al_al_symbols_->alGetInteger(AL_NONE));
				fail("Out of memory.");
			}
			break;

		case x_ram_al_storage_accessible_enum:
			// Always use system memory.
			x_ram_is_hardware = false;
			break;

		default:
			static_cast<void>(al_al_symbols_->alGetInteger(AL_NONE));
			fail("Unknown X-RAM buffer mode.");
	}


	// At-second, allocate the data.
	//
	static_cast<void>(al_al_symbols_->alGetError());
	al_al_symbols_->alBufferData(buffer, format, data, size, freq);
	const auto al_result = al_al_symbols_->alGetError();

	if (al_result != AL_NO_ERROR)
	{
		al_al_symbols_->alBufferData(buffer, format, data, size, freq);
		return;
	}


	// At-third, commit the changes.
	//
	our_buffer.size = size;
	our_buffer.x_ram_is_hardware = x_ram_is_hardware;
	our_buffer.x_ram_is_dirty = true;

	device.x_ram_free_size += x_ram_size_delta;

	assert(device.x_ram_free_size >= 0 && device.x_ram_free_size <= x_ram_max_size);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alBufferData);
}

void AL_APIENTRY AlApiImpl::alBufferf(
	ALuint buffer,
	ALenum param,
	ALfloat value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferf(buffer, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alBufferf);
}

void AL_APIENTRY AlApiImpl::alBuffer3f(
	ALuint buffer,
	ALenum param,
	ALfloat value1,
	ALfloat value2,
	ALfloat value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBuffer3f(buffer, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alBuffer3f);
}

void AL_APIENTRY AlApiImpl::alBufferfv(
	ALuint buffer,
	ALenum param,
	const ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferfv(buffer, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alBufferfv);
}

void AL_APIENTRY AlApiImpl::alBufferi(
	ALuint buffer,
	ALenum param,
	ALint value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferi(buffer, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alBufferi);
}

void AL_APIENTRY AlApiImpl::alBuffer3i(
	ALuint buffer,
	ALenum param,
	ALint value1,
	ALint value2,
	ALint value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBuffer3i(buffer, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alBuffer3i);
}

void AL_APIENTRY AlApiImpl::alBufferiv(
	ALuint buffer,
	ALenum param,
	const ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alBufferiv(buffer, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alBufferiv);
}

void AL_APIENTRY AlApiImpl::alGetBufferf(
	ALuint buffer,
	ALenum param,
	ALfloat* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferf(buffer, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBufferf);
}

void AL_APIENTRY AlApiImpl::alGetBuffer3f(
	ALuint buffer,
	ALenum param,
	ALfloat* value1,
	ALfloat* value2,
	ALfloat* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBuffer3f(buffer, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBuffer3f);
}

void AL_APIENTRY AlApiImpl::alGetBufferfv(
	ALuint buffer,
	ALenum param,
	ALfloat* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferfv(buffer, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBufferfv);
}

void AL_APIENTRY AlApiImpl::alGetBufferi(
	ALuint buffer,
	ALenum param,
	ALint* value) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferi(buffer, param, value);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBufferi);
}

void AL_APIENTRY AlApiImpl::alGetBuffer3i(
	ALuint buffer,
	ALenum param,
	ALint* value1,
	ALint* value2,
	ALint* value3) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBuffer3i(buffer, param, value1, value2, value3);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBuffer3i);
}

void AL_APIENTRY AlApiImpl::alGetBufferiv(
	ALuint buffer,
	ALenum param,
	ALint* values) noexcept
try
{
	const auto mt_lock = initialize();

	al_al_symbols_->alGetBufferiv(buffer, param, values);
}
catch (...)
{
	utils::log_exception(&logger_, AlAlSymbolsNames::alGetBufferiv);
}

// AL v1.1
// =========================================================================

[[noreturn]]
void AlApiImpl::fail(
	const char* message)
{
	throw AlApiException{message};
}

void AlApiImpl::apply_patch_collection() noexcept
try
{
	logger_.info("");
	logger_.info("Apply patches.");

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
catch (...)
{
	utils::log_exception(&logger_);
}

void AlApiImpl::initialize_logger()
{
	constexpr auto log_file_name = "eaxefx_log.txt";

	auto logger_param = LoggerParam{};
	logger_param.file_path = log_file_name;
	logger_.make(logger_param);

	if (!logger_.has_file())
	{
		try
		{
			auto log_path = env::get_special_folder(env::SpecialFolderType::app_data);

			log_path /= "bibendovsky";
			fs::create_directory(log_path.get_data());

			log_path /= "eaxefx";
			fs::create_directory(log_path.get_data());

			log_path /= log_file_name;

			logger_param.file_path = log_path.get_data();

			logger_.make(logger_param);
		}
		catch (...)
		{
		}
	}

	logger_.info("");
	logger_.info("<<<<<<<<<<<<<<<<<<<<<<<<");
	logger_.info("EAXEFX v" EAXEFX_VERSION);
	logger_.info("<<<<<<<<<<<<<<<<<<<<<<<<");
	logger_.info("");
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
		catch (...)
		{
			utils::log_exception(&logger_);
		}
	}

	fail("Failed to load any suitable driver.");
}

void AlApiImpl::initialize_al_symbols()
{
	logger_.info("Create AL symbol loader.");
	al_loader_ = make_al_loader(al_library_.get());

	logger_.info("Load ALC v1.1 symbols.");
	al_alc_symbols_ = al_loader_->resolve_alc_symbols();

	logger_.info("Load AL v1.1 symbols.");
	al_al_symbols_ = al_loader_->resolve_al_symbols();
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
	fail(ErrorMessages::invalid_state);
}

MoveableMutexLock AlApiImpl::initialize_not_initialized()
{
	fail(ErrorMessages::not_initialized);
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


	initialize_logger();
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
	ALCdevice* al_device) noexcept
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
	ALCdevice* al_device)
{
	auto device = find_device(al_device);

	if (!device)
	{
		fail("Device not found.");
	}

	return *device;
}

AlApiImpl::Device& AlApiImpl::get_current_device()
{
	if (!current_context_)
	{
		fail("No current context.");
	}

	for (auto& device : devices_)
	{
		for (auto& context : device.contexts)
		{
			if (context.get() == current_context_)
			{
				return device;
			}
		}
	}

	fail("Unregistered device.");
}

AlApiImpl::Buffer& AlApiImpl::get_buffer(
	Device& device,
	ALuint al_buffer_name)
{
	auto& buffers = device.buffers;
	const auto buffer_it = buffers.find(al_buffer_name);

	if (buffer_it == buffers.cend())
	{
		fail("Unregistered buffer.");
	}

	return buffer_it->second;
}

AlApiImpl::Buffer& AlApiImpl::get_current_buffer(
	ALuint al_buffer_name)
{
	auto& device = get_current_device();
	return get_buffer(device, al_buffer_name);
}

AlApiContext& AlApiImpl::get_context()
{
	if (!current_context_)
	{
		fail("No current context.");
	}

	return *current_context_;
}

AlApiContext& AlApiImpl::get_context(
	ALCcontext* al_context)
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

	fail("Unregistered context.");
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
		fail("Unregistered context.");
	}
}

void AlApiImpl::log_devices_info(
	ALCenum alc_token)
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
	ALCdevice* al_device)
{
	const auto al_device_extensions = al_alc_symbols_->alcGetString(al_device, ALC_EXTENSIONS);

	logger_.info("");
	logger_.info("Extensions:");
	al_api::log_extensions(&logger_, al_device_extensions);
}

void AlApiImpl::log_device_info(
	ALCdevice* al_device)
{
	log_device_extensions(al_device);
}

void AlApiImpl::log_desired_context_attribute_list(
	const ALCint* al_attributes)
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

ALC_API ALCcontext* ALC_APIENTRY alcCreateContext(
	ALCdevice* device,
	const ALCint* attrlist)
{
	return eaxefx::g_al_api_impl.alc_create_context(device, attrlist);
}

ALC_API ALCboolean ALC_APIENTRY alcMakeContextCurrent(
	ALCcontext* context)
{
	return eaxefx::g_al_api_impl.alc_make_context_current(context);
}

ALC_API void ALC_APIENTRY alcProcessContext(
	ALCcontext* context)
{
	eaxefx::g_al_api_impl.alc_process_context(context);
}

ALC_API void ALC_APIENTRY alcSuspendContext(
	ALCcontext* context)
{
	eaxefx::g_al_api_impl.alc_suspend_context(context);
}

ALC_API void ALC_APIENTRY alcDestroyContext(
	ALCcontext* context)
{
	eaxefx::g_al_api_impl.alc_destroy_context(context);
}

ALC_API ALCcontext* ALC_APIENTRY alcGetCurrentContext()
{
	return eaxefx::g_al_api_impl.alc_get_current_context();
}

ALC_API ALCdevice* ALC_APIENTRY alcGetContextsDevice(
	ALCcontext* context)
{
	return eaxefx::g_al_api_impl.alc_get_contexts_device(context);
}

ALC_API ALCdevice* ALC_APIENTRY alcOpenDevice(
	const ALCchar* devicename)
{
	return eaxefx::g_al_api_impl.alc_open_device(devicename);
}

ALC_API ALCboolean ALC_APIENTRY alcCloseDevice(
	ALCdevice* device)
{
	return eaxefx::g_al_api_impl.alc_close_device(device);
}

ALC_API ALCenum ALC_APIENTRY alcGetError(
	ALCdevice* device)
{
	return eaxefx::g_al_api_impl.alc_get_error(device);
}

ALC_API ALCboolean ALC_APIENTRY alcIsExtensionPresent(
	ALCdevice* device,
	const ALCchar* extname)
{
	return eaxefx::g_al_api_impl.alc_is_extension_present(device, extname);
}

ALC_API void* ALC_APIENTRY alcGetProcAddress(
	ALCdevice* device,
	const ALCchar* funcname)
{
	return eaxefx::g_al_api_impl.alc_get_proc_address(device, funcname);
}

ALC_API ALCenum ALC_APIENTRY alcGetEnumValue(
	ALCdevice* device,
	const ALCchar* enumname)
{
	return eaxefx::g_al_api_impl.alc_get_enum_value(device, enumname);
}

ALC_API const ALCchar* ALC_APIENTRY alcGetString(
	ALCdevice* device,
	ALCenum param)
{
	return eaxefx::g_al_api_impl.alc_get_string(device, param);
}

ALC_API void ALC_APIENTRY alcGetIntegerv(
	ALCdevice* device,
	ALCenum param,
	ALCsizei size,
	ALCint* values)
{
	eaxefx::g_al_api_impl.alc_get_integer_v(device, param, size, values);
}

ALC_API ALCdevice* ALC_APIENTRY alcCaptureOpenDevice(
	const ALCchar* devicename,
	ALCuint frequency,
	ALCenum format,
	ALCsizei buffersize)
{
	return eaxefx::g_al_api_impl.alc_capture_open_device(devicename, frequency, format, buffersize);
}

ALC_API ALCboolean ALC_APIENTRY alcCaptureCloseDevice(
	ALCdevice* device)
{
	return eaxefx::g_al_api_impl.alc_capture_close_device(device);
}

ALC_API void ALC_APIENTRY alcCaptureStart(
	ALCdevice* device)
{
	eaxefx::g_al_api_impl.alc_capture_start(device);
}

ALC_API void ALC_APIENTRY alcCaptureStop(ALCdevice* device)
{
	eaxefx::g_al_api_impl.alc_capture_stop(device);
}

ALC_API void ALC_APIENTRY alcCaptureSamples(
	ALCdevice* device,
	ALCvoid* buffer,
	ALCsizei samples)
{
	eaxefx::g_al_api_impl.alc_capture_samples(device, buffer, samples);
}

// ALC v1.1
// ==========================================================================


// ==========================================================================
// AL v1.1

AL_API void AL_APIENTRY alDopplerFactor(
	ALfloat value)
{
	eaxefx::g_al_api_impl.alDopplerFactor(value);
}

AL_API void AL_APIENTRY alDopplerVelocity(
	ALfloat value)
{
	eaxefx::g_al_api_impl.alDopplerVelocity(value);
}

AL_API void AL_APIENTRY alSpeedOfSound(
	ALfloat value)
{
	eaxefx::g_al_api_impl.alSpeedOfSound(value);
}

AL_API void AL_APIENTRY alDistanceModel(
	ALenum distanceModel)
{
	eaxefx::g_al_api_impl.alDistanceModel(distanceModel);
}

AL_API void AL_APIENTRY alEnable(
	ALenum capability)
{
	eaxefx::g_al_api_impl.alEnable(capability);
}

AL_API void AL_APIENTRY alDisable(
	ALenum capability)
{
	eaxefx::g_al_api_impl.alDisable(capability);
}

AL_API ALboolean AL_APIENTRY alIsEnabled(
	ALenum capability)
{
	return eaxefx::g_al_api_impl.alIsEnabled(capability);
}

AL_API const ALchar* AL_APIENTRY alGetString(
	ALenum param)
{
	return eaxefx::g_al_api_impl.alGetString(param);
}

AL_API void AL_APIENTRY alGetBooleanv(
	ALenum param,
	ALboolean* values)
{
	eaxefx::g_al_api_impl.alGetBooleanv(param, values);
}

AL_API void AL_APIENTRY alGetIntegerv(
	ALenum param,
	ALint* values)
{
	eaxefx::g_al_api_impl.alGetIntegerv(param, values);
}

AL_API void AL_APIENTRY alGetFloatv(
	ALenum param,
	ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetFloatv(param, values);
}

AL_API void AL_APIENTRY alGetDoublev(
	ALenum param,
	ALdouble* values)
{
	eaxefx::g_al_api_impl.alGetDoublev(param, values);
}

AL_API ALboolean AL_APIENTRY alGetBoolean(
	ALenum param)
{
	return eaxefx::g_al_api_impl.alGetBoolean(param);
}

AL_API ALint AL_APIENTRY alGetInteger(
	ALenum param)
{
	return eaxefx::g_al_api_impl.alGetInteger(param);
}

AL_API ALfloat AL_APIENTRY alGetFloat(
	ALenum param)
{
	return eaxefx::g_al_api_impl.alGetFloat(param);
}

AL_API ALdouble AL_APIENTRY alGetDouble(
	ALenum param)
{
	return eaxefx::g_al_api_impl.alGetDouble(param);
}

AL_API ALenum AL_APIENTRY alGetError()
{
	return eaxefx::g_al_api_impl.alGetError();
}

AL_API ALboolean AL_APIENTRY alIsExtensionPresent(
	const ALchar* extname)
{
	return eaxefx::g_al_api_impl.alIsExtensionPresent(extname);
}

AL_API void* AL_APIENTRY alGetProcAddress(
	const ALchar* fname)
{
	return eaxefx::g_al_api_impl.alGetProcAddress(fname);
}

AL_API ALenum AL_APIENTRY alGetEnumValue(
	const ALchar* ename)
{
	return eaxefx::g_al_api_impl.alGetEnumValue(ename);
}

AL_API void AL_APIENTRY alListenerf(
	ALenum param,
	ALfloat value)
{
	eaxefx::g_al_api_impl.alListenerf(param, value);
}

AL_API void AL_APIENTRY alListener3f(
	ALenum param,
	ALfloat value1,
	ALfloat value2,
	ALfloat value3)
{
	eaxefx::g_al_api_impl.alListener3f(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alListenerfv(
	ALenum param,
	const ALfloat* values)
{
	eaxefx::g_al_api_impl.alListenerfv(param, values);
}

AL_API void AL_APIENTRY alListeneri(
	ALenum param,
	ALint value)
{
	eaxefx::g_al_api_impl.alListeneri(param, value);
}

AL_API void AL_APIENTRY alListener3i(
	ALenum param,
	ALint value1,
	ALint value2,
	ALint value3)
{
	eaxefx::g_al_api_impl.alListener3i(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alListeneriv(
	ALenum param,
	const ALint* values)
{
	eaxefx::g_al_api_impl.alListeneriv(param, values);
}

AL_API void AL_APIENTRY alGetListenerf(
	ALenum param,
	ALfloat* value)
{
	eaxefx::g_al_api_impl.alGetListenerf(param, value);
}

AL_API void AL_APIENTRY alGetListener3f(
	ALenum param,
	ALfloat* value1,
	ALfloat* value2,
	ALfloat* value3)
{
	eaxefx::g_al_api_impl.alGetListener3f(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetListenerfv(
	ALenum param,
	ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetListenerfv(param, values);
}

AL_API void AL_APIENTRY alGetListeneri(
	ALenum param,
	ALint* value)
{
	eaxefx::g_al_api_impl.alGetListeneri(param, value);
}

AL_API void AL_APIENTRY alGetListener3i(
	ALenum param,
	ALint* value1,
	ALint* value2,
	ALint* value3)
{
	eaxefx::g_al_api_impl.alGetListener3i(param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetListeneriv(
	ALenum param,
	ALint* values)
{
	eaxefx::g_al_api_impl.alGetListeneriv(param, values);
}

AL_API void AL_APIENTRY alGenSources(
	ALsizei n,
	ALuint* sources)
{
	eaxefx::g_al_api_impl.alGenSources(n, sources);
}

AL_API void AL_APIENTRY alDeleteSources(
	ALsizei n,
	const ALuint* sources)
{
	eaxefx::g_al_api_impl.alDeleteSources(n, sources);
}

AL_API ALboolean AL_APIENTRY alIsSource(
	ALuint source)
{
	return eaxefx::g_al_api_impl.alIsSource(source);
}

AL_API void AL_APIENTRY alSourcef(
	ALuint source,
	ALenum param,
	ALfloat value)
{
	eaxefx::g_al_api_impl.alSourcef(source, param, value);
}

AL_API void AL_APIENTRY alSource3f(
	ALuint source,
	ALenum param,
	ALfloat value1,
	ALfloat value2,
	ALfloat value3)
{
	eaxefx::g_al_api_impl.alSource3f(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourcefv(
	ALuint source,
	ALenum param,
	const ALfloat* values)
{
	eaxefx::g_al_api_impl.alSourcefv(source, param, values);
}

AL_API void AL_APIENTRY alSourcei(
	ALuint source,
	ALenum param,
	ALint value)
{
	eaxefx::g_al_api_impl.alSourcei(source, param, value);
}

AL_API void AL_APIENTRY alSource3i(
	ALuint source,
	ALenum param,
	ALint value1,
	ALint value2,
	ALint value3)
{
	eaxefx::g_al_api_impl.alSource3i(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alSourceiv(
	ALuint source,
	ALenum param,
	const ALint* values)
{
	eaxefx::g_al_api_impl.alSourceiv(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcef(
	ALuint source,
	ALenum param,
	ALfloat* value)
{
	eaxefx::g_al_api_impl.alGetSourcef(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3f(
	ALuint source,
	ALenum param,
	ALfloat* value1,
	ALfloat* value2,
	ALfloat* value3)
{
	eaxefx::g_al_api_impl.alGetSource3f(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourcefv(
	ALuint source,
	ALenum param,
	ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetSourcefv(source, param, values);
}

AL_API void AL_APIENTRY alGetSourcei(
	ALuint source,
	ALenum param,
	ALint* value)
{
	eaxefx::g_al_api_impl.alGetSourcei(source, param, value);
}

AL_API void AL_APIENTRY alGetSource3i(
	ALuint source,
	ALenum param,
	ALint* value1,
	ALint* value2,
	ALint* value3)
{
	eaxefx::g_al_api_impl.alGetSource3i(source, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetSourceiv(
	ALuint source,
	ALenum param,
	ALint* values)
{
	eaxefx::g_al_api_impl.alGetSourceiv(source, param, values);
}

AL_API void AL_APIENTRY alSourcePlayv(
	ALsizei n,
	const ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourcePlayv(n, sources);
}

AL_API void AL_APIENTRY alSourceStopv(
	ALsizei n,
	const ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourceStopv(n, sources);
}

AL_API void AL_APIENTRY alSourceRewindv(
	ALsizei n,
	const ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourceRewindv(n, sources);
}

AL_API void AL_APIENTRY alSourcePausev(
	ALsizei n,
	const ALuint* sources)
{
	eaxefx::g_al_api_impl.alSourcePausev(n, sources);
}

AL_API void AL_APIENTRY alSourcePlay(
	ALuint source)
{
	eaxefx::g_al_api_impl.alSourcePlay(source);
}

AL_API void AL_APIENTRY alSourceStop(
	ALuint source)
{
	eaxefx::g_al_api_impl.alSourceStop(source);
}

AL_API void AL_APIENTRY alSourceRewind(
	ALuint source)
{
	eaxefx::g_al_api_impl.alSourceRewind(source);
}

AL_API void AL_APIENTRY alSourcePause(
	ALuint source)
{
	eaxefx::g_al_api_impl.alSourcePause(source);
}

AL_API void AL_APIENTRY alSourceQueueBuffers(
	ALuint source,
	ALsizei nb,
	const ALuint* buffers)
{
	eaxefx::g_al_api_impl.alSourceQueueBuffers(source, nb, buffers);
}

AL_API void AL_APIENTRY alSourceUnqueueBuffers(
	ALuint source,
	ALsizei nb,
	ALuint* buffers)
{
	eaxefx::g_al_api_impl.alSourceUnqueueBuffers(source, nb, buffers);
}

AL_API void AL_APIENTRY alGenBuffers(
	ALsizei n,
	ALuint* buffers)
{
	eaxefx::g_al_api_impl.alGenBuffers(n, buffers);
}

AL_API void AL_APIENTRY alDeleteBuffers(
	ALsizei n,
	const ALuint* buffers)
{
	eaxefx::g_al_api_impl.alDeleteBuffers(n, buffers);
}

AL_API ALboolean AL_APIENTRY alIsBuffer(
	ALuint buffer)
{
	return eaxefx::g_al_api_impl.alIsBuffer(buffer);
}

AL_API void AL_APIENTRY alBufferData(
	ALuint buffer,
	ALenum format,
	const ALvoid* data,
	ALsizei size,
	ALsizei freq)
{
	eaxefx::g_al_api_impl.alBufferData(buffer, format, data, size, freq);
}

AL_API void AL_APIENTRY alBufferf(
	ALuint buffer,
	ALenum param,
	ALfloat value)
{
	eaxefx::g_al_api_impl.alBufferf(buffer, param, value);
}

AL_API void AL_APIENTRY alBuffer3f(
	ALuint buffer,
	ALenum param,
	ALfloat value1,
	ALfloat value2,
	ALfloat value3)
{
	eaxefx::g_al_api_impl.alBuffer3f(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alBufferfv(
	ALuint buffer,
	ALenum param,
	const ALfloat* values)
{
	eaxefx::g_al_api_impl.alBufferfv(buffer, param, values);
}

AL_API void AL_APIENTRY alBufferi(
	ALuint buffer,
	ALenum param,
	ALint value)
{
	eaxefx::g_al_api_impl.alBufferi(buffer, param, value);
}

AL_API void AL_APIENTRY alBuffer3i(
	ALuint buffer,
	ALenum param,
	ALint value1,
	ALint value2,
	ALint value3)
{
	eaxefx::g_al_api_impl.alBuffer3i(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alBufferiv(
	ALuint buffer,
	ALenum param,
	const ALint* values)
{
	eaxefx::g_al_api_impl.alBufferiv(buffer, param, values);
}

AL_API void AL_APIENTRY alGetBufferf(
	ALuint buffer,
	ALenum param,
	ALfloat* value)
{
	eaxefx::g_al_api_impl.alGetBufferf(buffer, param, value);
}

AL_API void AL_APIENTRY alGetBuffer3f(
	ALuint buffer,
	ALenum param,
	ALfloat* value1,
	ALfloat* value2,
	ALfloat* value3)
{
	eaxefx::g_al_api_impl.alGetBuffer3f(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetBufferfv(
	ALuint buffer,
	ALenum param,
	ALfloat* values)
{
	eaxefx::g_al_api_impl.alGetBufferfv(buffer, param, values);
}

AL_API void AL_APIENTRY alGetBufferi(
	ALuint buffer,
	ALenum param,
	ALint* value)
{
	eaxefx::g_al_api_impl.alGetBufferi(buffer, param, value);
}

AL_API void AL_APIENTRY alGetBuffer3i(
	ALuint buffer,
	ALenum param,
	ALint* value1,
	ALint* value2,
	ALint* value3)
{
	eaxefx::g_al_api_impl.alGetBuffer3i(buffer, param, value1, value2, value3);
}

AL_API void AL_APIENTRY alGetBufferiv(
	ALuint buffer,
	ALenum param,
	ALint* values)
{
	eaxefx::g_al_api_impl.alGetBufferiv(buffer, param, values);
}

// AL v1.1
// ==========================================================================


} // extern "C"


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
