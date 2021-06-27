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


#ifndef EAXEFX_AL_SYMBOLS_INCLUDED
#define EAXEFX_AL_SYMBOLS_INCLUDED


#include <memory>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// ALC v1.1 symbols names
struct AlAlcSymbolsNames
{
	static constexpr auto alcCreateContext = "alcCreateContext";
	static constexpr auto alcMakeContextCurrent = "alcMakeContextCurrent";
	static constexpr auto alcProcessContext = "alcProcessContext";
	static constexpr auto alcSuspendContext = "alcSuspendContext";
	static constexpr auto alcDestroyContext = "alcDestroyContext";
	static constexpr auto alcGetCurrentContext = "alcGetCurrentContext";
	static constexpr auto alcGetContextsDevice = "alcGetContextsDevice";
	static constexpr auto alcOpenDevice = "alcOpenDevice";
	static constexpr auto alcCloseDevice = "alcCloseDevice";
	static constexpr auto alcGetError = "alcGetError";
	static constexpr auto alcIsExtensionPresent = "alcIsExtensionPresent";
	static constexpr auto alcGetProcAddress = "alcGetProcAddress";
	static constexpr auto alcGetEnumValue = "alcGetEnumValue";
	static constexpr auto alcGetString = "alcGetString";
	static constexpr auto alcGetIntegerv = "alcGetIntegerv";
	static constexpr auto alcCaptureOpenDevice = "alcCaptureOpenDevice";
	static constexpr auto alcCaptureCloseDevice = "alcCaptureCloseDevice";
	static constexpr auto alcCaptureStart = "alcCaptureStart";
	static constexpr auto alcCaptureStop = "alcCaptureStop";
	static constexpr auto alcCaptureSamples = "alcCaptureSamples";
}; // AlAlcSymbolsNames

// ALC v1.1 symbols
struct AlAlcSymbols
{
	::LPALCCREATECONTEXT alcCreateContext{};
	::LPALCMAKECONTEXTCURRENT alcMakeContextCurrent{};
	::LPALCPROCESSCONTEXT alcProcessContext{};
	::LPALCSUSPENDCONTEXT alcSuspendContext{};
	::LPALCDESTROYCONTEXT alcDestroyContext{};
	::LPALCGETCURRENTCONTEXT alcGetCurrentContext{};
	::LPALCGETCONTEXTSDEVICE alcGetContextsDevice{};
	::LPALCOPENDEVICE alcOpenDevice{};
	::LPALCCLOSEDEVICE alcCloseDevice{};
	::LPALCGETERROR alcGetError{};
	::LPALCISEXTENSIONPRESENT alcIsExtensionPresent{};
	::LPALCGETPROCADDRESS alcGetProcAddress{};
	::LPALCGETENUMVALUE alcGetEnumValue{};
	::LPALCGETSTRING alcGetString{};
	::LPALCGETINTEGERV alcGetIntegerv{};
	::LPALCCAPTUREOPENDEVICE alcCaptureOpenDevice{};
	::LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice{};
	::LPALCCAPTURESTART alcCaptureStart{};
	::LPALCCAPTURESTOP alcCaptureStop{};
	::LPALCCAPTURESAMPLES alcCaptureSamples{};
}; // AlAlcSymbols

