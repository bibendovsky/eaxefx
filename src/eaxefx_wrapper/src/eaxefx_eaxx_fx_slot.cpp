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


#include "eaxefx_eaxx_fx_slot.h"

#include <cassert>

#include <algorithm>

#include "eaxefx_exception.h"
#include "eaxefx_unit_converters.h"

#include "eaxefx_al_object.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eaxx_validators.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxFxSlotException :
	public Exception
{
public:
	explicit EaxxFxSlotException(
		const char* message)
		:
		Exception{"EAXX_FX_SLOT", message}
	{
	}
}; // EaxxFxSlotException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxxFxSlot::initialize(
	int index,
	const AlEfxSymbols* al_efx_symbols)
{
	if (index < 0 || index >= ::EAX_MAX_FXSLOTS)
	{
		fail("Index out of range.");
	}

	al_efx_symbols_ = al_efx_symbols;

	if (!al_efx_symbols_)
	{
		fail("Null EFX symbols.");
	}

	index_ = index;

	initialize_eax();
	initialize_efx();
	initialize_effects();
	set_dedicated_defaults();
}

void EaxxFxSlot::activate_dedicated_reverb_effect()
{
	set_fx_slot_effect(EaxxEffectType::eax_reverb, eax_reverb_effect_);
}

::ALuint EaxxFxSlot::get_efx_effect_slot() const noexcept
{
	return efx_.effect_slot;
}

const EAX50FXSLOTPROPERTIES& EaxxFxSlot::get_eax_fx_slot() const noexcept
{
	return eax_.fx_slot;
}

void EaxxFxSlot::validate_fx_slot_effect(
	const ::GUID& eax_effect_id)
{
	if (eax_effect_id != ::EAX_NULL_GUID &&
		eax_effect_id != ::EAX_REVERB_EFFECT)
	{
		fail("Unsupported EAX effect GUID.");
	}
}

void EaxxFxSlot::validate_fx_slot_volume(
	std::int32_t eax_volume)
{
	eaxx_validate_range<EaxxFxSlotException>(
		"Volume",
		eax_volume,
		::EAXFXSLOT_MINVOLUME,
		::EAXFXSLOT_MAXVOLUME
	);
}

void EaxxFxSlot::validate_fx_slot_lock(
	std::int32_t eax_lock)
{
	eaxx_validate_range<EaxxFxSlotException>(
		"Lock",
		eax_lock,
		::EAXFXSLOT_MINLOCK,
		::EAXFXSLOT_MAXLOCK
	);
}

void EaxxFxSlot::validate_fx_slot_lock_state(
	std::int32_t eax_lock,
	const ::GUID& eax_effect_id)
{
	if (eax_lock == ::EAXFXSLOT_LOCKED && eax_effect_id != eax_.fx_slot.guidLoadEffect)
	{
		fail("Loading effect while slot is locked forbidden.");
	}
}

void EaxxFxSlot::validate_fx_slot_flags(
	std::uint32_t eax_flags,
	int eax_version)
{
	eaxx_validate_range<EaxxFxSlotException>(
		"Flags",
		eax_flags,
		0U,
		~(eax_version == 4 ? ::EAX40FXSLOTFLAGS_RESERVED : ::EAX50FXSLOTFLAGS_RESERVED)
	);
}

void EaxxFxSlot::validate_fx_slot_occlusion(
	std::int32_t eax_occlusion)
{
	eaxx_validate_range<EaxxFxSlotException>(
		"Occlusion",
		eax_occlusion,
		::EAXFXSLOT_MINOCCLUSION,
		::EAXFXSLOT_MAXOCCLUSION
	);
}

void EaxxFxSlot::validate_fx_slot_occlusion_lf_ratio(
	float eax_occlusion_lf_ratio)
{
	eaxx_validate_range<EaxxFxSlotException>(
		"Occlusion LF Ratio",
		eax_occlusion_lf_ratio,
		::EAXFXSLOT_MINOCCLUSIONLFRATIO,
		::EAXFXSLOT_MAXOCCLUSIONLFRATIO
	);
}

