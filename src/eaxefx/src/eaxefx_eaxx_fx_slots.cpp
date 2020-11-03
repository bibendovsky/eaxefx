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


#include "eaxefx_eaxx_fx_slots.h"

#include <array>
#include <exception>
#include <string_view>

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
		std::string_view message)
		:
		Exception{"EAXEFX_EAXX_FX_SLOTS", message}
	{
	}
}; // EaxxFxSlotsException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxxFxSlots::initialize()
try
{
	initialize_fx_slots();
	initialize_default_slot();
}
catch (const std::exception&)
{
	std::throw_with_nested(EaxxFxSlotsException{"Failed to initialize."});
}

EaxxFxSlot& EaxxFxSlots::get(
	int index)
{
	if (index < 0 || index >= EAX_MAX_FXSLOTS)
	{
		throw EaxxFxSlotsException{"Index out of range."};
	}

	return items_[index];
}

int EaxxFxSlots::get_max_active_count() const noexcept
{
	return max_active_count_;
}

void EaxxFxSlots::set_max_active_count(
	int max_active_count)
{
	if (max_active_count <= 0 || max_active_count > EAX_MAX_FXSLOTS)
	{
		throw EaxxFxSlotsException{"Active index out of range."};
	}

	max_active_count_ = max_active_count;
}

const GUID& EaxxFxSlots::get_primary_id() const noexcept
{
	return primary_id_;
}

EaxxFxSlotIndex EaxxFxSlots::get_primary_index() const noexcept
{
	return primary_index_;
}

EaxxFxSlotIndex EaxxFxSlots::resolve_fx_slot_index(
	const GUID& eax_id) const
{
	if (eax_id == EAX_PrimaryFXSlotID)
	{
		return primary_index_;
	}
	else
	{
		return eax_id_to_index(eax_id);
	}
}

void EaxxFxSlots::set_primary(
	const GUID& primary_id)
{
	primary_index_ = eax_id_to_index(primary_id);
	primary_id_ = primary_id;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void EaxxFxSlots::initialize_fx_slots()
{
	for (auto& fx_slot : items_)
	{
		fx_slot.initialize();
	}
}

void EaxxFxSlots::initialize_default_slot()
{
	items_.front().set_effect(EAX_REVERB_EFFECT);
}

EaxxFxSlotIndex EaxxFxSlots::eax_id_to_index(
	const GUID& eax_id)
{
	if (eax_id == EAX_NULL_GUID)
	{
		return EaxxFxSlotIndex{};
	}
	else if (eax_id == EAXPROPERTYID_EAX40_FXSlot0 ||
		eax_id == EAXPROPERTYID_EAX50_FXSlot0)
	{
		return EaxxFxSlotIndex{0};
	}
	else if (eax_id == EAXPROPERTYID_EAX40_FXSlot1 ||
		eax_id == EAXPROPERTYID_EAX50_FXSlot1)
	{
		return EaxxFxSlotIndex{1};
	}
	else if (eax_id == EAXPROPERTYID_EAX40_FXSlot2 ||
		eax_id == EAXPROPERTYID_EAX50_FXSlot2)
	{
		return EaxxFxSlotIndex{2};
	}
	else if (eax_id == EAXPROPERTYID_EAX40_FXSlot3 ||
		eax_id == EAXPROPERTYID_EAX50_FXSlot3)
	{
		return EaxxFxSlotIndex{3};
	}
	else
	{
		throw EaxxFxSlotsException{"Unsupported id."};
	}
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