using AlAlcSymbolsUPtr = std::unique_ptr<AlAlcSymbols>;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// AL v1.1 symbols names
struct AlAlSymbolsNames
{
	static constexpr auto alDopplerFactor = "alDopplerFactor";
	static constexpr auto alDopplerVelocity = "alDopplerVelocity";
	static constexpr auto alSpeedOfSound = "alSpeedOfSound";
	static constexpr auto alDistanceModel = "alDistanceModel";
	static constexpr auto alEnable = "alEnable";
	static constexpr auto alDisable = "alDisable";
	static constexpr auto alIsEnabled = "alIsEnabled";
	static constexpr auto alGetString = "alGetString";
	static constexpr auto alGetBooleanv = "alGetBooleanv";
	static constexpr auto alGetIntegerv = "alGetIntegerv";
	static constexpr auto alGetFloatv = "alGetFloatv";
	static constexpr auto alGetDoublev = "alGetDoublev";
	static constexpr auto alGetBoolean = "alGetBoolean";
	static constexpr auto alGetInteger = "alGetInteger";
	static constexpr auto alGetFloat = "alGetFloat";
	static constexpr auto alGetDouble = "alGetDouble";
	static constexpr auto alGetError = "alGetError";
	static constexpr auto alIsExtensionPresent = "alIsExtensionPresent";
	static constexpr auto alGetProcAddress = "alGetProcAddress";
	static constexpr auto alGetEnumValue = "alGetEnumValue";
	static constexpr auto alListenerf = "alListenerf";
	static constexpr auto alListener3f = "alListener3f";
	static constexpr auto alListenerfv = "alListenerfv";
	static constexpr auto alListeneri = "alListeneri";
	static constexpr auto alListener3i = "alListener3i";
	static constexpr auto alListeneriv = "alListeneriv";
	static constexpr auto alGetListenerf = "alGetListenerf";
	static constexpr auto alGetListener3f = "alGetListener3f";
	static constexpr auto alGetListenerfv = "alGetListenerfv";
	static constexpr auto alGetListeneri = "alGetListeneri";
	static constexpr auto alGetListener3i = "alGetListener3i";
	static constexpr auto alGetListeneriv = "alGetListeneriv";
	static constexpr auto alGenSources = "alGenSources";
	static constexpr auto alDeleteSources = "alDeleteSources";
	static constexpr auto alIsSource = "alIsSource";
	static constexpr auto alSourcef = "alSourcef";
	static constexpr auto alSource3f = "alSource3f";
	static constexpr auto alSourcefv = "alSourcefv";
	static constexpr auto alSourcei = "alSourcei";
	static constexpr auto alSource3i = "alSource3i";
	static constexpr auto alSourceiv = "alSourceiv";
	static constexpr auto alGetSourcef = "alGetSourcef";
	static constexpr auto alGetSource3f = "alGetSource3f";
	static constexpr auto alGetSourcefv = "alGetSourcefv";
	static constexpr auto alGetSourcei = "alGetSourcei";
	static constexpr auto alGetSource3i = "alGetSource3i";
	static constexpr auto alGetSourceiv = "alGetSourceiv";
	static constexpr auto alSourcePlayv = "alSourcePlayv";
	static constexpr auto alSourceStopv = "alSourceStopv";
	static constexpr auto alSourceRewindv = "alSourceRewindv";
	static constexpr auto alSourcePausev = "alSourcePausev";
	static constexpr auto alSourcePlay = "alSourcePlay";
	static constexpr auto alSourceStop = "alSourceStop";
	static constexpr auto alSourceRewind = "alSourceRewind";
	static constexpr auto alSourcePause = "alSourcePause";
	static constexpr auto alSourceQueueBuffers = "alSourceQueueBuffers";
	static constexpr auto alSourceUnqueueBuffers = "alSourceUnqueueBuffers";
	static constexpr auto alGenBuffers = "alGenBuffers";
	static constexpr auto alDeleteBuffers = "alDeleteBuffers";
	static constexpr auto alIsBuffer = "alIsBuffer";
	static constexpr auto alBufferData = "alBufferData";
	static constexpr auto alBufferf = "alBufferf";
	static constexpr auto alBuffer3f = "alBuffer3f";
	static constexpr auto alBufferfv = "alBufferfv";
	static constexpr auto alBufferi = "alBufferi";
	static constexpr auto alBuffer3i = "alBuffer3i";
	static constexpr auto alBufferiv = "alBufferiv";
	static constexpr auto alGetBufferf = "alGetBufferf";
	static constexpr auto alGetBuffer3f = "alGetBuffer3f";
	static constexpr auto alGetBufferfv = "alGetBufferfv";
	static constexpr auto alGetBufferi = "alGetBufferi";
	static constexpr auto alGetBuffer3i = "alGetBuffer3i";
	static constexpr auto alGetBufferiv = "alGetBufferiv";
}; // AlAlSymbolsNames

