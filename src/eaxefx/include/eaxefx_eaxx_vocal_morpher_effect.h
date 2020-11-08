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


#ifndef EAXEFX_EAXX_VOCAL_MORPHER_EFFECT_INCLUDED
#define EAXEFX_EAXX_VOCAL_MORPHER_EFFECT_INCLUDED


#include <cstdint>

#include "eaxefx_al_object.h"
#include "eaxefx_eax_api.h"

#include "eaxefx_eaxx_eax_call.h"
#include "eaxefx_eaxx_effect.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using EaxxVocalMorpherEffectEaxDirtyFlagsValue = unsigned int;

struct EaxxVocalMorpherEffectEaxDirtyFlags
{
	EaxxVocalMorpherEffectEaxDirtyFlagsValue ulPhonemeA : 1;
	EaxxVocalMorpherEffectEaxDirtyFlagsValue lPhonemeACoarseTuning : 1;
	EaxxVocalMorpherEffectEaxDirtyFlagsValue ulPhonemeB : 1;
	EaxxVocalMorpherEffectEaxDirtyFlagsValue lPhonemeBCoarseTuning : 1;
	EaxxVocalMorpherEffectEaxDirtyFlagsValue ulWaveform : 1;
	EaxxVocalMorpherEffectEaxDirtyFlagsValue flRate : 1;
}; // EaxxPitchShifterEffectEaxDirtyFlags

static_assert(sizeof(EaxxVocalMorpherEffectEaxDirtyFlags) == sizeof(EaxxVocalMorpherEffectEaxDirtyFlagsValue));

bool operator==(
	const EaxxVocalMorpherEffectEaxDirtyFlags& lhs,
	const EaxxVocalMorpherEffectEaxDirtyFlags& rhs) noexcept;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxVocalMorpherEffect final :
	public EaxxEffect
{
public:
	EaxxVocalMorpherEffect(
		ALuint al_effect_slot);


	void load() override;

	void dispatch(
		const EaxxEaxCall& eax_call) override;


private:
	const ALuint al_effect_slot_;
	EfxEffectObject efx_effect_object_;

	EAXVOCALMORPHERPROPERTIES eax_;
	EAXVOCALMORPHERPROPERTIES eax_d_;
	EaxxVocalMorpherEffectEaxDirtyFlags eax_dirty_flags_{};


	void set_eax_defaults();


	void set_efx_phoneme_a();

	void set_efx_phoneme_a_coarse_tuning();

	void set_efx_phoneme_b();

	void set_efx_phoneme_b_coarse_tuning();

	void set_efx_waveform();

	void set_efx_rate();

	void set_efx_defaults();


	void get(
		const EaxxEaxCall& eax_call);


	void validate_phoneme_a(
		std::uint32_t ulPhonemeA);

	void validate_phoneme_a_coarse_tuning(
		std::int32_t lPhonemeACoarseTuning);

	void validate_phoneme_b(
		std::uint32_t ulPhonemeB);

	void validate_phoneme_b_coarse_tuning(
		std::int32_t lPhonemeBCoarseTuning);

	void validate_waveform(
		std::uint32_t ulWaveform);

	void validate_rate(
		float flRate);

	void validate_all(
		const EAXVOCALMORPHERPROPERTIES& all);


	void defer_phoneme_a(
		std::uint32_t ulPhonemeA);

	void defer_phoneme_a_coarse_tuning(
		std::int32_t lPhonemeACoarseTuning);

	void defer_phoneme_b(
		std::uint32_t ulPhonemeB);

	void defer_phoneme_b_coarse_tuning(
		std::int32_t lPhonemeBCoarseTuning);

	void defer_waveform(
		std::uint32_t ulWaveform);

	void defer_rate(
		float flRate);

	void defer_all(
		const EAXVOCALMORPHERPROPERTIES& all);


	void defer_phoneme_a(
		const EaxxEaxCall& eax_call);

	void defer_phoneme_a_coarse_tuning(
		const EaxxEaxCall& eax_call);

	void defer_phoneme_b(
		const EaxxEaxCall& eax_call);

	void defer_phoneme_b_coarse_tuning(
		const EaxxEaxCall& eax_call);

	void defer_waveform(
		const EaxxEaxCall& eax_call);

	void defer_rate(
		const EaxxEaxCall& eax_call);

	void defer_all(
		const EaxxEaxCall& eax_call);


	void apply_deferred();

	void set(
		const EaxxEaxCall& eax_call);
}; // EaxxVocalMorpherEffect

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_VOCAL_MORPHER_EFFECT_INCLUDED
