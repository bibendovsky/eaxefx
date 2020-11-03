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


#include "eaxefx_eax_converters.h"

#include <algorithm>

#include "AL/efx.h"

#include "eaxefx_unit_converters.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

float EaxReverbToEfx::environment_diffusion(
	float environment_diffusion)
{
	return std::clamp(
		environment_diffusion,
		AL_EAXREVERB_MIN_DIFFUSION,
		AL_EAXREVERB_MAX_DIFFUSION);
}

float EaxReverbToEfx::room(
	long room)
{
	return std::clamp(
		mb_to_gain(room),
		AL_EAXREVERB_MIN_GAIN,
		AL_EAXREVERB_MAX_GAIN);
}

float EaxReverbToEfx::room_hf(
	long room_hf)
{
	return std::clamp(
		mb_to_gain(room_hf),
		AL_EAXREVERB_MIN_GAINHF,
		AL_EAXREVERB_MAX_GAINHF);
}

float EaxReverbToEfx::room_lf(
	long room_lf)
{
	return std::clamp(
		mb_to_gain(room_lf),
		AL_EAXREVERB_MIN_GAINLF,
		AL_EAXREVERB_MAX_GAINLF);
}

float EaxReverbToEfx::decay_time(
	float decay_time)
{
	return std::clamp(
		decay_time,
		AL_EAXREVERB_MIN_DECAY_TIME,
		AL_EAXREVERB_MAX_DECAY_TIME);
}

float EaxReverbToEfx::decay_hf_ratio(
	float decay_hf_ratio)
{
	return std::clamp(
		decay_hf_ratio,
		AL_EAXREVERB_MIN_DECAY_HFRATIO,
		AL_EAXREVERB_MAX_DECAY_HFRATIO);
}

float EaxReverbToEfx::decay_lf_ratio(
	float decay_lf_ratio)
{
	return std::clamp(
		decay_lf_ratio,
		AL_EAXREVERB_MIN_DECAY_LFRATIO,
		AL_EAXREVERB_MAX_DECAY_LFRATIO);
}

float EaxReverbToEfx::reflections(
	long reflections)
{
	return std::clamp(
		mb_to_gain(reflections),
		AL_EAXREVERB_MIN_REFLECTIONS_GAIN,
		AL_EAXREVERB_MAX_REFLECTIONS_GAIN);
}

float EaxReverbToEfx::reflections_delay(
	float reflections_delay)
{
	return std::clamp(
		reflections_delay,
		AL_EAXREVERB_MIN_REFLECTIONS_DELAY,
		AL_EAXREVERB_MAX_REFLECTIONS_DELAY);
}

float EaxReverbToEfx::reverb(
	long reverb)
{
	return std::clamp(
		mb_to_gain(reverb),
		AL_EAXREVERB_MIN_LATE_REVERB_GAIN,
		AL_EAXREVERB_MAX_LATE_REVERB_GAIN);
}

float EaxReverbToEfx::reverb_delay(
	float reverb_delay)
{
	return std::clamp(
		reverb_delay,
		AL_EAXREVERB_MIN_LATE_REVERB_DELAY,
		AL_EAXREVERB_MAX_LATE_REVERB_DELAY);
}

float EaxReverbToEfx::echo_time(
	float echo_time)
{
	return std::clamp(
		echo_time,
		AL_EAXREVERB_MIN_ECHO_TIME,
		AL_EAXREVERB_MAX_ECHO_TIME);
}

float EaxReverbToEfx::echo_depth(
	float echo_depth)
{
	return std::clamp(
		echo_depth,
		AL_EAXREVERB_MIN_ECHO_DEPTH,
		AL_EAXREVERB_MAX_ECHO_DEPTH);
}

float EaxReverbToEfx::modulation_time(
	float modulation_time)
{
	return std::clamp(
		modulation_time,
		AL_EAXREVERB_MIN_MODULATION_TIME,
		AL_EAXREVERB_MAX_MODULATION_TIME);
}

float EaxReverbToEfx::modulation_depth(
	float modulation_depth)
{
	return std::clamp(
		modulation_depth,
		AL_EAXREVERB_MIN_MODULATION_DEPTH,
		AL_EAXREVERB_MAX_MODULATION_DEPTH);
}

float EaxReverbToEfx::air_absorption_hf(
	float air_absorption_hf)
{
	return std::clamp(
		mb_to_gain(air_absorption_hf),
		AL_EAXREVERB_MIN_AIR_ABSORPTION_GAINHF,
		AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF);
}

float EaxReverbToEfx::hf_reference(
	float hf_reference)
{
	return std::clamp(
		hf_reference,
		AL_EAXREVERB_MIN_HFREFERENCE,
		AL_EAXREVERB_MAX_HFREFERENCE);
}

float EaxReverbToEfx::lf_reference(
	float lf_reference)
{
	return std::clamp(
		lf_reference,
		AL_EAXREVERB_MIN_LFREFERENCE,
		AL_EAXREVERB_MAX_LFREFERENCE);
}

float EaxReverbToEfx::room_rolloff_factor(
	float room_rolloff_factor)
{
	return std::clamp(
		room_rolloff_factor,
		AL_EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR,
		AL_EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

float EaxFxSlotToEfx::volume(
	long volume)
{
	return std::clamp(mb_to_gain(volume), 0.0F, 1.0F);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
