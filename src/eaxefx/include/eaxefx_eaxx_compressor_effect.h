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


#ifndef EAXEFX_EAXX_COMPRESSOR_EFFECT_INCLUDED
#define EAXEFX_EAXX_COMPRESSOR_EFFECT_INCLUDED


#include <cstdint>

#include "eaxefx_al_object.h"
#include "eaxefx_eax_api.h"

#include "eaxefx_eaxx_eax_call.h"
#include "eaxefx_eaxx_effect.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using EaxxCompressorEffectEaxDirtyFlagsValue = unsigned int;

struct EaxxCompressorEffectEaxDirtyFlags
{
	EaxxCompressorEffectEaxDirtyFlagsValue ulOnOff : 1;
}; // EaxxCompressorEffectEaxDirtyFlags

static_assert(sizeof(EaxxCompressorEffectEaxDirtyFlags) == sizeof(EaxxCompressorEffectEaxDirtyFlags));

bool operator==(
	const EaxxCompressorEffectEaxDirtyFlags& lhs,
	const EaxxCompressorEffectEaxDirtyFlags& rhs) noexcept;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxCompressorEffect final :
	public EaxxEffect
{
public:
	EaxxCompressorEffect(
		ALuint al_effect_slot);


	void load() override;

	void dispatch(
		const EaxxEaxCall& eax_call) override;


private:
	const ALuint al_effect_slot_;
	EfxEffectObject efx_effect_object_;

	EAXAGCCOMPRESSORPROPERTIES eax_;
	EAXAGCCOMPRESSORPROPERTIES eax_d_;
	EaxxCompressorEffectEaxDirtyFlags eax_dirty_flags_{};


	void set_eax_defaults();


	void set_efx_on_off();

	void set_efx_defaults();


	void get(
		const EaxxEaxCall& eax_call);


	void validate_on_off(
		std::uint32_t ulOnOff);

	void validate_all(
		const EAXAGCCOMPRESSORPROPERTIES& eax_all);


	void defer_on_off(
		std::uint32_t ulOnOff);

	void defer_all(
		const EAXAGCCOMPRESSORPROPERTIES& eax_all);


	void defer_on_off(
		const EaxxEaxCall& eax_call);

	void defer_all(
		const EaxxEaxCall& eax_call);


	void apply_deferred();

	void set(
		const EaxxEaxCall& eax_call);
}; // EaxxCompressorEffect

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_COMPRESSOR_EFFECT_INCLUDED
