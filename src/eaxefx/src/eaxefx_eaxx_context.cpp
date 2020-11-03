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


#include "eaxefx_eaxx_context.h"

#include <exception>
#include <string_view>

#include "eaxefx_exception.h"
#include "eaxefx_not_null.h"

#include "eaxefx_al_object.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eax_validators.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxContextException :
	public Exception
{
public:
	explicit EaxxContextException(
		std::string_view message)
		:
		Exception{"EAXEFX_EAXX_CONTEXT", message}
	{
	}
}; // EaxxContextException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxContextNullDeviceException :
	public EaxxContextException
{
public:
	EaxxContextNullDeviceException()
		:
		EaxxContextException{"Null AL device."}
	{
	}
}; // EaxxContextNullDeviceException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxContextNullContextException :
	public EaxxContextException
{
public:
	EaxxContextNullContextException()
		:
		EaxxContextException{"Null AL context."}
	{
	}
}; // EaxxContextNullContextException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EaxxContext::EaxxContext(
	ALCdevice* al_device,
	ALCcontext* al_context)
{
	al_.device = not_null<EaxxContextNullDeviceException>(al_device);
	al_.context = not_null<EaxxContextNullContextException>(al_context);
}

bool EaxxContext::is_tried_to_initialize() const noexcept
{
	return is_tried_to_initialize_;
}

void EaxxContext::initialize()
try
{
	if (is_tried_to_initialize_)
	{
		throw EaxxContextException{"Already tried to initialize."};
	}

	is_tried_to_initialize_ = true;

	ensure_compatibility();
	initialize_filter();
	set_eax_defaults();
	initialize_fx_slots();

	is_initialized_ = true;
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxContextException{"Failed to initialize."});
}

bool EaxxContext::is_initialized() const noexcept
{
	return is_initialized_;
}

ALCcontext* EaxxContext::get_al_context() const noexcept
{
	return al_.context;
}

EaxxFxSlot& EaxxContext::get_slot(
	int fx_slot_index)
{
	return fx_slots_.get(fx_slot_index);
}

void EaxxContext::initialize_sources(
	ALsizei count,
	ALuint* al_names)
try
{
	auto param = EaxxSourceInitParam{};
	param.al_filter = al_.filter;
	param.fx_slots = &fx_slots_;

	for (auto i = ALsizei{}; i < count; ++i)
	{
		param.al_source = al_names[i];

		source_map_.emplace(
			param.al_source,
			EaxxSource{param}
		);
	}
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxContextException{"Failed to initialize sources."});
}

void EaxxContext::uninitialize_sources(
	ALsizei count,
	const ALuint* al_names)
try
{
	for (auto i = ALsizei{}; i < count; ++i)
	{
		const auto al_name = al_names[i];

		if (alIsSource_(al_name) == AL_FALSE)
		{
			return;
		}
	}

	for (auto i = ALsizei{}; i < count; ++i)
	{
		const auto al_name = al_names[i];

		source_map_.erase(al_name);
	}
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxContextException{"Failed to uninitialize sources."});
}

void EaxxContext::set_primary_fx_slot_id(
	const GUID& eax_fx_slot_id)
{
	EaxContextValidator::primary_fx_slot_id(eax_fx_slot_id);

	if (fx_slots_.get_primary_id() == eax_fx_slot_id)
	{
		return;
	}

	fx_slots_.set_primary(eax_fx_slot_id);

	set_primary_fx_slot_id();
}

void EaxxContext::set_context(
	const EAX50CONTEXTPROPERTIES& eax_context)
{
	EaxContextValidator::all(eax_context);

	if (eax_.context == eax_context)
	{
		return;
	}

	eax_.context = eax_context;

	set_context();
}

void EaxxContext::set_session(
	const EAXSESSIONPROPERTIES& eax_session)
{
	EaxSessionValidator::all(eax_session);

	eax_.session.ulEAXVersion = eax_session.ulEAXVersion;
	eax_.session.ulMaxActiveSends = eax_session.ulMaxActiveSends;

	fx_slots_.set_max_active_count(eax_.session.ulMaxActiveSends);
}

EaxxSource* EaxxContext::find_source(
	ALuint al_source_name)
{
	const auto map_it = source_map_.find(al_source_name);

	if (map_it != source_map_.cend())
	{
		return &map_it->second;
	}

	return nullptr;
}

