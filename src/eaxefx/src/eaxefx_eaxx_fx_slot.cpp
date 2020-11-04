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


#include "eaxefx_eaxx_fx_slot.h"

#include <cassert>

#include <algorithm>
#include <exception>
#include <string_view>

#include "eaxefx_exception.h"
#include "eaxefx_unit_converters.h"

#include "eaxefx_al_object.h"
#include "eaxefx_al_symbols.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxFxSlotException :
	public Exception
{
public:
	explicit EaxxFxSlotException(
		std::string_view message)
		:
		Exception{"EAXEFX_EAXX_FX_SLOT", message}
	{
	}
}; // EaxxFxSlotException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxFxSlotReverbException :
	public Exception
{
public:
	explicit EaxxFxSlotReverbException(
		std::string_view message)
		:
		Exception{"EAXEFX_EAXX_FX_SLOT_REVERB", message}
	{
	}
}; // EaxxFxSlotReverbException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxxFxSlot::initialize()
try
{
	initialize_eax();
	initialize_efx();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to initialize."});
}

ALuint EaxxFxSlot::get_efx_effect_slot() const noexcept
{
	return efx_.effect_slot;
}

const EAX50FXSLOTPROPERTIES& EaxxFxSlot::get_eax_fx_slot() const noexcept
{
	return eax_.fx_slot;
}

void EaxxFxSlot::validate_fx_slot_effect(
	const GUID& eax_effect_id)
{
	if (eax_effect_id != EAX_REVERB_EFFECT &&
		eax_effect_id != EAX_NULL_GUID)
	{
		throw EaxxFxSlotException{"Unsupported EAX effect id."};
	}
}

void EaxxFxSlot::set_fx_slot_effect(
	const GUID& eax_effect_id)
{
	validate_fx_slot_effect(eax_effect_id);

	if (eax_.fx_slot.guidLoadEffect == eax_effect_id)
	{
		return;
	}

	is_reverb_ = eax_effect_id == EAX_REVERB_EFFECT;
	eax_.fx_slot.guidLoadEffect = eax_effect_id;

	set_fx_slot_effect();
}

void EaxxFxSlot::set_fx_slot_volume(
	long eax_volume)
{
	if (eax_.fx_slot.lVolume == eax_volume)
	{
		return;
	}

	eax_.fx_slot.lVolume = eax_volume;

	set_fx_slot_volume();
}

void EaxxFxSlot::set_fx_slot_lock(
	long eax_lock)
{
	if (eax_.fx_slot.lLock == eax_lock)
	{
		return;
	}

	// TODO
	// State validation?

	eax_.fx_slot.lLock = eax_lock;
}

