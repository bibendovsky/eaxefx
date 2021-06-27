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


#include "eaxefx_al_object.h"

#include <cassert>

#include "AL/efx.h"

#include "eaxefx_exception.h"
#include "eaxefx_al_symbols.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlObjectException :
	public Exception
{
public:
	explicit AlObjectException(
		const char* message)
		:
		Exception{"AL_OBJECT", message}
	{
	}
}; // AlObjectException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EfxEffectSlotObjectDeleter::EfxEffectSlotObjectDeleter(
	const AlEfxSymbols* al_efx_symbols)
{
	if (!al_efx_symbols)
	{
		throw AlObjectException{"Null EFX symbols."};
	}

	al_efx_symbols_ = al_efx_symbols;
}

EfxEffectSlotObjectDeleter::EfxEffectSlotObjectDeleter(
	const EfxEffectSlotObjectDeleter& rhs) noexcept
	:
	al_efx_symbols_{rhs.al_efx_symbols_}
{
}

void EfxEffectSlotObjectDeleter::operator=(
	const EfxEffectSlotObjectDeleter& rhs) noexcept
{
	al_efx_symbols_ = rhs.al_efx_symbols_;
}

void EfxEffectSlotObjectDeleter::operator()(
	::ALuint al_name) const noexcept
{
	if (al_efx_symbols_)
	{
		al_efx_symbols_->alDeleteAuxiliaryEffectSlots(1, &al_name);
	}
	else
	{
		assert(!"Null EFX symbols.");
	}
}

EfxEffectSlotObject make_efx_effect_slot_object(
	const AlEfxSymbols* al_efx_symbols)
{
	if (!al_efx_symbols)
	{
		throw AlObjectException{"Null EFX symbols."};
	}

	auto al_effect_slot_name = ::ALuint{};
	al_efx_symbols->alGenAuxiliaryEffectSlots(1, &al_effect_slot_name);

	if (al_effect_slot_name == 0)
	{
		throw AlObjectException{"Failed to create EFX effect slot."};
	}

	return EfxEffectSlotObject{al_effect_slot_name, EfxEffectSlotObjectDeleter{al_efx_symbols}};
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EfxEffectObjectDeleter::EfxEffectObjectDeleter(
	const AlEfxSymbols* al_efx_symbols)
{
	if (!al_efx_symbols)
	{
		throw AlObjectException{"Null EFX symbols."};
	}

	al_efx_symbols_ = al_efx_symbols;
}

EfxEffectObjectDeleter::EfxEffectObjectDeleter(
	const EfxEffectObjectDeleter& rhs) noexcept
	:
	al_efx_symbols_{rhs.al_efx_symbols_}
{
}

void EfxEffectObjectDeleter::operator=(
	const EfxEffectObjectDeleter& rhs) noexcept
{
	al_efx_symbols_ = rhs.al_efx_symbols_;
}

void EfxEffectObjectDeleter::operator()(
	::ALuint al_name) const noexcept
{
	if (al_efx_symbols_)
	{
		al_efx_symbols_->alDeleteEffects(1, &al_name);
	}
	else
	{
		assert(!"Null EFX symbols.");
	}
}

EfxEffectObject make_efx_effect_object(
	::ALint al_effect_type,
	const AlEfxSymbols* al_efx_symbols)
{
	switch (al_effect_type)
	{
		case AL_EFFECT_NULL:
		case AL_EFFECT_CHORUS:
		case AL_EFFECT_DISTORTION:
		case AL_EFFECT_ECHO:
		case AL_EFFECT_FLANGER:
		case AL_EFFECT_FREQUENCY_SHIFTER:
		case AL_EFFECT_VOCAL_MORPHER:
		case AL_EFFECT_PITCH_SHIFTER:
		case AL_EFFECT_RING_MODULATOR:
		case AL_EFFECT_AUTOWAH:
		case AL_EFFECT_COMPRESSOR:
		case AL_EFFECT_EQUALIZER:
		case AL_EFFECT_EAXREVERB:
			break;

		default:
			throw AlObjectException{"Unsupported AL effect type."};
	}

	if (!al_efx_symbols)
	{
		throw AlObjectException{"Null EFX symbols."};
	}

	auto al_effect = ::ALuint{};
	al_efx_symbols->alGenEffects(1, &al_effect);

	if (al_effect == AL_NONE)
	{
		throw AlObjectException{"Failed to create AL effect object."};
	}

	auto efx_effect_object = EfxEffectObject{al_effect, EfxEffectObjectDeleter{al_efx_symbols}};

	auto new_al_effect_type = ::ALint{-1};

	al_efx_symbols->alEffecti(al_effect, AL_EFFECT_TYPE, al_effect_type);
	al_efx_symbols->alGetEffecti(al_effect, AL_EFFECT_TYPE, &new_al_effect_type);

	if (new_al_effect_type != al_effect_type)
	{
		throw AlObjectException{"Failed to set AL effect type."};
	}

	return efx_effect_object;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

EfxFilterObjectDeleter::EfxFilterObjectDeleter(
	const AlEfxSymbols* al_efx_symbols)
{
	if (!al_efx_symbols)
	{
		throw AlObjectException{"Null EFX symbols."};
	}

	al_efx_symbols_ = al_efx_symbols;
}

EfxFilterObjectDeleter::EfxFilterObjectDeleter(
	const EfxFilterObjectDeleter& rhs) noexcept
	:
	al_efx_symbols_{rhs.al_efx_symbols_}
{
}

void EfxFilterObjectDeleter::operator=(
	const EfxFilterObjectDeleter& rhs) noexcept
{
	al_efx_symbols_ = rhs.al_efx_symbols_;
}

void EfxFilterObjectDeleter::operator()(
	::ALuint al_name) const noexcept
{
	if (al_efx_symbols_)
	{
		al_efx_symbols_->alDeleteFilters(1, &al_name);
	}
	else
	{
		assert(!"Null EFX symbols.");
	}
}

EfxFilterObject make_efx_filter_object(
	const AlEfxSymbols* al_efx_symbols)
{
	if (!al_efx_symbols)
	{
		throw AlObjectException{"Null EFX symbols."};
	}

	auto al_filter_name = ::ALuint{};
	al_efx_symbols->alGenFilters(1, &al_filter_name);

	if (al_filter_name == AL_NONE)
	{
		throw AlObjectException{"Failed to create EFX filter."};
	}

	return EfxFilterObject{al_filter_name, EfxFilterObjectDeleter{al_efx_symbols}};
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
