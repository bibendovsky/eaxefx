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


#include "eaxefx_eax_validators.h"

#include <string>
#include <string_view>

#include "eaxefx_exception.h"


namespace eaxefx
{


namespace
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename T
>
static std::string eax_validator_make_out_of_range_message(
	std::string_view value_name,
	const T& value)
{
	return std::string{value_name} + " out of range (" + std::to_string(value) + ").";
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename TException,
	typename TValue,
	typename TMinValue,
	typename TMaxValue
>
void eax_validator_range_generic(
	std::string_view value_name,
	TValue&& value,
	TMinValue&& min_value,
	TMaxValue&& max_value)
{
	if (value < min_value || value > max_value)
	{
		throw TException{value_name, value};
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxValidatorException :
	public Exception
{
public:
	explicit EaxValidatorException(
		std::string_view message)
		:
		Exception{"EAX_VALIDATOR", message}
	{
	}
}; // EaxValidatorException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxContextValidatorException :
	public Exception
{
public:
	explicit EaxContextValidatorException(
		std::string_view message)
		:
		Exception{"EAX_CONTEXT_VALIDATOR", message}
	{
	}
}; // EaxContextValidatorException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxContextValidatorOutOfRangeException :
	public EaxContextValidatorException
{
public:
	template<
		typename T
	>
	explicit EaxContextValidatorOutOfRangeException(
		std::string_view value_name,
		const T& value)
		:
		EaxContextValidatorException{eax_validator_make_out_of_range_message<T>(value_name, value)}
	{
	}
}; // EaxSourceValidatorOutOfRangeException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxSessionValidatorException :
	public Exception
{
public:
	explicit EaxSessionValidatorException(
		std::string_view message)
		:
		Exception{"EAX_SESSION_VALIDATOR", message}
	{
	}
}; // EaxSessionValidatorException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxSourceValidatorException :
	public Exception
{
public:
	explicit EaxSourceValidatorException(
		std::string_view message)
		:
		Exception{"EAX_SOURCE_VALIDATOR", message}
	{
	}
}; // EaxSourceValidatorException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxSourceValidatorOutOfRangeException :
	public EaxSourceValidatorException
{
public:
	template<
		typename T
	>
	explicit EaxSourceValidatorOutOfRangeException(
		std::string_view value_name,
		const T& value)
		:
		EaxSourceValidatorException{eax_validator_make_out_of_range_message<T>(value_name, value)}
	{
	}
}; // EaxSourceValidatorOutOfRangeException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxReverbValidatorException :
	public Exception
{
public:
	explicit EaxReverbValidatorException(
		std::string_view message)
		:
		Exception{"EAX_REVERB_VALIDATOR", message}
	{
	}
}; // EaxReverbValidatorException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxReverbValidatorOutOfRangeException :
	public EaxReverbValidatorException
{
public:
	template<
		typename T
	>
	explicit EaxReverbValidatorOutOfRangeException(
		std::string_view value_name,
		const T& value)
		:
		EaxReverbValidatorException{eax_validator_make_out_of_range_message<T>(value_name, value)}
	{
	}
}; // EaxReverbValidatorOutOfRangeException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxReverbValidatorFlagsException :
	public EaxReverbValidatorException
{
public:
	explicit EaxReverbValidatorFlagsException(
		unsigned long flags)
		:
		EaxReverbValidatorException{make_message(flags)}
	{
	}


private:
	static std::string make_message(
		unsigned long flags)
	{
		return "Unsupported flags (" + std::to_string(flags) + ").";
	}
}; // EaxReverbValidatorFlagsException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxFxSlotValidatorException :
	public Exception
{
public:
	explicit EaxFxSlotValidatorException(
		std::string_view message)
		:
		Exception{"EAX_FX_SLOT_VALIDATOR", message}
	{
	}
}; // EaxFxSlotValidatorException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxFxSlotValidatorOutOfRangeException :
	public EaxFxSlotValidatorException
{
public:
	template<
		typename T
	>
	explicit EaxFxSlotValidatorOutOfRangeException(
		std::string_view value_name,
		const T& value)
		:
		EaxFxSlotValidatorException{eax_validator_make_out_of_range_message<T>(value_name, value)}
	{
	}
}; // EaxFxSlotValidatorOutOfRangeException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // namespace


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxValidator::version(
	int version)
{
	if (version < 1 || version > 5)
	{
		throw EaxValidatorException{"Unsupported version."};
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxContextValidator::primary_fx_slot_id(
	const GUID& eax_primary_fx_slot_id)
{
	if (
		eax_primary_fx_slot_id != EAX_NULL_GUID &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX40_FXSlot0 &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX50_FXSlot0 &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX40_FXSlot1 &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX50_FXSlot1 &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX40_FXSlot2 &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX50_FXSlot2 &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX40_FXSlot3 &&
		eax_primary_fx_slot_id != EAXPROPERTYID_EAX50_FXSlot3)
	{
		throw EaxContextValidatorException{"Unsupported primary FX slot id."};
	}
}

void EaxContextValidator::distance_factor(
	float eax_distance_factor)
{
	eax_validator_range_generic<EaxContextValidatorOutOfRangeException>(
		"Distance Factor",
		eax_distance_factor,
		EAXCONTEXT_MINDISTANCEFACTOR,
		EAXCONTEXT_MAXDISTANCEFACTOR
	);
}

void EaxContextValidator::air_absorption_hf(
	float eax_air_absorption_hf)
{
	eax_validator_range_generic<EaxContextValidatorOutOfRangeException>(
		"Air Absorption HF",
		eax_air_absorption_hf,
		EAXCONTEXT_MINAIRABSORPTIONHF,
		EAXCONTEXT_MAXAIRABSORPTIONHF
	);
}

void EaxContextValidator::hf_reference(
	float eax_hf_reference)
{
	eax_validator_range_generic<EaxContextValidatorOutOfRangeException>(
		"HF Reference",
		eax_hf_reference,
		EAXCONTEXT_MINHFREFERENCE,
		EAXCONTEXT_MAXHFREFERENCE
	);
}

void EaxContextValidator::macro_fx_factor(
	float eax_macro_fx_factor)
{
	eax_validator_range_generic<EaxContextValidatorOutOfRangeException>(
		"Macro FX Factor",
		eax_macro_fx_factor,
		EAXCONTEXT_MINMACROFXFACTOR,
		EAXCONTEXT_MAXMACROFXFACTOR
	);
}

void EaxContextValidator::all(
	const EAX50CONTEXTPROPERTIES& eax_context)
{
	primary_fx_slot_id(eax_context.guidPrimaryFXSlotID);
	distance_factor(eax_context.flDistanceFactor);
	air_absorption_hf(eax_context.flAirAbsorptionHF);
	hf_reference(eax_context.flHFReference);
	macro_fx_factor(eax_context.flMacroFXFactor);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxSessionValidator::version(
	unsigned long eax_version)
{
	switch (eax_version)
	{
		case EAX_40:
		case EAX_50:
			break;

		default:
			throw EaxSessionValidatorException{"Unsupported version."};
	}
}

void EaxSessionValidator::max_active_sends(
	unsigned long eax_max_active_sends)
{
	if (eax_max_active_sends < EAX40_MAX_ACTIVE_FXSLOTS ||
		eax_max_active_sends > EAX50_MAX_ACTIVE_FXSLOTS)
	{
		throw EaxSessionValidatorException{"Unsupported max active sends."};
	}
}

void EaxSessionValidator::all(
	const EAXSESSIONPROPERTIES& session)
{
	version(session.ulEAXVersion);
	max_active_sends(session.ulMaxActiveSends);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxFxSlotValidator::effect(
	const GUID& effect)
{
	if (effect != EAX_NULL_GUID &&
		effect != EAX_REVERB_EFFECT)
	{
		throw EaxFxSlotValidatorException{"Unsupported effect id."};
	}
}

void EaxFxSlotValidator::volume(
	long volume)
{
	eax_validator_range_generic<EaxFxSlotValidatorOutOfRangeException>(
		"Volume",
		volume,
		EAXFXSLOT_MINVOLUME,
		EAXFXSLOT_MAXVOLUME
	);
}

void EaxFxSlotValidator::lock(
	long lock)
{
	if (lock != EAXFXSLOT_LOCKED &&
		lock != EAXFXSLOT_UNLOCKED)
	{
		throw EaxFxSlotValidatorException{"Unsupported lock."};
	}
}

void EaxFxSlotValidator::flags(
	int eax_version,
	unsigned long flags)
{
	EaxValidator::version(eax_version);

	if (eax_version != 4 && eax_version != 5)
	{
		throw EaxFxSlotValidatorException{"Unsupported EAX version."};
	}

	if ((eax_version == 4 && (flags & EAX40FXSLOTFLAGS_RESERVED) != 0) ||
		(eax_version == 5 && (flags & EAX50FXSLOTFLAGS_RESERVED) != 0))
	{
		throw EaxFxSlotValidatorException{"Unsupported flags."};
	}
}

void EaxFxSlotValidator::occlusion(
	int eax_version,
	long occlusion)
{
	EaxValidator::version(eax_version);

	if (eax_version != 5)
	{
		throw EaxFxSlotValidatorException{"Unsupported EAX version."};
	}

	eax_validator_range_generic<EaxFxSlotValidatorOutOfRangeException>(
		"Occlusion",
		occlusion,
		EAXFXSLOT_MINOCCLUSION,
		EAXFXSLOT_MAXOCCLUSION
	);
}

void EaxFxSlotValidator::occlusion_lf_ratio(
	int eax_version,
	float occlusion_lf_ratio)
{
	EaxValidator::version(eax_version);

	if (eax_version != 5)
	{
		throw EaxFxSlotValidatorException{"Unsupported EAX version."};
	}

	eax_validator_range_generic<EaxFxSlotValidatorOutOfRangeException>(
		"Occlusion LF Ratio",
		occlusion_lf_ratio,
		EAXFXSLOT_MINOCCLUSIONLFRATIO,
		EAXFXSLOT_MAXOCCLUSIONLFRATIO
	);
}

void EaxFxSlotValidator::all(
	const EAX50FXSLOTPROPERTIES& eax)
{
	effect(eax.guidLoadEffect);
	volume(eax.lVolume);
	lock(eax.lLock);
	flags(5, eax.ulFlags);
	occlusion(5, eax.lOcclusion);
	occlusion_lf_ratio(5, eax.flOcclusionLFRatio);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxSourceValidator::room(
	long room)
{
	eax_validator_range_generic<EaxSourceValidatorOutOfRangeException>(
		"Room",
		room,
		EAXSOURCE_MINROOM,
		EAXSOURCE_MAXROOM
	);
}

void EaxSourceValidator::obstruction(
	long eax_obstruction)
{
	eax_validator_range_generic<EaxSourceValidatorOutOfRangeException>(
		"Obstruction",
		eax_obstruction,
		EAXSOURCE_MINOBSTRUCTION,
		EAXSOURCE_MAXOBSTRUCTION
	);
}

void EaxSourceValidator::obstruction_lf_ratio(
	float eax_obstruction_lf_ratio)
{
	eax_validator_range_generic<EaxSourceValidatorOutOfRangeException>(
		"Obstruction LF Ratio",
		eax_obstruction_lf_ratio,
		EAXSOURCE_MINOBSTRUCTIONLFRATIO,
		EAXSOURCE_MAXOBSTRUCTIONLFRATIO
	);
}

void EaxSourceValidator::obstruction_all(
	const EAXOBSTRUCTIONPROPERTIES& eax_obstruction_properties)
{
	obstruction(eax_obstruction_properties.lObstruction);
	obstruction_lf_ratio(eax_obstruction_properties.flObstructionLFRatio);
}

void EaxSourceValidator::occlusion(
	long occlusion)
{
	eax_validator_range_generic<EaxSourceValidatorOutOfRangeException>(
		"Occlusion",
		occlusion,
		EAXSOURCE_MINOCCLUSION,
		EAXSOURCE_MAXOCCLUSION
	);
}

void EaxSourceValidator::occlusion_lf_ratio(
	float eax_occlusion_lf_ratio)
{
	eax_validator_range_generic<EaxSourceValidatorOutOfRangeException>(
		"Occlusion LF Ratio",
		eax_occlusion_lf_ratio,
		EAXSOURCE_MINOCCLUSIONLFRATIO,
		EAXSOURCE_MAXOCCLUSIONLFRATIO
	);
}

void EaxSourceValidator::occlusion_room_ratio(
	float eax_occlusion_room_ratio)
{
	eax_validator_range_generic<EaxSourceValidatorOutOfRangeException>(
		"Occlusion Room Ratio",
		eax_occlusion_room_ratio,
		EAXSOURCE_MINOCCLUSIONROOMRATIO,
		EAXSOURCE_MAXOCCLUSIONROOMRATIO
	);
}

void EaxSourceValidator::occlusion_direct_ratio(
	float eax_occlusion_direct_ratio)
{
	eax_validator_range_generic<EaxSourceValidatorOutOfRangeException>(
		"Occlusion Direct Ratio",
		eax_occlusion_direct_ratio,
		EAXSOURCE_MINOCCLUSIONDIRECTRATIO,
		EAXSOURCE_MAXOCCLUSIONDIRECTRATIO
	);
}

void EaxSourceValidator::occlusion_all(
	const EAXOCCLUSIONPROPERTIES& eax_occlusion_properties)
{
	occlusion(eax_occlusion_properties.lOcclusion);
	occlusion_lf_ratio(eax_occlusion_properties.flOcclusionLFRatio);
	occlusion_room_ratio(eax_occlusion_properties.flOcclusionRoomRatio);
	occlusion_direct_ratio(eax_occlusion_properties.flOcclusionDirectRatio);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxReverbValidator::environment(
	int eax_version,
	unsigned long environment)
{
	EaxValidator::version(eax_version);

	const auto max_environment = (eax_version < 3 ? EAX20REVERB_MAXENVIRONMENT : EAX30REVERB_MAXENVIRONMENT);

	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Environment",
		environment,
		EAXREVERB_MINENVIRONMENT,
		max_environment
	);
}

void EaxReverbValidator::environment_size(
	float environment_size)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Environment Size",
		environment_size,
		EAXREVERB_MINENVIRONMENTSIZE,
		EAXREVERB_MAXENVIRONMENTSIZE
	);
}

void EaxReverbValidator::environment_diffusion(
	float environment_diffusion)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Environment Diffusion",
		environment_diffusion,
		EAXREVERB_MINENVIRONMENTDIFFUSION,
		EAXREVERB_MAXENVIRONMENTDIFFUSION
	);
}

void EaxReverbValidator::room(
	long room)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Room",
		room,
		EAXREVERB_MINROOM,
		EAXREVERB_MAXROOM
	);
}

void EaxReverbValidator::room_hf(
	long room_hf)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Room HF",
		room_hf,
		EAXREVERB_MINROOMHF,
		EAXREVERB_MAXROOMHF
	);
}

void EaxReverbValidator::room_lf(
	long room_lf)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Room LF",
		room_lf,
		EAXREVERB_MINROOMLF,
		EAXREVERB_MAXROOMLF
	);
}

