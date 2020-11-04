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


#include "eaxefx_eaxx_source.h"

#include <algorithm>
#include <memory>
#include <exception>
#include <string_view>

#include "eaxefx_al_symbols.h"
#include "eaxefx_eax_api.h"
#include "eaxefx_exception.h"
#include "eaxefx_unit_converters.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxSourceException :
	public Exception
{
public:
	explicit EaxxSourceException(
		std::string_view message)
		:
		Exception{"EAXEFX_EAXX_SOURCE", message}
	{
	}
}; // EaxxSourceException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EaxxSource::EaxxSource(
	const EaxxSourceInitParam& param)
try
{
	initialize(param);
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxSourceException{"Failed to initialize."});
}

void EaxxSource::set_obstruction_all(
	const EAXOBSTRUCTIONPROPERTIES& eax_obstruction_properties)
{
	if (eax_.source.lObstruction == eax_obstruction_properties.lObstruction &&
		eax_.source.flObstructionLFRatio == eax_obstruction_properties.flObstructionLFRatio)
	{
		return;
	}

	eax_.source.lObstruction = eax_obstruction_properties.lObstruction;
	eax_.source.flObstructionLFRatio = eax_obstruction_properties.flObstructionLFRatio;

	update_direct_filter_internal();
}

void EaxxSource::set_occlusion_all(
	const EAXOCCLUSIONPROPERTIES& eax_occlusion_properties)
{
	if (eax_.source.lOcclusion == eax_occlusion_properties.lOcclusion &&
		eax_.source.flOcclusionLFRatio == eax_occlusion_properties.flOcclusionLFRatio &&
		eax_.source.flOcclusionRoomRatio == eax_occlusion_properties.flOcclusionRoomRatio &&
		eax_.source.flOcclusionDirectRatio == eax_occlusion_properties.flOcclusionDirectRatio)
	{
		return;
	}

	eax_.source.lOcclusion = eax_occlusion_properties.lOcclusion;
	eax_.source.flOcclusionLFRatio = eax_occlusion_properties.flOcclusionLFRatio;
	eax_.source.flOcclusionRoomRatio = eax_occlusion_properties.flOcclusionRoomRatio;
	eax_.source.flOcclusionDirectRatio = eax_occlusion_properties.flOcclusionDirectRatio;

	update_filters_internal();
}

void EaxxSource::set_room(
	long eax_room)
{
	if (eax_.source.lRoom == eax_room)
	{
		return;
	}

	eax_.source.lRoom = eax_room;

	update_filters_internal();
}

void EaxxSource::set_occlusion(
	long eax_occlusion)
{
	if (eax_.source.lOcclusion == eax_occlusion)
	{
		return;
	}

	eax_.source.lOcclusion = eax_occlusion;

	update_filters_internal();
}

void EaxxSource::set_exclusion(
	long eax_exclusion)
{
	if (eax_.source.lExclusion == eax_exclusion)
	{
		return;
	}

	eax_.source.lExclusion = eax_exclusion;

	update_room_filters_internal();
}

void EaxxSource::set_flags(
	unsigned long eax_flags)
{
	if (eax_.source.ulFlags == eax_flags)
	{
		return;
	}

	eax_.source.ulFlags = eax_flags;

	set_flags();
}

void EaxxSource::set_active_fx_slots(
	int count,
	const GUID* eax_ids)
