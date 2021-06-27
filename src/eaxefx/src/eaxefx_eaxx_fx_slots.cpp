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


#include "eaxefx_eaxx_fx_slots.h"

#include <array>

#include "eaxefx_exception.h"

#include "eaxefx_eax_api.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxFxSlotsException :
	public Exception
{
public:
	explicit EaxxFxSlotsException(
		const char* message)
		:
		Exception{"EAXX_FX_SLOTS", message}
	{
	}
}; // EaxxFxSlotsException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxxFxSlots::initialize(
	const AlEfxSymbols* al_efx_symbols)
{
	if (!al_efx_symbols)
	{
		throw EaxxFxSlotsException{"Null EFX symbols."};
	}

	initialize_fx_slots(al_efx_symbols);
	initialize_default_slot();
}

EaxxFxSlot& EaxxFxSlots::get(
	EaxxFxSlotIndex index)
{
	if (!index.has_value())
	{
		throw EaxxFxSlotsException{"Empty index."};
	}

	return fx_slots_[index.get()];
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxxFxSlots::initialize_fx_slots(
	const AlEfxSymbols* al_efx_symbols)
{
	for (auto& fx_slot : fx_slots_)
	{
		fx_slot.initialize(al_efx_symbols);
	}
}

void EaxxFxSlots::initialize_default_slot()
{
	auto fx_slot_index = 0;

	for (auto& fx_slot : fx_slots_)
	{
		fx_slot.set_fx_slot_default_effect(fx_slot_index++);
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
