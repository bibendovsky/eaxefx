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


#ifndef EAXEFX_EAXX_RING_MODULATOR_EFFECT_INCLUDED
#define EAXEFX_EAXX_RING_MODULATOR_EFFECT_INCLUDED


#include <cstdint>

#include "eaxefx_al_object.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eax_api.h"

#include "eaxefx_eaxx_eax_call.h"
#include "eaxefx_eaxx_effect.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using EaxxRingModulatorEffectEaxDirtyFlagsValue = unsigned int;

struct EaxxRingModulatorEffectEaxDirtyFlags
{
	EaxxRingModulatorEffectEaxDirtyFlagsValue flFrequency : 1;
	EaxxRingModulatorEffectEaxDirtyFlagsValue flHighPassCutOff : 1;
	EaxxRingModulatorEffectEaxDirtyFlagsValue ulWaveform : 1;
}; // EaxxPitchShifterEffectEaxDirtyFlags

static_assert(sizeof(EaxxRingModulatorEffectEaxDirtyFlags) == sizeof(EaxxRingModulatorEffectEaxDirtyFlagsValue));

bool operator==(
	const EaxxRingModulatorEffectEaxDirtyFlags& lhs,
	const EaxxRingModulatorEffectEaxDirtyFlags& rhs) noexcept;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxRingModulatorEffect final :
	public EaxxEffect
{
public:
	EaxxRingModulatorEffect(
		::ALuint al_effect_slot,
		const AlEfxSymbols* al_efx_symbols);


	void load() override;

	void dispatch(
		const EaxxEaxCall& eax_call) override;


private:
	const ::ALuint al_effect_slot_;
	const AlEfxSymbols* al_efx_symbols_;
	EfxEffectObject efx_effect_object_;

	::EAXRINGMODULATORPROPERTIES eax_;
	::EAXRINGMODULATORPROPERTIES eax_d_;
	EaxxRingModulatorEffectEaxDirtyFlags eax_dirty_flags_{};


	void set_eax_defaults();


	void set_efx_frequency();

	void set_efx_high_pass_cutoff();

	void set_efx_waveform();

	void set_efx_defaults();


	void get(
		const EaxxEaxCall& eax_call);


	void validate_frequency(
		float flFrequency);

	void validate_high_pass_cutoff(
		float flHighPassCutOff);

	void validate_waveform(
		std::uint32_t ulWaveform);

	void validate_all(
		const ::EAXRINGMODULATORPROPERTIES& all);


	void defer_frequency(
		float flFrequency);

	void defer_high_pass_cutoff(
		float flHighPassCutOff);

	void defer_waveform(
		std::uint32_t ulWaveform);

	void defer_all(
		const ::EAXRINGMODULATORPROPERTIES& all);


	void defer_frequency(
		const EaxxEaxCall& eax_call);

	void defer_high_pass_cutoff(
		const EaxxEaxCall& eax_call);

	void defer_waveform(
		const EaxxEaxCall& eax_call);

	void defer_all(
		const EaxxEaxCall& eax_call);


	void apply_deferred();

	void set(
		const EaxxEaxCall& eax_call);
}; // EaxxRingModulatorEffect

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_RING_MODULATOR_EFFECT_INCLUDED