// AL v1.1 symbols
struct AlAlSymbols
{
	::LPALDOPPLERFACTOR alDopplerFactor{};
	::LPALDOPPLERVELOCITY alDopplerVelocity{};
	::LPALSPEEDOFSOUND alSpeedOfSound{};
	::LPALDISTANCEMODEL alDistanceModel{};
	::LPALENABLE alEnable{};
	::LPALDISABLE alDisable{};
	::LPALISENABLED alIsEnabled{};
	::LPALGETSTRING alGetString{};
	::LPALGETBOOLEANV alGetBooleanv{};
	::LPALGETINTEGERV alGetIntegerv{};
	::LPALGETFLOATV alGetFloatv{};
	::LPALGETDOUBLEV alGetDoublev{};
	::LPALGETBOOLEAN alGetBoolean{};
	::LPALGETINTEGER alGetInteger{};
	::LPALGETFLOAT alGetFloat{};
	::LPALGETDOUBLE alGetDouble{};
	::LPALGETERROR alGetError{};
	::LPALISEXTENSIONPRESENT alIsExtensionPresent{};
	::LPALGETPROCADDRESS alGetProcAddress{};
	::LPALGETENUMVALUE alGetEnumValue{};
	::LPALLISTENERF alListenerf{};
	::LPALLISTENER3F alListener3f{};
	::LPALLISTENERFV alListenerfv{};
	::LPALLISTENERI alListeneri{};
	::LPALLISTENER3I alListener3i{};
	::LPALLISTENERIV alListeneriv{};
	::LPALGETLISTENERF alGetListenerf{};
	::LPALGETLISTENER3F alGetListener3f{};
	::LPALGETLISTENERFV alGetListenerfv{};
	::LPALGETLISTENERI alGetListeneri{};
	::LPALGETLISTENER3I alGetListener3i{};
	::LPALGETLISTENERIV alGetListeneriv{};
	::LPALGENSOURCES alGenSources{};
	::LPALDELETESOURCES alDeleteSources{};
	::LPALISSOURCE alIsSource{};
	::LPALSOURCEF alSourcef{};
	::LPALSOURCE3F alSource3f{};
	::LPALSOURCEFV alSourcefv{};
	::LPALSOURCEI alSourcei{};
	::LPALSOURCE3I alSource3i{};
	::LPALSOURCEIV alSourceiv{};
	::LPALGETSOURCEF alGetSourcef{};
	::LPALGETSOURCE3F alGetSource3f{};
	::LPALGETSOURCEFV alGetSourcefv{};
	::LPALGETSOURCEI alGetSourcei{};
	::LPALGETSOURCE3I alGetSource3i{};
	::LPALGETSOURCEIV alGetSourceiv{};
	::LPALSOURCEPLAYV alSourcePlayv{};
	::LPALSOURCESTOPV alSourceStopv{};
	::LPALSOURCEREWINDV alSourceRewindv{};
	::LPALSOURCEPAUSEV alSourcePausev{};
	::LPALSOURCEPLAY alSourcePlay{};
	::LPALSOURCESTOP alSourceStop{};
	::LPALSOURCEREWIND alSourceRewind{};
	::LPALSOURCEPAUSE alSourcePause{};
	::LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers{};
	::LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers{};
	::LPALGENBUFFERS alGenBuffers{};
	::LPALDELETEBUFFERS alDeleteBuffers{};
	::LPALISBUFFER alIsBuffer{};
	::LPALBUFFERDATA alBufferData{};
	::LPALBUFFERF alBufferf{};
	::LPALBUFFER3F alBuffer3f{};
	::LPALBUFFERFV alBufferfv{};
	::LPALBUFFERI alBufferi{};
	::LPALBUFFER3I alBuffer3i{};
	::LPALBUFFERIV alBufferiv{};
	::LPALGETBUFFERF alGetBufferf{};
	::LPALGETBUFFER3F alGetBuffer3f{};
	::LPALGETBUFFERFV alGetBufferfv{};
	::LPALGETBUFFERI alGetBufferi{};
	::LPALGETBUFFER3I alGetBuffer3i{};
	::LPALGETBUFFERIV alGetBufferiv{};
}; // AlAlSymbols

