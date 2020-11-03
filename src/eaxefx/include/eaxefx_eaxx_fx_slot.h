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


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxFxSlot
{
public:
	void initialize();


	ALuint get_efx_effect_slot() const noexcept;

	const EAX50FXSLOTPROPERTIES& get_eax_fx_slot() const noexcept;


	void set_effect(
		const GUID& eax_id);

	void set_volume(
		long eax_volume);

	void set_lock(
		long eax_lock);

	void set_flags(
		int eax_version,
		unsigned long eax_flags);

	[[nodiscard]] bool set_occlusion(
		int eax_version,
		long eax_occlusion);

	[[nodiscard]] bool set_occlusion_lf_ratio(
		int eax_version,
		float eax_occlusion_lf_ratio);

	[[nodiscard]] bool set(
		const EAX50FXSLOTPROPERTIES& eax_fx_slot);


	void set_reverb_environment(
		int eax_version,
		unsigned long eax_reverb_environment);

	void set_reverb(
		int eax_version,
		const EAXREVERBPROPERTIES& eax_reverb);


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

	void set_efx_eax_reverb_properties();

	void attach_efx_effect();


	void initialize_efx_slot();

	void initialize_efx();


	void set_efx_effect_properties();

	void set_effect();


	void set_efx_effect_slot_gain();

	void set_volume();


	void set_effect_slot_send_auto();

	void set_flags();


	[[nodiscard]] bool set_eax_occlusion(
		int eax_version,
		long eax_occlusion);

	[[nodiscard]] bool set_eax_occlusion_lf_ratio(
		int eax_version,
		float eax_occlusion_lf_ratio);


	void set_reverb_environment();

	void set_reverb();
}; // EaxxFxSlot

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_FX_SLOT_INCLUDED
