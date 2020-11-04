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


const EAXREVERBPROPERTIES REVERB_PRESET_GENERIC =
{
	0,
	7.5F,
	1.0F,
	-1000,
	-100,
	0,
	1.49F,
	0.83F,
	1.0F,
	-2602,
	0.007F,
	{0.0F, 0.0F, 0.0F},
	200,
	0.011F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_PADDEDCELL =
{
	1,
	1.4F,
	1.0F,
	-1000,
	-6000,
	0,
	0.17F,
	0.10F,
	1.0F,
	-1204,
	0.001F,
	{0.0F, 0.0F, 0.0F},
	207,
	0.002F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_ROOM =
{
	2,
	1.9F,
	1.0F,
	-1000,
	-454,
	0,
	0.40F,
	0.83F,
	1.0F,
	-1646,
	0.002F,
	{0.0F, 0.0F, 0.0F},
	53,
	0.003F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_BATHROOM =
{
	3,
	1.4F,
	1.0F,
	-1000,
	-1200,
	0,
	1.49F,
	0.54F,
	1.0F,
	-370,
	0.007F,
	{0.0F, 0.0F, 0.0F},
	1030,
	0.011F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_LIVINGROOM =
{
	4,
	2.5F,
	1.0F,
	-1000,
	-6000,
	0,
	0.50F,
	0.10F,
	1.0F,
	-1376,
	0.003F,
	{0.0F, 0.0F, 0.0F},
	-1104,
	0.004F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_STONEROOM =
{
	5,
	11.6F,
	1.0F,
	-1000,
	-300,
	0,
	2.31F,
	0.64F,
	1.0F,
	-711,
	0.012F,
	{0.0F, 0.0F, 0.0F},
	83,
	0.017F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_AUDITORIUM =
{
	6,
	21.6F,
	1.0F,
	-1000,
	-476,
	0,
	4.32F,
	0.59F,
	1.0F,
	-789,
	0.020F,
	{0.0F, 0.0F, 0.0F},
	-289,
	0.030F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_CONCERTHALL =
{
	7,
	19.6F,
	1.0F,
	-1000,
	-500,
	0,
	3.92F,
	0.70F,
	1.0F,
	-1230,
	0.020F,
	{0.0F, 0.0F, 0.0F},
	-02,
	0.029F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_CAVE =
{
	8,
	14.6F,
	1.0F,
	-1000,
	0,
	0,
	2.91F,
	1.30F,
	1.0F,
	-602,
	0.015F,
	{0.0F, 0.0F, 0.0F},
	-302,
	0.022F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x1F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_ARENA =
{
	9,
	36.2F,
	1.0F,
	-1000,
	-698,
	0,
	7.24F,
	0.33F,
	1.0F,
	-1166,
	0.020F,
	{0.0F, 0.0F, 0.0F},
	16,
	0.030F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_HANGAR =
{
	10,
	50.3F,
	1.0F,
	-1000,
	-1000,
	0,
	10.05F,
	0.23F,
	1.0F,
	-602,
	0.020F,
	{0.0F, 0.0F, 0.0F},
	198,
	0.030F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_CARPETTEDHALLWAY =
{
	11,
	1.9F,
	1.0F,
	-1000,
	-4000,
	0,
	0.30F,
	0.10F,
	1.0F,
	-1831,
	0.002F,
	{0.0F, 0.0F, 0.0F},
	-1630,
	0.030F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_HALLWAY =
{
	12,
	1.8F,
	1.0F,
	-1000,
	-300,
	0,
	1.49F,
	0.59F,
	1.0F,
	-1219,
	0.007F,
	{0.0F, 0.0F, 0.0F},
	441,
	0.011F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_STONECORRIDOR =
{
	13,
	13.5F,
	1.0F,
	-1000,
	-237,
	0,
	2.70F,
	0.79F,
	1.0F,
	-1214,
	0.013F,
	{0.0F, 0.0F, 0.0F},
	395,
	0.020F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_ALLEY =
{
	14,
	7.5F,
	0.300F,
	-1000,
	-270,
	0,
	1.49F,
	0.86F,
	1.0F,
	-1204,
	0.007F,
	{0.0F, 0.0F, 0.0F},
	-4,
	0.011F,
	{0.0F, 0.0F, 0.0F},
	0.125F,
	0.950F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_FOREST =
{
	15,
	38.0F,
	0.300F,
	-1000,
	-3300,
	0,
	1.49F,
	0.54F,
	1.0F,
	-2560,
	0.162F,
	{0.0F, 0.0F, 0.0F},
	-229,
	0.088F,
	{0.0F, 0.0F, 0.0F},
	0.125F,
	1.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_CITY =
{
	16,
	7.5F,
	0.500F,
	-1000,
	-800,
	0,
	1.49F,
	0.67F,
	1.0F,
	-2273,
	0.007F,
	{0.0F, 0.0F, 0.0F},
	-1691,
	0.011F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_MOUNTAINS =
{
	17,
	100.0F,
	0.270F,
	-1000,
	-2500,
	0,
	1.49F,
	0.21F,
	1.0F,
	-2780,
	0.300F,
	{0.0F, 0.0F, 0.0F},
	-1434,
	0.100F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	1.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x1F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_QUARRY =
{
	18,
	17.5F,
	1.0F,
	-1000,
	-1000,
	0,
	1.49F,
	0.83F,
	1.0F,
	-10000,
	0.061F,
	{0.0F, 0.0F, 0.0F},
	500,
	0.025F,
	{0.0F, 0.0F, 0.0F},
	0.125F,
	0.700F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_PLAIN =
{
	19,
	42.5F,
	0.210F,
	-1000,
	-2000,
	0,
	1.49F,
	0.50F,
	1.0F,
	-2466,
	0.179F,
	{0.0F, 0.0F, 0.0F},
	-1926,
	0.100F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	1.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_PARKINGLOT =
{
	20,
	8.3F,
	1.0F,
	-1000,
	0,
	0,
	1.65F,
	1.50F,
	1.0F,
	-1363,
	0.008F,
	{0.0F, 0.0F, 0.0F},
	-1153,
	0.012F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x1F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_SEWERPIPE =
{
	21,
	1.7F,
	0.800F,
	-1000,
	-1000,
	0,
	2.81F,
	0.14F,
	1.0F,
	429,
	0.014F,
	{0.0F, 0.0F, 0.0F},
	1023,
	0.021F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	0.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_UNDERWATER =
{
	22,
	1.8F,
	1.0F,
	-1000,
	-4000,
	0,
	1.49F,
	0.10F,
	1.0F,
	-449,
	0.007F,
	{0.0F, 0.0F, 0.0F},
	1700,
	0.011F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	1.180F,
	0.348F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x3F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_DRUGGED =
{
	23,
	1.9F,
	0.500F,
	-1000,
	0,
	0,
	8.39F,
	1.39F,
	1.0F,
	-115,
	0.002F,
	{0.0F, 0.0F, 0.0F},
	985,
	0.030F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	0.250F,
	1.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x1F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_DIZZY =
{
	24,
	1.8F,
	0.600F,
	-1000,
	-400,
	0,
	17.23F,
	0.56F,
	1.0F,
	-1713,
	0.020F,
	{0.0F, 0.0F, 0.0F},
	-613,
	0.030F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	1.0F,
	0.810F,
	0.310F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x1F,
};

const EAXREVERBPROPERTIES REVERB_PRESET_PSYCHOTIC =
{
	25,
	1.0F,
	0.500F,
	-1000,
	-151,
	0,
	7.56F,
	0.91F,
	1.0F,
	-626,
	0.020F,
	{0.0F, 0.0F, 0.0F},
	774,
	0.030F,
	{0.0F, 0.0F, 0.0F},
	0.250F,
	0.0F,
	4.0F,
	1.0F,
	-5.0F,
	5000.0F,
	250.0F,
	0.0F,
	0x1F,
};


const ReverbPresets EAX_REVERB_PRESETS =
{
	REVERB_PRESET_GENERIC,
	REVERB_PRESET_PADDEDCELL,
	REVERB_PRESET_ROOM,
	REVERB_PRESET_BATHROOM,
	REVERB_PRESET_LIVINGROOM,
	REVERB_PRESET_STONEROOM,
	REVERB_PRESET_AUDITORIUM,
	REVERB_PRESET_CONCERTHALL,
	REVERB_PRESET_CAVE,
	REVERB_PRESET_ARENA,
	REVERB_PRESET_HANGAR,
	REVERB_PRESET_CARPETTEDHALLWAY,
	REVERB_PRESET_HALLWAY,
	REVERB_PRESET_STONECORRIDOR,
	REVERB_PRESET_ALLEY,
	REVERB_PRESET_FOREST,
	REVERB_PRESET_CITY,
	REVERB_PRESET_MOUNTAINS,
	REVERB_PRESET_QUARRY,
	REVERB_PRESET_PLAIN,
	REVERB_PRESET_PARKINGLOT,
	REVERB_PRESET_SEWERPIPE,
	REVERB_PRESET_UNDERWATER,
	REVERB_PRESET_DRUGGED,
	REVERB_PRESET_DIZZY,
	REVERB_PRESET_PSYCHOTIC,
};


} // eaxefx
