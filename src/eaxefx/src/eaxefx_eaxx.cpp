/*

EAX OpenAL Extension

Copyright (c) 2020 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

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


#include <initguid.h>

#include "eaxefx_eaxx.h"

#include <cmath>

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <thread>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"

#include "eaxefx_exception.h"

#include "eaxefx_al_low_pass_param.h"
#include "eaxefx_al_object.h"
#include "eaxefx_al_scope_exit.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_al_uresources.h"
#include "eaxefx_eax_api.h"
#include "eaxefx_eax_converters.h"
#include "eaxefx_eax_validators.h"
#include "eaxefx_eaxx_fx_slot_index.h"
#include "eaxefx_eaxx_fx_slot.h"
#include "eaxefx_eaxx_fx_slots.h"
#include "eaxefx_eaxx_context.h"
#include "eaxefx_eaxx_source.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplException :
	public Exception
{
public:
	explicit EaxxImplException(
		std::string_view message)
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
		std::string_view message)
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
		std::string_view message)
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
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_CONTEXT", message}
	{
	}
}; // EaxxImplSetEaxContextException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotEffectException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotEffectException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_EFFECT", message}
	{
	}
}; // EaxxImplSetEaxFxSlotEffectException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotVolumeException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotVolumeException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_VOLUME", message}
	{
	}
}; // EaxxImplSetEaxFxSlotVolumeException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotLockException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotLockException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_LOCK", message}
	{
	}
}; // EaxxImplSetEaxFxSlotLockException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotFlagsException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotFlagsException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_FLAGS", message}
	{
	}
}; // EaxxImplSetEaxFxSlotFlagsException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotOcclusionException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotOcclusionException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_OCCLUSION", message}
	{
	}
}; // EaxxImplSetEaxFxSlotOcclusionException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotOcclusionLfRatioException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotOcclusionLfRatioException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_OCCLUSION_LF_RATIO", message}
	{
	}
}; // EaxxImplSetEaxFxSlotOcclusionLfRatioException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotEaxReverbEnvironmentException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotEaxReverbEnvironmentException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_EAX_REVERB_ENVIRONMENT", message}
	{
	}
}; // EaxxImplSetEaxFxSlotEaxReverbEnvironmentException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotEaxReverbException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotEaxReverbException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT_EAX_REVERB", message}
	{
	}
}; // EaxxImplSetEaxFxSlotEaxReverbException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxFxSlotException :
	public Exception
{
public:
	explicit EaxxImplSetEaxFxSlotException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_FX_SLOT", message}
	{
	}
}; // EaxxImplSetEaxFxSlotException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxSourceObstructionPropertiesException :
	public Exception
{
public:
	explicit EaxxImplSetEaxSourceObstructionPropertiesException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_SOURCE_OBSTRUCTION_PROPERTIES", message}
	{
	}
}; // EaxxImplSetEaxSourceObstructionPropertiesException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxSourceOcclusionPropertiesException :
	public Exception
{
public:
	explicit EaxxImplSetEaxSourceOcclusionPropertiesException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_SOURCE_OCCLUSION_PROPERTIES", message}
	{
	}
}; // EaxxImplSetEaxSourceOcclusionPropertiesException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxSourceRoomException :
	public Exception
{
public:
	explicit EaxxImplSetEaxSourceRoomException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_SOURCE_ROOM", message}
	{
	}
}; // EaxxImplSetEaxSourceRoomException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxSourceOcclusionException :
	public Exception
{
public:
	explicit EaxxImplSetEaxSourceOcclusionException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_SOURCE_OCCLUSION", message}
	{
	}
}; // EaxxImplSetEaxSourceOcclusionException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxSourceActiveFxSlotIdException :
	public Exception
{
public:
	explicit EaxxImplSetEaxSourceActiveFxSlotIdException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_SOURCE_ACTIVE_FX_SLOT_ID", message}
	{
	}
}; // EaxxImplSetEaxSourceActiveFxSlotIdException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplSetEaxSourceException :
	public Exception
{
public:
	explicit EaxxImplSetEaxSourceException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_SET_SOURCE", message}
	{
	}
}; // EaxxImplSetEaxSourceException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetEaxFxSlotEffectException :
	public Exception
{
public:
	explicit EaxxImplGetEaxFxSlotEffectException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT_EFFECT", message}
	{
	}
}; // EaxxImplGetEaxFxSlotEffectException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetEaxFxSlotVolumeException :
	public Exception
{
public:
	explicit EaxxImplGetEaxFxSlotVolumeException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT_VOLUME", message}
	{
	}
}; // EaxxImplGetEaxFxSlotVolumeException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetEaxFxSlotLockException :
	public Exception
{
public:
	explicit EaxxImplGetEaxFxSlotLockException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT_LOCK", message}
	{
	}
}; // EaxxImplGetEaxFxSlotLockException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetEaxFxSlotFlagsException :
	public Exception
{
public:
	explicit EaxxImplGetEaxFxSlotFlagsException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT_FLAGS", message}
	{
	}
}; // EaxxImplGetEaxFxSlotFlagsException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetEaxFxSlotOcclusionException :
	public Exception
{
public:
	explicit EaxxImplGetEaxFxSlotOcclusionException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT_OCCLUSION", message}
	{
	}
}; // EaxxImplGetEaxFxSlotOcclusionException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetEaxFxSlotOcclusionLfRatioException :
	public Exception
{
public:
	explicit EaxxImplGetEaxFxSlotOcclusionLfRatioException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT_OCCLUSION_LF_RATIO", message}
	{
	}
}; // EaxxImplGetEaxFxSlotOcclusionLfRatioException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxImplGetEaxFxSlotException :
	public Exception
{
public:
	explicit EaxxImplGetEaxFxSlotException(
		std::string_view message)
		:
		Exception{"EAXX_EAX_GET_FX_SLOT", message}
	{
	}
}; // EaxxImplGetEaxFxSlotException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename T>
float mb_to_gain(
	T value)
{
	return std::pow(10.0F, static_cast<float>(value) / 2'000.0F);
}

template
<
	typename TValue,
	typename TRange
>
bool is_in_range(
	const TValue& value,
	const TRange& min_value,
	const TRange& max_value)
{
	return value >= static_cast<TValue>(min_value) && value <= static_cast<TValue>(max_value);
}

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


	ALboolean alIsExtensionPresent(
		const ALchar* extname) override;


	const ALchar* alGetString(
		ALenum param) override;


	ALCdevice* alcOpenDevice(
		const ALCchar* devicename) override;

	ALCboolean alcCloseDevice(
		ALCdevice* device) override;


	void alGenSources(
		ALsizei n,
		ALuint* sources) override;

	void alDeleteSources(
		ALsizei n,
		const ALuint* sources) override;


	ALCcontext* alcCreateContext(
		ALCdevice* device,
		const ALCint* attrlist) override;

	ALCboolean alcMakeContextCurrent(
		ALCcontext* context) override;

	void alcDestroyContext(
		ALCcontext* context) override;


	ALenum EAXSet(
		const GUID* property_set_id,
		ALuint property_id,
		ALuint al_name,
		ALvoid* property_value,
		ALuint property_value_size) noexcept override;

	ALenum EAXGet(
		const GUID* property_set_id,
		ALuint property_id,
		ALuint al_name,
		ALvoid* property_value,
		ALuint property_value_size) noexcept override;


private:
	static constexpr auto al_extension_reserve = 2048;
	static constexpr auto al_context_attrs_reserve = 32;
	static constexpr auto al_devices_reserve = 4;
	static constexpr auto al_contexts_reserve = al_devices_reserve;


	using Mutex = std::mutex;
	using AlcAttrCache = std::vector<ALCint>;
	using AlExtensionCache = std::string;


	struct Device
	{
		ALCdevice* al_device;


		explicit Device(
			ALCdevice* al_device) noexcept
			:
			al_device{al_device}
		{
		}
	}; // Device

	struct EaxCall
	{
		bool is_get{};
		int version{};
		int fx_slot_index{};

		const GUID* property_set_id{};
		ALuint property_id{};
		ALuint property_al_name{};
		ALvoid* property_value{};
		ALuint property_value_size{};
	}; // EaxCall

	using DeviceMap = std::unordered_map<ALCdevice*, Device>;
	using ContextMap = std::unordered_map<ALCcontext*, EaxxContext>;


	Logger* logger_{};
	AlLoader* al_loader_{};
	Mutex mutex_{};
	AlExtensionCache al_exts_cache_{};
	AlcAttrCache al_context_attr_cache_{};
	DeviceMap device_map_{};
	ContextMap context_map_{};
	EaxxContext* current_context_{};


	static constexpr auto eax_4_0_ext_name = std::string_view{"EAX4.0"};
	static constexpr auto eax_5_0_ext_name = std::string_view{"EAX5.0"};


	bool has_current_context() const noexcept;

	void open_device(
		const ALCchar* al_device_name,
		ALCdevice* al_device);

	Device* find_device(
		ALCdevice* al_device) noexcept;

	const ALchar* make_al_exts(
		const ALchar* al_exts);

	const ALCint* make_al_context_attrs(
		const ALCint* al_context_attrs);

	void initialize_context() noexcept;


	void eax_set_context_eaxsession(
		const EaxCall& eax_call);

	void eax_set_context_primaryfxslotid(
		const EaxCall& eax_call);

	void eax_set_context(
		const EaxCall& eax_call);


	void eax_set_fxslot_loadeffect(
		const EaxCall& eax_call);

	void eax_set_fxslot_volume(
		const EaxCall& eax_call);

	void eax_set_fxslot_lock(
		const EaxCall& eax_call);

	void eax_set_fxslot_flags(
		const EaxCall& eax_call);

	void eax_set_fxslot_occlusion(
		const EaxCall& eax_call);

	void eax_set_fxslot_occlusionlfratio(
		const EaxCall& eax_call);

	void eax_set_fxslot_eaxreverb_environment(
		const EaxCall& eax_call);

	void eax_set_fxslot_eaxreverb_allparameters(
		const EaxCall& eax_call);

	void eax_set_fxslot(
		const EaxCall& eax_call);


	void eax_set_source_obstructionparameters(
		const EaxCall& eax_call);

	void eax_set_source_occlusionparameters(
		const EaxCall& eax_call);

	void eax_set_source_room(
		const EaxCall& eax_call) noexcept;

	void eax_set_source_occlusion(
		const EaxCall& eax_call) noexcept;

	void eax_set_source_activefxslotid(
		const EaxCall& eax_call);

	void eax_set_source(
		const EaxCall& eax_call);


	void eax_get_fxslot_allparameters(
		const EaxCall& eax_call);

	void eax_get_fxslot_loadeffect(
		const EaxCall& eax_call);

	void eax_get_fxslot_volume(
		const EaxCall& eax_call);

	void eax_get_fxslot_lock(
		const EaxCall& eax_call);

	void eax_get_fxslot_flags(
		const EaxCall& eax_call);

	void eax_get_fxslot_occlusion(
		const EaxCall& eax_call);

	void eax_get_fxslot_occlusion_lf_ratio(
		const EaxCall& eax_call);

	void eax_get_fxslot(
		const EaxCall& eax_call);
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
	if (logger_ == nullptr)
	{
		throw EaxxImplException{"Null logger."};
	}

	if (al_loader_ == nullptr)
	{
		throw EaxxImplException{"Null AL loader."};
	}

	al_exts_cache_.reserve(al_extension_reserve);
	al_context_attr_cache_.reserve(al_context_attrs_reserve * 2);
	device_map_.reserve(al_devices_reserve);
	context_map_.reserve(al_contexts_reserve);
}

ALboolean EaxxImpl::alIsExtensionPresent(
	const ALchar* extname)
{
	const auto lock = std::scoped_lock{mutex_};

	if (extname != nullptr)
	{
		const auto extname_view = std::string_view{extname};

		if (extname_view == eax_4_0_ext_name ||
			extname_view == eax_5_0_ext_name)
		{
			return AL_TRUE;
		}
	}

	return alIsExtensionPresent_(extname);
}

const ALchar* EaxxImpl::alGetString(
	ALenum param)
{
	const auto lock = std::scoped_lock{mutex_};

	auto al_exts = alGetString_(param);

	if (param == AL_EXTENSIONS)
	{
		al_exts = make_al_exts(al_exts);
	}

	return al_exts;
}

ALCdevice* EaxxImpl::alcOpenDevice(
	const ALCchar* devicename)
{
	const auto lock = std::scoped_lock{mutex_};

	logger_->info(
		devicename != nullptr ?
			"Open device \"" + std::string{devicename} + "\"." :
			"Open default device.");

	const auto al_device = alcOpenDevice_(devicename);

	if (al_device != nullptr)
	{
		open_device(devicename, al_device);
	}

	return al_device;
}

ALCboolean EaxxImpl::alcCloseDevice(
	ALCdevice* device)
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
	ALsizei n,
	ALuint* sources)
{
	const auto lock = std::scoped_lock{mutex_};

	const auto has_sources =
		has_current_context() &&
		n > 0 &&
		sources != nullptr;

	alGenSources_(n, sources);

	if (has_sources)
	{
		const auto is_succeed = std::all_of(
			sources,
			sources + n,
			[](const auto al_name)
			{
				return al_name != 0;
			}
		);

		if (is_succeed)
		{
			current_context_->initialize_sources(n, sources);
		}
	}
}

void EaxxImpl::alDeleteSources(
	ALsizei n,
	const ALuint* sources)
{
	const auto lock = std::scoped_lock{mutex_};

	if (has_current_context() &&
		n > 0 &&
		sources != nullptr)
	{
		current_context_->uninitialize_sources(n, sources);
	}

	alDeleteSources_(n, sources);
}

ALCcontext* EaxxImpl::alcCreateContext(
	ALCdevice* device,
	const ALCint* attrlist)
{
	const auto lock = std::scoped_lock{mutex_};

	if (device != nullptr)
	{
		attrlist = make_al_context_attrs(attrlist);
	}

	const auto al_context = alcCreateContext_(device, attrlist);

	if (al_context != nullptr)
	{
		const auto our_device = find_device(device);

		if (our_device != nullptr)
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

ALCboolean EaxxImpl::alcMakeContextCurrent(
	ALCcontext* context)
{
	const auto lock = std::scoped_lock{mutex_};

	const auto has_context =
		context == nullptr ||
		context_map_.find(context) != context_map_.cend();

	if (has_context)
	{
		current_context_ = nullptr;

		for (auto& [context_key, context_value] : context_map_)
		{
			if (context_key == context)
			{
				current_context_ = &context_value;
			}
		}
	}

	const auto al_result = alcMakeContextCurrent_(context);

	if (has_context && context != nullptr)
	{
		initialize_context();
	}

	return al_result;
}

void EaxxImpl::alcDestroyContext(
	ALCcontext* context)
{
	const auto lock = std::scoped_lock{mutex_};

	if (context != nullptr)
	{
		if (current_context_ == nullptr ||
			(current_context_ != nullptr && current_context_->get_al_context() != context))
		{
			context_map_.erase(context);
		}
	}

	alcDestroyContext_(context);
}

ALenum EaxxImpl::EAXSet(
	const GUID* property_set_id,
	ALuint property_id,
	ALuint property_al_name,
	ALvoid* property_value,
	ALuint property_value_size) noexcept
try
{
	const auto lock = std::scoped_lock{mutex_};

	if (!has_current_context())
	{
		return AL_INVALID_OPERATION;
	}

	if (property_set_id == nullptr)
	{
		throw EaxxImplException{"Null property GUID."};
	}

	if (property_value == nullptr)
	{
		throw EaxxImplException{"Null property value."};
	}

	auto eax_call = EaxCall{};
	eax_call.is_get = false;
	eax_call.version = 0;
	eax_call.fx_slot_index = -1;
	eax_call.property_set_id = property_set_id;
	eax_call.property_id = property_id;
	eax_call.property_al_name = property_al_name;
	eax_call.property_value = property_value;
	eax_call.property_value_size = property_value_size;

	if (false)
	{
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_Context)
	{
		eax_call.version = 4;
		eax_set_context(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_Context)
	{
		eax_call.version = 5;
		eax_set_context(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot0)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 0;
		eax_set_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot0)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 0;
		eax_set_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot1)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 1;
		eax_set_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot1)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 1;
		eax_set_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot2)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 2;
		eax_set_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot2)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 2;
		eax_set_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot3)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 3;
		eax_set_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot3)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 3;
		eax_set_fxslot(eax_call);
	}
	else if (*property_set_id == EAXPROPERTYID_EAX40_Source)
	{
		eax_call.version = 4;
		eax_set_source(eax_call);
	}
	else if (*property_set_id == EAXPROPERTYID_EAX50_Source)
	{
		eax_call.version = 5;
		eax_set_source(eax_call);
	}
	else
	{
		return AL_INVALID_OPERATION;
	}

	return AL_NO_ERROR;
}
catch (const std::exception& ex)
{
	logger_->error(ex, "EAXSet failed.");
	return AL_INVALID_OPERATION;
}

ALenum EaxxImpl::EAXGet(
	const GUID* property_set_id,
	ALuint property_id,
	ALuint property_al_name,
	ALvoid* property_value,
	ALuint property_value_size) noexcept
try
{
	const auto lock = std::scoped_lock{mutex_};

	if (!has_current_context())
	{
		return AL_INVALID_OPERATION;
	}

	if (property_set_id == nullptr)
	{
		throw EaxxImplException{"Null property GUID."};
	}

	if (property_value == nullptr)
	{
		throw EaxxImplException{"Null property value."};
	}

	auto eax_call = EaxCall{};
	eax_call.is_get = true;
	eax_call.version = 0;
	eax_call.fx_slot_index = -1;
	eax_call.property_set_id = property_set_id;
	eax_call.property_id = property_id;
	eax_call.property_al_name = property_al_name;
	eax_call.property_value = property_value;
	eax_call.property_value_size = property_value_size;

	if (false)
	{
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot0)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 0;
		eax_get_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot0)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 0;
		eax_get_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot1)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 1;
		eax_get_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot1)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 1;
		eax_get_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot2)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 2;
		eax_get_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot2)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 2;
		eax_get_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX40_FXSlot3)
	{
		eax_call.version = 4;
		eax_call.fx_slot_index = 3;
		eax_get_fxslot(eax_call);
	}
	else if ((*property_set_id) == EAXPROPERTYID_EAX50_FXSlot3)
	{
		eax_call.version = 5;
		eax_call.fx_slot_index = 3;
		eax_get_fxslot(eax_call);
	}
	else
	{
		return AL_INVALID_OPERATION;
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
		current_context_ != nullptr &&
		current_context_->is_initialized();
}

void EaxxImpl::open_device(
	const ALCchar* al_device_name,
	ALCdevice* al_device)
{
	device_map_.emplace(al_device, Device{al_device});

	if (al_device_name == nullptr)
	{
		const auto real_device_name = alcGetString_(al_device, ALC_DEVICE_SPECIFIER);

		if (real_device_name != nullptr)
		{
			logger_->info("Device name: \"" + std::string{real_device_name} + "\".");
		}
		else
		{
			logger_->error("Failed to get device name.");
		}
	}
}

EaxxImpl::Device* EaxxImpl::find_device(
	ALCdevice* al_device) noexcept
{
	const auto device_it = device_map_.find(al_device);

	if (device_it != device_map_.cend())
	{
		return &device_it->second;
	}

	return nullptr;
}

const ALchar* EaxxImpl::make_al_exts(
	const ALchar* al_exts)
{
	al_exts_cache_.clear();

	const auto al_exts_view = std::string_view{al_exts};

	if (al_exts_view.find(eax_4_0_ext_name) != std::string_view::npos)
	{
		if (!al_exts_cache_.empty())
		{
			al_exts_cache_ += ' ';
		}

		al_exts_cache_ += eax_4_0_ext_name;
	}

	if (al_exts_view.find(eax_5_0_ext_name) != std::string_view::npos)
	{
		if (!al_exts_cache_.empty())
		{
			al_exts_cache_ += ' ';
		}

		al_exts_cache_ += eax_5_0_ext_name;
	}

	if (!al_exts_view.empty())
	{
		if (!al_exts_cache_.empty())
		{
			al_exts_cache_ += ' ';
		}

		al_exts_cache_ += al_exts_view;
	}

	al_exts_cache_ += '\0';

	return al_exts_cache_.data();
}

const ALCint* EaxxImpl::make_al_context_attrs(
	const ALCint* al_context_attrs)
{
	al_context_attr_cache_.clear();

	auto max_aux_sends = ALCint{};

	if (al_context_attrs != nullptr)
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
				al_context_attr_cache_.emplace_back(attr_name);
				al_context_attr_cache_.emplace_back(attr_value);
			}
		}
	}

	max_aux_sends = std::max(max_aux_sends, EAX_MAX_FXSLOTS);

	al_context_attr_cache_.emplace_back(ALC_MAX_AUXILIARY_SENDS);
	al_context_attr_cache_.emplace_back(max_aux_sends);
	al_context_attr_cache_.emplace_back(0);
	al_context_attr_cache_.emplace_back(0);

	return al_context_attr_cache_.data();
}

void EaxxImpl::initialize_context() noexcept
try
{
	if (current_context_ == nullptr)
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
	logger_->error("Failed to initialize context.");
	logger_->write(ex, 1);
}

void EaxxImpl::eax_set_context_eaxsession(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(EAXSESSIONPROPERTIES))
	{
		throw EaxxImplSetEaxSessionException{"Value size too small."};
	}

	current_context_->set_session(*static_cast<const EAXSESSIONPROPERTIES*>(eax_call.property_value));
}

void EaxxImpl::eax_set_context_primaryfxslotid(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(GUID))
	{
		throw EaxxImplSetEaxPrimaryFxSlotIdException{"Value size too small."};
	}

	current_context_->set_primary_fx_slot_id(*static_cast<const GUID*>(eax_call.property_value));
}

void EaxxImpl::eax_set_context(
	const EaxCall& eax_call)
{
	switch (eax_call.property_id)
	{
		case EAXCONTEXT_EAXSESSION:
			eax_set_context_eaxsession(eax_call);
			break;

		case EAXCONTEXT_PRIMARYFXSLOTID:
			eax_set_context_primaryfxslotid(eax_call);
			break;

		default:
			throw EaxxImplSetEaxContextException{"Unsupported property id."};
	}
}

void EaxxImpl::eax_set_fxslot_loadeffect(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(GUID))
	{
		throw EaxxImplSetEaxFxSlotEffectException{"Value size too small."};
	}

	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	fx_slot.set_effect(*static_cast<const GUID*>(eax_call.property_value));
}

void EaxxImpl::eax_set_fxslot_volume(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(long))
	{
		throw EaxxImplSetEaxFxSlotVolumeException{"Value size too small."};
	}

	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	fx_slot.set_volume(*static_cast<const long*>(eax_call.property_value));
}

void EaxxImpl::eax_set_fxslot_lock(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(long))
	{
		throw EaxxImplSetEaxFxSlotLockException{"Value size too small."};
	}

	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	fx_slot.set_lock(*static_cast<const long*>(eax_call.property_value));
}

void EaxxImpl::eax_set_fxslot_flags(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(unsigned long))
	{
		throw EaxxImplSetEaxFxSlotFlagsException{"Value size too small."};
	}

	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);

	fx_slot.set_flags(
		eax_call.version,
		*static_cast<const unsigned long*>(eax_call.property_value));
}

void EaxxImpl::eax_set_fxslot_occlusion(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(unsigned long))
	{
		throw EaxxImplSetEaxFxSlotOcclusionException{"Value size too small."};
	}

	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);

	const auto is_modified = fx_slot.set_occlusion(
		eax_call.version,
		*static_cast<const long*>(eax_call.property_value));

	if (is_modified)
	{
		current_context_->update_filters();
	}
}

void EaxxImpl::eax_set_fxslot_occlusionlfratio(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(float))
	{
		throw EaxxImplSetEaxFxSlotOcclusionLfRatioException{"Value size too small."};
	}

	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);

	const auto is_modified = fx_slot.set_occlusion_lf_ratio(
		eax_call.version,
		*static_cast<const float*>(eax_call.property_value));

	if (is_modified)
	{
		current_context_->update_filters();
	}
}

void EaxxImpl::eax_set_fxslot_eaxreverb_environment(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(unsigned long))
	{
		throw EaxxImplSetEaxFxSlotEaxReverbEnvironmentException{"Value size too small."};
	}

	const auto eax_reverb_environment = *static_cast<const unsigned long*>(eax_call.property_value);
	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	fx_slot.set_reverb_environment(eax_call.version, eax_reverb_environment);
}

void EaxxImpl::eax_set_fxslot_eaxreverb_allparameters(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(EAXREVERBPROPERTIES))
	{
		throw EaxxImplSetEaxFxSlotEaxReverbException{"Value size too small."};
	}

	const auto& eax_reverb = *static_cast<const EAXREVERBPROPERTIES*>(eax_call.property_value);
	auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	fx_slot.set_reverb(eax_call.version, eax_reverb);
}

void EaxxImpl::eax_set_fxslot(
	const EaxCall& eax_call)
{
	switch (eax_call.property_id)
	{
		case EAXFXSLOT_LOADEFFECT:
			eax_set_fxslot_loadeffect(eax_call);
			break;

		case EAXFXSLOT_VOLUME:
			eax_set_fxslot_volume(eax_call);
			break;

		case EAXFXSLOT_LOCK:
			eax_set_fxslot_lock(eax_call);
			break;

		case EAXFXSLOT_FLAGS:
			eax_set_fxslot_flags(eax_call);
			break;

		case EAXFXSLOT_OCCLUSION:
			eax_set_fxslot_occlusion(eax_call);
			break;

		case EAXFXSLOT_OCCLUSIONLFRATIO:
			eax_set_fxslot_occlusionlfratio(eax_call);
			break;

		case EAXREVERB_ENVIRONMENT:
			eax_set_fxslot_eaxreverb_environment(eax_call);
			break;

		case EAXREVERB_ALLPARAMETERS:
			eax_set_fxslot_eaxreverb_allparameters(eax_call);
			break;

		default:
			throw EaxxImplSetEaxFxSlotException{"Unsupported property id."};
	}
}

void EaxxImpl::eax_set_source_obstructionparameters(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(EAXOBSTRUCTIONPROPERTIES))
	{
		throw EaxxImplSetEaxSourceObstructionPropertiesException{"Value size too small."};
	}

	auto source = current_context_->find_source(eax_call.property_al_name);

	if (source == nullptr)
	{
		throw EaxxImplSetEaxSourceObstructionPropertiesException{"Null source."};
	}

	const auto& eax_obstruction_parameters =
		*static_cast<const EAXOBSTRUCTIONPROPERTIES*>(eax_call.property_value);

	source->set_obstruction_properties(eax_obstruction_parameters);
}

void EaxxImpl::eax_set_source_occlusionparameters(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(EAXOCCLUSIONPROPERTIES))
	{
		throw EaxxImplSetEaxSourceOcclusionPropertiesException{"Value size too small."};
	}

	auto source = current_context_->find_source(eax_call.property_al_name);

	if (source == nullptr)
	{
		throw EaxxImplSetEaxSourceOcclusionPropertiesException{"Null source."};
	}

	const auto& eax_occlusion_parameters =
		*static_cast<const EAXOCCLUSIONPROPERTIES*>(eax_call.property_value);

	source->set_occlusion_properties(eax_occlusion_parameters);
}

void EaxxImpl::eax_set_source_room(
	const EaxCall& eax_call) noexcept
try
{
	if (eax_call.property_value_size < sizeof(long))
	{
		throw EaxxImplSetEaxSourceRoomException{"Value size too small."};
	}

	auto source = current_context_->find_source(eax_call.property_al_name);

	if (source == nullptr)
	{
		throw EaxxImplSetEaxSourceRoomException{"Null source."};
	}

	const auto eax_room = *static_cast<const long*>(eax_call.property_value);

	source->set_room(eax_room);
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxImplSetEaxSourceRoomException{"Failed."});
}

void EaxxImpl::eax_set_source_occlusion(
	const EaxCall& eax_call) noexcept
try
{
	if (eax_call.property_value_size < sizeof(long))
	{
		throw EaxxImplSetEaxSourceOcclusionException{"Value size too small."};
	}

	auto source = current_context_->find_source(eax_call.property_al_name);

	if (source == nullptr)
	{
		throw EaxxImplSetEaxSourceOcclusionException{"Null source."};
	}

	const auto eax_occlusion = *static_cast<const long*>(eax_call.property_value);

	source->set_occlusion(eax_occlusion);
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxImplSetEaxSourceOcclusionException{"Failed."});
}

void EaxxImpl::eax_set_source_activefxslotid(
	const EaxCall& eax_call)
{
	if (eax_call.version < 4)
	{
		throw EaxxImplSetEaxSourceActiveFxSlotIdException{"Unsupported EAX version."};
	}

	auto source = current_context_->find_source(eax_call.property_al_name);

	if (source == nullptr)
	{
		throw EaxxImplSetEaxSourceActiveFxSlotIdException{"Null source."};
	}

	const auto max_count = std::min(
		static_cast<int>(eax_call.property_value_size / sizeof(GUID)),
		EAX50_MAX_ACTIVE_FXSLOTS);

	if (max_count <= 0)
	{
		throw EaxxImplSetEaxSourceActiveFxSlotIdException{"Value size to small."};
	}

	source->set_active_fx_slots(
		max_count,
		static_cast<const GUID*>(eax_call.property_value)
	);
}

void EaxxImpl::eax_set_source(
	const EaxCall& eax_call)
{
	switch (eax_call.property_id)
	{
		case EAXSOURCE_OBSTRUCTIONPARAMETERS:
			eax_set_source_obstructionparameters(eax_call);
			break;

		case EAXSOURCE_OCCLUSIONPARAMETERS:
			eax_set_source_occlusionparameters(eax_call);
			break;

		case EAXSOURCE_ROOM:
			eax_set_source_room(eax_call);
			break;

		case EAXSOURCE_OCCLUSION:
			eax_set_source_occlusion(eax_call);
			break;

		case EAXSOURCE_ACTIVEFXSLOTID:
			eax_set_source_activefxslotid(eax_call);
			break;

		default:
			throw EaxxImplSetEaxSourceException{"Unsupported property id."};
	}
}

void EaxxImpl::eax_get_fxslot_allparameters(
	const EaxCall& eax_call)
{
	if (eax_call.version < 4)
	{
		throw EaxxImplGetEaxFxSlotException{"Unsupported EAX version."};
	}

	if (eax_call.version == 5 && eax_call.property_value_size >= sizeof(EAX50FXSLOTPROPERTIES))
	{
		const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);

		*static_cast<EAX50FXSLOTPROPERTIES*>(eax_call.property_value) = fx_slot.get_eax_fx_slot();
	}
	else if (eax_call.version == 4 && eax_call.property_value_size >= sizeof(EAX40FXSLOTPROPERTIES))
	{
		const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);

		*static_cast<EAX40FXSLOTPROPERTIES*>(eax_call.property_value) = fx_slot.get_eax_fx_slot();
	}
	else
	{
		throw EaxxImplGetEaxFxSlotException{"Value size too small."};
	}
}

void EaxxImpl::eax_get_fxslot_loadeffect(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(GUID))
	{
		throw EaxxImplGetEaxFxSlotEffectException{"Value size too small."};
	}

	const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	const auto eax_fx_slot = fx_slot.get_eax_fx_slot();

	*static_cast<GUID*>(eax_call.property_value) = eax_fx_slot.guidLoadEffect;
}

void EaxxImpl::eax_get_fxslot_volume(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(long))
	{
		throw EaxxImplGetEaxFxSlotVolumeException{"Value size too small."};
	}

	const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	const auto eax_fx_slot = fx_slot.get_eax_fx_slot();

	*static_cast<long*>(eax_call.property_value) = eax_fx_slot.lVolume;
}

void EaxxImpl::eax_get_fxslot_lock(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(long))
	{
		throw EaxxImplGetEaxFxSlotLockException{"Value size too small."};
	}

	const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	const auto eax_fx_slot = fx_slot.get_eax_fx_slot();

	*static_cast<long*>(eax_call.property_value) = eax_fx_slot.lLock;
}

void EaxxImpl::eax_get_fxslot_flags(
	const EaxCall& eax_call)
{
	if (eax_call.property_value_size < sizeof(unsigned long))
	{
		throw EaxxImplGetEaxFxSlotFlagsException{"Value size too small."};
	}

	const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	const auto eax_fx_slot = fx_slot.get_eax_fx_slot();

	*static_cast<unsigned long*>(eax_call.property_value) = eax_fx_slot.ulFlags;
}

void EaxxImpl::eax_get_fxslot_occlusion(
	const EaxCall& eax_call)
{
	if (eax_call.version != 5)
	{
		throw EaxxImplGetEaxFxSlotOcclusionException{"Unsupported EAX version."};
	}

	if (eax_call.property_value_size < sizeof(long))
	{
		throw EaxxImplGetEaxFxSlotOcclusionException{"Value size too small."};
	}

	const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	const auto eax_fx_slot = fx_slot.get_eax_fx_slot();

	*static_cast<long*>(eax_call.property_value) = eax_fx_slot.lOcclusion;
}

void EaxxImpl::eax_get_fxslot_occlusion_lf_ratio(
	const EaxCall& eax_call)
{
	if (eax_call.version != 5)
	{
		throw EaxxImplGetEaxFxSlotOcclusionLfRatioException{"Unsupported EAX version."};
	}

	if (eax_call.property_value_size < sizeof(float))
	{
		throw EaxxImplGetEaxFxSlotOcclusionLfRatioException{"Value size too small."};
	}

	const auto& fx_slot = current_context_->get_slot(eax_call.fx_slot_index);
	const auto eax_fx_slot = fx_slot.get_eax_fx_slot();

	*static_cast<float*>(eax_call.property_value) = eax_fx_slot.flOcclusionLFRatio;
}

void EaxxImpl::eax_get_fxslot(
	const EaxCall& eax_call)
{
	switch (eax_call.property_id)
	{
		case EAXFXSLOT_ALLPARAMETERS:
			eax_get_fxslot_allparameters(eax_call);
			break;

		case EAXFXSLOT_LOADEFFECT:
			eax_get_fxslot_loadeffect(eax_call);
			break;

		case EAXFXSLOT_VOLUME:
			eax_get_fxslot_volume(eax_call);
			break;

		case EAXFXSLOT_LOCK:
			eax_get_fxslot_lock(eax_call);
			break;

		case EAXFXSLOT_FLAGS:
			eax_get_fxslot_flags(eax_call);
			break;

		case EAXFXSLOT_OCCLUSION:
			eax_get_fxslot_occlusion(eax_call);
			break;

		case EAXFXSLOT_OCCLUSIONLFRATIO:
			eax_get_fxslot_occlusion_lf_ratio(eax_call);
			break;

		default:
			throw EaxxImplGetEaxFxSlotException{"Unsupported property id."};
	}
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