using AlAlSymbolsUPtr = std::unique_ptr<AlAlSymbols>;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// EFX v1.0 symbols names
struct AlEfxSymbolsNames
{
	static constexpr auto alGenEffects = "alGenEffects";
	static constexpr auto alDeleteEffects = "alDeleteEffects";
	static constexpr auto alIsEffect = "alIsEffect";
	static constexpr auto alEffecti = "alEffecti";
	static constexpr auto alEffectiv = "alEffectiv";
	static constexpr auto alEffectf = "alEffectf";
	static constexpr auto alEffectfv = "alEffectfv";
	static constexpr auto alGetEffecti = "alGetEffecti";
	static constexpr auto alGetEffectiv = "alGetEffectiv";
	static constexpr auto alGetEffectf = "alGetEffectf";
	static constexpr auto alGetEffectfv = "alGetEffectfv";
	static constexpr auto alGenFilters = "alGenFilters";
	static constexpr auto alDeleteFilters = "alDeleteFilters";
	static constexpr auto alIsFilter = "alIsFilter";
	static constexpr auto alFilteri = "alFilteri";
	static constexpr auto alFilteriv = "alFilteriv";
	static constexpr auto alFilterf = "alFilterf";
	static constexpr auto alFilterfv = "alFilterfv";
	static constexpr auto alGetFilteri = "alGetFilteri";
	static constexpr auto alGetFilteriv = "alGetFilteriv";
	static constexpr auto alGetFilterf = "alGetFilterf";
	static constexpr auto alGetFilterfv = "alGetFilterfv";
	static constexpr auto alGenAuxiliaryEffectSlots = "alGenAuxiliaryEffectSlots";
	static constexpr auto alDeleteAuxiliaryEffectSlots = "alDeleteAuxiliaryEffectSlots";
	static constexpr auto alIsAuxiliaryEffectSlot = "alIsAuxiliaryEffectSlot";
	static constexpr auto alAuxiliaryEffectSloti = "alAuxiliaryEffectSloti";
	static constexpr auto alAuxiliaryEffectSlotiv = "alAuxiliaryEffectSlotiv";
	static constexpr auto alAuxiliaryEffectSlotf = "alAuxiliaryEffectSlotf";
	static constexpr auto alAuxiliaryEffectSlotfv = "alAuxiliaryEffectSlotfv";
	static constexpr auto alGetAuxiliaryEffectSloti = "alGetAuxiliaryEffectSloti";
	static constexpr auto alGetAuxiliaryEffectSlotiv = "alGetAuxiliaryEffectSlotiv";
	static constexpr auto alGetAuxiliaryEffectSlotf = "alGetAuxiliaryEffectSlotf";
	static constexpr auto alGetAuxiliaryEffectSlotfv = "alGetAuxiliaryEffectSlotfv";
}; // AlEfxSymbolsNames

// EFX v1.0 symbols
struct AlEfxSymbols
{
	::LPALGENEFFECTS alGenEffects{};
	::LPALDELETEEFFECTS alDeleteEffects{};
	::LPALISEFFECT alIsEffect{};
	::LPALEFFECTI alEffecti{};
	::LPALEFFECTIV alEffectiv{};
	::LPALEFFECTF alEffectf{};
	::LPALEFFECTFV alEffectfv{};
	::LPALGETEFFECTI alGetEffecti{};
	::LPALGETEFFECTIV alGetEffectiv{};
	::LPALGETEFFECTF alGetEffectf{};
	::LPALGETEFFECTFV alGetEffectfv{};
	::LPALGENFILTERS alGenFilters{};
	::LPALDELETEFILTERS alDeleteFilters{};
	::LPALISFILTER alIsFilter{};
	::LPALFILTERI alFilteri{};
	::LPALFILTERIV alFilteriv{};
	::LPALFILTERF alFilterf{};
	::LPALFILTERFV alFilterfv{};
	::LPALGETFILTERI alGetFilteri{};
	::LPALGETFILTERIV alGetFilteriv{};
	::LPALGETFILTERF alGetFilterf{};
	::LPALGETFILTERFV alGetFilterfv{};
	::LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots{};
	::LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots{};
	::LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot{};
	::LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti{};
	::LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv{};
	::LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf{};
	::LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv{};
	::LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti{};
	::LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv{};
	::LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf{};
	::LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv{};
}; // AlEfxSymbols

using AlEfxSymbolsUPtr = std::unique_ptr<AlEfxSymbols>;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_AL_SYMBOLS_INCLUDED
