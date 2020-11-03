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

#include <exception>
#include <string_view>

#include "eaxefx_exception.h"
#include "eaxefx_unit_converters.h"

#include "eaxefx_al_object.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eax_converters.h"
#include "eaxefx_eax_validators.h"


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

void EaxxFxSlot::set_effect(
	const GUID& eax_id)
try
{
	EaxFxSlotValidator::effect(eax_id);

	if (eax_.fx_slot.guidLoadEffect == eax_id)
	{
		return;
	}

	is_reverb_ = eax_id == EAX_REVERB_EFFECT;
	eax_.fx_slot.guidLoadEffect = eax_id;

	set_effect();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set effect."});
}

void EaxxFxSlot::set_volume(
	long eax_volume)
try
{
	EaxFxSlotValidator::volume(eax_volume);

	if (eax_.fx_slot.lVolume == eax_volume)
	{
		return;
	}

	eax_.fx_slot.lVolume = eax_volume;

	set_volume();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set volume."});
}

void EaxxFxSlot::set_lock(
	long eax_lock)
try
{
	EaxFxSlotValidator::lock(eax_lock);

	if (eax_.fx_slot.lLock == eax_lock)
	{
		return;
	}

	// TODO
	// State validation?

	eax_.fx_slot.lLock = eax_lock;
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set lock."});
}

void EaxxFxSlot::set_flags(
	int eax_version,
	unsigned long eax_flags)
try
{
	EaxFxSlotValidator::flags(eax_version, eax_flags);

	if (eax_.fx_slot.ulFlags == eax_flags)
	{
		return;
	}

	eax_.fx_slot.ulFlags = eax_flags;

	set_flags();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set flags."});
}

[[nodiscard]] bool EaxxFxSlot::set_occlusion(
	int eax_version,
	long eax_occlusion)
try
{
	if (set_eax_occlusion(eax_version, eax_occlusion))
	{
		return true;
	}

	return false;
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set occlusion."});
}

[[nodiscard]] bool EaxxFxSlot::set_occlusion_lf_ratio(
	int eax_version,
	float eax_occlusion_lf_ratio)
try
{
	if (set_eax_occlusion_lf_ratio(eax_version, eax_occlusion_lf_ratio))
	{
		return true;
	}

	return false;
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set occlusion LF ratio."});
}

[[nodiscard]] bool EaxxFxSlot::set(
	const EAX50FXSLOTPROPERTIES& eax_fx_slot)
try
{
	// TODO
	// Should we set lock first?

	set_effect(eax_fx_slot.guidLoadEffect);
	set_volume(eax_fx_slot.lVolume);
	set_lock(eax_fx_slot.lLock);
	set_flags(5, eax_fx_slot.ulFlags);

	const auto is_occlusion_modified = set_eax_occlusion(5, eax_fx_slot.lOcclusion);
	const auto is_occlusion_lf_ratio_modified = set_eax_occlusion_lf_ratio(5, eax_fx_slot.flOcclusionLFRatio);

	if (is_occlusion_modified || is_occlusion_lf_ratio_modified)
	{
		return true;
	}

	return false;
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to all FX slot properties."});
}

void EaxxFxSlot::set_reverb_environment(
	int eax_version,
	unsigned long eax_reverb_environment)
try
{
	if (!is_reverb_)
	{
		throw EaxxFxSlotReverbException{"Effect not attached."};
	}

	EaxReverbValidator::environment(eax_version, eax_reverb_environment);

	if (eax_reverb_environment != EAX_ENVIRONMENT_GENERIC)
	{
		throw EaxxFxSlotReverbException{"Expected generic environment."};
	}

	set_reverb_environment();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set reverb environment."});
}

void EaxxFxSlot::set_reverb(
	int eax_version,
	const EAXREVERBPROPERTIES& eax_reverb)