try
{
	if (count <= 0 || count > EAX_MAX_FXSLOTS)
	{
		throw EaxxSourceException{"FX slot count out of range."};
	}

	if (eax_ids == nullptr)
	{
		throw EaxxSourceException{"Null FX slot id list."};
	}


	auto new_eax_active_fx_slots = EAX50ACTIVEFXSLOTS{};

	std::uninitialized_copy_n(
		eax_ids,
		count,
		new_eax_active_fx_slots.guidActiveFXSlots
	);

	auto new_active_fx_slots = ActiveFxSlots{};

	auto new_count = 0;
	auto new_has_primary_id = false;

	for (const auto& eax_id : new_eax_active_fx_slots.guidActiveFXSlots)
	{
		auto is_primary_id = false;

		if (eax_id == EAX_PrimaryFXSlotID)
		{
			is_primary_id = true;

			if (!new_has_primary_id)
			{
				new_count += 1;
			}

			new_has_primary_id = true;
		}

		const auto fx_slot_index_opt = fx_slots_->resolve_fx_slot_index(eax_id);

		if (fx_slot_index_opt.has_value())
		{
			const auto fx_slot_index = fx_slot_index_opt.value();

			if (!new_active_fx_slots[fx_slot_index])
			{
				if (!is_primary_id)
				{
					new_count += 1;
				}

				new_active_fx_slots[fx_slot_index] = true;
			}
		}
	}

	if (new_count > fx_slots_->get_max_active_count())
	{
		throw EaxxSourceException{"Too many active FX slots."};
	}

	for (auto i = 0; i < EAX_MAX_FXSLOTS; ++i)
	{
		if (active_fx_slots_[i] != new_active_fx_slots[i] &&
			!new_active_fx_slots[i])
		{
			alSource3i_(al_.source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, i, AL_FILTER_NULL);
		}
	}

	has_active_fx_slots_ = new_count > 0;
	uses_primary_id_ = new_has_primary_id;
	eax_.active_fx_slots = new_eax_active_fx_slots;
	active_fx_slots_ = new_active_fx_slots;

	update_filters_internal();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxSourceException{"Failed to set active FX slots."});
}

void EaxxSource::set(
	const EaxxEaxCall& eax_call)
{
	switch (eax_call.property_id)
	{
		case EAXSOURCE_OBSTRUCTIONPARAMETERS:
			set_obstruction_all(eax_call);
			break;

		case EAXSOURCE_OCCLUSIONPARAMETERS:
			set_occlusion_all(eax_call);
			break;

		case EAXSOURCE_ROOM:
			set_room(eax_call);
			break;

		case EAXSOURCE_OCCLUSION:
			set_occlusion(eax_call);
			break;

		case EAXSOURCE_EXCLUSION:
			set_exclusion(eax_call);
			break;

		case EAXSOURCE_FLAGS:
			set_flags(eax_call);
			break;

		case EAXSOURCE_ACTIVEFXSLOTID:
			set_active_fx_slots(eax_call);
			break;

		default:
			throw EaxxSourceException{"Unsupported property id."};
	}
}

void EaxxSource::update_filters()
try
{
	update_filters_internal();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxSourceException{"Failed to update filters."});
}

void EaxxSource::on_set_primary_fx_slot_id()
{
	if (uses_primary_id_)
	{
		set_active_fx_slots(EAX_MAX_FXSLOTS, eax_.active_fx_slots.guidActiveFXSlots);
	}
}

void EaxxSource::validate_init_param(
	const EaxxSourceInitParam& param)
{
	if (param.al_source == 0)
	{
		throw EaxxSourceException{"Null AL source."};
	}

	if (param.al_filter == 0)
	{
		throw EaxxSourceException{"Null EFX filter."};
	}

	if (param.fx_slots == nullptr)
	{
		throw EaxxSourceException{"Null FX slots."};
	}
}

void EaxxSource::copy_init_param(
	const EaxxSourceInitParam& param)
{
	al_.source = param.al_source;
	al_.filter = param.al_filter;
	fx_slots_ = param.fx_slots;
}

void EaxxSource::set_eax_source_defaults()
{
	eax_.source.lDirect = EAXSOURCE_DEFAULTDIRECT;
	eax_.source.lDirectHF = EAXSOURCE_DEFAULTDIRECTHF;
	eax_.source.lRoom = EAXSOURCE_DEFAULTROOM;
	eax_.source.lRoomHF = EAXSOURCE_DEFAULTROOMHF;
	eax_.source.lObstruction = EAXSOURCE_DEFAULTOBSTRUCTION;
	eax_.source.flObstructionLFRatio = EAXSOURCE_DEFAULTOBSTRUCTIONLFRATIO;
	eax_.source.lOcclusion = EAXSOURCE_DEFAULTOCCLUSION;
	eax_.source.flOcclusionLFRatio = EAXSOURCE_DEFAULTOCCLUSIONLFRATIO;
	eax_.source.flOcclusionRoomRatio = EAXSOURCE_DEFAULTOCCLUSIONROOMRATIO;
	eax_.source.flOcclusionDirectRatio = EAXSOURCE_DEFAULTOCCLUSIONDIRECTRATIO;
	eax_.source.lExclusion = EAXSOURCE_DEFAULTEXCLUSION;
	eax_.source.flExclusionLFRatio = EAXSOURCE_DEFAULTEXCLUSIONLFRATIO;
	eax_.source.lOutsideVolumeHF = EAXSOURCE_DEFAULTOUTSIDEVOLUMEHF;
	eax_.source.flDopplerFactor = EAXSOURCE_DEFAULTDOPPLERFACTOR;
	eax_.source.flRolloffFactor = EAXSOURCE_DEFAULTROLLOFFFACTOR;
	eax_.source.flRoomRolloffFactor = EAXSOURCE_DEFAULTROOMROLLOFFFACTOR;
	eax_.source.flAirAbsorptionFactor = EAXSOURCE_DEFAULTAIRABSORPTIONFACTOR;
	eax_.source.ulFlags = EAXSOURCE_DEFAULTFLAGS;
}