void EaxxContext::update_filters()
{
	for (auto& [source_key, source_value] : source_map_)
	{
		source_value.update_filters();
	}
}

void EaxxContext::ensure_compatibility()
{
	const auto has_efx_extension = alcIsExtensionPresent_(al_.device, ALC_EXT_EFX_NAME);

	if (!has_efx_extension)
	{
		throw EaxxContextException{"EFX extension not found."};
	}

	auto aux_send_count = ALint{};

	alcGetIntegerv_(al_.device, ALC_MAX_AUXILIARY_SENDS, 1, &aux_send_count);

	if (aux_send_count < EAX_MAX_FXSLOTS)
	{
		const auto& message =
			"Expected at least " +
			std::to_string(EAX_MAX_FXSLOTS) +
			" EFX auxiliary effect slots.";

		throw EaxxContextException{message};
	}

	const auto low_pass_efx_object = make_efx_filter_object();
	const auto low_pass_al_name = low_pass_efx_object.get();
	auto efx_filter_type = ALint{};
	alFilteri_(low_pass_al_name, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
	alGetFilteri_(low_pass_al_name, AL_FILTER_TYPE, &efx_filter_type);

	if (efx_filter_type != AL_FILTER_LOWPASS)
	{
		throw EaxxContextException{"EFX low-pass filter not supported."};
	}

	auto eaxreverb_efx_object = make_efx_effect_object();
	const auto eaxreverb_al_name = eaxreverb_efx_object.get();
	auto efx_effect_type = ALint{};
	alGetEffecti_(eaxreverb_al_name, AL_EFFECT_TYPE, &efx_effect_type);

	if (efx_effect_type != AL_EFFECT_NULL)
	{
		throw EaxxContextException{"EFX null effect not supported."};
	}

	alEffecti_(eaxreverb_al_name, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
	alGetEffecti_(eaxreverb_al_name, AL_EFFECT_TYPE, &efx_effect_type);

	if (efx_effect_type != AL_EFFECT_EAXREVERB)
	{
		throw EaxxContextException{"EFX EAX-reverb not supported."};
	}
}

void EaxxContext::set_eax_session_defaults()
{
	eax_.session.ulEAXVersion = EAXCONTEXT_MINEAXSESSION;
	eax_.session.ulMaxActiveSends = EAXCONTEXT_DEFAULTMAXACTIVESENDS;
}

void EaxxContext::set_eax_context_defaults()
{
	eax_.context.guidPrimaryFXSlotID = EAXCONTEXT_DEFAULTPRIMARYFXSLOTID;
	eax_.context.flDistanceFactor = EAXCONTEXT_DEFAULTDISTANCEFACTOR;
	eax_.context.flAirAbsorptionHF = EAXCONTEXT_DEFAULTAIRABSORPTIONHF;
	eax_.context.flHFReference = EAXCONTEXT_DEFAULTHFREFERENCE;
}

void EaxxContext::set_eax_defaults()
{
	set_eax_session_defaults();
	set_eax_context_defaults();
}

void EaxxContext::initialize_filter()
{
	al_.filter = make_efx_filter_object().release();
	const auto al_filter = al_.filter;

	alFilteri_(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
	auto al_filter_type = ALint{};
	alGetFilteri(al_filter, AL_FILTER_TYPE, &al_filter_type);

	if (al_filter_type != AL_FILTER_LOWPASS)
	{
		throw EaxxContextException{"Failed to set EFX filter type to low-pass."};
	}
}

void EaxxContext::set_primary_fx_slot_id()
{
	for (auto& [source_key, source_value] : source_map_)
	{
		source_value.on_set_primary_fx_slot_id();
	}
}

void EaxxContext::set_distance_factor()
{
	// TODO
}

void EaxxContext::set_air_absorbtion_hf()
{
	// TODO
}

void EaxxContext::set_hf_reference()
{
	// TODO
}

void EaxxContext::set_context()
{
	set_primary_fx_slot_id();
	set_distance_factor();
	set_air_absorbtion_hf();
	set_hf_reference();
}

void EaxxContext::initialize_fx_slots()
{
	fx_slots_.initialize();
	fx_slots_.set_primary(eax_.context.guidPrimaryFXSlotID);
	fx_slots_.set_max_active_count(eax_.session.ulMaxActiveSends);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
