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


#ifndef EAXEFX_EAX_API_INCLUDED
#define EAXEFX_EAX_API_INCLUDED


//
// EAX API.
//
// Based on headers eax[2-5].h included into Doom 3 source:
// http://github.com/id-Software/DOOM-3/tree/master/neo/openal/include
//


#include <cfloat>
#include <cstdint>

#include <array>

#include "AL/al.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
	std::uint32_t Data1;
	std::uint16_t Data2;
	std::uint16_t Data3;
	std::uint8_t Data4[8];
} GUID;

bool operator==(
	const ::GUID& lhs,
	const ::GUID& rhs) noexcept;

bool operator!=(
	const ::GUID& lhs,
	const ::GUID& rhs) noexcept;
#endif // GUID_DEFINED

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern "C" const ::GUID DSPROPSETID_EAX20_ListenerProperties;

enum DSPROPERTY_EAX20_LISTENERPROPERTY :
	std::uint32_t
{
	DSPROPERTY_EAX20LISTENER_NONE,
	DSPROPERTY_EAX20LISTENER_ALLPARAMETERS,
	DSPROPERTY_EAX20LISTENER_ROOM,
	DSPROPERTY_EAX20LISTENER_ROOMHF,
	DSPROPERTY_EAX20LISTENER_ROOMROLLOFFFACTOR,
	DSPROPERTY_EAX20LISTENER_DECAYTIME,
	DSPROPERTY_EAX20LISTENER_DECAYHFRATIO,
	DSPROPERTY_EAX20LISTENER_REFLECTIONS,
	DSPROPERTY_EAX20LISTENER_REFLECTIONSDELAY,
	DSPROPERTY_EAX20LISTENER_REVERB,
	DSPROPERTY_EAX20LISTENER_REVERBDELAY,
	DSPROPERTY_EAX20LISTENER_ENVIRONMENT,
	DSPROPERTY_EAX20LISTENER_ENVIRONMENTSIZE,
	DSPROPERTY_EAX20LISTENER_ENVIRONMENTDIFFUSION,
	DSPROPERTY_EAX20LISTENER_AIRABSORPTIONHF,
	DSPROPERTY_EAX20LISTENER_FLAGS
}; // DSPROPERTY_EAX20_LISTENERPROPERTY

struct EAX20LISTENERPROPERTIES
{
	std::int32_t lRoom; // room effect level at low frequencies
	std::int32_t lRoomHF; // room effect high-frequency level re. low frequency level
	float flRoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
	float flDecayTime; // reverberation decay time at low frequencies
	float flDecayHFRatio; // high-frequency to low-frequency decay time ratio
	std::int32_t lReflections; // early reflections level relative to room effect
	float flReflectionsDelay; // initial reflection delay time
	std::int32_t lReverb; // late reverberation level relative to room effect
	float flReverbDelay; // late reverberation delay time relative to initial reflection
	std::uint32_t dwEnvironment; // sets all listener properties
	float flEnvironmentSize; // environment size in meters
	float flEnvironmentDiffusion; // environment diffusion
	float flAirAbsorptionHF; // change in level per meter at 5 kHz
	std::uint32_t dwFlags; // modifies the behavior of properties
}; // EAX20LISTENERPROPERTIES

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern "C" const ::GUID DSPROPSETID_EAX20_BufferProperties;


enum DSPROPERTY_EAX20_BUFFERPROPERTY :
	std::uint32_t
{
	DSPROPERTY_EAX20BUFFER_NONE,
	DSPROPERTY_EAX20BUFFER_ALLPARAMETERS,
	DSPROPERTY_EAX20BUFFER_DIRECT,
	DSPROPERTY_EAX20BUFFER_DIRECTHF,
	DSPROPERTY_EAX20BUFFER_ROOM,
	DSPROPERTY_EAX20BUFFER_ROOMHF,
	DSPROPERTY_EAX20BUFFER_ROOMROLLOFFFACTOR,
	DSPROPERTY_EAX20BUFFER_OBSTRUCTION,
	DSPROPERTY_EAX20BUFFER_OBSTRUCTIONLFRATIO,
	DSPROPERTY_EAX20BUFFER_OCCLUSION,
	DSPROPERTY_EAX20BUFFER_OCCLUSIONLFRATIO,
	DSPROPERTY_EAX20BUFFER_OCCLUSIONROOMRATIO,
	DSPROPERTY_EAX20BUFFER_OUTSIDEVOLUMEHF,
	DSPROPERTY_EAX20BUFFER_AIRABSORPTIONFACTOR,
	DSPROPERTY_EAX20BUFFER_FLAGS
}; // DSPROPERTY_EAX20_BUFFERPROPERTY


struct EAX20BUFFERPROPERTIES
{
	std::int32_t lDirect; // direct path level
	std::int32_t lDirectHF; // direct path level at high frequencies
	std::int32_t lRoom; // room effect level
	std::int32_t lRoomHF; // room effect level at high frequencies
	float flRoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
	std::int32_t lObstruction; // main obstruction control (attenuation at high frequencies) 
	float flObstructionLFRatio; // obstruction low-frequency level re. main control
	std::int32_t lOcclusion; // main occlusion control (attenuation at high frequencies)
	float flOcclusionLFRatio; // occlusion low-frequency level re. main control
	float flOcclusionRoomRatio; // occlusion room effect level re. main control
	std::int32_t lOutsideVolumeHF; // outside sound cone level at high frequencies
	float flAirAbsorptionFactor; // multiplies ::DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF
	std::uint32_t dwFlags; // modifies the behavior of properties
}; // EAX20BUFFERPROPERTIES

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern "C" const ::GUID DSPROPSETID_EAX30_ListenerProperties;

extern "C" const ::GUID DSPROPSETID_EAX30_BufferProperties;

enum DSPROPERTY_EAX_BUFFERPROPERTY :
	std::uint32_t
{
	DSPROPERTY_EAXBUFFER_NONE,
	DSPROPERTY_EAXBUFFER_ALLPARAMETERS,
	DSPROPERTY_EAXBUFFER_OBSTRUCTIONPARAMETERS,
	DSPROPERTY_EAXBUFFER_OCCLUSIONPARAMETERS,
	DSPROPERTY_EAXBUFFER_EXCLUSIONPARAMETERS,
	DSPROPERTY_EAXBUFFER_DIRECT,
	DSPROPERTY_EAXBUFFER_DIRECTHF,
	DSPROPERTY_EAXBUFFER_ROOM,
	DSPROPERTY_EAXBUFFER_ROOMHF,
	DSPROPERTY_EAXBUFFER_OBSTRUCTION,
	DSPROPERTY_EAXBUFFER_OBSTRUCTIONLFRATIO,
	DSPROPERTY_EAXBUFFER_OCCLUSION,
	DSPROPERTY_EAXBUFFER_OCCLUSIONLFRATIO,
	DSPROPERTY_EAXBUFFER_OCCLUSIONROOMRATIO,
	DSPROPERTY_EAXBUFFER_OCCLUSIONDIRECTRATIO,
	DSPROPERTY_EAXBUFFER_EXCLUSION,
	DSPROPERTY_EAXBUFFER_EXCLUSIONLFRATIO,
	DSPROPERTY_EAXBUFFER_OUTSIDEVOLUMEHF,
	DSPROPERTY_EAXBUFFER_DOPPLERFACTOR,
	DSPROPERTY_EAXBUFFER_ROLLOFFFACTOR,
	DSPROPERTY_EAXBUFFER_ROOMROLLOFFFACTOR,
	DSPROPERTY_EAXBUFFER_AIRABSORPTIONFACTOR,
	DSPROPERTY_EAXBUFFER_FLAGS
}; // DSPROPERTY_EAX_BUFFERPROPERTY

