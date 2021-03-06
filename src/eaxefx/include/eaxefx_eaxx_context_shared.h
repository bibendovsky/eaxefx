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


#ifndef EAXEFX_EAXX_CONTEXT_SHARED_INCLUDED
#define EAXEFX_EAXX_CONTEXT_SHARED_INCLUDED


#include "eaxefx_eaxx_fx_slot_index.h"
#include "eaxefx_eaxx_fx_slots.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxxContextShared
{
	float max_filter_gain{};
	float air_absorption_factor{};
	EaxxFxSlotIndex previous_primary_fx_slot_index{};
	EaxxFxSlotIndex primary_fx_slot_index{};
	EaxxFxSlots fx_slots{};
}; // EaxxContextShared

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

using EaxxContextSharedDirtyFlagsValue = unsigned int;

struct EaxxContextSharedDirtyFlags
{
	EaxxContextSharedDirtyFlagsValue primary_fx_slot_id : 1;
	EaxxContextSharedDirtyFlagsValue air_absorption_hf : 1;
}; // EaxxContextSharedDirtyFlags

static_assert(sizeof(EaxxContextSharedDirtyFlags) == sizeof(EaxxContextSharedDirtyFlagsValue));


bool operator==(
	const EaxxContextSharedDirtyFlags& lhs,
	const EaxxContextSharedDirtyFlags& rhs) noexcept;

bool operator!=(
	const EaxxContextSharedDirtyFlags& lhs,
	const EaxxContextSharedDirtyFlags& rhs) noexcept;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_CONTEXT_SHARED_INCLUDED