void EaxxFxSlot::set_fx_slot_flags(
	unsigned long eax_flags)
{
	if (eax_.fx_slot.ulFlags == eax_flags)
	{
		return;
	}

	eax_.fx_slot.ulFlags = eax_flags;

	set_fx_slot_flags();
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_occlusion(
	long eax_occlusion)
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
	// TODO
	// Should we set lock first?

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

void EaxxFxSlot::set_fx_slot_default_effect()
{
	set_fx_slot_effect(EAX_REVERB_EFFECT);
}

void EaxxFxSlot::get(
	const EaxxEaxCall& eax_call) const
{
	switch (eax_call.property_id)
	{
		case EAXFXSLOT_ALLPARAMETERS:
			get_fx_slot_all(eax_call);
			break;

		case EAXFXSLOT_LOADEFFECT:
			get_fx_slot_ffect(eax_call);
			break;

		case EAXFXSLOT_VOLUME:
			get_fx_slot_volume(eax_call);
			break;

		case EAXFXSLOT_LOCK:
			get_fx_slot_lock(eax_call);
			break;

		case EAXFXSLOT_FLAGS:
			get_fx_slot_flags(eax_call);
			break;

		case EAXFXSLOT_OCCLUSION:
			get_fx_slot_occlusion(eax_call);
			break;

		case EAXFXSLOT_OCCLUSIONLFRATIO:
			get_fx_slot_occlusion_lf_ratio(eax_call);
			break;

		default:
			throw EaxxFxSlotException{"Unsupported property id."};
	}
}

[[nodiscard]] bool EaxxFxSlot::set(
	const EaxxEaxCall& eax_call)
{
	switch (eax_call.property_id)
	{
		case EAXFXSLOT_ALLPARAMETERS:
			return set_fx_slot_all(eax_call);

		case EAXFXSLOT_LOADEFFECT:
			return set_fx_slot_effect(eax_call);

		case EAXFXSLOT_VOLUME:
			return set_fx_slot_volume(eax_call);

		case EAXFXSLOT_LOCK:
			return set_fx_slot_lock(eax_call);

		case EAXFXSLOT_FLAGS:
			return set_fx_slot_flags(eax_call);

		case EAXFXSLOT_OCCLUSION:
			return set_fx_slot_occlusion(eax_call);

		case EAXFXSLOT_OCCLUSIONLFRATIO:
			return set_fx_slot_occlusion_lf_ratio(eax_call);

		case EAXREVERB_ALLPARAMETERS:
			return set_reverb_all(eax_call);

		case EAXREVERB_ENVIRONMENT:
			return set_reverb_environment(eax_call);

		case EAXREVERB_ROOM:
			return set_reverb_room(eax_call);

		case EAXREVERB_REFLECTIONSPAN:
			return set_reverb_reflections_pan(eax_call);

		case EAXREVERB_REVERBPAN:
			return set_reverb_reverb_pan(eax_call);

		default:
			throw EaxxFxSlotException{"Unsupported property id."};
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxxFxSlot::set_eax_fx_slot_defaults()
{
	eax_.fx_slot.guidLoadEffect = EAX_NULL_GUID;
	eax_.fx_slot.lVolume = EAXFXSLOT_DEFAULTVOLUME;
	eax_.fx_slot.lLock = EAXFXSLOT_LOCKED;
	eax_.fx_slot.ulFlags = EAX50FXSLOT_DEFAULTFLAGS;
}

void EaxxFxSlot::set_eax_reverb_defaults()
{
	eax_.reverb = EAX_REVERB_PRESETS[EAX_ENVIRONMENT_GENERIC];
	eax_.reverb.lRoom = EAXREVERB_MINROOM;
}

void EaxxFxSlot::initialize_eax()
{
	set_eax_fx_slot_defaults();
	set_eax_reverb_defaults();
}

void EaxxFxSlot::create_efx_effect()
{
	efx_.effect = make_efx_effect_object().release();
}

void EaxxFxSlot::create_efx_effect_slot()
{
	efx_.effect_slot = make_efx_effect_slot_object().release();
}

void EaxxFxSlot::create_efx_objects()
{
	create_efx_effect();
	create_efx_effect_slot();
}

void EaxxFxSlot::set_efx_effect()
{
	auto al_effect_type = ALint{-1};

	if (is_reverb_)
	{
		al_effect_type = AL_EFFECT_EAXREVERB;
	}
	else
	{
		al_effect_type = AL_EFFECT_NULL;
	}

	const auto al_effect = efx_.effect;

	alGetError_();
	alEffecti_(al_effect, AL_EFFECT_TYPE, al_effect_type);
	auto new_al_effect_type = ALint{-1};
	alGetEffecti_(al_effect, AL_EFFECT_TYPE, &new_al_effect_type);
	assert(alGetError_() == AL_NO_ERROR);

	if (new_al_effect_type != al_effect_type)
	{
		throw EaxxFxSlotException{"Failed to set EFX effect type."};
	}
}

void EaxxFxSlot::set_efx_eax_reverb_environment_diffusion()
{
	const auto efx_eax_reverb_environment_diffusion = std::clamp(
		std::clamp(
			eax_.reverb.flEnvironmentDiffusion,
			EAXREVERB_MINENVIRONMENTDIFFUSION,
			EAXREVERB_MAXENVIRONMENTDIFFUSION
		),
		AL_EAXREVERB_MIN_DIFFUSION,
		AL_EAXREVERB_MAX_DIFFUSION
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_DIFFUSION, efx_eax_reverb_environment_diffusion);
}

void EaxxFxSlot::set_efx_eax_reverb_room()
{
	const auto efx_eax_reverb_room = std::clamp(
		mb_to_gain(
			std::clamp(
				eax_.reverb.lRoom,
				EAXREVERB_MINROOM,
				EAXREVERB_MAXROOM
			)
		),
		AL_EAXREVERB_MIN_GAIN,
		AL_EAXREVERB_MAX_GAIN
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_GAIN, efx_eax_reverb_room);
}

void EaxxFxSlot::set_efx_eax_reverb_room_hf()
{
	const auto efx_eax_reverb_room_hf = std::clamp(
		mb_to_gain(
			std::clamp(
				eax_.reverb.lRoomHF,
				EAXREVERB_MINROOMHF,
				EAXREVERB_MAXROOMHF
			)
		),
		AL_EAXREVERB_MIN_GAINHF,
		AL_EAXREVERB_MAX_GAINHF
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_GAINHF, efx_eax_reverb_room_hf);
}

void EaxxFxSlot::set_efx_eax_reverb_room_lf()
{
	const auto efx_eax_reverb_room_lf = std::clamp(
		mb_to_gain(
			std::clamp(
				eax_.reverb.lRoomLF,
				EAXREVERB_MINROOMLF,
				EAXREVERB_MAXROOMLF
			)
		),
		AL_EAXREVERB_MIN_GAINLF,
		AL_EAXREVERB_MAX_GAINLF
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_GAINLF, efx_eax_reverb_room_lf);
}

void EaxxFxSlot::set_efx_eax_reverb_decay_time()
{
	const auto efx_eax_reverb_decay_time = std::clamp(
		std::clamp(
			eax_.reverb.flDecayTime,
			EAXREVERB_MINDECAYTIME,
			EAXREVERB_MAXDECAYTIME
		),
		AL_EAXREVERB_MIN_DECAY_TIME,
		AL_EAXREVERB_MAX_DECAY_TIME
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_DECAY_TIME, efx_eax_reverb_decay_time);
}

void EaxxFxSlot::set_efx_eax_reverb_decay_hf_ratio()
{
	const auto efx_eax_reverb_decay_hf_ratio = std::clamp(
		std::clamp(
			eax_.reverb.flDecayHFRatio,
			EAXREVERB_MINDECAYHFRATIO,
			EAXREVERB_MAXDECAYHFRATIO
		),
		AL_EAXREVERB_MIN_DECAY_HFRATIO,
		AL_EAXREVERB_MAX_DECAY_HFRATIO
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_DECAY_HFRATIO, efx_eax_reverb_decay_hf_ratio);
}

void EaxxFxSlot::set_efx_eax_reverb_decay_lf_ratio()
{
	const auto efx_eax_reverb_decay_lf_ratio = std::clamp(
		std::clamp(
			eax_.reverb.flDecayLFRatio,
			EAXREVERB_MINDECAYLFRATIO,
			EAXREVERB_MAXDECAYLFRATIO
		),
		AL_EAXREVERB_MIN_DECAY_LFRATIO,
		AL_EAXREVERB_MAX_DECAY_LFRATIO
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_DECAY_LFRATIO, efx_eax_reverb_decay_lf_ratio);
}

void EaxxFxSlot::set_efx_eax_reverb_reflections()
{
	const auto efx_eax_reverb_reflections = std::clamp(
		mb_to_gain(
			std::clamp(
				eax_.reverb.lReflections,
				EAXREVERB_MINREFLECTIONS,
				EAXREVERB_MAXREFLECTIONS
			)
		),
		AL_EAXREVERB_MIN_REFLECTIONS_GAIN,
		AL_EAXREVERB_MAX_REFLECTIONS_GAIN
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_REFLECTIONS_GAIN, efx_eax_reverb_reflections);
}

void EaxxFxSlot::set_efx_eax_reverb_reflections_delay()
{
	const auto efx_eax_reverb_reflections_delay = std::clamp(
		std::clamp(
			eax_.reverb.flReflectionsDelay,
			EAXREVERB_MINREFLECTIONSDELAY,
			EAXREVERB_MAXREFLECTIONSDELAY
		),
		AL_EAXREVERB_MIN_REFLECTIONS_DELAY,
		AL_EAXREVERB_MAX_REFLECTIONS_DELAY
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_REFLECTIONS_DELAY, efx_eax_reverb_reflections_delay);
}

void EaxxFxSlot::set_efx_eax_reverb_reflections_pan()
{
	alEffectfv_(
		efx_.effect,
		AL_EAXREVERB_REFLECTIONS_PAN,
		&eax_.reverb.vReflectionsPan.x
	);
}

void EaxxFxSlot::set_efx_eax_reverb_reverb()
{
	const auto efx_eax_reverb_reverb = std::clamp(
		mb_to_gain(
			std::clamp(
				eax_.reverb.lReverb,
				EAXREVERB_MINREVERB,
				EAXREVERB_MAXREVERB
			)
		),
		AL_EAXREVERB_MIN_LATE_REVERB_GAIN,
		AL_EAXREVERB_MAX_LATE_REVERB_GAIN
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_LATE_REVERB_GAIN, efx_eax_reverb_reverb);
}

void EaxxFxSlot::set_efx_eax_reverb_reverb_delay()
{
	const auto efx_eax_reverb_reverb_delay = std::clamp(
		std::clamp(
			eax_.reverb.flReverbDelay,
			EAXREVERB_MINREVERBDELAY,
			EAXREVERB_MAXREVERBDELAY
		),
		AL_EAXREVERB_MIN_LATE_REVERB_DELAY,
		AL_EAXREVERB_MAX_LATE_REVERB_DELAY);

	alEffectf_(efx_.effect, AL_EAXREVERB_LATE_REVERB_DELAY, efx_eax_reverb_reverb_delay);
}

void EaxxFxSlot::set_efx_eax_reverb_reverb_pan()
{
	alEffectfv_(
		efx_.effect,
		AL_EAXREVERB_LATE_REVERB_PAN,
		&eax_.reverb.vReverbPan.x
	);
}

void EaxxFxSlot::set_efx_eax_reverb_echo_time()
{
	const auto efx_eax_reverb_echo_time = std::clamp(
		std::clamp(
			eax_.reverb.flEchoTime,
			EAXREVERB_MINECHOTIME,
			EAXREVERB_MAXECHOTIME
		),
		AL_EAXREVERB_MIN_ECHO_TIME,
		AL_EAXREVERB_MAX_ECHO_TIME
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_ECHO_TIME, efx_eax_reverb_echo_time);
}

void EaxxFxSlot::set_efx_eax_reverb_echo_depth()
{
	const auto efx_eax_reverb_echo_depth = std::clamp(
		std::clamp(
			eax_.reverb.flEchoDepth,
			EAXREVERB_MINECHODEPTH,
			EAXREVERB_MAXECHODEPTH
		),
		AL_EAXREVERB_MIN_ECHO_DEPTH,
		AL_EAXREVERB_MAX_ECHO_DEPTH
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_ECHO_DEPTH, efx_eax_reverb_echo_depth);
}

void EaxxFxSlot::set_efx_eax_reverb_modulation_time()
{
	const auto efx_eax_reverb_modulation_time = std::clamp(
		std::clamp(
			eax_.reverb.flModulationTime,
			EAXREVERB_MINMODULATIONTIME,
			EAXREVERB_MAXMODULATIONTIME
		),
		AL_EAXREVERB_MIN_MODULATION_TIME,
		AL_EAXREVERB_MAX_MODULATION_TIME
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_MODULATION_TIME, efx_eax_reverb_modulation_time);
}

void EaxxFxSlot::set_efx_eax_reverb_modulation_depth()
{
	const auto efx_eax_reverb_modulation_depth = std::clamp(
		std::clamp(
			eax_.reverb.flModulationDepth,
			EAXREVERB_MINMODULATIONDEPTH,
			EAXREVERB_MAXMODULATIONDEPTH
		),
		AL_EAXREVERB_MIN_MODULATION_DEPTH,
		AL_EAXREVERB_MAX_MODULATION_DEPTH
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_MODULATION_DEPTH, efx_eax_reverb_modulation_depth);
}

void EaxxFxSlot::set_efx_eax_reverb_air_absorption_hf()
{
	const auto efx_eax_reverb_air_absorption_hf = std::clamp(
		mb_to_gain(
			std::clamp(
				eax_.reverb.flAirAbsorptionHF,
				EAXREVERB_MINAIRABSORPTIONHF,
				EAXREVERB_MAXAIRABSORPTIONHF
			)
		),
		AL_EAXREVERB_MIN_AIR_ABSORPTION_GAINHF,
		AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, efx_eax_reverb_air_absorption_hf);
}

void EaxxFxSlot::set_efx_eax_reverb_hf_reference()
{
	const auto efx_eax_reverb_hf_reference = std::clamp(
		std::clamp(
			eax_.reverb.flHFReference,
			EAXREVERB_MINHFREFERENCE,
			EAXREVERB_MAXHFREFERENCE
		),
		AL_EAXREVERB_MIN_HFREFERENCE,
		AL_EAXREVERB_MAX_HFREFERENCE
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_HFREFERENCE, efx_eax_reverb_hf_reference);
}

void EaxxFxSlot::set_efx_eax_reverb_lf_reference()
{
	const auto efx_eax_reverb_lf_reference = std::clamp(
		std::clamp(
			eax_.reverb.flLFReference,
			EAXREVERB_MINLFREFERENCE,
			EAXREVERB_MAXLFREFERENCE
		),
		AL_EAXREVERB_MIN_LFREFERENCE,
		AL_EAXREVERB_MAX_LFREFERENCE
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_LFREFERENCE, efx_eax_reverb_lf_reference);
}

void EaxxFxSlot::set_efx_eax_reverb_room_rolloff_factor()
{
	const auto efx_eax_reverb_room_rolloff_factor = std::clamp(
		std::clamp(
			eax_.reverb.flRoomRolloffFactor,
			EAXREVERB_MINROOMROLLOFFFACTOR,
			EAXREVERB_MAXROOMROLLOFFFACTOR
		),
		AL_EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR,
		AL_EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR
	);

	alEffectf_(efx_.effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, efx_eax_reverb_room_rolloff_factor);
}

void EaxxFxSlot::set_efx_eax_reverb_flags()
{
	alEffecti_(
		efx_.effect,
		AL_EAXREVERB_DECAY_HFLIMIT,
		(eax_.reverb.ulFlags & EAXREVERBFLAGS_DECAYHFLIMIT) != 0
	);
}

void EaxxFxSlot::set_efx_eax_reverb_properties()
{
	set_efx_eax_reverb_environment_diffusion();
	set_efx_eax_reverb_room();
	set_efx_eax_reverb_room_hf();
	set_efx_eax_reverb_room_lf();
	set_efx_eax_reverb_decay_time();
	set_efx_eax_reverb_decay_hf_ratio();
	set_efx_eax_reverb_decay_lf_ratio();
	set_efx_eax_reverb_reflections();
	set_efx_eax_reverb_reflections_delay();
	set_efx_eax_reverb_reflections_pan();
	set_efx_eax_reverb_reverb();
	set_efx_eax_reverb_reverb_delay();
	set_efx_eax_reverb_reverb_pan();
	set_efx_eax_reverb_echo_time();
	set_efx_eax_reverb_echo_depth();
	set_efx_eax_reverb_modulation_time();
	set_efx_eax_reverb_modulation_depth();
	set_efx_eax_reverb_air_absorption_hf();
	set_efx_eax_reverb_hf_reference();
	set_efx_eax_reverb_lf_reference();
	set_efx_eax_reverb_room_rolloff_factor();
	set_efx_eax_reverb_flags();
}

void EaxxFxSlot::attach_efx_effect()
{
	alAuxiliaryEffectSloti_(
		efx_.effect_slot,
		AL_EFFECTSLOT_EFFECT,
		static_cast<ALint>(efx_.effect)
	);
}

void EaxxFxSlot::initialize_efx_slot()
{
	attach_efx_effect();
}

void EaxxFxSlot::initialize_efx()
{
	create_efx_objects();
	initialize_efx_slot();
}

void EaxxFxSlot::get_fx_slot_all(
	const EaxxEaxCall& eax_call) const
{
	switch (eax_call.version)
	{
		case 4:
			eax_call.set_value<EaxxFxSlotException, EAX40FXSLOTPROPERTIES>(eax_.fx_slot);
			break;

		case 5:
			eax_call.set_value<EaxxFxSlotException, EAX50FXSLOTPROPERTIES>(eax_.fx_slot);
			break;

		default:
			throw EaxxFxSlotException{"Unsupported EAX version."};
	}
}

void EaxxFxSlot::get_fx_slot_ffect(
	const EaxxEaxCall& eax_call) const
{
	eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.guidLoadEffect);
}

void EaxxFxSlot::get_fx_slot_volume(
	const EaxxEaxCall& eax_call) const
{
	eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.lVolume);
}

void EaxxFxSlot::get_fx_slot_lock(
	const EaxxEaxCall& eax_call) const
{
	eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.lLock);
}