struct EAXBUFFERPROPERTIES
{
	std::int32_t lDirect; // direct path level (at low and mid frequencies)
	std::int32_t lDirectHF; // relative direct path level at high frequencies
	std::int32_t lRoom; // room effect level (at low and mid frequencies)
	std::int32_t lRoomHF; // relative room effect level at high frequencies
	std::int32_t lObstruction; // main obstruction control (attenuation at high frequencies) 
	float flObstructionLFRatio; // obstruction low-frequency level re. main control
	std::int32_t lOcclusion; // main occlusion control (attenuation at high frequencies)
	float flOcclusionLFRatio; // occlusion low-frequency level re. main control
	float flOcclusionRoomRatio; // relative occlusion control for room effect
	float flOcclusionDirectRatio; // relative occlusion control for direct path
	std::int32_t lExclusion; // main exlusion control (attenuation at high frequencies)
	float flExclusionLFRatio; // exclusion low-frequency level re. main control
	std::int32_t lOutsideVolumeHF; // outside sound cone level at high frequencies
	float flDopplerFactor; // like DS3D flDopplerFactor but per source
	float flRolloffFactor; // like DS3D flRolloffFactor but per source
	float flRoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
	float flAirAbsorptionFactor; // multiplies ::DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF
	std::uint32_t ulFlags; // modifies the behavior of properties
}; // EAXBUFFERPROPERTIES

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

constexpr auto EAX_MAX_FXSLOTS = 4;

constexpr auto EAX40_MAX_ACTIVE_FXSLOTS = 2;
constexpr auto EAX50_MAX_ACTIVE_FXSLOTS = 4;


constexpr auto EAX_OK = 0;
constexpr auto EAXERR_INVALID_OPERATION = -1;
constexpr auto EAXERR_INVALID_VALUE = -2;
constexpr auto EAXERR_NO_EFFECT_LOADED = -3;
constexpr auto EAXERR_UNKNOWN_EFFECT = -4;
constexpr auto EAXERR_INCOMPATIBLE_SOURCE_TYPE = -5;
constexpr auto EAXERR_INCOMPATIBLE_EAX_VERSION = -6;


extern "C" const ::GUID EAX_NULL_GUID;

extern "C" const ::GUID EAX_PrimaryFXSlotID;


struct EAXVECTOR
{
	float x;
	float y;
	float z;
}; // EAXVECTOR

bool operator==(
	const ::EAXVECTOR& lhs,
	const ::EAXVECTOR& rhs) noexcept;

bool operator!=(
	const ::EAXVECTOR& lhs,
	const ::EAXVECTOR& rhs) noexcept;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern "C" const ::GUID EAXPROPERTYID_EAX40_Context;

extern "C" const ::GUID EAXPROPERTYID_EAX50_Context;

// EAX50
enum :
	std::uint32_t
{
	HEADPHONES = 0,
	SPEAKERS_2,
	SPEAKERS_4,
	SPEAKERS_5,	// 5.1 speakers
	SPEAKERS_6, // 6.1 speakers
	SPEAKERS_7, // 7.1 speakers
};

// EAX50
enum :
	std::uint32_t
{
	EAX_40 = 5, // EAX 4.0
	EAX_50 = 6, // EAX 5.0
};

constexpr auto EAXCONTEXT_MINEAXSESSION = ::EAX_40;
constexpr auto EAXCONTEXT_MAXEAXSESSION = ::EAX_50;
constexpr auto EAXCONTEXT_DEFAULTEAXSESSION = ::EAX_40;

constexpr auto EAXCONTEXT_MINMAXACTIVESENDS = 2U;
constexpr auto EAXCONTEXT_MAXMAXACTIVESENDS = 4U;
constexpr auto EAXCONTEXT_DEFAULTMAXACTIVESENDS = 2U;

// EAX50
struct EAXSESSIONPROPERTIES
{
	std::uint32_t ulEAXVersion;
	std::uint32_t ulMaxActiveSends;
}; // EAXSESSIONPROPERTIES

enum EAXCONTEXT_PROPERTY :
	std::uint32_t
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
}; // EAXCONTEXT_PROPERTY

struct EAX40CONTEXTPROPERTIES
{
	::GUID guidPrimaryFXSlotID;
	float flDistanceFactor;
	float flAirAbsorptionHF;
	float flHFReference;
}; // EAX40CONTEXTPROPERTIES

struct EAX50CONTEXTPROPERTIES :
	public ::EAX40CONTEXTPROPERTIES
{
	float flMacroFXFactor;
}; // EAX40CONTEXTPROPERTIES


bool operator==(
	const ::EAX40CONTEXTPROPERTIES& lhs,
	const ::EAX40CONTEXTPROPERTIES& rhs) noexcept;

bool operator==(
	const ::EAX50CONTEXTPROPERTIES& lhs,
	const ::EAX50CONTEXTPROPERTIES& rhs) noexcept;


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


extern "C" const ::GUID EAXPROPERTYID_EAX40_FXSlot0;

extern "C" const ::GUID EAXPROPERTYID_EAX50_FXSlot0;

extern "C" const ::GUID EAXPROPERTYID_EAX40_FXSlot1;

extern "C" const ::GUID EAXPROPERTYID_EAX50_FXSlot1;

extern "C" const ::GUID EAXPROPERTYID_EAX40_FXSlot2;

extern "C" const ::GUID EAXPROPERTYID_EAX50_FXSlot2;

extern "C" const ::GUID EAXPROPERTYID_EAX40_FXSlot3;

extern "C" const ::GUID EAXPROPERTYID_EAX50_FXSlot3;

extern "C" const ::GUID EAXCONTEXT_DEFAULTPRIMARYFXSLOTID;

enum EAXFXSLOT_PROPERTY :
	std::uint32_t
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
}; // EAXFXSLOT_PROPERTY

constexpr auto EAXFXSLOTFLAGS_ENVIRONMENT = 0x00000001U;
// EAX50
constexpr auto EAXFXSLOTFLAGS_UPMIX = 0x00000002U;

constexpr auto EAX40FXSLOTFLAGS_RESERVED = 0xFFFFFFFEU; // reserved future use
constexpr auto EAX50FXSLOTFLAGS_RESERVED = 0xFFFFFFFCU; // reserved future use


constexpr auto EAXFXSLOT_MINVOLUME = -10'000;
constexpr auto EAXFXSLOT_MAXVOLUME = 0;
constexpr auto EAXFXSLOT_DEFAULTVOLUME = 0;

constexpr auto EAXFXSLOT_MINLOCK = 0;
constexpr auto EAXFXSLOT_MAXLOCK = 1;

enum :
	std::int32_t
{
	EAXFXSLOT_UNLOCKED = 0,
	EAXFXSLOT_LOCKED = 1
};

constexpr auto EAXFXSLOT_MINOCCLUSION = -10'000;
constexpr auto EAXFXSLOT_MAXOCCLUSION = 0;
constexpr auto EAXFXSLOT_DEFAULTOCCLUSION = 0;

constexpr auto EAXFXSLOT_MINOCCLUSIONLFRATIO = 0.0F;
constexpr auto EAXFXSLOT_MAXOCCLUSIONLFRATIO = 1.0F;
constexpr auto EAXFXSLOT_DEFAULTOCCLUSIONLFRATIO = 0.25F;

constexpr auto EAX40FXSLOT_DEFAULTFLAGS = ::EAXFXSLOTFLAGS_ENVIRONMENT;

constexpr auto EAX50FXSLOT_DEFAULTFLAGS =
	::EAXFXSLOTFLAGS_ENVIRONMENT |
	::EAXFXSLOTFLAGS_UPMIX // ignored for reverb
;

struct EAX40FXSLOTPROPERTIES
{
	::GUID guidLoadEffect;
	std::int32_t lVolume;
	std::int32_t lLock;
	std::uint32_t ulFlags;
}; // EAX40FXSLOTPROPERTIES

struct EAX50FXSLOTPROPERTIES :
	public ::EAX40FXSLOTPROPERTIES
{
	std::int32_t lOcclusion;
	float flOcclusionLFRatio;
}; // EAX50FXSLOTPROPERTIES

bool operator==(
	const ::EAX40FXSLOTPROPERTIES& lhs,
	const ::EAX40FXSLOTPROPERTIES& rhs) noexcept;