void EaxxSource::set_eax_active_fx_slots_defaults()
{
	eax_.active_fx_slots = EAX50SOURCE_3DDEFAULTACTIVEFXSLOTID;
}

void EaxxSource::set_eax_defaults()
{
	set_eax_source_defaults();
	set_eax_active_fx_slots_defaults();
}

float EaxxSource::calculate_dst_occlusion_mb(
	long src_occlusion_mb,
	float path_ratio,
	float lf_ratio) noexcept
{
	const auto ratio_1 = path_ratio * lf_ratio;
	const auto ratio_2 = path_ratio + lf_ratio - 1.0F;
	const auto ratio = ratio_1 <= ratio_2 ? ratio_2 : ratio_1;
	const auto dst_occlustion_mb = src_occlusion_mb * ratio;

	return dst_occlustion_mb;
}

AlLowPassParam EaxxSource::make_direct_filter() const noexcept
{
	const auto occlusion_mb = calculate_dst_occlusion_mb(
		eax_.source.lOcclusion,
		eax_.source.flOcclusionDirectRatio,
		eax_.source.flOcclusionLFRatio
	);

	auto al_low_pass_param = AlLowPassParam{};

	al_low_pass_param.gain =
		std::clamp(
			mb_to_gain(
				eax_.source.lDirect +
				(eax_.source.lObstruction * eax_.source.flObstructionLFRatio) +
				occlusion_mb
			),
			0.0F,
			1.0F
		);

	al_low_pass_param.gain_hf =
		std::clamp(
			mb_to_gain(
				eax_.source.lDirectHF +
				eax_.source.lObstruction +
				(eax_.source.lOcclusion * eax_.source.flOcclusionDirectRatio)
			),
			0.0F,
			1.0F
		);

	return al_low_pass_param;
}

AlLowPassParam EaxxSource::make_room_filter(
	const EaxxFxSlot& fx_slot) const noexcept
{
	const auto& fx_slot_eax = fx_slot.get_eax_fx_slot();

	const auto occlusion_mb = calculate_dst_occlusion_mb(
		eax_.source.lOcclusion,
		eax_.source.flOcclusionRoomRatio,
		eax_.source.flOcclusionLFRatio
	);

	auto al_low_pass_param = AlLowPassParam{};

	al_low_pass_param.gain =
		std::clamp(
			mb_to_gain(
				eax_.source.lRoom +
				(eax_.source.lExclusion * eax_.source.flExclusionLFRatio) +
				occlusion_mb
			),
			0.0F,
			1.0F
		);

	al_low_pass_param.gain_hf =
		std::clamp(
			mb_to_gain(
				eax_.source.lRoomHF +
				eax_.source.lExclusion +
				((fx_slot_eax.lOcclusion + eax_.source.lOcclusion) * eax_.source.flOcclusionRoomRatio)
			),
			0.0F,
			1.0F
		);

	return al_low_pass_param;
}

void EaxxSource::set_al_filter_parameters(
	const AlLowPassParam& al_low_pass_param) const noexcept
{
	alFilterf_(al_.filter, AL_LOWPASS_GAIN, al_low_pass_param.gain);
	alFilterf_(al_.filter, AL_LOWPASS_GAINHF, al_low_pass_param.gain_hf);
}