void EaxReverbValidator::decay_time(
	float decay_time)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Decay Time",
		decay_time,
		EAXREVERB_MINDECAYTIME,
		EAXREVERB_MAXDECAYTIME
	);
}

void EaxReverbValidator::decay_hf_ratio(
	float decay_hf_ratio)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Decay HF Ratio",
		decay_hf_ratio,
		EAXREVERB_MINDECAYHFRATIO,
		EAXREVERB_MAXDECAYHFRATIO
	);
}

void EaxReverbValidator::decay_lf_ratio(
	float decay_lf_ratio)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Decay LF Ratio",
		decay_lf_ratio,
		EAXREVERB_MINDECAYLFRATIO,
		EAXREVERB_MAXDECAYLFRATIO
	);
}

void EaxReverbValidator::reflections(
	long reflections)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Reflections",
		reflections,
		EAXREVERB_MINREFLECTIONS,
		EAXREVERB_MAXREFLECTIONS
	);
}

void EaxReverbValidator::reflections_delay(
	float reflections_delay)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Reflections Delay",
		reflections_delay,
		EAXREVERB_MINREFLECTIONSDELAY,
		EAXREVERB_MAXREFLECTIONSDELAY
	);
}

void EaxReverbValidator::reverb(
	long reverb)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Reverb",
		reverb,
		EAXREVERB_MINREVERB,
		EAXREVERB_MAXREVERB
	);
}