try
{
	if (!is_reverb_)
	{
		throw EaxxFxSlotReverbException{"Effect not attached."};
	}

	EaxReverbValidator::all(eax_version, eax_reverb);

	if (eax_.reverb == eax_reverb)
	{
		return;
	}

	eax_.reverb = eax_reverb;

	set_reverb();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotException{"Failed to set all reverb properties."});
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
	eax_.reverb.ulEnvironment = EAXREVERB_DEFAULTENVIRONMENT;
	eax_.reverb.flEnvironmentSize = EAXREVERB_DEFAULTENVIRONMENTSIZE;
	eax_.reverb.flEnvironmentDiffusion = EAXREVERB_DEFAULTENVIRONMENTDIFFUSION;
	eax_.reverb.lRoom = EAXREVERB_MINROOM;
	eax_.reverb.lRoomHF = EAXREVERB_DEFAULTROOMHF;
	eax_.reverb.lRoomLF = EAXREVERB_DEFAULTROOMLF;
	eax_.reverb.flDecayTime = EAXREVERB_DEFAULTDECAYTIME;
	eax_.reverb.flDecayHFRatio = EAXREVERB_DEFAULTDECAYHFRATIO;
	eax_.reverb.flDecayLFRatio = EAXREVERB_DEFAULTDECAYLFRATIO;
	eax_.reverb.lReflections = EAXREVERB_DEFAULTREFLECTIONS;
	eax_.reverb.flReflectionsDelay = EAXREVERB_DEFAULTREFLECTIONSDELAY;
	eax_.reverb.vReflectionsPan = EAXREVERB_DEFAULTREFLECTIONSPAN;
	eax_.reverb.lReverb = EAXREVERB_DEFAULTREVERB;
	eax_.reverb.flReverbDelay = EAXREVERB_DEFAULTREVERBDELAY;
	eax_.reverb.vReverbPan = EAXREVERB_DEFAULTREVERBPAN;
	eax_.reverb.flEchoTime = EAXREVERB_DEFAULTECHOTIME;
	eax_.reverb.flEchoDepth = EAXREVERB_DEFAULTECHODEPTH;
	eax_.reverb.flModulationTime = EAXREVERB_DEFAULTMODULATIONTIME;
	eax_.reverb.flModulationDepth = EAXREVERB_DEFAULTMODULATIONDEPTH;
	eax_.reverb.flAirAbsorptionHF = EAXREVERB_DEFAULTAIRABSORPTIONHF;
	eax_.reverb.flHFReference = EAXREVERB_DEFAULTHFREFERENCE;
	eax_.reverb.flLFReference = EAXREVERB_DEFAULTLFREFERENCE;
	eax_.reverb.flRoomRolloffFactor = EAXREVERB_DEFAULTROOMROLLOFFFACTOR;
	eax_.reverb.ulFlags = EAXREVERB_DEFAULTFLAGS;
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

	alEffecti_(al_effect, AL_EFFECT_TYPE, al_effect_type);
	auto new_al_effect_type = ALint{-1};
	alGetEffecti_(al_effect, AL_EFFECT_TYPE, &new_al_effect_type);

	if (new_al_effect_type != al_effect_type)
	{
		throw EaxxFxSlotException{"Failed to set EFX effect type."};
	}
}

