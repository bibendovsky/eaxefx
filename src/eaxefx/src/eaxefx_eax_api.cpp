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


//
// EAX API.
//
// Based on headers eax4.h and eax5.h included into Doom 3 source:
// http://github.com/id-Software/DOOM-3/tree/master/neo/openal/include
//


#include "eaxefx_eax_api.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Common.
//

bool operator==(
	const EAXVECTOR& lhs,
	const EAXVECTOR& rhs) noexcept
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool operator!=(
	const EAXVECTOR& lhs,
	const EAXVECTOR& rhs) noexcept
{
	return !(lhs == rhs);
}

//
// Common.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


bool operator==(
	const EAX40CONTEXTPROPERTIES& lhs,
	const EAX40CONTEXTPROPERTIES& rhs) noexcept
{
	return
		lhs.guidPrimaryFXSlotID == rhs.guidPrimaryFXSlotID &&
		lhs.flDistanceFactor == rhs.flDistanceFactor &&
		lhs.flAirAbsorptionHF == rhs.flAirAbsorptionHF &&
		lhs.flHFReference == rhs.flHFReference;
}

bool operator==(
	const EAX50CONTEXTPROPERTIES& lhs,
	const EAX50CONTEXTPROPERTIES& rhs) noexcept
{
	return
		static_cast<const EAX40CONTEXTPROPERTIES&>(lhs) == static_cast<const EAX40CONTEXTPROPERTIES&>(rhs) &&
		lhs.flMacroFXFactor == rhs.flMacroFXFactor;
}


const GUID EAXCONTEXT_DEFAULTPRIMARYFXSLOTID = EAXPROPERTYID_EAX40_FXSlot0;

bool operator==(
	const EAX40FXSLOTPROPERTIES& lhs,
	const EAX40FXSLOTPROPERTIES& rhs) noexcept
{
	return
		lhs.guidLoadEffect == rhs.guidLoadEffect &&
		lhs.lVolume == rhs.lVolume &&
		lhs.lLock == rhs.lLock &&
		lhs.ulFlags == rhs.ulFlags;
}

bool operator==(
	const EAX50FXSLOTPROPERTIES& lhs,
	const EAX50FXSLOTPROPERTIES& rhs) noexcept
{
	return
		static_cast<const EAX40FXSLOTPROPERTIES&>(lhs) == static_cast<const EAX40FXSLOTPROPERTIES&>(rhs) &&
		lhs.lOcclusion == rhs.lOcclusion &&
		lhs.flOcclusionLFRatio == rhs.flOcclusionLFRatio;
}

const EAX50ACTIVEFXSLOTS EAX40SOURCE_DEFAULTACTIVEFXSLOTID = EAX50ACTIVEFXSLOTS
{{
		EAX_NULL_GUID,
		EAXPROPERTYID_EAX40_FXSlot0,
}};

// A 3D Source's default ACTIVEFXSLOTID is { EAX_NULL_GUID, EAX_PrimaryFXSlotID, EAX_NULL_GUID, EAX_NULL_GUID }
const EAX50ACTIVEFXSLOTS EAX50SOURCE_3DDEFAULTACTIVEFXSLOTID = EAX50ACTIVEFXSLOTS
{{
	EAX_NULL_GUID,
	EAX_PrimaryFXSlotID,
	EAX_NULL_GUID,
	EAX_NULL_GUID,
}};


// A 2D Source's default ACTIVEFXSLOTID is { EAX_NULL_GUID, EAX_NULL_GUID, EAX_NULL_GUID, EAX_NULL_GUID }
const EAX50ACTIVEFXSLOTS EAX50SOURCE_2DDEFAULTACTIVEFXSLOTID = EAX50ACTIVEFXSLOTS
{{
	EAX_NULL_GUID,
	EAX_NULL_GUID,
	EAX_NULL_GUID,
	EAX_NULL_GUID,
}};

bool operator==(
	const EAXREVERBPROPERTIES& lhs,
	const EAXREVERBPROPERTIES& rhs) noexcept
{
	return
		lhs.ulEnvironment == rhs.ulEnvironment &&
		lhs.flEnvironmentSize == rhs.flEnvironmentSize &&
		lhs.flEnvironmentDiffusion == rhs.flEnvironmentDiffusion &&
		lhs.lRoom == rhs.lRoom &&
		lhs.lRoomHF == rhs.lRoomHF &&
		lhs.lRoomLF == rhs.lRoomLF &&
		lhs.flDecayTime == rhs.flDecayTime &&
		lhs.flDecayHFRatio == rhs.flDecayHFRatio &&
		lhs.flDecayLFRatio == rhs.flDecayLFRatio &&
		lhs.lReflections == rhs.lReflections &&
		lhs.flReflectionsDelay == rhs.flReflectionsDelay &&
		lhs.vReflectionsPan == rhs.vReflectionsPan &&
		lhs.lReverb == rhs.lReverb &&
		lhs.flReverbDelay == rhs.flReverbDelay &&
		lhs.vReverbPan == rhs.vReverbPan &&
		lhs.flEchoTime == rhs.flEchoTime &&
		lhs.flEchoDepth == rhs.flEchoDepth &&
		lhs.flModulationTime == rhs.flModulationTime &&
		lhs.flModulationDepth == rhs.flModulationDepth &&
		lhs.flAirAbsorptionHF == rhs.flAirAbsorptionHF &&
		lhs.flHFReference == rhs.flHFReference &&
		lhs.flLFReference == rhs.flLFReference &&
		lhs.flRoomRolloffFactor == rhs.flRoomRolloffFactor &&
		lhs.ulFlags == rhs.ulFlags
	;
}

bool operator!=(
	const EAXREVERBPROPERTIES& lhs,
	const EAXREVERBPROPERTIES& rhs) noexcept
{
	return !(lhs == rhs);
}


} // eaxefx
