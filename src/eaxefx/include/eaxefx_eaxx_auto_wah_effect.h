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


#ifndef EAXEFX_EAXX_AUTO_WAH_EFFECT_INCLUDED
#define EAXEFX_EAXX_AUTO_WAH_EFFECT_INCLUDED


#include <cstdint>

#include "eaxefx_al_object.h"
#include "eaxefx_eax_api.h"

#include "eaxefx_eaxx_eax_call.h"
#include "eaxefx_eaxx_effect.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using EaxxAutoWahEffectEaxDirtyFlagsValue = unsigned int;

struct EaxxAutoWahEffectEaxDirtyFlags
{
	EaxxAutoWahEffectEaxDirtyFlagsValue flAttackTime : 1;
	EaxxAutoWahEffectEaxDirtyFlagsValue flReleaseTime : 1;
	EaxxAutoWahEffectEaxDirtyFlagsValue lResonance : 1;
	EaxxAutoWahEffectEaxDirtyFlagsValue lPeakLevel : 1;
}; // EaxxAutoWahEffectEaxDirtyFlags

static_assert(sizeof(EaxxAutoWahEffectEaxDirtyFlags) == sizeof(EaxxAutoWahEffectEaxDirtyFlagsValue));

bool operator==(
	const EaxxAutoWahEffectEaxDirtyFlags& lhs,
	const EaxxAutoWahEffectEaxDirtyFlags& rhs) noexcept;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxAutoWahEffect final :
	public EaxxEffect
{
public:
	EaxxAutoWahEffect(
		ALuint al_effect_slot);


	void load() override;

	void dispatch(
		const EaxxEaxCall& eax_call) override;


private:
	const ALuint al_effect_slot_;
	EfxEffectObject efx_effect_object_;

	EAXAUTOWAHPROPERTIES eax_;
	EAXAUTOWAHPROPERTIES eax_d_;
	EaxxAutoWahEffectEaxDirtyFlags eax_dirty_flags_{};


	void set_eax_defaults();


	void set_efx_attack_time();

	void set_efx_release_time();

	void set_efx_resonance();

	void set_efx_peak_gain();

	void set_efx_defaults();


	void get(
		const EaxxEaxCall& eax_call);


	void validate_attack_time(
		float flAttackTime);

	void validate_release_time(
		float flReleaseTime);

	void validate_resonance(
		std::int32_t lResonance);

	void validate_peak_level(
		std::int32_t lPeakLevel);

	void validate_all(
		const EAXAUTOWAHPROPERTIES& eax_all);


	void defer_attack_time(
		float flAttackTime);

	void defer_release_time(
		float flReleaseTime);

	void defer_resonance(
		std::int32_t lResonance);

	void defer_peak_level(
		std::int32_t lPeakLevel);

	void defer_all(
		const EAXAUTOWAHPROPERTIES& eax_all);


	void defer_attack_time(
		const EaxxEaxCall& eax_call);

	void defer_release_time(
		const EaxxEaxCall& eax_call);

	void defer_resonance(
		const EaxxEaxCall& eax_call);

	void defer_peak_level(
		const EaxxEaxCall& eax_call);

	void defer_all(
		const EaxxEaxCall& eax_call);

	void apply_deferred();

	void set(
		const EaxxEaxCall& eax_call);
}; // EaxxAutoWahEffect

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_AUTO_WAH_EFFECT_INCLUDED