void EaxReverbValidator::reverb_delay(
	float reverb_delay)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Reverb Delay",
		reverb_delay,
		EAXREVERB_MINREVERBDELAY,
		EAXREVERB_MAXREVERBDELAY
	);
}

void EaxReverbValidator::echo_time(
	float echo_time)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Echo Time",
		echo_time,
		EAXREVERB_MINECHOTIME,
		EAXREVERB_MAXECHOTIME
	);
}

void EaxReverbValidator::echo_depth(
	float echo_depth)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Echo Depth",
		echo_depth,
		EAXREVERB_MINECHODEPTH,
		EAXREVERB_MAXECHODEPTH
	);
}

void EaxReverbValidator::modulation_time(
	float modulation_time)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Modulation Time",
		modulation_time,
		EAXREVERB_MINMODULATIONTIME,
		EAXREVERB_MAXMODULATIONTIME
	);
}

void EaxReverbValidator::modulation_depth(
	float modulation_depth)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Modulation Depth",
		modulation_depth,
		EAXREVERB_MINMODULATIONDEPTH,
		EAXREVERB_MAXMODULATIONDEPTH
	);
}

void EaxReverbValidator::air_absorption_hf(
	float air_absorption_hf)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Air Absorption HF",
		air_absorption_hf,
		EAXREVERB_MINAIRABSORPTIONHF,
		EAXREVERB_MAXAIRABSORPTIONHF
	);
}