bool operator==(
	const ::EAX50FXSLOTPROPERTIES& lhs,
	const ::EAX50FXSLOTPROPERTIES& rhs) noexcept;

extern "C" const ::GUID EAXPROPERTYID_EAX40_Source;

extern "C" const ::GUID EAXPROPERTYID_EAX50_Source;

// Source object properties
enum EAXSOURCE_PROPERTY :
	std::uint32_t
{
	// EAX30
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
}; // EAXSOURCE_PROPERTY


constexpr auto EAXSOURCEFLAGS_DIRECTHFAUTO = 0x00000001U; // relates to ::EAXSOURCE_DIRECTHF
constexpr auto EAXSOURCEFLAGS_ROOMAUTO = 0x00000002U; // relates to ::EAXSOURCE_ROOM
constexpr auto EAXSOURCEFLAGS_ROOMHFAUTO = 0x00000004U; // relates to ::EAXSOURCE_ROOMHF
// EAX50
constexpr auto EAXSOURCEFLAGS_3DELEVATIONFILTER = 0x00000008U;
// EAX50
constexpr auto EAXSOURCEFLAGS_UPMIX = 0x00000010U;
// EAX50
constexpr auto EAXSOURCEFLAGS_APPLYSPEAKERLEVELS = 0x00000020U;

constexpr auto EAX20SOURCEFLAGS_RESERVED = 0xFFFFFFF8U; // reserved future use
constexpr auto EAX50SOURCEFLAGS_RESERVED = 0xFFFFFFC0U; // reserved future use


constexpr auto EAXSOURCE_MINSEND = -10'000;
constexpr auto EAXSOURCE_MAXSEND = 0;
constexpr auto EAXSOURCE_DEFAULTSEND = 0;

constexpr auto EAXSOURCE_MINSENDHF = -10'000;
constexpr auto EAXSOURCE_MAXSENDHF = 0;
constexpr auto EAXSOURCE_DEFAULTSENDHF = 0;

constexpr auto EAXSOURCE_MINDIRECT = -10'000;
constexpr auto EAXSOURCE_MAXDIRECT = 1'000;
constexpr auto EAXSOURCE_DEFAULTDIRECT = 0;

constexpr auto EAXSOURCE_MINDIRECTHF = -10'000;
constexpr auto EAXSOURCE_MAXDIRECTHF = 0;
constexpr auto EAXSOURCE_DEFAULTDIRECTHF = 0;

constexpr auto EAXSOURCE_MINROOM = -10'000;
constexpr auto EAXSOURCE_MAXROOM = 1'000;
constexpr auto EAXSOURCE_DEFAULTROOM = 0;

constexpr auto EAXSOURCE_MINROOMHF = -10'000;
constexpr auto EAXSOURCE_MAXROOMHF = 0;
constexpr auto EAXSOURCE_DEFAULTROOMHF = 0;

constexpr auto EAXSOURCE_MINOBSTRUCTION = -10'000;
constexpr auto EAXSOURCE_MAXOBSTRUCTION = 0;
constexpr auto EAXSOURCE_DEFAULTOBSTRUCTION = 0;

constexpr auto EAXSOURCE_MINOBSTRUCTIONLFRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOBSTRUCTIONLFRATIO = 1.0F;
constexpr auto EAXSOURCE_DEFAULTOBSTRUCTIONLFRATIO = 0.0F;

constexpr auto EAXSOURCE_MINOCCLUSION = -10'000;
constexpr auto EAXSOURCE_MAXOCCLUSION = 0;
constexpr auto EAXSOURCE_DEFAULTOCCLUSION = 0;

constexpr auto EAXSOURCE_MINOCCLUSIONLFRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOCCLUSIONLFRATIO = 1.0F;
constexpr auto EAXSOURCE_DEFAULTOCCLUSIONLFRATIO = 0.25F;

constexpr auto EAXSOURCE_MINOCCLUSIONROOMRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOCCLUSIONROOMRATIO = 10.0F;
constexpr auto EAXSOURCE_DEFAULTOCCLUSIONROOMRATIO = 1.5F;

constexpr auto EAXSOURCE_MINOCCLUSIONDIRECTRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXOCCLUSIONDIRECTRATIO = 10.0F;
constexpr auto EAXSOURCE_DEFAULTOCCLUSIONDIRECTRATIO = 1.0F;

constexpr auto EAXSOURCE_MINEXCLUSION = -10'000;
constexpr auto EAXSOURCE_MAXEXCLUSION = 0;
constexpr auto EAXSOURCE_DEFAULTEXCLUSION = 0;

constexpr auto EAXSOURCE_MINEXCLUSIONLFRATIO = 0.0F;
constexpr auto EAXSOURCE_MAXEXCLUSIONLFRATIO = 1.0F;
constexpr auto EAXSOURCE_DEFAULTEXCLUSIONLFRATIO = 1.0F;

constexpr auto EAXSOURCE_MINOUTSIDEVOLUMEHF = -10'000;
constexpr auto EAXSOURCE_MAXOUTSIDEVOLUMEHF = 0;
constexpr auto EAXSOURCE_DEFAULTOUTSIDEVOLUMEHF = 0;

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

constexpr auto EAXSOURCE_MINSPEAKERLEVEL = -10'000;
constexpr auto EAXSOURCE_MAXSPEAKERLEVEL = 0;
constexpr auto EAXSOURCE_DEFAULTSPEAKERLEVEL = -10'000;

constexpr auto EAXSOURCE_DEFAULTFLAGS =
	::EAXSOURCEFLAGS_DIRECTHFAUTO |
	::EAXSOURCEFLAGS_ROOMAUTO |
	::EAXSOURCEFLAGS_ROOMHFAUTO
;

enum :
	std::int32_t
{
	EAXSPEAKER_FRONT_LEFT = 1,
	EAXSPEAKER_FRONT_CENTER = 2,
	EAXSPEAKER_FRONT_RIGHT = 3,
	EAXSPEAKER_SIDE_RIGHT = 4,
	EAXSPEAKER_REAR_RIGHT = 5,
	EAXSPEAKER_REAR_CENTER = 6,
	EAXSPEAKER_REAR_LEFT = 7,
	EAXSPEAKER_SIDE_LEFT = 8,
	EAXSPEAKER_LOW_FREQUENCY = 9
};

// EAXSOURCEFLAGS_DIRECTHFAUTO, EAXSOURCEFLAGS_ROOMAUTO and EAXSOURCEFLAGS_ROOMHFAUTO are ignored for 2D sources
// EAXSOURCEFLAGS_UPMIX is ignored for 3D sources
constexpr auto EAX50SOURCE_DEFAULTFLAGS =
	::EAXSOURCEFLAGS_DIRECTHFAUTO |
	::EAXSOURCEFLAGS_ROOMAUTO |
	::EAXSOURCEFLAGS_ROOMHFAUTO |
	::EAXSOURCEFLAGS_UPMIX
;

struct EAX30SOURCEPROPERTIES
{
	std::int32_t lDirect; // direct path level (at low and mid frequencies)
	std::int32_t lDirectHF; // relative direct path level at high frequencies
	std::int32_t lRoom; // room effect level (at low and mid frequencies)
	std::int32_t lRoomHF; // relative room effect level at high frequencies
	std::int32_t lObstruction; // main obstruction control (attenuation at high frequencies) 
	float flObstructionLFRatio; // obstruction low-frequency level re. main control
	std::int32_t lOcclusion; // main occlusion control (attenuation at high frequencies)
	float flOcclusionLFRatio; // occlusion low-frequency level re. main control
	float flOcclusionRoomRatio; // relative occlusion control for room effect
	float flOcclusionDirectRatio; // relative occlusion control for direct path
	std::int32_t lExclusion; // main exlusion control (attenuation at high frequencies)
	float flExclusionLFRatio; // exclusion low-frequency level re. main control
	std::int32_t lOutsideVolumeHF; // outside sound cone level at high frequencies
	float flDopplerFactor; // like DS3D flDopplerFactor but per source
	float flRolloffFactor; // like DS3D flRolloffFactor but per source
	float flRoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
	float flAirAbsorptionFactor; // multiplies ::EAXREVERB_AIRABSORPTIONHF
	std::uint32_t ulFlags; // modifies the behavior of properties
}; // EAX30SOURCEPROPERTIES

