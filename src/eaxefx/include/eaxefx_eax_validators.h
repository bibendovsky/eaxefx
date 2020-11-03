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


#ifndef EAXEFX_EAX_VALIDATORS_INCLUDED
#define EAXEFX_EAX_VALIDATORS_INCLUDED


#include "eaxefx_eax_api.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxValidator
{
	static void version(
		int version);
}; // EaxFxSlotValidator

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxContextValidator
{
	static void primary_fx_slot_id(
		const GUID& eax_primary_fx_slot_id);

	static void distance_factor(
		float eax_distance_factor);

	static void air_absorption_hf(
		float eax_air_absorption_hf);

	static void hf_reference(
		float eax_hf_reference);

	static void macro_fx_factor(
		float eax_macro_fx_factor);

	static void all(
		const EAX50CONTEXTPROPERTIES& eax_context);
}; // EaxContextValidator

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxSessionValidator
{
	static void version(
		unsigned long eax_version);

	static void max_active_sends(
		unsigned long eax_max_active_sends);

	static void all(
		const EAXSESSIONPROPERTIES& session);
}; // EaxFxSlotValidator

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxFxSlotValidator
{
	static void effect(
		const GUID& effect);

	static void volume(
		long volume);

	static void lock(
		long lock);

	static void flags(
		int eax_version,
		unsigned long flags);

	static void occlusion(
		int eax_version,
		long occlusion);

	static void occlusion_lf_ratio(
		int eax_version,
		float occlusion_lf_ratio);

	static void all(
		const EAX50FXSLOTPROPERTIES& eax);
}; // EaxFxSlotValidator

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxSourceValidator
{
	static void room(
		long room);

	static void obstruction(
		long eax_obstruction);

	static void obstruction_lf_ratio(
		float eax_obstruction_lf_ratio);

	static void obstruction_all(
		const EAXOBSTRUCTIONPROPERTIES& eax_obstruction_properties);

	static void occlusion(
		long occlusion);

	static void occlusion_lf_ratio(
		float eax_occlusion_lf_ratio);

	static void occlusion_room_ratio(
		float eax_occlusion_room_ratio);

	static void occlusion_direct_ratio(
		float eax_occlusion_direct_ratio);

	static void occlusion_all(
		const EAXOCCLUSIONPROPERTIES& eax_occlusion_properties);
}; // EaxSourceValidator

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxReverbValidator
{
	static void environment(
		int eax_version,
		unsigned long environment);

	static void environment_size(
		float environment_size);

	static void environment_diffusion(
		float environment_diffusion);

	static void room(
		long room);

	static void room_hf(
		long room_hf);

	static void room_lf(
		long room_lf);

	static void decay_time(
		float decay_time);

	static void decay_hf_ratio(
		float decay_hf_ratio);

	static void decay_lf_ratio(
		float decay_lf_ratio);

	static void reflections(
		long reflections);

	static void reflections_delay(
		float reflections_delay);

	static void reverb(
		long reverb);

	static void reverb_delay(
		float reverb_delay);

	static void echo_time(
		float echo_time);

	static void echo_depth(
		float echo_depth);

	static void modulation_time(
		float modulation_time);

	static void modulation_depth(
		float modulation_depth);

	static void air_absorption_hf(
		float air_absorption_hf);

	static void hf_reference(
		float hf_reference);

	static void lf_reference(
		float lf_reference);

	static void room_rolloff_factor(
		float room_rolloff_factor);

	static void flags(
		unsigned long flags);


	static void all(
		int eax_version,
		const EAXREVERBPROPERTIES& properties);
}; // EaxReverbValidator


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAX_VALIDATORS_INCLUDED
