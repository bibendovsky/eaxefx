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
#include "eaxefx_utils.h"

#include "eaxefx_al_api.h"
#include "eaxefx_al_api_utils.h"
#include "eaxefx_al_low_pass_param.h"
#include "eaxefx_al_object.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_al_uresources.h"
#include "eaxefx_eax_api.h"
#include "eaxefx_eaxx_eax_call.h"
#include "eaxefx_eaxx_context.h"
#include "eaxefx_eaxx_fx_slot_index.h"
#include "eaxefx_eaxx_fx_slot.h"
#include "eaxefx_eaxx_fx_slots.h"
#include "eaxefx_eaxx_source.h"
#include "eaxefx_eaxx_validators.h"


namespace eaxefx
{


namespace
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

ALenum AL_APIENTRY EAXSet(
	const GUID* property_set_guid,
	ALuint property_id,
	ALuint property_al_name,
	ALvoid* property_buffer,
	ALuint property_size)
try
{
	const auto mutex_lock = g_al_api.get_lock();
	auto& al_api_context = g_al_api.get_current_context();
	auto& eaxx = al_api_context.get_eaxx();

	try
	{
		return eaxx.eax_set(
			property_set_guid,
			property_id,
			property_al_name,
			property_buffer,
			property_size
		);
	}
	catch (...)
	{
		eaxx.set_last_error();
		throw;
	}
}
catch (...)
{
	utils::log_exception(g_al_api.get_logger(), AlEaxSymbolsNames::EAXSet);
	return AL_INVALID_OPERATION;
}

ALenum AL_APIENTRY EAXGet(
	const GUID* property_set_guid,
	ALuint property_id,
	ALuint property_al_name,
	ALvoid* property_buffer,
	ALuint property_size)
try
{
	const auto mutex_lock = g_al_api.get_lock();
	auto& al_api_context = g_al_api.get_current_context();
	auto& eaxx = al_api_context.get_eaxx();

	try
	{
		return eaxx.eax_get(
			property_set_guid,
			property_id,
			property_al_name,
			property_buffer,
			property_size
		);
	}
	catch (...)
	{
		eaxx.set_last_error();
		throw;
	}
}
catch (...)
{
	utils::log_exception(g_al_api.get_logger(), AlEaxSymbolsNames::EAXGet);
	return AL_INVALID_OPERATION;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // namespace


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
		const EaxxCreateParam& param);


	void set_last_error() noexcept override;


	void* al_get_proc_address(
		std::string_view symbol_name) override;

	void al_gen_sources(
		ALsizei n,
		ALuint* sources) override;

	void al_delete_sources(
		ALsizei n,
		const ALuint* sources) override;


	ALenum eax_set(
		const GUID* property_set_guid,
		ALuint property_id,
		ALuint property_al_name,
		ALvoid* property_buffer,
		ALuint property_size) override;

	ALenum eax_get(
		const GUID* property_set_guid,
		ALuint property_id,
		ALuint property_al_name,
		ALvoid* property_buffer,
		ALuint property_size) override;


private:
	static constexpr auto al_exts_buffer_reserve = 2048;
	static constexpr auto al_context_attrs_reserve = 32;
	static constexpr auto al_devices_reserve = 4;
	static constexpr auto al_contexts_reserve = al_devices_reserve;


	using AlcAttrCache = std::vector<ALCint>;
	using AlExtsBuffer = String;
	using EaxxContextUPtr = std::unique_ptr<EaxxContext>;


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

	using DeviceMap = std::unordered_map<ALCdevice*, Device>;
	using ContextMap = std::unordered_map<ALCcontext*, EaxxContext>;


	Logger* logger_{};
	const AlEfxSymbols* al_efx_symbols_{};
	EaxxContextUPtr eaxx_context_{};
	bool is_dedicated_reverb_effect_activated_{};


	[[noreturn]]
	static void fail(
		const char* message);


	void activate_dedicated_reverb_effect();


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
	const EaxxCreateParam& param)
{
	logger_ = g_al_api.get_logger();

	if (!logger_)
	{
		fail("Null logger.");
	}

	al_efx_symbols_ = param.al_efx_symbols;

	if (!al_efx_symbols_)
	{
		fail("Null EFX symbols.");
	}

	auto al_alc_symbols = g_al_api.get_al_alc_symbols();

	if (!al_alc_symbols)
	{
		fail("Null ALC symbols.");
	}

	auto eaxx_create_param = EaxxContextCreateParam{};
	eaxx_create_param.al_efx_symbols = al_efx_symbols_;
	eaxx_context_ = std::make_unique<EaxxContext>(eaxx_create_param);
}

void EaxxImpl::set_last_error() noexcept
{
	if (eaxx_context_)
	{
		eaxx_context_->set_last_error();
	}
}

void* EaxxImpl::al_get_proc_address(
	std::string_view symbol_name)
{
	constexpr auto eax_get_view = std::string_view{AlEaxSymbolsNames::EAXGet};
	constexpr auto eax_set_view = std::string_view{AlEaxSymbolsNames::EAXSet};

	if (false)
	{
	}
	else if (symbol_name == eax_get_view)
	{
		return reinterpret_cast<void*>(eaxefx::EAXGet);
	}
	else if (symbol_name == eax_set_view)
	{
		return reinterpret_cast<void*>(eaxefx::EAXSet);
	}
	else
	{
		return nullptr;
	}
}

void EaxxImpl::al_gen_sources(
	ALsizei n,
	ALuint* sources)
{
	eaxx_context_->initialize_sources(n, sources);
}

void EaxxImpl::al_delete_sources(
	ALsizei n,
	const ALuint* sources)
{
	eaxx_context_->uninitialize_sources(n, sources);
}

ALenum EaxxImpl::eax_set(
	const GUID* property_set_guid,
	ALuint property_id,
	ALuint property_al_name,
	ALvoid* property_buffer,
	ALuint property_size)
{
	activate_dedicated_reverb_effect();

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
			fail("Unsupported property set id.");
	}

	return AL_NO_ERROR;
}

ALenum EaxxImpl::eax_get(
	const GUID* property_set_guid,
	ALuint property_id,
	ALuint property_al_name,
	ALvoid* property_buffer,
	ALuint property_size)
{
	activate_dedicated_reverb_effect();

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
			fail("Unsupported property set id.");
	}

	return AL_NO_ERROR;
}

[[noreturn]]
void EaxxImpl::fail(
	const char* message)
{
	throw EaxxImplException{message};
}

void EaxxImpl::activate_dedicated_reverb_effect()
{
	if (is_dedicated_reverb_effect_activated_)
	{
		return;
	}

	is_dedicated_reverb_effect_activated_ = true;

	eaxx_context_->activate_dedicated_reverb_effect();
}

void EaxxImpl::dispatch_context(
	const EaxxEaxCall& eax_call)
{
	eaxx_context_->dispatch(eax_call);
}

void EaxxImpl::dispatch_fxslot(
	const EaxxEaxCall& eax_call)
{
	auto& fx_slot = eaxx_context_->get_slot(eax_call.get_fx_slot_index());

	if (fx_slot.dispatch(eax_call))
	{
		eaxx_context_->update_filters();
	}
}

void EaxxImpl::dispatch_source(
	const EaxxEaxCall& eax_call)
{
	auto source = eaxx_context_->find_source(eax_call.get_property_al_name());

	if (!source)
	{
		fail("Source not found.");
	}

	source->dispatch(eax_call);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EaxxUPtr make_eaxx(
	const EaxxCreateParam& param)
{
	return std::make_unique<EaxxImpl>(param);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