struct EAX50SOURCEPROPERTIES :
	public ::EAX30SOURCEPROPERTIES
{
	float flMacroFXFactor;
}; // EAX50SOURCEPROPERTIES

struct EAXSOURCEALLSENDPROPERTIES
{
	::GUID guidReceivingFXSlotID;
	std::int32_t lSend; // send level (at low and mid frequencies)
	std::int32_t lSendHF; // relative send level at high frequencies
	std::int32_t lOcclusion;
	float flOcclusionLFRatio;
	float flOcclusionRoomRatio;
	float flOcclusionDirectRatio;
	std::int32_t lExclusion;
	float flExclusionLFRatio;
}; // EAXSOURCEALLSENDPROPERTIES

struct EAXSOURCE2DPROPERTIES
{
	std::int32_t lDirect; // direct path level (at low and mid frequencies)
	std::int32_t lDirectHF; // relative direct path level at high frequencies
	std::int32_t lRoom; // room effect level (at low and mid frequencies)
	std::int32_t lRoomHF; // relative room effect level at high frequencies
	std::uint32_t ulFlags; // modifies the behavior of properties
}; // EAXSOURCE2DPROPERTIES

struct EAXSPEAKERLEVELPROPERTIES
{
	std::int32_t lSpeakerID;
	std::int32_t lLevel;
}; // EAXSPEAKERLEVELPROPERTIES

struct EAX40ACTIVEFXSLOTS
{
	::GUID guidActiveFXSlots[::EAX40_MAX_ACTIVE_FXSLOTS];
}; // EAX40ACTIVEFXSLOTS

struct EAX50ACTIVEFXSLOTS
{
	::GUID guidActiveFXSlots[::EAX50_MAX_ACTIVE_FXSLOTS];
}; // EAX50ACTIVEFXSLOTS

bool operator==(
	const ::EAX50ACTIVEFXSLOTS& lhs,
	const ::EAX50ACTIVEFXSLOTS& rhs) noexcept;

bool operator!=(
	const ::EAX50ACTIVEFXSLOTS& lhs,
	const ::EAX50ACTIVEFXSLOTS& rhs) noexcept;

// Use this structure for EAXSOURCE_OBSTRUCTIONPARAMETERS property.
struct EAXOBSTRUCTIONPROPERTIES
{
	std::int32_t lObstruction;
	float flObstructionLFRatio;
}; // EAXOBSTRUCTIONPROPERTIES

// Use this structure for EAXSOURCE_OCCLUSIONPARAMETERS property.
struct EAXOCCLUSIONPROPERTIES
{
	std::int32_t lOcclusion;
	float flOcclusionLFRatio;
	float flOcclusionRoomRatio;
	float flOcclusionDirectRatio;
}; // EAXOCCLUSIONPROPERTIES

// Use this structure for EAXSOURCE_EXCLUSIONPARAMETERS property.
struct EAXEXCLUSIONPROPERTIES
{
	std::int32_t lExclusion;
	float flExclusionLFRatio;
}; // EAXEXCLUSIONPROPERTIES

// Use this structure for EAXSOURCE_SENDPARAMETERS properties.
struct EAXSOURCESENDPROPERTIES
{
	::GUID guidReceivingFXSlotID;
	std::int32_t lSend;
	std::int32_t lSendHF;
}; // EAXSOURCESENDPROPERTIES

// Use this structure for EAXSOURCE_OCCLUSIONSENDPARAMETERS 
struct EAXSOURCEOCCLUSIONSENDPROPERTIES
{
	::GUID guidReceivingFXSlotID;
	std::int32_t lOcclusion;
	float flOcclusionLFRatio;
	float flOcclusionRoomRatio;
	float flOcclusionDirectRatio;
}; // EAXSOURCEOCCLUSIONSENDPROPERTIES

// Use this structure for EAXSOURCE_EXCLUSIONSENDPARAMETERS
struct EAXSOURCEEXCLUSIONSENDPROPERTIES
{
	::GUID guidReceivingFXSlotID;
	std::int32_t lExclusion;
	float flExclusionLFRatio;
}; // EAXSOURCEEXCLUSIONSENDPROPERTIES

extern const ::EAX50ACTIVEFXSLOTS EAX40SOURCE_DEFAULTACTIVEFXSLOTID;

extern const ::EAX50ACTIVEFXSLOTS EAX50SOURCE_3DDEFAULTACTIVEFXSLOTID;

extern const ::EAX50ACTIVEFXSLOTS EAX50SOURCE_2DDEFAULTACTIVEFXSLOTID;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// EAX Reverb Effect

extern "C" const ::GUID EAX_REVERB_EFFECT;

// Reverb effect properties
enum EAXREVERB_PROPERTY :
	std::uint32_t
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
}; // EAXREVERB_PROPERTY

// used by EAXREVERB_ENVIRONMENT
enum :
	std::uint32_t
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


// reverberation decay time
constexpr auto EAXREVERBFLAGS_DECAYTIMESCALE = 0x00000001U;

// reflection level
constexpr auto EAXREVERBFLAGS_REFLECTIONSSCALE = 0x00000002U;

// initial reflection delay time
constexpr auto EAXREVERBFLAGS_REFLECTIONSDELAYSCALE = 0x00000004U;

// reflections level
constexpr auto EAXREVERBFLAGS_REVERBSCALE = 0x00000008U;

// late reverberation delay time
constexpr auto EAXREVERBFLAGS_REVERBDELAYSCALE = 0x00000010U;

// echo time
// EAX30+
constexpr auto EAXREVERBFLAGS_ECHOTIMESCALE = 0x00000040U;

// modulation time
// EAX30+
constexpr auto EAXREVERBFLAGS_MODULATIONTIMESCALE = 0x00000080U;

// This flag limits high-frequency decay time according to air absorption.
constexpr auto EAXREVERBFLAGS_DECAYHFLIMIT = 0x00000020U;

constexpr auto EAXREVERBFLAGS_RESERVED = 0xFFFFFF00U; // reserved future use


struct EAXREVERBPROPERTIES
{
	std::uint32_t ulEnvironment; // sets all reverb properties
	float flEnvironmentSize; // environment size in meters
	float flEnvironmentDiffusion; // environment diffusion
	std::int32_t lRoom; // room effect level (at mid frequencies)
	std::int32_t lRoomHF; // relative room effect level at high frequencies
	std::int32_t lRoomLF; // relative room effect level at low frequencies  
	float flDecayTime; // reverberation decay time at mid frequencies
	float flDecayHFRatio; // high-frequency to mid-frequency decay time ratio
	float flDecayLFRatio; // low-frequency to mid-frequency decay time ratio   
	std::int32_t lReflections; // early reflections level relative to room effect
	float flReflectionsDelay; // initial reflection delay time
	::EAXVECTOR vReflectionsPan; // early reflections panning vector
	std::int32_t lReverb; // late reverberation level relative to room effect
	float flReverbDelay; // late reverberation delay time relative to initial reflection
	::EAXVECTOR vReverbPan; // late reverberation panning vector
	float flEchoTime; // echo time
	float flEchoDepth; // echo depth
	float flModulationTime; // modulation time
	float flModulationDepth; // modulation depth
	float flAirAbsorptionHF; // change in level per meter at high frequencies
	float flHFReference; // reference high frequency
	float flLFReference; // reference low frequency 
	float flRoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
	std::uint32_t ulFlags; // modifies the behavior of properties
}; // EAXREVERBPROPERTIES

bool operator==(
	const ::EAXREVERBPROPERTIES& lhs,
	const ::EAXREVERBPROPERTIES& rhs) noexcept;

bool operator!=(
	const ::EAXREVERBPROPERTIES& lhs,
	const ::EAXREVERBPROPERTIES& rhs) noexcept;


