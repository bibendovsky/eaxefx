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


#include "eaxefx_eaxx.h"

#include <cmath>

#include <algorithm>
#include <array>
#include <exception>
#include <iterator>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"

#include "eaxefx_exception.h"
#include "eaxefx_mutex.h"
#include "eaxefx_string.h"

#include "eaxefx_al_low_pass_param.h"
#include "eaxefx_al_object.h"
#include "eaxefx_al_scope_exit.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_al_uresources.h"
#include "eaxefx_eax_api.h"
#include "eaxefx_eaxx_eax_call.h"
#include "eaxefx_eaxx_fx_slot_index.h"
#include "eaxefx_eaxx_fx_slot.h"
#include "eaxefx_eaxx_fx_slots.h"
#include "eaxefx_eaxx_context.h"
#include "eaxefx_eaxx_source.h"
#include "eaxefx_eaxx_validators.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplException :
	public Exception
{
public:
	explicit EaxxImplException(
		const char* message)
		:
		Exception{"EAXX", message}
	{
	}
}; // EaxxImplException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxSessionException :
	public Exception
{
public:
	explicit EaxxImplSetEaxSessionException(
		const char* message)
		:
		Exception{"EAXX_EAX_SET_SESSION", message}
	{
	}
}; // EaxxImplSetEaxSessionException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxPrimaryFxSlotIdException :
	public Exception
{
public:
	explicit EaxxImplSetEaxPrimaryFxSlotIdException(
		const char* message)
		:
		Exception{"EAXX_EAX_SET_PRIMARY_FX_SLOT_ID", message}
	{
	}
}; // EaxxImplSetEaxPrimaryFxSlotIdException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxContextException :
	public Exception
{
public:
	explicit EaxxImplSetEaxContextException(
		const char* message)
		:
		Exception{"EAXX_EAX_SET_CONTEXT", message}
	{
	}
}; // EaxxImplSetEaxContextException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetFxSlotException :
	public Exception
{
public:
	explicit EaxxImplGetFxSlotException(
		const char* message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT", message}
	{
	}
}; // EaxxImplGetFxSlotException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetFxSlotException :
	public Exception
{
public:
	explicit EaxxImplSetFxSlotException(
		const char* message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT", message}
	{
	}
}; // EaxxImplSetFxSlotException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetSourceException :
	public Exception
{
public:
	explicit EaxxImplSetSourceException(
		const char* message)
		:
		Exception{"EAXX_EAX_SET_SOURCE", message}
	{
	}
}; // EaxxImplSetSourceException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImpl :
	public Eaxx
{
public:
	EaxxImpl(
		Logger* logger,
		AlLoader* al_loader);

	~EaxxImpl() override = default;


	::ALboolean alIsExtensionPresent(
		const ::ALchar* extname) override;


	const ::ALchar* alGetString(
		::ALenum param) override;


	::ALCdevice* alcOpenDevice(
		const ::ALCchar* devicename) override;

	::ALCboolean alcCloseDevice(
		::ALCdevice* device) override;


	void alGenSources(
		::ALsizei n,
		::ALuint* sources) override;

	void alDeleteSources(
		::ALsizei n,
		const ::ALuint* sources) override;


	::ALCcontext* alcCreateContext(
		::ALCdevice* device,
		const ::ALCint* attrlist) override;

	::ALCboolean alcMakeContextCurrent(
		::ALCcontext* context) override;

	void alcDestroyContext(
		::ALCcontext* context) override;


	::ALenum EAXSet(
		const ::GUID* property_set_guid,
		::ALuint property_id,
		::ALuint property_al_name,
		::ALvoid* property_buffer,
		::ALuint property_size) noexcept override;

	::ALenum EAXGet(
		const ::GUID* property_set_guid,
		::ALuint property_id,
		::ALuint property_al_name,
		::ALvoid* property_buffer,
		::ALuint property_size) noexcept override;


private:
	static constexpr auto al_exts_buffer_reserve = 2048;
	static constexpr auto al_context_attrs_reserve = 32;
	static constexpr auto al_devices_reserve = 4;
	static constexpr auto al_contexts_reserve = al_devices_reserve;


	using AlcAttrCache = std::vector<::ALCint>;
	using AlExtsBuffer = String;


	struct Device
	{
		::ALCdevice* al_device;


		explicit Device(
			::ALCdevice* al_device) noexcept
			:
			al_device{al_device}
		{
		}
	}; // Device

	using DeviceMap = std::unordered_map<::ALCdevice*, Device>;
	using ContextMap = std::unordered_map<::ALCcontext*, EaxxContext>;


	Logger* logger_{};
	AlLoader* al_loader_{};
	Mutex mutex_{};
	const ::ALchar* al_exts_ref_{};
	AlExtsBuffer al_exts_buffer_{};
	AlcAttrCache al_context_attr_buffer_{};
	DeviceMap device_map_{};
	ContextMap context_map_{};
	EaxxContext* current_context_{};


	static constexpr auto eax_2_0_ext_name = std::string_view{"EAX2.0"};
	static constexpr auto eax_3_0_ext_name = std::string_view{"EAX3.0"};
	static constexpr auto eax_4_0_ext_name = std::string_view{"EAX4.0"};
	static constexpr auto eax_5_0_ext_name = std::string_view{"EAX5.0"};


	bool has_current_context() const noexcept;

	void ensure_context_for_eax_get_set();

	void open_device(
		const ::ALCchar* al_device_name,
		::ALCdevice* al_device);

	Device* find_device(
		::ALCdevice* al_device) noexcept;

	void append_eax_extension_if_not_exist(
		std::string_view al_exts,
		std::string_view eax_ext);

	const ::ALchar* make_al_exts(
		const ::ALchar* al_exts);

	const ::ALCint* make_al_context_attrs(
		const ::ALCint* al_context_attrs);

	void initialize_current_context() noexcept;


	void dispatch_context(
		const EaxxEaxCall& eax_call);

	void dispatch_fxslot(
		const EaxxEaxCall& eax_call);

	void dispatch_source(
		const EaxxEaxCall& eax_call);
}; // EaxxImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EaxxImpl::EaxxImpl(
	Logger* logger,
	AlLoader* al_loader)
	:
	logger_{logger},
	al_loader_{al_loader}
{
	if (!logger_)
	{
		throw EaxxImplException{"Null logger."};
	}

	if (!al_loader_)
	{
		throw EaxxImplException{"Null AL loader."};
	}

	al_exts_buffer_.reserve(al_exts_buffer_reserve);
	al_context_attr_buffer_.reserve(al_context_attrs_reserve * 2);
	device_map_.reserve(al_devices_reserve);
	context_map_.reserve(al_contexts_reserve);
}

::ALboolean EaxxImpl::alIsExtensionPresent(
	const ::ALchar* extname)
{
	const auto lock = std::scoped_lock{mutex_};

	if (extname)
	{
		const auto extname_view = std::string_view{extname};

		if (extname_view == eax_2_0_ext_name ||
			extname_view == eax_3_0_ext_name ||
			extname_view == eax_4_0_ext_name ||
			extname_view == eax_5_0_ext_name)
		{
			return AL_TRUE;
		}
	}

	return alIsExtensionPresent_(extname);
}

const ::ALchar* EaxxImpl::alGetString(
	::ALenum param)
{
	const auto lock = std::scoped_lock{mutex_};

	auto al_exts = alGetString_(param);

	if (param == AL_EXTENSIONS)
	{
		al_exts = make_al_exts(al_exts);
	}

	return al_exts;
}

::ALCdevice* EaxxImpl::alcOpenDevice(
	const ::ALCchar* devicename)
{
	const auto lock = std::scoped_lock{mutex_};

	const auto message =
		devicename ?
		"Open device \"" + String{devicename} + "\"." :
		"Open default device."
	;

	logger_->info(message.c_str());

	const auto al_device = alcOpenDevice_(devicename);

	if (al_device)
	{
		open_device(devicename, al_device);
	}

	return al_device;
}

::ALCboolean EaxxImpl::alcCloseDevice(
	::ALCdevice* device)
{
	const auto lock = std::scoped_lock{mutex_};

	logger_->info("Close device.");

	const auto al_result = alcCloseDevice_(device);

	if (al_result != ALC_FALSE)
	{
		device_map_.erase(device);
	}

	return al_result;
}

void EaxxImpl::alGenSources(
	::ALsizei n,
	::ALuint* sources)
{
	auto has_sources = false;

	if (n > 0 && sources)
	{
		has_sources = true;

		for (auto i = ::ALsizei{}; i < n; ++i)
		{
			sources[i] = 0;
		}
	}

	alGenSources_(n, sources);

	if (has_sources && sources[0] != 0)
	{
		const auto lock = std::scoped_lock{mutex_};

		if (current_context_)
		{
			current_context_->initialize_sources(n, sources);
		}
	}
}

void EaxxImpl::alDeleteSources(
	::ALsizei n,
	const ::ALuint* sources)
{
	alDeleteSources_(n, sources);


	const auto lock = std::scoped_lock{mutex_};

	if (current_context_ &&
		n > 0 &&
		sources &&
		sources[0] != 0)
	{
		current_context_->uninitialize_sources(n, sources);
	}
}

::ALCcontext* EaxxImpl::alcCreateContext(
	::ALCdevice* device,
	const ::ALCint* attrlist)
{
	const auto lock = std::scoped_lock{mutex_};

	if (device)
	{
		attrlist = make_al_context_attrs(attrlist);
	}

	const auto al_context = alcCreateContext_(device, attrlist);

	if (al_context)
	{
		const auto our_device = find_device(device);

		if (our_device)
		{
			context_map_.emplace(al_context, EaxxContext{device, al_context});
		}
		else
		{
			logger_->error("Device not found.");
		}
	}

	return al_context;
}

::ALCboolean EaxxImpl::alcMakeContextCurrent(
	::ALCcontext* context)
{
	const auto al_result = alcMakeContextCurrent_(context);

	if (al_result != ALC_FALSE)
	{
		const auto lock = std::scoped_lock{mutex_};

		current_context_ = nullptr;

		if (context)
		{
			for (auto& [context_key, context_value] : context_map_)
			{
				if (context_key == context)
				{
					current_context_ = &context_value;
					break;
				}
			}
		}
	}

	return al_result;
}

void EaxxImpl::alcDestroyContext(
	::ALCcontext* context)
{
	const auto lock = std::scoped_lock{mutex_};

	if (context)
	{
		if (current_context_ && current_context_->get_al_context() == context)
		{
			current_context_ = nullptr;
		}

		context_map_.erase(context);
	}

	alcDestroyContext_(context);
}

::ALenum EaxxImpl::EAXSet(
	const ::GUID* property_set_guid,
	::ALuint property_id,
	::ALuint property_al_name,
	::ALvoid* property_buffer,
	::ALuint property_size) noexcept
try
{
	const auto lock = std::scoped_lock{mutex_};

	ensure_context_for_eax_get_set();

	const auto eax_call = make_eax_call(
		false,
		property_set_guid,
		property_id,
		property_al_name,
		property_buffer,
		property_size
	);

	switch (eax_call.get_property_set_id())
	{
		case EaxxEaxCallPropertySetId::context:
			dispatch_context(eax_call);
			break;

		case EaxxEaxCallPropertySetId::fx_slot:
		case EaxxEaxCallPropertySetId::fx_slot_effect:
			dispatch_fxslot(eax_call);
			break;

		case EaxxEaxCallPropertySetId::source:
			dispatch_source(eax_call);
			break;

		default:
			throw EaxxImplException{"Unsupported property set id."};
	}

	return AL_NO_ERROR;
}
catch (const std::exception& ex)
{
	logger_->error(ex, "EAXSet failed.");
	return AL_INVALID_OPERATION;
}

::ALenum EaxxImpl::EAXGet(
	const ::GUID* property_set_guid,
	::ALuint property_id,
	::ALuint property_al_name,
	::ALvoid* property_buffer,
	::ALuint property_size) noexcept
try
{
	const auto lock = std::scoped_lock{mutex_};

	ensure_context_for_eax_get_set();

	const auto eax_call = make_eax_call(
		true,
		property_set_guid,
		property_id,
		property_al_name,
		property_buffer,
		property_size
	);

	switch (eax_call.get_property_set_id())
	{
		case EaxxEaxCallPropertySetId::context:
			dispatch_context(eax_call);
			break;

		case EaxxEaxCallPropertySetId::fx_slot:
		case EaxxEaxCallPropertySetId::fx_slot_effect:
			dispatch_fxslot(eax_call);
			break;

		case EaxxEaxCallPropertySetId::source:
			dispatch_source(eax_call);
			break;

		default:
			throw EaxxImplException{"Unsupported property set id."};
	}

	return AL_NO_ERROR;
}
catch (const std::exception& ex)
{
	logger_->error(ex, "EAXGet failed.");
	return AL_INVALID_OPERATION;
}

bool EaxxImpl::has_current_context() const noexcept
{
	return
		current_context_ &&
		current_context_->is_initialized();
}

void EaxxImpl::ensure_context_for_eax_get_set()
{
	if (!current_context_)
	{
		throw EaxxImplException{"Null current context."};
	}

	current_context_->initialize();

	if (!current_context_->is_initialized())
	{
		throw EaxxImplException{"Context not initialized."};
	}
}

void EaxxImpl::open_device(
	const ::ALCchar* al_device_name,
	::ALCdevice* al_device)
{
	device_map_.emplace(al_device, Device{al_device});

	if (!al_device_name)
	{
		const auto real_device_name = alcGetString_(al_device, ALC_DEVICE_SPECIFIER);

		if (real_device_name)
		{
			const auto message = "Device name: \"" + String{real_device_name} + "\".";
			logger_->info(message.c_str());
		}
		else
		{
			logger_->error("Failed to get device name.");
		}
	}
}

EaxxImpl::Device* EaxxImpl::find_device(
	::ALCdevice* al_device) noexcept
{
	const auto device_it = device_map_.find(al_device);

	if (device_it != device_map_.cend())
	{
		return &device_it->second;
	}

	return nullptr;
}

void EaxxImpl::append_eax_extension_if_not_exist(
	std::string_view al_exts,
	std::string_view eax_ext)
{
	if (al_exts.find(eax_ext) == std::string_view::npos)
	{
		if (!al_exts_buffer_.empty())
		{
			al_exts_buffer_ += ' ';
		}

		al_exts_buffer_ += eax_ext;
	}
}

const ::ALchar* EaxxImpl::make_al_exts(
	const ::ALchar* al_exts)
{
	if (!al_exts)
	{
		return nullptr;
	}

	if (al_exts == al_exts_ref_)
	{
		return al_exts_buffer_.data();
	}

	al_exts_buffer_.clear();

	const auto al_exts_view = std::string_view{al_exts};

	append_eax_extension_if_not_exist(al_exts_view, eax_2_0_ext_name);
	append_eax_extension_if_not_exist(al_exts_view, eax_3_0_ext_name);
	append_eax_extension_if_not_exist(al_exts_view, eax_4_0_ext_name);
	append_eax_extension_if_not_exist(al_exts_view, eax_5_0_ext_name);

	if (!al_exts_view.empty())
	{
		if (!al_exts_buffer_.empty())
		{
			al_exts_buffer_ += ' ';
		}

		al_exts_buffer_ += al_exts_view;
	}

	al_exts_buffer_ += '\0';

	al_exts_ref_ = al_exts;

	return al_exts_buffer_.data();
}

const ::ALCint* EaxxImpl::make_al_context_attrs(
	const ::ALCint* al_context_attrs)
{
	al_context_attr_buffer_.clear();

	auto max_aux_sends = ::ALCint{};

	if (al_context_attrs)
	{
		while (true)
		{
			const auto attr_name = *al_context_attrs++;

			if (attr_name == 0)
			{
				break;
			}

			const auto attr_value = *al_context_attrs++;

			if (attr_name == ALC_MAX_AUXILIARY_SENDS)
			{
				max_aux_sends = std::max(max_aux_sends, attr_value);
			}
			else
			{
				al_context_attr_buffer_.emplace_back(attr_name);
				al_context_attr_buffer_.emplace_back(attr_value);
			}
		}
	}

	max_aux_sends = std::max(max_aux_sends, ::EAX_MAX_FXSLOTS);

	al_context_attr_buffer_.emplace_back(ALC_MAX_AUXILIARY_SENDS);
	al_context_attr_buffer_.emplace_back(max_aux_sends);
	al_context_attr_buffer_.emplace_back(0);
	al_context_attr_buffer_.emplace_back(0);

	return al_context_attr_buffer_.data();
}

void EaxxImpl::initialize_current_context() noexcept
try
{
	if (!current_context_)
	{
		throw EaxxImplException{"Null current context."};
	}

	if (current_context_->is_tried_to_initialize())
	{
		return;
	}

	logger_->info("Initialize context.");

	current_context_->initialize();
}
catch (const std::exception& ex)
{
	logger_->write(ex);
}

void EaxxImpl::dispatch_context(
	const EaxxEaxCall& eax_call)
{
	current_context_->dispatch(eax_call);
}

void EaxxImpl::dispatch_fxslot(
	const EaxxEaxCall& eax_call)
{
	auto& fx_slot = current_context_->get_slot(eax_call.get_fx_slot_index());

	if (fx_slot.dispatch(eax_call))
	{
		current_context_->update_filters();
	}
}

void EaxxImpl::dispatch_source(
	const EaxxEaxCall& eax_call)
{
	auto source = current_context_->find_source(eax_call.get_property_al_name());

	if (!source)
	{
		throw EaxxImplException{"Source not found."};
	}

	source->dispatch(eax_call);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EaxxUPtr make_eaxx(
	Logger* logger,
	AlLoader* al_loader)
{
	return std::make_unique<EaxxImpl>(logger, al_loader);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