void EaxxFxSlot::validate_fx_slot_all(
	const EAX40FXSLOTPROPERTIES& fx_slot,
	int eax_version)
{
	validate_fx_slot_effect(fx_slot.guidLoadEffect);
	validate_fx_slot_volume(fx_slot.lVolume);
	validate_fx_slot_lock(fx_slot.lLock);
	validate_fx_slot_flags(fx_slot.ulFlags, eax_version);
}

void EaxxFxSlot::validate_fx_slot_all(
	const EAX50FXSLOTPROPERTIES& fx_slot,
	int eax_version)
{
	validate_fx_slot_all(static_cast<const EAX40FXSLOTPROPERTIES&>(fx_slot), eax_version);

	validate_fx_slot_occlusion(fx_slot.lOcclusion);
	validate_fx_slot_occlusion_lf_ratio(fx_slot.flOcclusionLFRatio);
}

void EaxxFxSlot::set_fx_slot_effect(
	const ::GUID& eax_effect_id)
{
	if (eax_.fx_slot.guidLoadEffect == eax_effect_id)
	{
		return;
	}

	eax_.fx_slot.guidLoadEffect = eax_effect_id;

	set_fx_slot_effect();
}

void EaxxFxSlot::set_fx_slot_volume(
	std::int32_t eax_volume)
{
	if (eax_.fx_slot.lVolume == eax_volume)
	{
		return;
	}

	eax_.fx_slot.lVolume = eax_volume;

	set_fx_slot_volume();
}

void EaxxFxSlot::set_fx_slot_lock(
	std::int32_t eax_lock)
{
	if (eax_.fx_slot.lLock == eax_lock)
	{
		return;
	}

	eax_.fx_slot.lLock = eax_lock;
}