void EaxxFxSlot::get_fx_slot_flags(
	const EaxxEaxCall& eax_call) const
{
	eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.ulFlags);
}

void EaxxFxSlot::get_fx_slot_occlusion(
	const EaxxEaxCall& eax_call) const
{
	eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.lOcclusion);
}

void EaxxFxSlot::get_fx_slot_occlusion_lf_ratio(
	const EaxxEaxCall& eax_call) const
{
	eax_call.set_value<EaxxFxSlotException>(eax_.fx_slot.flOcclusionLFRatio);
}

void EaxxFxSlot::set_efx_effect_properties()
{
	if (is_reverb_)
	{
		set_efx_eax_reverb_properties();
	}
}

void EaxxFxSlot::set_fx_slot_effect()
{
	set_efx_effect();
	set_efx_effect_properties();
	attach_efx_effect();
}

void EaxxFxSlot::set_efx_effect_slot_gain()
{
	const auto gain = mb_to_gain(
		std::clamp(
			eax_.fx_slot.lVolume,
			EAXFXSLOT_MINVOLUME,
			EAXFXSLOT_MAXVOLUME
		)
	);

	alAuxiliaryEffectSlotf_(efx_.effect_slot, AL_EFFECTSLOT_GAIN, gain);
}

void EaxxFxSlot::set_fx_slot_volume()
{
	set_efx_effect_slot_gain();
}