constexpr auto EAXREVERB_MINENVIRONMENT = 0U;
constexpr auto EAX20REVERB_MAXENVIRONMENT = ::EAX_ENVIRONMENT_COUNT - 2U;
constexpr auto EAX30REVERB_MAXENVIRONMENT = ::EAX_ENVIRONMENT_COUNT - 1U;
constexpr auto EAXREVERB_DEFAULTENVIRONMENT = ::EAX_ENVIRONMENT_GENERIC;

constexpr auto EAXREVERB_MINENVIRONMENTSIZE = 1.0F;
constexpr auto EAXREVERB_MAXENVIRONMENTSIZE = 100.0F;
constexpr auto EAXREVERB_DEFAULTENVIRONMENTSIZE = 7.5F;

constexpr auto EAXREVERB_MINENVIRONMENTDIFFUSION = 0.0F;
constexpr auto EAXREVERB_MAXENVIRONMENTDIFFUSION = 1.0F;
constexpr auto EAXREVERB_DEFAULTENVIRONMENTDIFFUSION = 1.0F;

constexpr auto EAXREVERB_MINROOM = -10'000;
constexpr auto EAXREVERB_MAXROOM = 0;
constexpr auto EAXREVERB_DEFAULTROOM = -1'000;

constexpr auto EAXREVERB_MINROOMHF = -10'000;
constexpr auto EAXREVERB_MAXROOMHF = 0;
constexpr auto EAXREVERB_DEFAULTROOMHF = -100;

constexpr auto EAXREVERB_MINROOMLF = -10'000;
constexpr auto EAXREVERB_MAXROOMLF = 0;
constexpr auto EAXREVERB_DEFAULTROOMLF = 0;

constexpr auto EAXREVERB_MINDECAYTIME = 0.1F;
constexpr auto EAXREVERB_MAXDECAYTIME = 20.0F;
constexpr auto EAXREVERB_DEFAULTDECAYTIME = 1.49F;

constexpr auto EAXREVERB_MINDECAYHFRATIO = 0.1F;
constexpr auto EAXREVERB_MAXDECAYHFRATIO = 2.0F;
constexpr auto EAXREVERB_DEFAULTDECAYHFRATIO = 0.83F;

constexpr auto EAXREVERB_MINDECAYLFRATIO = 0.1F;
constexpr auto EAXREVERB_MAXDECAYLFRATIO = 2.0F;
constexpr auto EAXREVERB_DEFAULTDECAYLFRATIO = 1.0F;

constexpr auto EAXREVERB_MINREFLECTIONS = -10'000;
constexpr auto EAXREVERB_MAXREFLECTIONS = 1'000;
constexpr auto EAXREVERB_DEFAULTREFLECTIONS = -2'602;

constexpr auto EAXREVERB_MINREFLECTIONSDELAY = 0.0F;
constexpr auto EAXREVERB_MAXREFLECTIONSDELAY = 0.3F;
constexpr auto EAXREVERB_DEFAULTREFLECTIONSDELAY = 0.007F;

constexpr auto EAXREVERB_DEFAULTREFLECTIONSPAN = ::EAXVECTOR{0.0F, 0.0F, 0.0F};

constexpr auto EAXREVERB_MINREVERB = -10'000;
constexpr auto EAXREVERB_MAXREVERB = 2'000;
constexpr auto EAXREVERB_DEFAULTREVERB = 200;

constexpr auto EAXREVERB_MINREVERBDELAY = 0.0F;
constexpr auto EAXREVERB_MAXREVERBDELAY = 0.1F;
constexpr auto EAXREVERB_DEFAULTREVERBDELAY = 0.011F;

constexpr auto EAXREVERB_DEFAULTREVERBPAN = ::EAXVECTOR{0.0F, 0.0F, 0.0F};

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

constexpr auto EAXREVERB_MINHFREFERENCE = 1'000.0F;
constexpr auto EAXREVERB_MAXHFREFERENCE = 20'000.0F;
constexpr auto EAXREVERB_DEFAULTHFREFERENCE = 5'000.0F;

constexpr auto EAXREVERB_MINLFREFERENCE = 20.0F;
constexpr auto EAXREVERB_MAXLFREFERENCE = 1'000.0F;
constexpr auto EAXREVERB_DEFAULTLFREFERENCE = 250.0F;

constexpr auto EAXREVERB_MINROOMROLLOFFFACTOR = 0.0F;
constexpr auto EAXREVERB_MAXROOMROLLOFFFACTOR = 10.0F;
constexpr auto EAXREVERB_DEFAULTROOMROLLOFFFACTOR = 0.0F;

constexpr auto EAXREVERB_DEFAULTFLAGS =
	::EAXREVERBFLAGS_DECAYTIMESCALE |
	::EAXREVERBFLAGS_REFLECTIONSSCALE |
	::EAXREVERBFLAGS_REFLECTIONSDELAYSCALE |
	::EAXREVERBFLAGS_REVERBSCALE |
	::EAXREVERBFLAGS_REVERBDELAYSCALE |
	::EAXREVERBFLAGS_DECAYHFLIMIT
;


extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_GENERIC;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_PADDEDCEL;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_ROOM;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_BATHROOM;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_LIVINGROOM;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_STONEROOM;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_AUDITORIUM;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_CONCERTHAL;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_CAVE;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_ARENA;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_HANGAR;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_CARPETTEDHALLWAY;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_HALLWAY;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_STONECORRIDOR;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_ALLEY;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_FOREST;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_CITY;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_MOUNTAINS;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_QUARRY;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_PLAIN;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_PARKINGLOT;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_SEWERPIPE;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_UNDERWATER;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_DRUGGED;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_DIZZY;
extern const ::EAXREVERBPROPERTIES EAXREVERB_PRESET_PSYCHOTIC;


using EaxReverbPresets = std::array<::EAXREVERBPROPERTIES, ::EAX_ENVIRONMENT_UNDEFINED>;
extern const ::EaxReverbPresets EAXREVERB_PRESETS;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// AGC Compressor Effect

extern "C" const ::GUID EAX_AGCCOMPRESSOR_EFFECT;

enum EAXAGCCOMPRESSOR_PROPERTY :
	std::uint32_t
{
	EAXAGCCOMPRESSOR_NONE,
	EAXAGCCOMPRESSOR_ALLPARAMETERS,
	EAXAGCCOMPRESSOR_ONOFF,
}; // EAXAGCCOMPRESSOR_PROPERTY

struct EAXAGCCOMPRESSORPROPERTIES
{
	std::uint32_t ulOnOff; // Switch Compressor on or off
}; // EAXAGCCOMPRESSORPROPERTIES


constexpr auto EAXAGCCOMPRESSOR_MINONOFF = 0U;
constexpr auto EAXAGCCOMPRESSOR_MAXONOFF = 1U;
constexpr auto EAXAGCCOMPRESSOR_DEFAULTONOFF = ::EAXAGCCOMPRESSOR_MAXONOFF;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Autowah Effect

extern "C" const ::GUID EAX_AUTOWAH_EFFECT;

enum EAXAUTOWAH_PROPERTY :
	std::uint32_t
{
	EAXAUTOWAH_NONE,
	EAXAUTOWAH_ALLPARAMETERS,
	EAXAUTOWAH_ATTACKTIME,
	EAXAUTOWAH_RELEASETIME,
	EAXAUTOWAH_RESONANCE,
	EAXAUTOWAH_PEAKLEVEL,
}; // EAXAUTOWAH_PROPERTY

struct EAXAUTOWAHPROPERTIES
{
	float flAttackTime; // Attack time (seconds)
	float flReleaseTime; // Release time (seconds)
	std::int32_t lResonance; // Resonance (mB)
	std::int32_t lPeakLevel; // Peak level (mB)
}; // ::EAXAUTOWAHPROPERTIES


constexpr auto EAXAUTOWAH_MINATTACKTIME = 0.0001F;
constexpr auto EAXAUTOWAH_MAXATTACKTIME = 1.0F;
constexpr auto EAXAUTOWAH_DEFAULTATTACKTIME = 0.06F;