void EaxxFxSlot::set_fx_slot_flags(
	std::uint32_t eax_flags)
{
	if (eax_.fx_slot.ulFlags == eax_flags)
	{
		return;
	}

	eax_.fx_slot.ulFlags = eax_flags;

	set_fx_slot_flags();
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_occlusion(
	std::int32_t eax_occlusion)
{
	if (eax_.fx_slot.lOcclusion == eax_occlusion)
	{
		return false;
	}

	eax_.fx_slot.lOcclusion = eax_occlusion;

	return true;
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_occlusion_lf_ratio(
	float eax_occlusion_lf_ratio)
{
	if (eax_.fx_slot.flOcclusionLFRatio == eax_occlusion_lf_ratio)
	{
		return false;
	}

	eax_.fx_slot.flOcclusionLFRatio = eax_occlusion_lf_ratio;

	return true;
}

void EaxxFxSlot::set_fx_slot_all(
	const EAX40FXSLOTPROPERTIES& eax_fx_slot)
{
	set_fx_slot_effect(eax_fx_slot.guidLoadEffect);
	set_fx_slot_volume(eax_fx_slot.lVolume);
	set_fx_slot_lock(eax_fx_slot.lLock);
	set_fx_slot_flags(eax_fx_slot.ulFlags);
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_all(
	const EAX50FXSLOTPROPERTIES& eax_fx_slot)
{
	set_fx_slot_all(static_cast<const EAX40FXSLOTPROPERTIES&>(eax_fx_slot));

	const auto is_occlusion_modified = set_fx_slot_occlusion(eax_fx_slot.lOcclusion);
	const auto is_occlusion_lf_ratio_modified = set_fx_slot_occlusion_lf_ratio(eax_fx_slot.flOcclusionLFRatio);

	return is_occlusion_modified || is_occlusion_lf_ratio_modified;
}

[[nodiscard]] bool EaxxFxSlot::dispatch(
	const EaxxEaxCall& eax_call)
{
	if (eax_call.is_get())
	{
		get(eax_call);
		return false;
	}
	else
	{
		return set(eax_call);
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

[[noreturn]]
void EaxxFxSlot::fail(
	const char* message)
{
	throw EaxxFxSlotException{message};
}

void EaxxFxSlot::fail_set_if_dedicated()
{
	if (index_ < dedicated_count)
	{
		fail("Changing \"effect\" or \"lock\" on dedicated slot not supported.");
	}
}

void EaxxFxSlot::set_eax_fx_slot_defaults()
{
	eax_.fx_slot.guidLoadEffect = ::EAX_NULL_GUID;
	eax_.fx_slot.lVolume = ::EAXFXSLOT_DEFAULTVOLUME;
	eax_.fx_slot.lLock = ::EAXFXSLOT_UNLOCKED;
	eax_.fx_slot.ulFlags = ::EAX50FXSLOT_DEFAULTFLAGS;
	eax_.fx_slot.lOcclusion = ::EAXFXSLOT_DEFAULTOCCLUSION;
	eax_.fx_slot.flOcclusionLFRatio = ::EAXFXSLOT_DEFAULTOCCLUSIONLFRATIO;
}

void EaxxFxSlot::initialize_eax()
{
	set_eax_fx_slot_defaults();
}

void EaxxFxSlot::create_efx_effect_slot()
{
	efx_.effect_slot = make_efx_effect_slot_object(al_efx_symbols_).release();
}

void EaxxFxSlot::create_efx_objects()
{
	create_efx_effect_slot();
}

void EaxxFxSlot::initialize_efx()
{
	create_efx_objects();
}

EaxxEffectUPtr EaxxFxSlot::create_effect(
	EaxxEffectType effect_type)
{
	auto effect_param = EaxxEffectParam{};
	effect_param.effect_type = effect_type;
	effect_param.al_effect_slot = efx_.effect_slot;
	effect_param.al_efx_symbols = al_efx_symbols_;
	return make_eaxx_effect(effect_param);
}

void EaxxFxSlot::initialize_effects()
{
	set_fx_slot_effect();
}

void EaxxFxSlot::set_dedicated_0_defaults()
{
	set_fx_slot_effect(::EAX_REVERB_EFFECT);
	deactivate_dedicated_reverb_effect();
}

void EaxxFxSlot::set_dedicated_1_defaults()
{
	set_fx_slot_effect(::EAX_CHORUS_EFFECT);
}

void EaxxFxSlot::set_dedicated_defaults()
{
	switch (index_)
	{
		case 0:
			set_dedicated_0_defaults();
			break;

		case 1:
			set_dedicated_1_defaults();
			break;

		case 2:
		case 3:
			break;

		default:
			fail("FX slot index out of range.");
	}
}

void EaxxFxSlot::deactivate_dedicated_reverb_effect()
{
	set_fx_slot_effect(EaxxEffectType::null, null_effect_);
}

void EaxxFxSlot::get_fx_slot_all(
	const EaxxEaxCall& eax_call) const
{
	switch (eax_call.get_version())
	{
		case 4:
			eax_call.set_value<EaxxFxSlotException, EAX40FXSLOTPROPERTIES>(eax_.fx_slot);
			break;

		case 5:
			eax_call.set_value<EaxxFxSlotException, EAX50FXSLOTPROPERTIES>(eax_.fx_slot);
			break;

		default:
			fail("Unsupported EAX version.");
	}
}

void EaxxFxSlot::get_fx_slot(
	const EaxxEaxCall& eax_call) const
{
	const auto property_id = eax_call.get_property_id();

	switch (property_id)
	{
		case ::EAXFXSLOT_ALLPARAMETERS:
			get_fx_slot_all(eax_call);
			break;

		case ::EAXFXSLOT_LOADEFFECT:
			eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.guidLoadEffect);
			break;

		case ::EAXFXSLOT_VOLUME:
			eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.lVolume);
			break;

		case ::EAXFXSLOT_LOCK:
			eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.lLock);
			break;

		case ::EAXFXSLOT_FLAGS:
			eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.ulFlags);
			break;

		case ::EAXFXSLOT_OCCLUSION:
			eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.lOcclusion);
			break;

		case ::EAXFXSLOT_OCCLUSIONLFRATIO:
			eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.flOcclusionLFRatio);
			break;

		default:
			fail("Unsupported FX slot property id.");
	}
}