void EaxReverbValidator::hf_reference(
	float hf_reference)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"HF Reference",
		hf_reference,
		EAXREVERB_MINHFREFERENCE,
		EAXREVERB_MAXHFREFERENCE
	);
}

void EaxReverbValidator::lf_reference(
	float lf_reference)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"LF Reference",
		lf_reference,
		EAXREVERB_MINLFREFERENCE,
		EAXREVERB_MAXLFREFERENCE
	);
}

void EaxReverbValidator::room_rolloff_factor(
	float room_rolloff_factor)
{
	eax_validator_range_generic<EaxReverbValidatorOutOfRangeException>(
		"Room Rolloff Factor",
		room_rolloff_factor,
		EAXREVERB_MINROOMROLLOFFFACTOR,
		EAXREVERB_MAXROOMROLLOFFFACTOR
	);
}

void EaxReverbValidator::flags(
	unsigned long flags)
{
	if ((flags & EAXREVERBFLAGS_RESERVED) != 0)
	{
		throw EaxReverbValidatorFlagsException{flags};
	}
}

void EaxReverbValidator::all(
	int eax_version,
	const EAXREVERBPROPERTIES& properties)
{
	EaxValidator::version(eax_version);

	environment(eax_version, properties.ulEnvironment);
	environment_size(properties.flEnvironmentSize);
	environment_diffusion(properties.flEnvironmentDiffusion);
	room(properties.lRoom);
	room_hf(properties.lRoomHF);
	room_lf(properties.lRoomLF);
	decay_time(properties.flDecayTime);
	decay_hf_ratio(properties.flDecayHFRatio);
	decay_lf_ratio(properties.flDecayLFRatio);
	reflections(properties.lReflections);
	reflections_delay(properties.flReflectionsDelay);
	reverb(properties.lReverb);
	reverb_delay(properties.flReverbDelay);
	echo_time(properties.flEchoTime);
	echo_depth(properties.flEchoDepth);
	modulation_time(properties.flModulationTime);
	modulation_depth(properties.flModulationDepth);
	air_absorption_hf(properties.flAirAbsorptionHF);
	hf_reference(properties.flHFReference);
	lf_reference(properties.flLFReference);
	room_rolloff_factor(properties.flRoomRolloffFactor);
	flags(properties.ulFlags);
}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