constexpr auto EAXAUTOWAH_MINRELEASETIME = 0.0001F;
constexpr auto EAXAUTOWAH_MAXRELEASETIME = 1.0F;
constexpr auto EAXAUTOWAH_DEFAULTRELEASETIME = 0.06F;

constexpr auto EAXAUTOWAH_MINRESONANCE = 600;
constexpr auto EAXAUTOWAH_MAXRESONANCE = 6000;
constexpr auto EAXAUTOWAH_DEFAULTRESONANCE = 6000;

constexpr auto EAXAUTOWAH_MINPEAKLEVEL = -9000;
constexpr auto EAXAUTOWAH_MAXPEAKLEVEL = 9000;
constexpr auto EAXAUTOWAH_DEFAULTPEAKLEVEL = 2100;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Chorus Effect

extern "C" const ::GUID EAX_CHORUS_EFFECT;


enum EAXCHORUS_PROPERTY :
	std::uint32_t
{
	EAXCHORUS_NONE,
	EAXCHORUS_ALLPARAMETERS,
	EAXCHORUS_WAVEFORM,
	EAXCHORUS_PHASE,
	EAXCHORUS_RATE,
	EAXCHORUS_DEPTH,
	EAXCHORUS_FEEDBACK,
	EAXCHORUS_DELAY,
}; // EAXCHORUS_PROPERTY

enum :
	std::uint32_t
{
	EAX_CHORUS_SINUSOID,
	EAX_CHORUS_TRIANGLE,
};

struct EAXCHORUSPROPERTIES
{
	std::uint32_t ulWaveform; // Waveform selector - see enum above
	std::int32_t lPhase; // Phase (Degrees)
	float flRate; // Rate (Hz)
	float flDepth; // Depth (0 to 1)
	float flFeedback; // Feedback (-1 to 1)
	float flDelay; // Delay (seconds)
}; // EAXCHORUSPROPERTIES


constexpr auto EAXCHORUS_MINWAVEFORM = 0U;
constexpr auto EAXCHORUS_MAXWAVEFORM = 1U;
constexpr auto EAXCHORUS_DEFAULTWAVEFORM = 1U;

constexpr auto EAXCHORUS_MINPHASE = -180;
constexpr auto EAXCHORUS_MAXPHASE = 180;
constexpr auto EAXCHORUS_DEFAULTPHASE = 90;

constexpr auto EAXCHORUS_MINRATE = 0.0F;
constexpr auto EAXCHORUS_MAXRATE = 10.0F;
constexpr auto EAXCHORUS_DEFAULTRATE = 1.1F;

constexpr auto EAXCHORUS_MINDEPTH = 0.0F;
constexpr auto EAXCHORUS_MAXDEPTH = 1.0F;
constexpr auto EAXCHORUS_DEFAULTDEPTH = 0.1F;

constexpr auto EAXCHORUS_MINFEEDBACK = -1.0F;
constexpr auto EAXCHORUS_MAXFEEDBACK = 1.0F;
constexpr auto EAXCHORUS_DEFAULTFEEDBACK = 0.25F;

constexpr auto EAXCHORUS_MINDELAY = 0.0002F;
constexpr auto EAXCHORUS_MAXDELAY = 0.016F;
constexpr auto EAXCHORUS_DEFAULTDELAY = 0.016F;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Distortion Effect

extern "C" const ::GUID EAX_DISTORTION_EFFECT;

enum EAXDISTORTION_PROPERTY :
	std::uint32_t
{
	EAXDISTORTION_NONE,
	EAXDISTORTION_ALLPARAMETERS,
	EAXDISTORTION_EDGE,
	EAXDISTORTION_GAIN,
	EAXDISTORTION_LOWPASSCUTOFF,
	EAXDISTORTION_EQCENTER,
	EAXDISTORTION_EQBANDWIDTH,
}; // EAXDISTORTION_PROPERTY


struct EAXDISTORTIONPROPERTIES
{
	float flEdge; // Controls the shape of the distortion (0 to 1)
	std::int32_t lGain; // Controls the post distortion gain (mB)
	float flLowPassCutOff; // Controls the cut-off of the filter pre-distortion (Hz)
	float flEQCenter; // Controls the center frequency of the EQ post-distortion (Hz)
	float flEQBandwidth; // Controls the bandwidth of the EQ post-distortion (Hz)
}; // EAXDISTORTIONPROPERTIES


constexpr auto EAXDISTORTION_MINEDGE = 0.0F;
constexpr auto EAXDISTORTION_MAXEDGE = 1.0F;
constexpr auto EAXDISTORTION_DEFAULTEDGE = 0.2F;

constexpr auto EAXDISTORTION_MINGAIN = -6000;
constexpr auto EAXDISTORTION_MAXGAIN = 0;
constexpr auto EAXDISTORTION_DEFAULTGAIN = -2600;

constexpr auto EAXDISTORTION_MINLOWPASSCUTOFF = 80.0F;
constexpr auto EAXDISTORTION_MAXLOWPASSCUTOFF = 24000.0F;
constexpr auto EAXDISTORTION_DEFAULTLOWPASSCUTOFF = 8000.0F;

constexpr auto EAXDISTORTION_MINEQCENTER = 80.0F;
constexpr auto EAXDISTORTION_MAXEQCENTER = 24000.0F;
constexpr auto EAXDISTORTION_DEFAULTEQCENTER = 3600.0F;

constexpr auto EAXDISTORTION_MINEQBANDWIDTH = 80.0F;
constexpr auto EAXDISTORTION_MAXEQBANDWIDTH = 24000.0F;
constexpr auto EAXDISTORTION_DEFAULTEQBANDWIDTH = 3600.0F;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Echo Effect

extern "C" const ::GUID EAX_ECHO_EFFECT;


enum EAXECHO_PROPERTY :
	std::uint32_t
{
	EAXECHO_NONE,
	EAXECHO_ALLPARAMETERS,
	EAXECHO_DELAY,
	EAXECHO_LRDELAY,
	EAXECHO_DAMPING,
	EAXECHO_FEEDBACK,
	EAXECHO_SPREAD,
}; // EAXECHO_PROPERTY


struct EAXECHOPROPERTIES
{
	float flDelay; // Controls the initial delay time (seconds)
	float flLRDelay; // Controls the delay time between the first and second taps (seconds)
	float flDamping; // Controls a low-pass filter that dampens the echoes (0 to 1)
	float flFeedback; // Controls the duration of echo repetition (0 to 1)
	float flSpread; // Controls the left-right spread of the echoes
}; // EAXECHOPROPERTIES


constexpr auto EAXECHO_MINDAMPING = 0.0F;
constexpr auto EAXECHO_MAXDAMPING = 0.99F;
constexpr auto EAXECHO_DEFAULTDAMPING = 0.5F;

constexpr auto EAXECHO_MINDELAY = 0.002F;
constexpr auto EAXECHO_MAXDELAY = 0.207F;
constexpr auto EAXECHO_DEFAULTDELAY = 0.1F;

constexpr auto EAXECHO_MINLRDELAY = 0.0F;
constexpr auto EAXECHO_MAXLRDELAY = 0.404F;
constexpr auto EAXECHO_DEFAULTLRDELAY = 0.1F;

constexpr auto EAXECHO_MINFEEDBACK = 0.0F;
constexpr auto EAXECHO_MAXFEEDBACK = 1.0F;
constexpr auto EAXECHO_DEFAULTFEEDBACK = 0.5F;

constexpr auto EAXECHO_MINSPREAD = -1.0F;
constexpr auto EAXECHO_MAXSPREAD = 1.0F;
constexpr auto EAXECHO_DEFAULTSPREAD = -1.0F;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Equalizer Effect

extern "C" const ::GUID EAX_EQUALIZER_EFFECT;