void EaxxFxSlot::get(
	const EaxxEaxCall& eax_call)
{
	const auto property_set_id = eax_call.get_property_set_id();

	switch (property_set_id)
	{
		case EaxxEaxCallPropertySetId::fx_slot:
			get_fx_slot(eax_call);
			break;

		case EaxxEaxCallPropertySetId::fx_slot_effect:
			dispatch_effect(eax_call);
			break;

		default:
			fail("Unsupported property id.");
	}
}

void EaxxFxSlot::set_fx_slot_effect(
	EaxxEffectUPtr& effect)
{
	effect_ = effect.get();
	effect_->load();
}

void EaxxFxSlot::set_fx_slot_effect(
	EaxxEffectType effect_type,
	EaxxEffectUPtr& effect)
{
	if (!effect)
	{
		auto effect_param = EaxxEffectParam{};
		effect_param.al_effect_slot = efx_.effect_slot;
		effect_param.effect_type = effect_type;
		effect_param.al_efx_symbols = al_efx_symbols_;

		effect = make_eaxx_effect(effect_param);
	}

	set_fx_slot_effect(effect);
}

void EaxxFxSlot::set_fx_slot_effect()
{
	if (false)
	{
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_NULL_GUID)
	{
		set_fx_slot_effect(EaxxEffectType::null, null_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_AUTOWAH_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::auto_wah, auto_wah_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_CHORUS_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::chorus, chorus_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_AGCCOMPRESSOR_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::compressor, compressor_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_DISTORTION_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::distortion, distortion_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_REVERB_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::eax_reverb, eax_reverb_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_ECHO_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::echo, echo_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_EQUALIZER_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::equalizer, equalizer_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_FLANGER_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::flanger, flanger_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_FREQUENCYSHIFTER_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::frequency_shifter, frequency_shifter_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_PITCHSHIFTER_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::pitch_shifter, pitch_shifter_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_RINGMODULATOR_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::ring_modulator, ring_modulator_effect_);
	}
	else if (eax_.fx_slot.guidLoadEffect == ::EAX_VOCALMORPHER_EFFECT)
	{
		set_fx_slot_effect(EaxxEffectType::vocal_morpher, vocal_morpher_effect_);
	}
	else
	{
		fail("Unsupported effect.");
	}
}

void EaxxFxSlot::set_efx_effect_slot_gain()
{
	const auto gain = level_mb_to_gain(
		std::clamp(
			eax_.fx_slot.lVolume,
			::EAXFXSLOT_MINVOLUME,
			::EAXFXSLOT_MAXVOLUME
		)
	);

	al_efx_symbols_->alAuxiliaryEffectSlotf(efx_.effect_slot, AL_EFFECTSLOT_GAIN, gain);
}

void EaxxFxSlot::set_fx_slot_volume()
{
	set_efx_effect_slot_gain();
}

void EaxxFxSlot::set_effect_slot_send_auto()
{
	al_efx_symbols_->alAuxiliaryEffectSloti(
		efx_.effect_slot,
		AL_EFFECTSLOT_AUXILIARY_SEND_AUTO,
		(eax_.fx_slot.ulFlags & ::EAXFXSLOTFLAGS_ENVIRONMENT) != 0
	);
}

void EaxxFxSlot::set_fx_slot_flags()
{
	set_effect_slot_send_auto();
}

void EaxxFxSlot::set_fx_slot_effect(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_effect_id =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::guidLoadEffect)>();

	validate_fx_slot_effect(eax_effect_id);
	validate_fx_slot_lock_state(eax_.fx_slot.lLock, eax_effect_id);

	set_fx_slot_effect(eax_effect_id);
}

