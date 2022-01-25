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


#ifndef EAXEFX_EAXX_PITCH_SHIFTER_EFFECT_INCLUDED
#define EAXEFX_EAXX_PITCH_SHIFTER_EFFECT_INCLUDED


#include <cstdint>

#include "eaxefx_al_object.h"
#include "eaxefx_al_symbols.h"
#include "eaxefx_eax_api.h"

#include "eaxefx_eaxx_eax_call.h"
#include "eaxefx_eaxx_effect.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using EaxxPitchShifterEffectEaxDirtyFlagsValue = unsigned int;

struct EaxxPitchShifterEffectEaxDirtyFlags
{
	EaxxPitchShifterEffectEaxDirtyFlagsValue lCoarseTune : 1;
	EaxxPitchShifterEffectEaxDirtyFlagsValue lFineTune : 1;
}; // EaxxPitchShifterEffectEaxDirtyFlags

static_assert(sizeof(EaxxPitchShifterEffectEaxDirtyFlags) == sizeof(EaxxPitchShifterEffectEaxDirtyFlagsValue));

bool operator==(
	const EaxxPitchShifterEffectEaxDirtyFlags& lhs,
	const EaxxPitchShifterEffectEaxDirtyFlags& rhs) noexcept;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxPitchShifterEffect final :
	public EaxxEffect
{
public:
	EaxxPitchShifterEffect(
		ALuint al_effect_slot,
		const AlEfxSymbols* al_efx_symbols);


	void load() override;

	void dispatch(
		const EaxxEaxCall& eax_call) override;


private:
	const ALuint al_effect_slot_;
	const AlEfxSymbols* al_efx_symbols_;
	EfxEffectObject efx_effect_object_;

	EAXPITCHSHIFTERPROPERTIES eax_;
	EAXPITCHSHIFTERPROPERTIES eax_d_;
	EaxxPitchShifterEffectEaxDirtyFlags eax_dirty_flags_{};


	void set_eax_defaults();


	void set_efx_coarse_tune();

	void set_efx_fine_tune();

	void set_efx_defaults();


	void get(
		const EaxxEaxCall& eax_call);


	void validate_coarse_tune(
		std::int32_t lCoarseTune);

	void validate_fine_tune(
		std::int32_t lFineTune);

	void validate_all(
		const EAXPITCHSHIFTERPROPERTIES& all);


	void defer_coarse_tune(
		std::int32_t lCoarseTune);

	void defer_fine_tune(
		std::int32_t lFineTune);

	void defer_all(
		const EAXPITCHSHIFTERPROPERTIES& all);


	void defer_coarse_tune(
		const EaxxEaxCall& eax_call);

	void defer_fine_tune(
		const EaxxEaxCall& eax_call);

	void defer_all(
		const EaxxEaxCall& eax_call);


	void apply_deferred();

	void set(
		const EaxxEaxCall& eax_call);
}; // EaxxPitchShifterEffect

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_PITCH_SHIFTER_EFFECT_INCLUDED