enum EAXEQUALIZER_PROPERTY :
	std::uint32_t
{
	EAXEQUALIZER_NONE,
	EAXEQUALIZER_ALLPARAMETERS,
	EAXEQUALIZER_LOWGAIN,
	EAXEQUALIZER_LOWCUTOFF,
	EAXEQUALIZER_MID1GAIN,
	EAXEQUALIZER_MID1CENTER,
	EAXEQUALIZER_MID1WIDTH,
	EAXEQUALIZER_MID2GAIN,
	EAXEQUALIZER_MID2CENTER,
	EAXEQUALIZER_MID2WIDTH,
	EAXEQUALIZER_HIGHGAIN,
	EAXEQUALIZER_HIGHCUTOFF,
}; // EAXEQUALIZER_PROPERTY


struct EAXEQUALIZERPROPERTIES
{
	std::int32_t lLowGain; // (mB)
	float flLowCutOff; // (Hz)
	std::int32_t lMid1Gain; // (mB)
	float flMid1Center; // (Hz)
	float flMid1Width; // (octaves)
	std::int32_t lMid2Gain; // (mB)
	float flMid2Center; // (Hz)
	float flMid2Width; // (octaves)
	std::int32_t lHighGain; // (mB)
	float flHighCutOff; // (Hz)
}; // EAXEQUALIZERPROPERTIES


constexpr auto EAXEQUALIZER_MINLOWGAIN = -1800;
constexpr auto EAXEQUALIZER_MAXLOWGAIN = 1800;
constexpr auto EAXEQUALIZER_DEFAULTLOWGAIN = 0;

constexpr auto EAXEQUALIZER_MINLOWCUTOFF = 50.0F;
constexpr auto EAXEQUALIZER_MAXLOWCUTOFF = 800.0F;
constexpr auto EAXEQUALIZER_DEFAULTLOWCUTOFF = 200.0F;

constexpr auto EAXEQUALIZER_MINMID1GAIN = -1800;
constexpr auto EAXEQUALIZER_MAXMID1GAIN = 1800;
constexpr auto EAXEQUALIZER_DEFAULTMID1GAIN = 0;

constexpr auto EAXEQUALIZER_MINMID1CENTER = 200.0F;
constexpr auto EAXEQUALIZER_MAXMID1CENTER = 3000.0F;
constexpr auto EAXEQUALIZER_DEFAULTMID1CENTER = 500.0F;

constexpr auto EAXEQUALIZER_MINMID1WIDTH = 0.01F;
constexpr auto EAXEQUALIZER_MAXMID1WIDTH = 1.0F;
constexpr auto EAXEQUALIZER_DEFAULTMID1WIDTH = 1.0F;

constexpr auto EAXEQUALIZER_MINMID2GAIN = -1800;
constexpr auto EAXEQUALIZER_MAXMID2GAIN = 1800;
constexpr auto EAXEQUALIZER_DEFAULTMID2GAIN = 0;

constexpr auto EAXEQUALIZER_MINMID2CENTER = 1000.0F;
constexpr auto EAXEQUALIZER_MAXMID2CENTER = 8000.0F;
constexpr auto EAXEQUALIZER_DEFAULTMID2CENTER = 3000.0F;

constexpr auto EAXEQUALIZER_MINMID2WIDTH = 0.01F;
constexpr auto EAXEQUALIZER_MAXMID2WIDTH = 1.0F;
constexpr auto EAXEQUALIZER_DEFAULTMID2WIDTH = 1.0F;

constexpr auto EAXEQUALIZER_MINHIGHGAIN = -1800;
constexpr auto EAXEQUALIZER_MAXHIGHGAIN = 1800;
constexpr auto EAXEQUALIZER_DEFAULTHIGHGAIN = 0;

constexpr auto EAXEQUALIZER_MINHIGHCUTOFF = 4000.0F;
constexpr auto EAXEQUALIZER_MAXHIGHCUTOFF = 16000.0F;
constexpr auto EAXEQUALIZER_DEFAULTHIGHCUTOFF = 6000.0F;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Flanger Effect

extern "C" const ::GUID EAX_FLANGER_EFFECT;

enum EAXFLANGER_PROPERTY :
	std::uint32_t
{
	EAXFLANGER_NONE,
	EAXFLANGER_ALLPARAMETERS,
	EAXFLANGER_WAVEFORM,
	EAXFLANGER_PHASE,
	EAXFLANGER_RATE,
	EAXFLANGER_DEPTH,
	EAXFLANGER_FEEDBACK,
	EAXFLANGER_DELAY,
}; // EAXFLANGER_PROPERTY

enum :
	std::uint32_t
{
	EAX_FLANGER_SINUSOID,
	EAX_FLANGER_TRIANGLE,
};

struct EAXFLANGERPROPERTIES
{
	std::uint32_t ulWaveform; // Waveform selector - see enum above
	std::int32_t lPhase; // Phase (Degrees)
	float flRate; // Rate (Hz)
	float flDepth; // Depth (0 to 1)
	float flFeedback; // Feedback (0 to 1)
	float flDelay; // Delay (seconds)
}; // EAXFLANGERPROPERTIES


constexpr auto EAXFLANGER_MINWAVEFORM = 0U;
constexpr auto EAXFLANGER_MAXWAVEFORM = 1U;
constexpr auto EAXFLANGER_DEFAULTWAVEFORM = 1U;

constexpr auto EAXFLANGER_MINPHASE = -180;
constexpr auto EAXFLANGER_MAXPHASE = 180;
constexpr auto EAXFLANGER_DEFAULTPHASE = 0;

constexpr auto EAXFLANGER_MINRATE = 0.0F;
constexpr auto EAXFLANGER_MAXRATE = 10.0F;
constexpr auto EAXFLANGER_DEFAULTRATE = 0.27F;

constexpr auto EAXFLANGER_MINDEPTH = 0.0F;
constexpr auto EAXFLANGER_MAXDEPTH = 1.0F;
constexpr auto EAXFLANGER_DEFAULTDEPTH = 1.0F;

constexpr auto EAXFLANGER_MINFEEDBACK = -1.0F;
constexpr auto EAXFLANGER_MAXFEEDBACK = 1.0F;
constexpr auto EAXFLANGER_DEFAULTFEEDBACK = -0.5F;

constexpr auto EAXFLANGER_MINDELAY = 0.0002F;
constexpr auto EAXFLANGER_MAXDELAY = 0.004F;
constexpr auto EAXFLANGER_DEFAULTDELAY = 0.002F;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Frequency Shifter Effect

extern "C" const ::GUID EAX_FREQUENCYSHIFTER_EFFECT;

enum EAXFREQUENCYSHIFTER_PROPERTY :
	std::uint32_t
{
	EAXFREQUENCYSHIFTER_NONE,
	EAXFREQUENCYSHIFTER_ALLPARAMETERS,
	EAXFREQUENCYSHIFTER_FREQUENCY,
	EAXFREQUENCYSHIFTER_LEFTDIRECTION,
	EAXFREQUENCYSHIFTER_RIGHTDIRECTION,
}; // EAXFREQUENCYSHIFTER_PROPERTY

enum :
	std::uint32_t
{
	EAX_FREQUENCYSHIFTER_DOWN,
	EAX_FREQUENCYSHIFTER_UP,
	EAX_FREQUENCYSHIFTER_OFF
};

struct EAXFREQUENCYSHIFTERPROPERTIES
{
	float flFrequency; // (Hz)
	std::uint32_t ulLeftDirection; // see enum above
	std::uint32_t ulRightDirection; // see enum above
}; // ::EAXFREQUENCYSHIFTERPROPERTIES


constexpr auto EAXFREQUENCYSHIFTER_MINFREQUENCY = 0.0F;
constexpr auto EAXFREQUENCYSHIFTER_MAXFREQUENCY = 24000.0F;
constexpr auto EAXFREQUENCYSHIFTER_DEFAULTFREQUENCY = ::EAXFREQUENCYSHIFTER_MINFREQUENCY;

constexpr auto EAXFREQUENCYSHIFTER_MINLEFTDIRECTION = 0U;
constexpr auto EAXFREQUENCYSHIFTER_MAXLEFTDIRECTION = 2U;
constexpr auto EAXFREQUENCYSHIFTER_DEFAULTLEFTDIRECTION = ::EAXFREQUENCYSHIFTER_MINLEFTDIRECTION;

