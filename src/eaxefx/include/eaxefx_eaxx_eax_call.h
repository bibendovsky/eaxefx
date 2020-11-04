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


#ifndef EAXEFX_EAXX_EAX_CALL_INCLUDED
#define EAXEFX_EAXX_EAX_CALL_INCLUDED


#include "AL/al.h"

#include "eaxefx_eax_api.h"
#include "eaxefx_eaxx_fx_slot_index.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename T
>
struct EaxxEaxCallSpan
{
	int size{};
	T* values{};
}; // EaxxEaxCallSpan

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxxEaxCall
{
	bool is_get{};
	int version{};
	EaxxFxSlotIndex fx_slot_index{};

	const GUID* property_set_id{};
	ALuint property_id{};
	ALuint property_al_name{};
	ALvoid* property_buffer{};
	ALuint property_size{};


	template<
		typename TException,
		typename TValue
	>
	TValue& get_value() const
	{
		if (property_buffer == nullptr)
		{
			throw TException{"Null property buffer."};
		}

		if (property_size < static_cast<ALuint>(sizeof(TValue)))
		{
			throw TException{"Property buffer too small."};
		}

		return *static_cast<TValue*>(property_buffer);
	}

	template<
		typename TException,
		typename TValue
	>
	EaxxEaxCallSpan<TValue> get_values() const
	{
		if (property_buffer == nullptr)
		{
			throw TException{"Null property buffer."};
		}

		if (property_size < static_cast<ALuint>(sizeof(TValue)))
		{
			throw TException{"Property buffer too small."};
		}

		const auto count = static_cast<int>(property_size / sizeof(TValue));

		return EaxxEaxCallSpan<TValue>{count, static_cast<TValue*>(property_buffer)};
	}

	template<
		typename TException,
		typename TValue>
	void set_value(
		const TValue& value) const
	{
		get_value<TException, TValue>() = value;
	}
}; // EaxxEaxCall

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_EAX_CALL_INCLUDED