void EaxxSource::initialize_fx_slots()
{
	uses_primary_id_ = false;
	has_active_fx_slots_ = false;

	for (auto i = 0; i < EAX_MAX_FXSLOTS; ++i)
	{
		const auto& eax_active_fx_slot_id = eax_.active_fx_slots.guidActiveFXSlots[i];

		if (eax_active_fx_slot_id == EAX_PrimaryFXSlotID)
		{
			uses_primary_id_ = true;
		}

		const auto fx_slot_index_opt = fx_slots_->resolve_fx_slot_index(eax_active_fx_slot_id);

		if (fx_slot_index_opt.has_value())
		{
			has_active_fx_slots_ = true;
			active_fx_slots_[fx_slot_index_opt.value()] = true;
		}
	}

	update_filters_internal();
}

void EaxxSource::initialize(
	const EaxxSourceInitParam& param)
{
	is_initialized_ = false;

	validate_init_param(param);
	copy_init_param(param);
	set_eax_defaults();
	initialize_fx_slots();

	is_initialized_ = true;
}

void EaxxSource::update_direct_filter_internal()
{
	const auto& direct_param = make_direct_filter();
	set_al_filter_parameters(direct_param);
	alSourcei_(al_.source, AL_DIRECT_FILTER, al_.filter);
}

void EaxxSource::update_room_filters_internal()
{
	if (!has_active_fx_slots_)
	{
		return;
	}

	for (auto i = 0; i < EAX_MAX_FXSLOTS; ++i)
	{
		if (active_fx_slots_[i])
		{
			const auto& fx_slot = fx_slots_->get(i);
			const auto& room_param = make_room_filter(fx_slot);
			const auto efx_effect_slot = fx_slot.get_efx_effect_slot();
			set_al_filter_parameters(room_param);
			alSource3i_(al_.source, AL_AUXILIARY_SEND_FILTER, efx_effect_slot, i, al_.filter);
		}
	}
}

void EaxxSource::update_filters_internal()
{
	update_direct_filter_internal();
	update_room_filters_internal();
}

void EaxxSource::set_direct_hf_auto_flag()
{
	const auto is_enable = (eax_.source.ulFlags & EAXSOURCEFLAGS_DIRECTHFAUTO) != 0;
	alSourcei(al_.source, AL_DIRECT_FILTER_GAINHF_AUTO, is_enable);
}

void EaxxSource::set_room_auto_flag()
{
	const auto is_enable = (eax_.source.ulFlags & EAXSOURCEFLAGS_ROOMAUTO) != 0;
	alSourcei(al_.source, AL_AUXILIARY_SEND_FILTER_GAIN_AUTO, is_enable);
}

void EaxxSource::set_room_hf_auto_flag()
{
	const auto is_enable = (eax_.source.ulFlags & EAXSOURCEFLAGS_ROOMHFAUTO) != 0;
	alSourcei(al_.source, AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO, is_enable);
}

void EaxxSource::set_flags()
{
	set_direct_hf_auto_flag();
	set_room_auto_flag();
	set_room_hf_auto_flag();
}

void EaxxSource::set_obstruction_all(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_obstruction_all =
		eax_call.get_value<EaxxSourceException, const EAXOBSTRUCTIONPROPERTIES>();

	set_obstruction_all(eax_obstruction_all);
}

void EaxxSource::set_occlusion_all(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_occlusion_all =
		eax_call.get_value<EaxxSourceException, const EAXOCCLUSIONPROPERTIES>();

	set_occlusion_all(eax_occlusion_all);
}

void EaxxSource::set_room(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_room =
		eax_call.get_value<EaxxSourceException, const decltype(EAX40SOURCEPROPERTIES::lRoom)>();

	set_room(eax_room);
}

void EaxxSource::set_occlusion(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_occlusion =
		eax_call.get_value<EaxxSourceException, const decltype(EAX40SOURCEPROPERTIES::lOcclusion)>();

	set_occlusion(eax_occlusion);
}

void EaxxSource::set_exclusion(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_exclusion =
		eax_call.get_value<EaxxSourceException, const decltype(EAX40SOURCEPROPERTIES::lExclusion)>();

	set_exclusion(eax_exclusion);
}

void EaxxSource::set_flags(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_flags =
		eax_call.get_value<EaxxSourceException, const decltype(EAX40SOURCEPROPERTIES::ulFlags)>();

	set_flags(eax_flags);
}

void EaxxSource::set_active_fx_slots(
	const EaxxEaxCall& eax_call)
{
	const auto eax_active_fx_slots = eax_call.get_values<EaxxSourceException, const GUID>();

	set_active_fx_slots(eax_active_fx_slots.size, eax_active_fx_slots.values);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