constexpr auto EAXFREQUENCYSHIFTER_MINRIGHTDIRECTION = 0U;
constexpr auto EAXFREQUENCYSHIFTER_MAXRIGHTDIRECTION = 2U;
constexpr auto EAXFREQUENCYSHIFTER_DEFAULTRIGHTDIRECTION = ::EAXFREQUENCYSHIFTER_MINRIGHTDIRECTION;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Vocal Morpher Effect

extern "C" const ::GUID EAX_VOCALMORPHER_EFFECT;

enum EAXVOCALMORPHER_PROPERTY :
	std::uint32_t
{
	EAXVOCALMORPHER_NONE,
	EAXVOCALMORPHER_ALLPARAMETERS,
	EAXVOCALMORPHER_PHONEMEA,
	EAXVOCALMORPHER_PHONEMEACOARSETUNING,
	EAXVOCALMORPHER_PHONEMEB,
	EAXVOCALMORPHER_PHONEMEBCOARSETUNING,
	EAXVOCALMORPHER_WAVEFORM,
	EAXVOCALMORPHER_RATE,
}; // EAXVOCALMORPHER_PROPERTY

enum :
	std::uint32_t
{
	A,
	E,
	I,
	O,
	U,
	AA,
	AE,
	AH,
	AO,
	EH,
	ER,
	IH,
	IY,
	UH,
	UW,
	B,
	D,
	F,
	G,
	J,
	K,
	L,
	M,
	N,
	P,
	R,
	S,
	T,
	V,
	Z,
};

enum :
	std::uint32_t
{
	EAX_VOCALMORPHER_SINUSOID,
	EAX_VOCALMORPHER_TRIANGLE,
	EAX_VOCALMORPHER_SAWTOOTH
};

// Use this structure for EAXVOCALMORPHER_ALLPARAMETERS
struct EAXVOCALMORPHERPROPERTIES
{
	std::uint32_t ulPhonemeA; // see enum above
	std::int32_t lPhonemeACoarseTuning; // (semitones)
	std::uint32_t ulPhonemeB; // see enum above
	std::int32_t lPhonemeBCoarseTuning; // (semitones)
	std::uint32_t ulWaveform; // Waveform selector - see enum above
	float flRate; // (Hz)
}; // EAXVOCALMORPHERPROPERTIES


constexpr auto EAXVOCALMORPHER_MINPHONEMEA = 0U;
constexpr auto EAXVOCALMORPHER_MAXPHONEMEA = 29U;
constexpr auto EAXVOCALMORPHER_DEFAULTPHONEMEA = ::EAXVOCALMORPHER_MINPHONEMEA;

constexpr auto EAXVOCALMORPHER_MINPHONEMEACOARSETUNING = -24;
constexpr auto EAXVOCALMORPHER_MAXPHONEMEACOARSETUNING = 24;
constexpr auto EAXVOCALMORPHER_DEFAULTPHONEMEACOARSETUNING = 0;

constexpr auto EAXVOCALMORPHER_MINPHONEMEB = 0U;
constexpr auto EAXVOCALMORPHER_MAXPHONEMEB = 29U;
constexpr auto EAXVOCALMORPHER_DEFAULTPHONEMEB = 10U;

constexpr auto EAXVOCALMORPHER_MINPHONEMEBCOARSETUNING = -24;
constexpr auto EAXVOCALMORPHER_MAXPHONEMEBCOARSETUNING = 24;
constexpr auto EAXVOCALMORPHER_DEFAULTPHONEMEBCOARSETUNING = 0;

constexpr auto EAXVOCALMORPHER_MINWAVEFORM = 0U;
constexpr auto EAXVOCALMORPHER_MAXWAVEFORM = 2U;
constexpr auto EAXVOCALMORPHER_DEFAULTWAVEFORM = ::EAXVOCALMORPHER_MINWAVEFORM;

constexpr auto EAXVOCALMORPHER_MINRATE = 0.0F;
constexpr auto EAXVOCALMORPHER_MAXRATE = 10.0F;
constexpr auto EAXVOCALMORPHER_DEFAULTRATE = 1.41F;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Pitch Shifter Effect

extern "C" const ::GUID EAX_PITCHSHIFTER_EFFECT;

enum EAXPITCHSHIFTER_PROPERTY :
	std::uint32_t
{
	EAXPITCHSHIFTER_NONE,
	EAXPITCHSHIFTER_ALLPARAMETERS,
	EAXPITCHSHIFTER_COARSETUNE,
	EAXPITCHSHIFTER_FINETUNE,
}; // EAXPITCHSHIFTER_PROPERTY

struct EAXPITCHSHIFTERPROPERTIES
{
	std::int32_t lCoarseTune; // Amount of pitch shift (semitones)
	std::int32_t lFineTune; // Amount of pitch shift (cents)
}; // EAXPITCHSHIFTERPROPERTIES


constexpr auto EAXPITCHSHIFTER_MINCOARSETUNE = -12;
constexpr auto EAXPITCHSHIFTER_MAXCOARSETUNE = 12;
constexpr auto EAXPITCHSHIFTER_DEFAULTCOARSETUNE = 12;

constexpr auto EAXPITCHSHIFTER_MINFINETUNE = -50;
constexpr auto EAXPITCHSHIFTER_MAXFINETUNE = 50;
constexpr auto EAXPITCHSHIFTER_DEFAULTFINETUNE = 0;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Ring Modulator Effect

extern "C" const ::GUID EAX_RINGMODULATOR_EFFECT;

enum EAXRINGMODULATOR_PROPERTY
{
	EAXRINGMODULATOR_NONE,
	EAXRINGMODULATOR_ALLPARAMETERS,
	EAXRINGMODULATOR_FREQUENCY,
	EAXRINGMODULATOR_HIGHPASSCUTOFF,
	EAXRINGMODULATOR_WAVEFORM,
}; // EAXRINGMODULATOR_PROPERTY

enum :
	std::uint32_t
{
	EAX_RINGMODULATOR_SINUSOID,
	EAX_RINGMODULATOR_SAWTOOTH,
	EAX_RINGMODULATOR_SQUARE,
};

// Use this structure for EAXRINGMODULATOR_ALLPARAMETERS
struct EAXRINGMODULATORPROPERTIES
{
	float flFrequency; // Frequency of modulation (Hz)
	float flHighPassCutOff; // Cut-off frequency of high-pass filter (Hz)
	std::uint32_t ulWaveform; // Waveform selector - see enum above
}; // EAXRINGMODULATORPROPERTIES


constexpr auto EAXRINGMODULATOR_MINFREQUENCY = 0.0F;
constexpr auto EAXRINGMODULATOR_MAXFREQUENCY = 8000.0F;
constexpr auto EAXRINGMODULATOR_DEFAULTFREQUENCY = 440.0F;

constexpr auto EAXRINGMODULATOR_MINHIGHPASSCUTOFF = 0.0F;
constexpr auto EAXRINGMODULATOR_MAXHIGHPASSCUTOFF = 24000.0F;
constexpr auto EAXRINGMODULATOR_DEFAULTHIGHPASSCUTOFF = 800.0F;

constexpr auto EAXRINGMODULATOR_MINWAVEFORM = 0U;
constexpr auto EAXRINGMODULATOR_MAXWAVEFORM = 2U;
constexpr auto EAXRINGMODULATOR_DEFAULTWAVEFORM = ::EAXRINGMODULATOR_MINWAVEFORM;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


using EAXSet = ::ALenum (AL_APIENTRY *)(
	const ::GUID* property_set_guid,
	::ALuint property_id,
	::ALuint property_al_name,
	::ALvoid* property_buffer,
	::ALuint property_size);

using EAXGet = ::ALenum (AL_APIENTRY *)(
	const ::GUID* property_set_guid,
	::ALuint property_id,
	::ALuint property_al_name,
	::ALvoid* property_buffer,
	::ALuint property_size);


#endif // !EAXEFX_EAX_API_INCLUDED