void EaxxFxSlot::set_fx_slot_volume(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_volume =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::lVolume)>();

	validate_fx_slot_volume(eax_volume);
	set_fx_slot_volume(eax_volume);
}

void EaxxFxSlot::set_fx_slot_lock(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_lock =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::lLock)>();

	validate_fx_slot_lock(eax_lock);
	set_fx_slot_lock(eax_lock);
}

void EaxxFxSlot::set_fx_slot_flags(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_flags =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::ulFlags)>();

	validate_fx_slot_flags(eax_flags, eax_call.get_version());
	set_fx_slot_flags(eax_flags);
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_occlusion(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_occlusion =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX50FXSLOTPROPERTIES::lOcclusion)>();

	validate_fx_slot_occlusion(eax_occlusion);

	return set_fx_slot_occlusion(eax_occlusion);
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_occlusion_lf_ratio(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_occlusion_lf_ratio =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX50FXSLOTPROPERTIES::flOcclusionLFRatio)>();

	validate_fx_slot_occlusion_lf_ratio(eax_occlusion_lf_ratio);

	return set_fx_slot_occlusion_lf_ratio(eax_occlusion_lf_ratio);
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_all(
	const EaxxEaxCall& eax_call)
{
	switch (eax_call.get_version())
	{
		case 4:
			{
				const auto& eax_all =
					eax_call.get_value<EaxxFxSlotException, const EAX40FXSLOTPROPERTIES>();

				validate_fx_slot_all(eax_all, eax_call.get_version());
				set_fx_slot_all(eax_all);

				return false;
			}

		case 5:
			{
				const auto& eax_all =
					eax_call.get_value<EaxxFxSlotException, const EAX50FXSLOTPROPERTIES>();

				validate_fx_slot_all(eax_all, eax_call.get_version());
				return set_fx_slot_all(eax_all);
			}

		default:
			fail("Unsupported EAX version.");
	}
}

bool EaxxFxSlot::set_fx_slot(
	const EaxxEaxCall& eax_call)
{
	const auto property_id = eax_call.get_property_id();

	switch (property_id)
	{
		case ::EAXFXSLOT_NONE:
			return false;

		case ::EAXFXSLOT_ALLPARAMETERS:
			fail_set_if_dedicated();
			return set_fx_slot_all(eax_call);

		case ::EAXFXSLOT_LOADEFFECT:
			fail_set_if_dedicated();
			set_fx_slot_effect(eax_call);
			return false;

		case ::EAXFXSLOT_VOLUME:
			set_fx_slot_volume(eax_call);
			return false;

		case ::EAXFXSLOT_LOCK:
			fail_set_if_dedicated();
			set_fx_slot_lock(eax_call);
			return false;

		case ::EAXFXSLOT_FLAGS:
			set_fx_slot_flags(eax_call);
			return false;

		case ::EAXFXSLOT_OCCLUSION:
			return set_fx_slot_occlusion(eax_call);

		case ::EAXFXSLOT_OCCLUSIONLFRATIO:
			return set_fx_slot_occlusion_lf_ratio(eax_call);


		default:
			fail("Unsupported FX slot property id.");
	}
}

[[nodiscard]] bool EaxxFxSlot::set(
	const EaxxEaxCall& eax_call)
{
	const auto property_set_id = eax_call.get_property_set_id();

	switch (property_set_id)
	{
		case EaxxEaxCallPropertySetId::fx_slot:
			return set_fx_slot(eax_call);

		case EaxxEaxCallPropertySetId::fx_slot_effect:
			dispatch_effect(eax_call);
			return false;

		default:
			fail("Unsupported property id.");
	}
}

void EaxxFxSlot::dispatch_effect(
	const EaxxEaxCall& eax_call)
{
	effect_->dispatch(eax_call);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
