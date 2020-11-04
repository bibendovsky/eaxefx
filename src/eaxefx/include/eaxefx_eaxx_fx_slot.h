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


#ifndef EAXEFX_EAXX_FX_SLOT_INCLUDED
#define EAXEFX_EAXX_FX_SLOT_INCLUDED


#include "AL/al.h"

#include "eaxefx_eax_api.h"
#include "eaxefx_eaxx_eax_call.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxFxSlot
{
public:
	void initialize();


	ALuint get_efx_effect_slot() const noexcept;

	const EAX50FXSLOTPROPERTIES& get_eax_fx_slot() const noexcept;


	void set_fx_slot_default_effect();


	void get(
		const EaxxEaxCall& eax_call) const;

	[[nodiscard]] bool set(
		const EaxxEaxCall& eax_call);


private:
	struct Eax
	{
		EAX50FXSLOTPROPERTIES fx_slot{};
		EAXREVERBPROPERTIES reverb{};
	}; // Eax

	struct Efx
	{
		ALuint effect{};
		ALuint effect_slot{};
	}; // Efx


	bool is_reverb_{};
	Eax eax_{};
	Efx efx_{};


	void set_eax_fx_slot_defaults();

	void set_eax_reverb_defaults();

	void initialize_eax();


	void create_efx_effect();

	void create_efx_effect_slot();

	void create_efx_objects();


	void set_efx_effect();

	void set_efx_eax_reverb_environment_diffusion();

	void set_efx_eax_reverb_room();

	void set_efx_eax_reverb_room_hf();

	void set_efx_eax_reverb_room_lf();

	void set_efx_eax_reverb_decay_time();

	void set_efx_eax_reverb_decay_hf_ratio();

	void set_efx_eax_reverb_decay_lf_ratio();

	void set_efx_eax_reverb_reflections();

	void set_efx_eax_reverb_reflections_delay();

	void set_efx_eax_reverb_reflections_pan();

	void set_efx_eax_reverb_reverb();

	void set_efx_eax_reverb_reverb_delay();

	void set_efx_eax_reverb_reverb_pan();

	void set_efx_eax_reverb_echo_time();

	void set_efx_eax_reverb_echo_depth();

	void set_efx_eax_reverb_modulation_time();

	void set_efx_eax_reverb_modulation_depth();

	void set_efx_eax_reverb_air_absorption_hf();

	void set_efx_eax_reverb_hf_reference();

	void set_efx_eax_reverb_lf_reference();

	void set_efx_eax_reverb_room_rolloff_factor();

	void set_efx_eax_reverb_flags();

	void set_efx_eax_reverb_properties();

	void attach_efx_effect();


	void initialize_efx_slot();

	void initialize_efx();


	void get_fx_slot_all(
		const EaxxEaxCall& eax_call) const;

	void get_fx_slot_ffect(
		const EaxxEaxCall& eax_call) const;

	void get_fx_slot_volume(
		const EaxxEaxCall& eax_call) const;

	void get_fx_slot_lock(
		const EaxxEaxCall& eax_call) const;

	void get_fx_slot_flags(
		const EaxxEaxCall& eax_call) const;

	void get_fx_slot_occlusion(
		const EaxxEaxCall& eax_call) const;

	void get_fx_slot_occlusion_lf_ratio(
		const EaxxEaxCall& eax_call) const;


	void set_efx_effect_properties();

	void set_fx_slot_effect();


	void set_efx_effect_slot_gain();

	void set_fx_slot_volume();


	void set_effect_slot_send_auto();

	void set_fx_slot_flags();


	static void validate_fx_slot_effect(
		const GUID& eax_effect_id);


	void set_fx_slot_effect(
		const GUID& eax_effect_id);

	void set_fx_slot_volume(
		long eax_volume);

	void set_fx_slot_lock(
		long eax_lock);

	void set_fx_slot_flags(
		unsigned long eax_flags);

	[[nodiscard]] bool set_fx_slot_occlusion(
		long eax_occlusion);

	[[nodiscard]] bool set_fx_slot_occlusion_lf_ratio(
		float eax_occlusion_lf_ratio);

	void set_fx_slot_all(
		const EAX40FXSLOTPROPERTIES& eax_fx_slot);

	[[nodiscard]] bool set_fx_slot_all(
		const EAX50FXSLOTPROPERTIES& eax_fx_slot);


	[[nodiscard]] bool set_fx_slot_effect(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_fx_slot_volume(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_fx_slot_lock(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_fx_slot_flags(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_fx_slot_occlusion(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_fx_slot_occlusion_lf_ratio(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_fx_slot_all(
		const EaxxEaxCall& eax_call);


	void set_reverb_environment();

	void set_reverb_room();

	void set_reverb_reflections_pan();

	void set_reverb_reverb_pan();

	void set_reverb_all();


	void ensure_reverb_effect();


	void set_reverb_all(
		const EAXREVERBPROPERTIES& eax_reverb);

	void set_reverb_environment(
		unsigned long eax_reverb_environment);

	void set_reverb_room(
		long eax_reverb_room);

	void set_reverb_reflections_pan(
		const EAXVECTOR& eax_reverb_reflections_pan);

	void set_reverb_reverb_pan(
		const EAXVECTOR& eax_reverb_pan);


	[[nodiscard]] bool set_reverb_all(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_reverb_environment(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_reverb_room(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_reverb_reflections_pan(
		const EaxxEaxCall& eax_call);

	[[nodiscard]] bool set_reverb_reverb_pan(
		const EaxxEaxCall& eax_call);
}; // EaxxFxSlot

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_FX_SLOT_INCLUDED