void EaxxFxSlot::set_effect_slot_send_auto()
{
	alAuxiliaryEffectSloti_(
		efx_.effect_slot,
		AL_EFFECTSLOT_AUXILIARY_SEND_AUTO,
		(eax_.fx_slot.ulFlags & EAXFXSLOTFLAGS_ENVIRONMENT) != 0
	);
}

void EaxxFxSlot::set_fx_slot_flags()
{
	set_effect_slot_send_auto();
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_effect(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_effect_id =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::guidLoadEffect)>();

	set_fx_slot_effect(eax_effect_id);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_volume(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_volume =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::lVolume)>();

	set_fx_slot_volume(eax_volume);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_lock(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_lock =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::lLock)>();

	set_fx_slot_lock(eax_lock);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_flags(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_flags =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX40FXSLOTPROPERTIES::ulFlags)>();

	set_fx_slot_flags(eax_flags);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_occlusion(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_occlusion =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX50FXSLOTPROPERTIES::lOcclusion)>();

	return set_fx_slot_occlusion(eax_occlusion);
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_occlusion_lf_ratio(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_occlusion_lf_ratio =
		eax_call.get_value<EaxxFxSlotException, const decltype(EAX50FXSLOTPROPERTIES::flOcclusionLFRatio)>();

	return set_fx_slot_occlusion_lf_ratio(eax_occlusion_lf_ratio);
}

[[nodiscard]] bool EaxxFxSlot::set_fx_slot_all(
	const EaxxEaxCall& eax_call)
{
	switch (eax_call.version)
	{
		case 4:
			{
				const auto& eax_all =
					eax_call.get_value<EaxxFxSlotException, const EAX40FXSLOTPROPERTIES>();

				set_fx_slot_all(eax_all);

				return false;
			}

		case 5:
			{
				const auto& eax_all =
					eax_call.get_value<EaxxFxSlotException, const EAX50FXSLOTPROPERTIES>();

				return set_fx_slot_all(eax_all);
			}

		default:
			throw EaxxFxSlotException{"Unsupported EAX version."};
	}
}

void EaxxFxSlot::set_reverb_environment()
{
	set_efx_eax_reverb_properties();
	attach_efx_effect();
}

void EaxxFxSlot::set_reverb_room()
{
	set_efx_eax_reverb_room();
	attach_efx_effect();
}

void EaxxFxSlot::set_reverb_reflections_pan()
{
	set_efx_eax_reverb_reflections_pan();
	attach_efx_effect();
}

void EaxxFxSlot::set_reverb_reverb_pan()
{
	set_efx_eax_reverb_reverb_pan();
	attach_efx_effect();
}

void EaxxFxSlot::set_reverb_all()
{
	set_efx_eax_reverb_properties();
	attach_efx_effect();
}

void EaxxFxSlot::ensure_reverb_effect()
{
	if (!is_reverb_)
	{
		throw EaxxFxSlotReverbException{"Effect not attached."};
	}
}

void EaxxFxSlot::set_reverb_all(
	const EAXREVERBPROPERTIES& eax_reverb)
{
	ensure_reverb_effect();


	if (eax_reverb.ulEnvironment > EAX_ENVIRONMENT_UNDEFINED)
	{
		throw EaxxFxSlotReverbException{"Environment index out of range."};
	}

	if (eax_.reverb == eax_reverb)
	{
		return;
	}

	eax_.reverb = eax_reverb;

	set_reverb_all();
}

void EaxxFxSlot::set_reverb_environment(
	unsigned long eax_reverb_environment)
{
	ensure_reverb_effect();

	if (eax_reverb_environment >= EAX_ENVIRONMENT_UNDEFINED)
	{
		throw EaxxFxSlotReverbException{"Environment index out of range."};
	}

	eax_.reverb = EAX_REVERB_PRESETS[eax_reverb_environment];

	set_reverb_environment();
}

void EaxxFxSlot::set_reverb_room(
	long eax_reverb_room)
{
	ensure_reverb_effect();

	if (eax_.reverb.lRoom == eax_reverb_room)
	{
		return;
	}

	eax_.reverb.lRoom = eax_reverb_room;

	set_reverb_room();
}

void EaxxFxSlot::set_reverb_reflections_pan(
	const EAXVECTOR& eax_reverb_reflections_pan)
{
	ensure_reverb_effect();

	if (eax_.reverb.vReflectionsPan == eax_reverb_reflections_pan)
	{
		return;
	}

	eax_.reverb.vReflectionsPan = eax_reverb_reflections_pan;

	set_reverb_reflections_pan();
}

void EaxxFxSlot::set_reverb_reverb_pan(
	const EAXVECTOR& eax_reverb_pan)
{
	ensure_reverb_effect();

	if (eax_.reverb.vReverbPan == eax_reverb_pan)
	{
		return;
	}

	eax_.reverb.vReverbPan = eax_reverb_pan;

	set_reverb_reverb_pan();
}

[[nodiscard]] bool EaxxFxSlot::set_reverb_all(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_reverb_all =
		eax_call.get_value<EaxxFxSlotException, const EAXREVERBPROPERTIES>();

	set_reverb_all(eax_reverb_all);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_reverb_environment(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_reverb_environment =
		eax_call.get_value<EaxxFxSlotReverbException, const decltype(EAXREVERBPROPERTIES::ulEnvironment)>();

	set_reverb_environment(eax_reverb_environment);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_reverb_room(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_reverb_room =
		eax_call.get_value<EaxxFxSlotReverbException, const decltype(EAXREVERBPROPERTIES::lRoom)>();

	set_reverb_room(eax_reverb_room);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_reverb_reflections_pan(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_reverb_reflections_pan =
		eax_call.get_value<EaxxFxSlotReverbException, const decltype(EAXREVERBPROPERTIES::vReflectionsPan)>();

	set_reverb_reflections_pan(eax_reverb_reflections_pan);

	return false;
}

[[nodiscard]] bool EaxxFxSlot::set_reverb_reverb_pan(
	const EaxxEaxCall& eax_call)
{
	const auto& eax_reverb_reverb_pan =
		eax_call.get_value<EaxxFxSlotReverbException, const decltype(EAXREVERBPROPERTIES::vReverbPan)>();

	set_reverb_reverb_pan(eax_reverb_reverb_pan);

	return false;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
