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


#ifndef EAXEFX_EAX_API_INCLUDED
#define EAXEFX_EAX_API_INCLUDED


//
// EAX API.
//
// Based on headers eax3.h, eax4.h and eax5.h included into Doom 3 source:
// http://github.com/id-Software/DOOM-3/tree/master/neo/openal/include
//


#include <cfloat>

#include <array>

#include <guiddef.h>

#include "AL/al.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Common.
//

constexpr auto EAX_MAX_FXSLOTS = 4;


// The EAX_NULL_GUID is used by EAXFXSLOT_LOADEFFECT, EAXCONTEXT_PRIMARYFXSLOTID
// and EAXSOURCE_ACTIVEFXSLOTID
// {00000000-0000-0000-0000-000000000000}
DEFINE_GUID(
	EAX_NULL_GUID,
	0x00000000,
	0x0000,
	0x0000,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

// The EAX_PrimaryFXSlotID GUID is used by EAXSOURCE_ACTIVEFXSLOTID 
// {F317866D-924C-450C-861B-E6DAA25E7C20}
DEFINE_GUID(
	EAX_PrimaryFXSlotID,
	0xF317866D,
	0x924C,
	0x450C,
	0x86, 0x1B, 0xE6, 0xDA, 0xA2, 0x5E, 0x7C, 0x20);


struct EAXVECTOR
{
	float x;
	float y;
	float z;
};

bool operator==(
	const EAXVECTOR& lhs,
	const EAXVECTOR& rhs) noexcept;

bool operator!=(
	const EAXVECTOR& lhs,
	const EAXVECTOR& rhs) noexcept;

//
// Common.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


//
// EAX 3.0 listener property set {A8FA6880-B476-11d3-BDB9-00C0F02DDF87}
//
DEFINE_GUID(
	DSPROPSETID_EAX30_ListenerProperties,
	0xA8FA6882,
	0xB476,
	0x11D3,
	0xBD, 0xB9, 0x00, 0xC0, 0xF0, 0x2D, 0xDF, 0x87);

//
// EAX 3.0 buffer property set {A8FA6881-B476-11d3-BDB9-00C0F02DDF87}
//
DEFINE_GUID(
	DSPROPSETID_EAX30_BufferProperties,
	0xA8FA6881,
	0xB476,
	0x11D3,
	0xBD, 0xB9, 0x0, 0xC0, 0xF0, 0x2D, 0xDF, 0x87);



constexpr auto EAX40_MAX_ACTIVE_FXSLOTS = 2;
constexpr auto EAX50_MAX_ACTIVE_FXSLOTS = 4;


// {1D4870AD-0DEF-43c0-A40C-523632296342}
DEFINE_GUID(
	EAXPROPERTYID_EAX40_Context,
	0x1D4870AD,
	0xDEF,
	0x43C0,
	0xA4, 0xC, 0x52, 0x36, 0x32, 0x29, 0x63, 0x42);

// {57E13437-B932-4ab2-B8BD-5266C1A887EE}
DEFINE_GUID(
	EAXPROPERTYID_EAX50_Context,
	0x57E13437,
	0xB932,
	0x4AB2,
	0xB8, 0xBD, 0x52, 0x66, 0xC1, 0xA8, 0x87, 0xEE);

// EAX50
enum : unsigned long
{
	EAX_40 = 5, // EAX 4.0
	EAX_50 = 6, // EAX 5.0
};

// min,max, default values for ulEAXVersion in struct EAXSESSIONPROPERTIES
constexpr auto EAXCONTEXT_MINEAXSESSION = EAX_40;
constexpr auto EAXCONTEXT_MAXEAXSESSION = EAX_50;
constexpr auto EAXCONTEXT_DEFAULTEAXSESSION = EAX_40;

// min,max, default values for ulMaxActiveSends in struct EAXSESSIONPROPERTIES
constexpr auto EAXCONTEXT_MINMAXACTIVESENDS = 2;
constexpr auto EAXCONTEXT_MAXMAXACTIVESENDS = 4;
constexpr auto EAXCONTEXT_DEFAULTMAXACTIVESENDS = 2;

// EAX50
struct EAXSESSIONPROPERTIES
{
	unsigned long ulEAXVersion;
	unsigned long ulMaxActiveSends;
}; // EAXSESSIONPROPERTIES

enum EAXCONTEXT_PROPERTY
{
	EAXCONTEXT_NONE = 0,
	EAXCONTEXT_ALLPARAMETERS,
	EAXCONTEXT_PRIMARYFXSLOTID,
	EAXCONTEXT_DISTANCEFACTOR,
	EAXCONTEXT_AIRABSORPTIONHF,
	EAXCONTEXT_HFREFERENCE,
	EAXCONTEXT_LASTERROR,

	// EAX50
	EAXCONTEXT_SPEAKERCONFIG,
	EAXCONTEXT_EAXSESSION,
	EAXCONTEXT_MACROFXFACTOR,
};

// Use this structure for EAXCONTEXT_ALL property.
struct EAX40CONTEXTPROPERTIES
{
	GUID guidPrimaryFXSlotID;
	float flDistanceFactor;
	float flAirAbsorptionHF;
	float flHFReference;
}; // EAX40CONTEXTPROPERTIES

struct EAX50CONTEXTPROPERTIES :
	public EAX40CONTEXTPROPERTIES
{
	float flMacroFXFactor;
}; // EAX40CONTEXTPROPERTIES


bool operator==(
	const EAX40CONTEXTPROPERTIES& lhs,
	const EAX40CONTEXTPROPERTIES& rhs) noexcept;

bool operator==(
	const EAX50CONTEXTPROPERTIES& lhs,
	const EAX50CONTEXTPROPERTIES& rhs) noexcept;


// EAX Context property ranges and defaults:

constexpr auto EAXCONTEXT_MINDISTANCEFACTOR = FLT_MIN;
constexpr auto EAXCONTEXT_MAXDISTANCEFACTOR = FLT_MAX;
constexpr auto EAXCONTEXT_DEFAULTDISTANCEFACTOR = 1.0F;

constexpr auto EAXCONTEXT_MINAIRABSORPTIONHF = -100.0F;
constexpr auto EAXCONTEXT_MAXAIRABSORPTIONHF = 0.0F;
constexpr auto EAXCONTEXT_DEFAULTAIRABSORPTIONHF = -5.0F;

constexpr auto EAXCONTEXT_MINHFREFERENCE = 1000.0F;
constexpr auto EAXCONTEXT_MAXHFREFERENCE = 20000.0F;
constexpr auto EAXCONTEXT_DEFAULTHFREFERENCE = 5000.0F;

constexpr auto EAXCONTEXT_MINMACROFXFACTOR = 0.0F;
constexpr auto EAXCONTEXT_MAXMACROFXFACTOR = 1.0F;
constexpr auto EAXCONTEXT_DEFAULTMACROFXFACTOR = 0.0F;


// {C4D79F1E-F1AC-436b-A81D-A738E7045469}
DEFINE_GUID(
	EAXPROPERTYID_EAX40_FXSlot0,
	0xC4D79F1E,
	0xF1AC,
	0x436B,
	0xA8, 0x1D, 0xA7, 0x38, 0xE7, 0x4, 0x54, 0x69);

// {91F9590F-C388-407a-84B0-1BAE0EF71ABC}
DEFINE_GUID(
	EAXPROPERTYID_EAX50_FXSlot0,
	0x91F9590F,
	0xC388,
	0x407A,
	0x84, 0xB0, 0x1B, 0xAE, 0xE, 0xF7, 0x1A, 0xBC);

// {08C00E96-74BE-4491-93AA-E8AD35A49117}
DEFINE_GUID(
	EAXPROPERTYID_EAX40_FXSlot1,
	0x8C00E96,
	0x74BE,
	0x4491,
	0x93, 0xAA, 0xE8, 0xAD, 0x35, 0xA4, 0x91, 0x17);

// {8F5F7ACA-9608-4965-8137-8213C7B9D9DE}
DEFINE_GUID(
	EAXPROPERTYID_EAX50_FXSlot1,
	0x8F5F7ACA,
	0x9608,
	0x4965,
	0x81, 0x37, 0x82, 0x13, 0xC7, 0xB9, 0xD9, 0xDE);

// {1D433B88-F0F6-4637-919F-60E7E06B5EDD}
DEFINE_GUID(
	EAXPROPERTYID_EAX40_FXSlot2,
	0x1D433B88,
	0xF0F6,
	0x4637,
	0x91, 0x9F, 0x60, 0xE7, 0xE0, 0x6B, 0x5E, 0xDD);

// {3C0F5252-9834-46f0-A1D8-5B95C4A00A30}
DEFINE_GUID(
	EAXPROPERTYID_EAX50_FXSlot2,
	0x3C0F5252,
	0x9834,
	0x46F0,
	0xA1, 0xD8, 0x5B, 0x95, 0xC4, 0xA0, 0xA, 0x30);

// {EFFF08EA-C7D8-44ab-93AD-6DBD5F910064}
DEFINE_GUID(
	EAXPROPERTYID_EAX40_FXSlot3,
	0xEFFF08EA,
	0xC7D8,
	0x44AB,
	0x93, 0xAD, 0x6D, 0xBD, 0x5F, 0x91, 0x0, 0x64);

// {E2EB0EAA-E806-45e7-9F86-06C1571A6FA3}
DEFINE_GUID(
	EAXPROPERTYID_EAX50_FXSlot3,
	0xE2EB0EAA,
	0xE806,
	0x45E7,
	0x9F, 0x86, 0x6, 0xC1, 0x57, 0x1A, 0x6F, 0xA3);

extern const GUID EAXCONTEXT_DEFAULTPRIMARYFXSLOTID;

// range 0-0x40 reserved for loaded effect parameters
enum EAXFXSLOT_PROPERTY
{
	EAXFXSLOT_PARAMETER = 0,
	EAXFXSLOT_NONE = 0x10000,
	EAXFXSLOT_ALLPARAMETERS,
	EAXFXSLOT_LOADEFFECT,
	EAXFXSLOT_VOLUME,
	EAXFXSLOT_LOCK,
	EAXFXSLOT_FLAGS,

	// EAX50
	EAXFXSLOT_OCCLUSION,
	EAXFXSLOT_OCCLUSIONLFRATIO,
};

// Note: The number and order of flags may change in future EAX versions.
//       To insure future compatibility, use flag defines as follows:
//              myFlags = EAXFXSLOTFLAGS_ENVIRONMENT;
//       instead of:
//              myFlags = 0x00000001;
//
constexpr auto EAXFXSLOTFLAGS_ENVIRONMENT = 0x00000001UL;
// EAX50
constexpr auto EAXFXSLOTFLAGS_UPMIX = 0x00000002UL;

constexpr auto EAX40FXSLOTFLAGS_RESERVED = 0xFFFFFFFEUL; // reserved future use
constexpr auto EAX50FXSLOTFLAGS_RESERVED = 0xFFFFFFFCUL; // reserved future use

// EAX Effect Slot property ranges and defaults:
constexpr auto EAXFXSLOT_MINVOLUME = -10000L;
constexpr auto EAXFXSLOT_MAXVOLUME = 0L;
constexpr auto EAXFXSLOT_DEFAULTVOLUME = 0L;

constexpr auto EAXFXSLOT_MINLOCK = 0L;
constexpr auto EAXFXSLOT_MAXLOCK = 1L;

enum : long
{
	EAXFXSLOT_UNLOCKED = 0,
	EAXFXSLOT_LOCKED = 1
};

constexpr auto EAXFXSLOT_MINOCCLUSION = -10000L;
constexpr auto EAXFXSLOT_MAXOCCLUSION = 0L;
constexpr auto EAXFXSLOT_DEFAULTOCCLUSION = 0L;

constexpr auto EAXFXSLOT_MINOCCLUSIONLFRATIO = 0.0F;
constexpr auto EAXFXSLOT_MAXOCCLUSIONLFRATIO = 1.0F;
constexpr auto EAXFXSLOT_DEFAULTOCCLUSIONLFRATIO = 0.25F;

constexpr auto EAX40FXSLOT_DEFAULTFLAGS = EAXFXSLOTFLAGS_ENVIRONMENT;

constexpr auto EAX50FXSLOT_DEFAULTFLAGS =
	EAXFXSLOTFLAGS_ENVIRONMENT |
	EAXFXSLOTFLAGS_UPMIX // ignored for reverb
;

// Use this structure for EAXFXSLOT_ALLPARAMETERS
// - all levels are hundredths of decibels
//
// NOTE: This structure may change in future EAX versions.
//       It is recommended to initialize fields by name:
//              myFXSlot.guidLoadEffect = EAX_REVERB_EFFECT;
//              myFXSlot.lVolume = 0;
//              myFXSlot.lLock = 1;
//              myFXSlot.ulFlags = myFlags /* see EAXFXSLOTFLAGS below */ ;
//       instead of:
//              myFXSlot = { EAX_REVERB_EFFECT, 0, 1, 0x00000001 };
//
struct EAX40FXSLOTPROPERTIES
{
	GUID guidLoadEffect;
	long lVolume;
	long lLock;
	unsigned long ulFlags;
};

struct EAX50FXSLOTPROPERTIES :
	public EAX40FXSLOTPROPERTIES
{
	long lOcclusion;
	float flOcclusionLFRatio;
};

bool operator==(
	const EAX40FXSLOTPROPERTIES& lhs,
	const EAX40FXSLOTPROPERTIES& rhs) noexcept;

bool operator==(
	const EAX50FXSLOTPROPERTIES& lhs,
	const EAX50FXSLOTPROPERTIES& rhs) noexcept;

// {1B86B823-22DF-4eae-8B3C-1278CE544227}
DEFINE_GUID(EAXPROPERTYID_EAX40_Source,
	0x1B86B823,
	0x22DF,
	0x4EAE,
	0x8B, 0x3C, 0x12, 0x78, 0xCE, 0x54, 0x42, 0x27);

// {5EDF82F0-24A7-4f38-8E64-2F09CA05DEE1}
DEFINE_GUID(EAXPROPERTYID_EAX50_Source,
	0x5EDF82F0,
	0x24A7,
	0x4F38,
	0x8E, 0x64, 0x2F, 0x9, 0xCA, 0x5, 0xDE, 0xE1);

// Source object properties
enum EAXSOURCE_PROPERTY
{
	EAXSOURCE_NONE,
	EAXSOURCE_ALLPARAMETERS,
	EAXSOURCE_OBSTRUCTIONPARAMETERS,
	EAXSOURCE_OCCLUSIONPARAMETERS,
	EAXSOURCE_EXCLUSIONPARAMETERS,
	EAXSOURCE_DIRECT,
	EAXSOURCE_DIRECTHF,
	EAXSOURCE_ROOM,
	EAXSOURCE_ROOMHF,
	EAXSOURCE_OBSTRUCTION,
	EAXSOURCE_OBSTRUCTIONLFRATIO,
	EAXSOURCE_OCCLUSION,
	EAXSOURCE_OCCLUSIONLFRATIO,
	EAXSOURCE_OCCLUSIONROOMRATIO,
	EAXSOURCE_OCCLUSIONDIRECTRATIO,
	EAXSOURCE_EXCLUSION,
	EAXSOURCE_EXCLUSIONLFRATIO,
	EAXSOURCE_OUTSIDEVOLUMEHF,
	EAXSOURCE_DOPPLERFACTOR,
	EAXSOURCE_ROLLOFFFACTOR,
	EAXSOURCE_ROOMROLLOFFFACTOR,
	EAXSOURCE_AIRABSORPTIONFACTOR,
	EAXSOURCE_FLAGS,

	// EAX40
	EAXSOURCE_SENDPARAMETERS,
	EAXSOURCE_ALLSENDPARAMETERS,
	EAXSOURCE_OCCLUSIONSENDPARAMETERS,
	EAXSOURCE_EXCLUSIONSENDPARAMETERS,
	EAXSOURCE_ACTIVEFXSLOTID,

	// EAX50
	EAXSOURCE_MACROFXFACTOR,
	EAXSOURCE_SPEAKERLEVELS,
	EAXSOURCE_ALL2DPARAMETERS,
};


// Used by EAXSOURCE_FLAGS for EAXSOURCEFLAGS_xxxAUTO
//    TRUE:    value is computed automatically - property is an offset
//    FALSE:   value is used directly
//
// Note: The number and order of flags may change in future EAX versions.
//       To insure future compatibility, use flag defines as follows:
//              myFlags = EAXSOURCE_DIRECTHFAUTO | EAXSOURCE_ROOMAUTO;
//       instead of:
//              myFlags = 0x00000003;
//
constexpr auto EAXSOURCEFLAGS_DIRECTHFAUTO = 0x00000001UL; // relates to EAXSOURCE_DIRECTHF
constexpr auto EAXSOURCEFLAGS_ROOMAUTO = 0x00000002UL; // relates to EAXSOURCE_ROOM
constexpr auto EAXSOURCEFLAGS_ROOMHFAUTO = 0x00000004UL; // relates to EAXSOURCE_ROOMHF
// EAX50
constexpr auto EAXSOURCEFLAGS_3DELEVATIONFILTER = 0x00000008UL;
// EAX50
constexpr auto EAXSOURCEFLAGS_UPMIX = 0x00000010UL;
// EAX50
constexpr auto EAXSOURCEFLAGS_APPLYSPEAKERLEVELS = 0x00000020UL;

constexpr auto EAX40SOURCEFLAGS_RESERVED = 0xFFFFFFF8UL; // reserved future use
constexpr auto EAX50SOURCEFLAGS_RESERVED = 0xFFFFFFC0UL; // reserved future use

// EAX Source property ranges and defaults:
constexpr auto EAXSOURCE_MINSEND = -10000L;
constexpr auto EAXSOURCE_MAXSEND = 0L;
constexpr auto EAXSOURCE_DEFAULTSEND = 0L;

constexpr auto EAXSOURCE_MINSENDHF = -10000L;
constexpr auto EAXSOURCE_MAXSENDHF = 0L;
constexpr auto EAXSOURCE_DEFAULTSENDHF = 0L;

constexpr auto EAXSOURCE_MINDIRECT = -10000L;
constexpr auto EAXSOURCE_MAXDIRECT = 1000L;
constexpr auto EAXSOURCE_DEFAULTDIRECT = 0L;

constexpr auto EAXSOURCE_MINDIRECTHF = -10000L;
constexpr auto EAXSOURCE_MAXDIRECTHF = 0L;
constexpr auto EAXSOURCE_DEFAULTDIRECTHF = 0L;

constexpr auto EAXSOURCE_MINROOM = -10000L;
constexpr auto EAXSOURCE_MAXROOM = 1000L;
constexpr auto EAXSOURCE_DEFAULTROOM = 0L;

constexpr auto EAXSOURCE_MINROOMHF = -10000L;
constexpr auto EAXSOURCE_MAXROOMHF = 0L;
constexpr auto EAXSOURCE_DEFAULTROOMHF = 0L;

constexpr auto EAXSOURCE_MINOBSTRUCTION = -10000L;
constexpr auto EAXSOURCE_MAXOBSTRUCTION = 0L;
constexpr auto EAXSOURCE_DEFAULTOBSTRUCTION = 0L;

constexpr auto EAXSOURCE_MINOBSTRUCTIONLFRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOBSTRUCTIONLFRATIO = 1.0F;
constexpr auto EAXSOURCE_DEFAULTOBSTRUCTIONLFRATIO = 0.0F;

constexpr auto EAXSOURCE_MINOCCLUSION = -10000L;
constexpr auto EAXSOURCE_MAXOCCLUSION = 0L;
constexpr auto EAXSOURCE_DEFAULTOCCLUSION = 0L;

constexpr auto EAXSOURCE_MINOCCLUSIONLFRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOCCLUSIONLFRATIO = 1.0F;
constexpr auto EAXSOURCE_DEFAULTOCCLUSIONLFRATIO = 0.25F;

constexpr auto EAXSOURCE_MINOCCLUSIONROOMRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOCCLUSIONROOMRATIO = 10.0F;
constexpr auto EAXSOURCE_DEFAULTOCCLUSIONROOMRATIO = 1.5F;

constexpr auto EAXSOURCE_MINOCCLUSIONDIRECTRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOCCLUSIONDIRECTRATIO = 10.0F;
constexpr auto EAXSOURCE_DEFAULTOCCLUSIONDIRECTRATIO = 1.0F;

constexpr auto EAXSOURCE_MINEXCLUSION = -10000L;
constexpr auto EAXSOURCE_MAXEXCLUSION = 0L;
constexpr auto EAXSOURCE_DEFAULTEXCLUSION = 0L;

constexpr auto EAXSOURCE_MINEXCLUSIONLFRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXEXCLUSIONLFRATIO = 1.0F;
constexpr auto EAXSOURCE_DEFAULTEXCLUSIONLFRATIO = 1.0F;

constexpr auto EAXSOURCE_MINOUTSIDEVOLUMEHF = -10000L;
constexpr auto EAXSOURCE_MAXOUTSIDEVOLUMEHF = 0L;
constexpr auto EAXSOURCE_DEFAULTOUTSIDEVOLUMEHF = 0L;

constexpr auto EAXSOURCE_MINDOPPLERFACTOR = 0.0F;
constexpr auto EAXSOURCE_MAXDOPPLERFACTOR = 10.0F;
constexpr auto EAXSOURCE_DEFAULTDOPPLERFACTOR = 1.0F;

constexpr auto EAXSOURCE_MINROLLOFFFACTOR = 0.0F;
constexpr auto EAXSOURCE_MAXROLLOFFFACTOR = 10.0F;
constexpr auto EAXSOURCE_DEFAULTROLLOFFFACTOR = 0.0F;

constexpr auto EAXSOURCE_MINROOMROLLOFFFACTOR = 0.0F;
constexpr auto EAXSOURCE_MAXROOMROLLOFFFACTOR = 10.0F;
constexpr auto EAXSOURCE_DEFAULTROOMROLLOFFFACTOR = 0.0F;

constexpr auto EAXSOURCE_MINAIRABSORPTIONFACTOR = 0.0F;
constexpr auto EAXSOURCE_MAXAIRABSORPTIONFACTOR = 10.0F;
constexpr auto EAXSOURCE_DEFAULTAIRABSORPTIONFACTOR = 0.0F;

// EAX50
constexpr auto EAXSOURCE_MINMACROFXFACTOR = 0.0F;
constexpr auto EAXSOURCE_MAXMACROFXFACTOR = 1.0F;
constexpr auto EAXSOURCE_DEFAULTMACROFXFACTOR = 1.0F;

// EAX50
constexpr auto EAXSOURCE_MINSPEAKERLEVEL = -10000L;
constexpr auto EAXSOURCE_MAXSPEAKERLEVEL = 0L;
constexpr auto EAXSOURCE_DEFAULTSPEAKERLEVEL = -10000L;

constexpr auto EAXSOURCE_DEFAULTFLAGS =
	EAXSOURCEFLAGS_DIRECTHFAUTO |
	EAXSOURCEFLAGS_ROOMAUTO |
	EAXSOURCEFLAGS_ROOMHFAUTO
;

// EAXSOURCEFLAGS_DIRECTHFAUTO, EAXSOURCEFLAGS_ROOMAUTO and EAXSOURCEFLAGS_ROOMHFAUTO are ignored for 2D sources
// EAXSOURCEFLAGS_UPMIX is ignored for 3D sources
constexpr auto EAX50SOURCE_DEFAULTFLAGS =
	EAXSOURCEFLAGS_DIRECTHFAUTO |
	EAXSOURCEFLAGS_ROOMAUTO |
	EAXSOURCEFLAGS_ROOMHFAUTO |
	EAXSOURCEFLAGS_UPMIX
;

// Use this structure for EAXSOURCE_ALLPARAMETERS
// - all levels are hundredths of decibels
// - all delays are in seconds
//
// NOTE: This structure may change in future EAX versions.
//       It is recommended to initialize fields by name:
//              myBuffer.lDirect = 0;
//              myBuffer.lDirectHF = -200;
//              ...
//              myBuffer.dwFlags = myFlags /* see EAXSOURCEFLAGS below */ ;
//       instead of:
//              myBuffer = { 0, -200, ... , 0x00000003 };
//
struct EAX40SOURCEPROPERTIES
{
	long lDirect; // direct path level (at low and mid frequencies)
	long lDirectHF; // relative direct path level at high frequencies
	long lRoom; // room effect level (at low and mid frequencies)
	long lRoomHF; // relative room effect level at high frequencies
	long lObstruction; // main obstruction control (attenuation at high frequencies) 
	float flObstructionLFRatio; // obstruction low-frequency level re. main control
	long lOcclusion; // main occlusion control (attenuation at high frequencies)
	float flOcclusionLFRatio; // occlusion low-frequency level re. main control
	float flOcclusionRoomRatio; // relative occlusion control for room effect
	float flOcclusionDirectRatio; // relative occlusion control for direct path
	long lExclusion; // main exlusion control (attenuation at high frequencies)
	float flExclusionLFRatio; // exclusion low-frequency level re. main control
	long lOutsideVolumeHF; // outside sound cone level at high frequencies
	float flDopplerFactor; // like DS3D flDopplerFactor but per source
	float flRolloffFactor; // like DS3D flRolloffFactor but per source
	float flRoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
	float flAirAbsorptionFactor; // multiplies EAXREVERB_AIRABSORPTIONHF
	unsigned long ulFlags; // modifies the behavior of properties
}; // EAX40SOURCEPROPERTIES

struct EAX50SOURCEPROPERTIES :
	public EAX40SOURCEPROPERTIES
{
	float flMacroFXFactor;
}; // EAX50SOURCEPROPERTIES

// Use this structure for EAXSOURCE_ALLSENDPARAMETERS
// - all levels are hundredths of decibels
//
struct EAXSOURCEALLSENDPROPERTIES
{
	GUID guidReceivingFXSlotID;
	long lSend; // send level (at low and mid frequencies)
	long lSendHF; // relative send level at high frequencies
	long lOcclusion;
	float flOcclusionLFRatio;
	float flOcclusionRoomRatio;
	float flOcclusionDirectRatio;
	long lExclusion;
	float flExclusionLFRatio;
}; // EAXSOURCEALLSENDPROPERTIES

// Use this structure for EAXSOURCE_ACTIVEFXSLOTID
struct EAX40ACTIVEFXSLOTS
{
	GUID guidActiveFXSlots[EAX40_MAX_ACTIVE_FXSLOTS];
};

struct EAX50ACTIVEFXSLOTS
{
	GUID guidActiveFXSlots[EAX50_MAX_ACTIVE_FXSLOTS];
};

// Use this structure for EAXSOURCE_OBSTRUCTIONPARAMETERS property.
struct EAXOBSTRUCTIONPROPERTIES
{
	long lObstruction;
	float flObstructionLFRatio;
};

// Use this structure for EAXSOURCE_OCCLUSIONPARAMETERS property.
struct EAXOCCLUSIONPROPERTIES
{
	long lOcclusion;
	float flOcclusionLFRatio;
	float flOcclusionRoomRatio;
	float flOcclusionDirectRatio;
};

// Use this structure for EAXSOURCE_EXCLUSIONPARAMETERS property.
struct EAXEXCLUSIONPROPERTIES
{
	long lExclusion;
	float flExclusionLFRatio;
};

// Use this structure for EAXSOURCE_SENDPARAMETERS properties.
struct EAXSOURCESENDPROPERTIES
{
	GUID guidReceivingFXSlotID;
	long lSend;
	long lSendHF;
};

// Use this structure for EAXSOURCE_OCCLUSIONSENDPARAMETERS 
struct EAXSOURCEOCCLUSIONSENDPROPERTIES
{
	GUID guidReceivingFXSlotID;
	long lOcclusion;
	float flOcclusionLFRatio;
	float flOcclusionRoomRatio;
	float flOcclusionDirectRatio;
};

// Use this structure for EAXSOURCE_EXCLUSIONSENDPARAMETERS
struct EAXSOURCEEXCLUSIONSENDPROPERTIES
{
	GUID guidReceivingFXSlotID;
	long lExclusion;
	float flExclusionLFRatio;
};

extern const EAX50ACTIVEFXSLOTS EAX40SOURCE_DEFAULTACTIVEFXSLOTID;

// A 3D Source's default ACTIVEFXSLOTID is { EAX_NULL_GUID, EAX_PrimaryFXSlotID, EAX_NULL_GUID, EAX_NULL_GUID }
extern const EAX50ACTIVEFXSLOTS EAX50SOURCE_3DDEFAULTACTIVEFXSLOTID;


// A 2D Source's default ACTIVEFXSLOTID is { EAX_NULL_GUID, EAX_NULL_GUID, EAX_NULL_GUID, EAX_NULL_GUID }
extern const EAX50ACTIVEFXSLOTS EAX50SOURCE_2DDEFAULTACTIVEFXSLOTID;

// EAX REVERB {0CF95C8F-A3CC-4849-B0B6-832ECC1822DF}
DEFINE_GUID(EAX_REVERB_EFFECT,
	0xCF95C8F,
	0xA3CC,
	0x4849,
	0xB0, 0xB6, 0x83, 0x2E, 0xCC, 0x18, 0x22, 0xDF);

// Reverb effect properties
enum EAXREVERB_PROPERTY
{
	EAXREVERB_NONE,
	EAXREVERB_ALLPARAMETERS,
	EAXREVERB_ENVIRONMENT,
	EAXREVERB_ENVIRONMENTSIZE,
	EAXREVERB_ENVIRONMENTDIFFUSION,
	EAXREVERB_ROOM,
	EAXREVERB_ROOMHF,
	EAXREVERB_ROOMLF,
	EAXREVERB_DECAYTIME,
	EAXREVERB_DECAYHFRATIO,
	EAXREVERB_DECAYLFRATIO,
	EAXREVERB_REFLECTIONS,
	EAXREVERB_REFLECTIONSDELAY,
	EAXREVERB_REFLECTIONSPAN,
	EAXREVERB_REVERB,
	EAXREVERB_REVERBDELAY,
	EAXREVERB_REVERBPAN,
	EAXREVERB_ECHOTIME,
	EAXREVERB_ECHODEPTH,
	EAXREVERB_MODULATIONTIME,
	EAXREVERB_MODULATIONDEPTH,
	EAXREVERB_AIRABSORPTIONHF,
	EAXREVERB_HFREFERENCE,
	EAXREVERB_LFREFERENCE,
	EAXREVERB_ROOMROLLOFFFACTOR,
	EAXREVERB_FLAGS,
};

// used by EAXREVERB_ENVIRONMENT
enum : unsigned long
{
	EAX_ENVIRONMENT_GENERIC,
	EAX_ENVIRONMENT_PADDEDCELL,
	EAX_ENVIRONMENT_ROOM,
	EAX_ENVIRONMENT_BATHROOM,
	EAX_ENVIRONMENT_LIVINGROOM,
	EAX_ENVIRONMENT_STONEROOM,
	EAX_ENVIRONMENT_AUDITORIUM,
	EAX_ENVIRONMENT_CONCERTHALL,
	EAX_ENVIRONMENT_CAVE,
	EAX_ENVIRONMENT_ARENA,
	EAX_ENVIRONMENT_HANGAR,
	EAX_ENVIRONMENT_CARPETEDHALLWAY,
	EAX_ENVIRONMENT_HALLWAY,
	EAX_ENVIRONMENT_STONECORRIDOR,
	EAX_ENVIRONMENT_ALLEY,
	EAX_ENVIRONMENT_FOREST,
	EAX_ENVIRONMENT_CITY,
	EAX_ENVIRONMENT_MOUNTAINS,
	EAX_ENVIRONMENT_QUARRY,
	EAX_ENVIRONMENT_PLAIN,
	EAX_ENVIRONMENT_PARKINGLOT,
	EAX_ENVIRONMENT_SEWERPIPE,
	EAX_ENVIRONMENT_UNDERWATER,
	EAX_ENVIRONMENT_DRUGGED,
	EAX_ENVIRONMENT_DIZZY,
	EAX_ENVIRONMENT_PSYCHOTIC,

	// EAX30
	EAX_ENVIRONMENT_UNDEFINED,

	EAX_ENVIRONMENT_COUNT,
};

// Used by EAXREVERB_FLAGS
//
// Note: The number and order of flags may change in future EAX versions.
//       It is recommended to use the flag defines as follows:
//              myFlags = EAXREVERBFLAGS_DECAYTIMESCALE | EAXREVERBFLAGS_REVERBSCALE;
//       instead of:
//              myFlags = 0x00000009;
//
// These flags determine what properties are affected by environment size.
constexpr auto EAXREVERBFLAGS_DECAYTIMESCALE = 0x00000001UL; // reverberation decay time
constexpr auto EAXREVERBFLAGS_REFLECTIONSSCALE = 0x00000002UL; // reflection level
constexpr auto EAXREVERBFLAGS_REFLECTIONSDELAYSCALE = 0x00000004UL; // initial reflection delay time
constexpr auto EAXREVERBFLAGS_REVERBSCALE = 0x00000008UL; // reflections level
constexpr auto EAXREVERBFLAGS_REVERBDELAYSCALE = 0x00000010UL; // late reverberation delay time
constexpr auto EAXREVERBFLAGS_ECHOTIMESCALE = 0x00000040UL; // echo time
constexpr auto EAXREVERBFLAGS_MODULATIONTIMESCALE = 0x00000080UL; // modulation time
// This flag limits high-frequency decay time according to air absorption.
constexpr auto EAXREVERBFLAGS_DECAYHFLIMIT = 0x00000020UL;
constexpr auto EAXREVERBFLAGS_RESERVED = 0xFFFFFF00UL; // reserved future use

// Use this structure for EAXREVERB_ALLPARAMETERS
// - all levels are hundredths of decibels
// - all times and delays are in seconds
//
// NOTE: This structure may change in future EAX versions.
//       It is recommended to initialize fields by name:
//              myReverb.lRoom = -1000;
//              myReverb.lRoomHF = -100;
//              ...
//              myReverb.dwFlags = myFlags /* see EAXREVERBFLAGS below */ ;
//       instead of:
//              myReverb = { -1000, -100, ... , 0x00000009 };
//       If you want to save and load presets in binary form, you 
//       should define your own structure to insure future compatibility.
//
struct EAXREVERBPROPERTIES
{
	unsigned long ulEnvironment; // sets all reverb properties
	float flEnvironmentSize; // environment size in meters
	float flEnvironmentDiffusion; // environment diffusion
	long lRoom; // room effect level (at mid frequencies)
	long lRoomHF; // relative room effect level at high frequencies
	long lRoomLF; // relative room effect level at low frequencies  
	float flDecayTime; // reverberation decay time at mid frequencies
	float flDecayHFRatio; // high-frequency to mid-frequency decay time ratio
	float flDecayLFRatio; // low-frequency to mid-frequency decay time ratio   
	long lReflections; // early reflections level relative to room effect
	float flReflectionsDelay; // initial reflection delay time
	EAXVECTOR vReflectionsPan; // early reflections panning vector
	long lReverb; // late reverberation level relative to room effect
	float flReverbDelay; // late reverberation delay time relative to initial reflection
	EAXVECTOR vReverbPan; // late reverberation panning vector
	float flEchoTime; // echo time
	float flEchoDepth; // echo depth
	float flModulationTime; // modulation time
	float flModulationDepth; // modulation depth
	float flAirAbsorptionHF; // change in level per meter at high frequencies
	float flHFReference; // reference high frequency
	float flLFReference; // reference low frequency 
	float flRoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
	unsigned long ulFlags; // modifies the behavior of properties
};

bool operator==(
	const EAXREVERBPROPERTIES& lhs,
	const EAXREVERBPROPERTIES& rhs) noexcept;

bool operator!=(
	const EAXREVERBPROPERTIES& lhs,
	const EAXREVERBPROPERTIES& rhs) noexcept;


// Property ranges and defaults:
constexpr auto EAXREVERB_MINENVIRONMENT = 0UL;
constexpr auto EAX20REVERB_MAXENVIRONMENT = EAX_ENVIRONMENT_COUNT - 2;
constexpr auto EAX30REVERB_MAXENVIRONMENT = EAX_ENVIRONMENT_COUNT - 1;
constexpr auto EAXREVERB_DEFAULTENVIRONMENT = EAX_ENVIRONMENT_GENERIC;

constexpr auto EAXREVERB_MINENVIRONMENTSIZE = 1.0F;
constexpr auto EAXREVERB_MAXENVIRONMENTSIZE = 100.0F;
constexpr auto EAXREVERB_DEFAULTENVIRONMENTSIZE = 7.5F;

constexpr auto EAXREVERB_MINENVIRONMENTDIFFUSION = 0.0F;
constexpr auto EAXREVERB_MAXENVIRONMENTDIFFUSION = 1.0F;
constexpr auto EAXREVERB_DEFAULTENVIRONMENTDIFFUSION = 1.0F;

constexpr auto EAXREVERB_MINROOM = -10000L;
constexpr auto EAXREVERB_MAXROOM = 0L;
constexpr auto EAXREVERB_DEFAULTROOM = -1000L;

constexpr auto EAXREVERB_MINROOMHF = -10000L;
constexpr auto EAXREVERB_MAXROOMHF = 0L;
constexpr auto EAXREVERB_DEFAULTROOMHF = -100L;

constexpr auto EAXREVERB_MINROOMLF = -10000L;
constexpr auto EAXREVERB_MAXROOMLF = 0L;
constexpr auto EAXREVERB_DEFAULTROOMLF = 0L;

constexpr auto EAXREVERB_MINDECAYTIME = 0.1F;
constexpr auto EAXREVERB_MAXDECAYTIME = 20.0F;
constexpr auto EAXREVERB_DEFAULTDECAYTIME = 1.49F;

constexpr auto EAXREVERB_MINDECAYHFRATIO = 0.1F;
constexpr auto EAXREVERB_MAXDECAYHFRATIO = 2.0F;
constexpr auto EAXREVERB_DEFAULTDECAYHFRATIO = 0.83F;

constexpr auto EAXREVERB_MINDECAYLFRATIO = 0.1F;
constexpr auto EAXREVERB_MAXDECAYLFRATIO = 2.0F;
constexpr auto EAXREVERB_DEFAULTDECAYLFRATIO = 1.0F;

constexpr auto EAXREVERB_MINREFLECTIONS = -10000L;
constexpr auto EAXREVERB_MAXREFLECTIONS = 1000L;
constexpr auto EAXREVERB_DEFAULTREFLECTIONS = -2602L;

constexpr auto EAXREVERB_MINREFLECTIONSDELAY = 0.0F;
constexpr auto EAXREVERB_MAXREFLECTIONSDELAY = 0.3F;
constexpr auto EAXREVERB_DEFAULTREFLECTIONSDELAY = 0.007F;

constexpr auto EAXREVERB_DEFAULTREFLECTIONSPAN = EAXVECTOR{0.0F, 0.0F, 0.0F};

constexpr auto EAXREVERB_MINREVERB = -10000L;
constexpr auto EAXREVERB_MAXREVERB = 2000L;
constexpr auto EAXREVERB_DEFAULTREVERB = 200L;

constexpr auto EAXREVERB_MINREVERBDELAY = 0.0F;
constexpr auto EAXREVERB_MAXREVERBDELAY = 0.1F;
constexpr auto EAXREVERB_DEFAULTREVERBDELAY = 0.011F;

constexpr auto EAXREVERB_DEFAULTREVERBPAN = EAXVECTOR{0.0F, 0.0F, 0.0F};

constexpr auto EAXREVERB_MINECHOTIME = 0.075F;
constexpr auto EAXREVERB_MAXECHOTIME = 0.25F;
constexpr auto EAXREVERB_DEFAULTECHOTIME = 0.25F;

constexpr auto EAXREVERB_MINECHODEPTH = 0.0F;
constexpr auto EAXREVERB_MAXECHODEPTH = 1.0F;
constexpr auto EAXREVERB_DEFAULTECHODEPTH = 0.0F;

constexpr auto EAXREVERB_MINMODULATIONTIME = 0.04F;
constexpr auto EAXREVERB_MAXMODULATIONTIME = 4.0F;
constexpr auto EAXREVERB_DEFAULTMODULATIONTIME = 0.25F;

constexpr auto EAXREVERB_MINMODULATIONDEPTH = 0.0F;
constexpr auto EAXREVERB_MAXMODULATIONDEPTH = 1.0F;
constexpr auto EAXREVERB_DEFAULTMODULATIONDEPTH = 0.0F;

constexpr auto EAXREVERB_MINAIRABSORPTIONHF = -100.0F;
constexpr auto EAXREVERB_MAXAIRABSORPTIONHF = 0.0F;
constexpr auto EAXREVERB_DEFAULTAIRABSORPTIONHF = -5.0F;

constexpr auto EAXREVERB_MINHFREFERENCE = 1000.0F;
constexpr auto EAXREVERB_MAXHFREFERENCE = 20000.0F;
constexpr auto EAXREVERB_DEFAULTHFREFERENCE = 5000.0F;

constexpr auto EAXREVERB_MINLFREFERENCE = 20.0F;
constexpr auto EAXREVERB_MAXLFREFERENCE = 1000.0F;
constexpr auto EAXREVERB_DEFAULTLFREFERENCE = 250.0F;

constexpr auto EAXREVERB_MINROOMROLLOFFFACTOR = 0.0F;
constexpr auto EAXREVERB_MAXROOMROLLOFFFACTOR = 10.0F;
constexpr auto EAXREVERB_DEFAULTROOMROLLOFFFACTOR = 0.0F;

constexpr auto EAXREVERB_DEFAULTFLAGS =
	EAXREVERBFLAGS_DECAYTIMESCALE |
	EAXREVERBFLAGS_REFLECTIONSSCALE |
	EAXREVERBFLAGS_REFLECTIONSDELAYSCALE |
	EAXREVERBFLAGS_REVERBSCALE |
	EAXREVERBFLAGS_REVERBDELAYSCALE |
	EAXREVERBFLAGS_DECAYHFLIMIT
;


extern const EAXREVERBPROPERTIES REVERB_PRESET_GENERIC;
extern const EAXREVERBPROPERTIES REVERB_PRESET_PADDEDCELL;
extern const EAXREVERBPROPERTIES REVERB_PRESET_ROOM;
extern const EAXREVERBPROPERTIES REVERB_PRESET_BATHROOM;
extern const EAXREVERBPROPERTIES REVERB_PRESET_LIVINGROOM;
extern const EAXREVERBPROPERTIES REVERB_PRESET_STONEROOM;
extern const EAXREVERBPROPERTIES REVERB_PRESET_AUDITORIUM;
extern const EAXREVERBPROPERTIES REVERB_PRESET_CONCERTHALL;
extern const EAXREVERBPROPERTIES REVERB_PRESET_CAVE;
extern const EAXREVERBPROPERTIES REVERB_PRESET_ARENA;
extern const EAXREVERBPROPERTIES REVERB_PRESET_HANGAR;
extern const EAXREVERBPROPERTIES REVERB_PRESET_CARPETTEDHALLWAY;
extern const EAXREVERBPROPERTIES REVERB_PRESET_HALLWAY;
extern const EAXREVERBPROPERTIES REVERB_PRESET_STONECORRIDOR;
extern const EAXREVERBPROPERTIES REVERB_PRESET_ALLEY;
extern const EAXREVERBPROPERTIES REVERB_PRESET_FOREST;
extern const EAXREVERBPROPERTIES REVERB_PRESET_CITY;
extern const EAXREVERBPROPERTIES REVERB_PRESET_MOUNTAINS;
extern const EAXREVERBPROPERTIES REVERB_PRESET_QUARRY;
extern const EAXREVERBPROPERTIES REVERB_PRESET_PLAIN;
extern const EAXREVERBPROPERTIES REVERB_PRESET_PARKINGLOT;
extern const EAXREVERBPROPERTIES REVERB_PRESET_SEWERPIPE;
extern const EAXREVERBPROPERTIES REVERB_PRESET_UNDERWATER;
extern const EAXREVERBPROPERTIES REVERB_PRESET_DRUGGED;
extern const EAXREVERBPROPERTIES REVERB_PRESET_DIZZY;
extern const EAXREVERBPROPERTIES REVERB_PRESET_PSYCHOTIC;


using ReverbPresets = std::array<EAXREVERBPROPERTIES, EAX_ENVIRONMENT_UNDEFINED>;
extern const ReverbPresets EAX_REVERB_PRESETS;


} // eaxefx


extern "C"
{


AL_API ALenum AL_APIENTRY EAXSet(
	const GUID* property_set_id,
	ALuint property_id,
	ALuint al_name,
	ALvoid* property_buffer,
	ALuint property_size);

AL_API ALenum AL_APIENTRY EAXGet(
	const GUID* property_set_id,
	ALuint property_id,
	ALuint al_name,
	ALvoid* property_buffer,
	ALuint property_size);


} // extern "C"


#endif // !EAXEFX_EAX_API_INCLUDED
