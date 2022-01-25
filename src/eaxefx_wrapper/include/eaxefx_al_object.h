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


#ifndef EAXEFX_AL_OBJECT_INCLUDED
#define EAXEFX_AL_OBJECT_INCLUDED


#include <algorithm>
#include <memory>

#include "AL/al.h"

#include "eaxefx_al_symbols.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

template<
	typename TDeleter
>
class AlObject
{
public:
	using Value = ALuint;
	using Deleter = TDeleter;


	AlObject() noexcept = default;

	explicit AlObject(
		Value value) noexcept
		:
		value_{value}
	{
	}

	AlObject(
		Value value,
		Deleter deleter) noexcept
		:
		value_{value},
		deleter_{deleter}
	{
	}

	AlObject(
		const AlObject& rhs) = delete;

	AlObject(
		AlObject&& rhs) noexcept
	{
		std::swap(value_, rhs.value_);
		std::swap(deleter_, rhs.deleter_);
	}

	AlObject& operator=(
		const AlObject& rhs) = delete;

	void operator=(
		AlObject&& rhs) noexcept
	{
		destroy();
		std::swap(value_, rhs.value_);
		std::swap(deleter_, rhs.deleter_);
	}

	~AlObject()
	{
		destroy();
	}


	bool has_value() const noexcept
	{
		return value_ != AL_NONE;
	}

	Value get() const noexcept
	{
		return value_;
	}

	void reset() noexcept
	{
		destroy();
	}

	void reset(
		Value value) noexcept
	{
		destroy();

		value_ = value;
	}

	Value release() noexcept
	{
		const auto al_name = value_;
		value_ = Value{};
		return al_name;
	}


private:
	Value value_{};
	Deleter deleter_{};


	void destroy() noexcept
	{
		if (value_ != AL_NONE)
		{
			const auto value = value_;
			value_ = AL_NONE;
			deleter_(value);
		}
	}
}; // AlObject

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EfxEffectSlotObjectDeleter
{
public:
	EfxEffectSlotObjectDeleter() noexcept = default;

	explicit EfxEffectSlotObjectDeleter(
		const AlEfxSymbols* al_efx_symbols);

	EfxEffectSlotObjectDeleter(
		const EfxEffectSlotObjectDeleter& rhs) noexcept;

	void operator=(
		const EfxEffectSlotObjectDeleter& rhs) noexcept;


	void operator()(
		ALuint al_name) const noexcept;


private:
	const AlEfxSymbols* al_efx_symbols_{};
}; // EfxEffectSlotObjectDeleter

using EfxEffectSlotObject = AlObject<EfxEffectSlotObjectDeleter>;

EfxEffectSlotObject make_efx_effect_slot_object(
	const AlEfxSymbols* al_efx_symbols);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EfxEffectObjectDeleter
{
public:
	EfxEffectObjectDeleter() noexcept = default;

	explicit EfxEffectObjectDeleter(
		const AlEfxSymbols* al_efx_symbols);

	EfxEffectObjectDeleter(
		const EfxEffectObjectDeleter& rhs) noexcept;

	void operator=(
		const EfxEffectObjectDeleter& rhs) noexcept;


	void operator()(
		ALuint al_name) const noexcept;


private:
	const AlEfxSymbols* al_efx_symbols_{};
}; // EfxEffectObjectDeleter

using EfxEffectObject = AlObject<EfxEffectObjectDeleter>;

EfxEffectObject make_efx_effect_object(
	ALint al_effect_type,
	const AlEfxSymbols* al_efx_symbols);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EfxFilterObjectDeleter
{
public:
	EfxFilterObjectDeleter() noexcept = default;

	explicit EfxFilterObjectDeleter(
		const AlEfxSymbols* al_efx_symbols);

	EfxFilterObjectDeleter(
		const EfxFilterObjectDeleter& rhs) noexcept;

	void operator=(
		const EfxFilterObjectDeleter& rhs) noexcept;


	void operator()(
		ALuint al_name) const noexcept;


private:
	const AlEfxSymbols* al_efx_symbols_{};
}; // EfxFilterObjectDeleter

using EfxFilterObject = AlObject<EfxFilterObjectDeleter>;

EfxFilterObject make_efx_filter_object(
	const AlEfxSymbols* al_efx_symbols);

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_AL_OBJECT_INCLUDED