void EaxxFxSlot::set_efx_eax_reverb_properties()
{
	const auto al_effect = efx_.effect;

	alEffectf_(
		al_effect,
		AL_EAXREVERB_DIFFUSION,
		EaxReverbToEfx::environment_diffusion(eax_.reverb.flEnvironmentDiffusion)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_GAIN,
		EaxReverbToEfx::room(eax_.reverb.lRoom)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_GAINHF,
		EaxReverbToEfx::room_hf(eax_.reverb.lRoomHF)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_GAINLF,
		EaxReverbToEfx::room_lf(eax_.reverb.lRoomLF)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_DECAY_TIME,
		EaxReverbToEfx::decay_time(eax_.reverb.flDecayTime)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_DECAY_HFRATIO,
		EaxReverbToEfx::decay_hf_ratio(eax_.reverb.flDecayHFRatio)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_DECAY_LFRATIO,
		EaxReverbToEfx::decay_lf_ratio(eax_.reverb.flDecayLFRatio)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_REFLECTIONS_GAIN,
		EaxReverbToEfx::reflections(eax_.reverb.lReflections)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_REFLECTIONS_DELAY,
		EaxReverbToEfx::reflections_delay(eax_.reverb.flReflectionsDelay)
	);

	alEffectfv_(
		al_effect,
		AL_EAXREVERB_REFLECTIONS_PAN,
		&eax_.reverb.vReflectionsPan.x);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_LATE_REVERB_GAIN,
		EaxReverbToEfx::reverb(eax_.reverb.lReverb)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_LATE_REVERB_DELAY,
		EaxReverbToEfx::reverb_delay(eax_.reverb.flReverbDelay)
	);

	alEffectfv_(
		al_effect,
		AL_EAXREVERB_LATE_REVERB_PAN,
		&eax_.reverb.vReverbPan.x
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_ECHO_TIME,
		EaxReverbToEfx::echo_time(eax_.reverb.flEchoTime)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_ECHO_DEPTH,
		EaxReverbToEfx::echo_depth(eax_.reverb.flEchoDepth)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_MODULATION_TIME,
		EaxReverbToEfx::modulation_time(eax_.reverb.flModulationTime)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_MODULATION_DEPTH,
		EaxReverbToEfx::modulation_depth(eax_.reverb.flModulationDepth)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_AIR_ABSORPTION_GAINHF,
		EaxReverbToEfx::air_absorption_hf(eax_.reverb.flAirAbsorptionHF)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_HFREFERENCE,
		EaxReverbToEfx::hf_reference(eax_.reverb.flHFReference)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_LFREFERENCE,
		EaxReverbToEfx::lf_reference(eax_.reverb.flLFReference)
	);

	alEffectf_(
		al_effect,
		AL_EAXREVERB_ROOM_ROLLOFF_FACTOR,
		EaxReverbToEfx::room_rolloff_factor(eax_.reverb.flRoomRolloffFactor)
	);

	alEffecti_(
		al_effect,
		AL_EAXREVERB_DECAY_HFLIMIT,
		(eax_.reverb.ulFlags & EAXREVERBFLAGS_DECAYHFLIMIT) != 0
	);
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

void EaxxFxSlot::set_efx_effect_properties()
{
	if (is_reverb_)
	{
		set_efx_eax_reverb_properties();
	}
}

void EaxxFxSlot::set_effect()
{
	set_efx_effect();
	set_efx_effect_properties();
	attach_efx_effect();
}

void EaxxFxSlot::set_efx_effect_slot_gain()
{
	const auto gain = mb_to_gain(eax_.fx_slot.lVolume);
	alAuxiliaryEffectSlotf_(efx_.effect_slot, AL_EFFECTSLOT_GAIN, gain);
}

void EaxxFxSlot::set_volume()
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

void EaxxFxSlot::set_flags()
{
	set_effect_slot_send_auto();
}

[[nodiscard]] bool EaxxFxSlot::set_eax_occlusion(
	int eax_version,
	long eax_occlusion)
{
	EaxFxSlotValidator::occlusion(eax_version, eax_occlusion);

	if (eax_.fx_slot.lOcclusion == eax_occlusion)
	{
		return false;
	}

	eax_.fx_slot.lOcclusion = eax_occlusion;

	return true;
}

[[nodiscard]] bool EaxxFxSlot::set_eax_occlusion_lf_ratio(
	int eax_version,
	float eax_occlusion_lf_ratio)
{
	EaxFxSlotValidator::occlusion_lf_ratio(eax_version, eax_occlusion_lf_ratio);

	if (eax_.fx_slot.flOcclusionLFRatio == eax_occlusion_lf_ratio)
	{
		return false;
	}

	eax_.fx_slot.flOcclusionLFRatio = eax_occlusion_lf_ratio;

	return true;
}

void EaxxFxSlot::set_reverb_environment()
{
	set_eax_reverb_defaults();
	set_efx_eax_reverb_properties();
	attach_efx_effect();
}

void EaxxFxSlot::set_reverb()
{
	set_efx_eax_reverb_properties();
	attach_efx_effect();
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
