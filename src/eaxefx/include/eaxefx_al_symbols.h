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


#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//
// AL 1.1
//

extern LPALDOPPLERFACTOR alDopplerFactor_;
extern LPALDOPPLERVELOCITY alDopplerVelocity_;
extern LPALSPEEDOFSOUND alSpeedOfSound_;
extern LPALDISTANCEMODEL alDistanceModel_;
extern LPALENABLE alEnable_;
extern LPALDISABLE alDisable_;
extern LPALISENABLED alIsEnabled_;
extern LPALGETSTRING alGetString_;
extern LPALGETBOOLEANV alGetBooleanv_;
extern LPALGETINTEGERV alGetIntegerv_;
extern LPALGETFLOATV alGetFloatv_;
extern LPALGETDOUBLEV alGetDoublev_;
extern LPALGETBOOLEAN alGetBoolean_;
extern LPALGETINTEGER alGetInteger_;
extern LPALGETFLOAT alGetFloat_;
extern LPALGETDOUBLE alGetDouble_;
extern LPALGETERROR alGetError_;
extern LPALISEXTENSIONPRESENT alIsExtensionPresent_;
extern LPALGETPROCADDRESS alGetProcAddress_;
extern LPALGETENUMVALUE alGetEnumValue_;
extern LPALLISTENERF alListenerf_;
extern LPALLISTENER3F alListener3f_;
extern LPALLISTENERFV alListenerfv_;
extern LPALLISTENERI alListeneri_;
extern LPALLISTENER3I alListener3i_;
extern LPALLISTENERIV alListeneriv_;
extern LPALGETLISTENERF alGetListenerf_;
extern LPALGETLISTENER3F alGetListener3f_;
extern LPALGETLISTENERFV alGetListenerfv_;
extern LPALGETLISTENERI alGetListeneri_;
extern LPALGETLISTENER3I alGetListener3i_;
extern LPALGETLISTENERIV alGetListeneriv_;
extern LPALGENSOURCES alGenSources_;
extern LPALDELETESOURCES alDeleteSources_;
extern LPALISSOURCE alIsSource_;
extern LPALSOURCEF alSourcef_;
extern LPALSOURCE3F alSource3f_;
extern LPALSOURCEFV alSourcefv_;
extern LPALSOURCEI alSourcei_;
extern LPALSOURCE3I alSource3i_;
extern LPALSOURCEIV alSourceiv_;
extern LPALGETSOURCEF alGetSourcef_;
extern LPALGETSOURCE3F alGetSource3f_;
extern LPALGETSOURCEFV alGetSourcefv_;
extern LPALGETSOURCEI alGetSourcei_;
extern LPALGETSOURCE3I alGetSource3i_;
extern LPALGETSOURCEIV alGetSourceiv_;
extern LPALSOURCEPLAYV alSourcePlayv_;
extern LPALSOURCESTOPV alSourceStopv_;
extern LPALSOURCEREWINDV alSourceRewindv_;
extern LPALSOURCEPAUSEV alSourcePausev_;
extern LPALSOURCEPLAY alSourcePlay_;
extern LPALSOURCESTOP alSourceStop_;
extern LPALSOURCEREWIND alSourceRewind_;
extern LPALSOURCEPAUSE alSourcePause_;
extern LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers_;
extern LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers_;
extern LPALGENBUFFERS alGenBuffers_;
extern LPALDELETEBUFFERS alDeleteBuffers_;
extern LPALISBUFFER alIsBuffer_;
extern LPALBUFFERDATA alBufferData_;
extern LPALBUFFERF alBufferf_;
extern LPALBUFFER3F alBuffer3f_;
extern LPALBUFFERFV alBufferfv_;
extern LPALBUFFERI alBufferi_;
extern LPALBUFFER3I alBuffer3i_;
extern LPALBUFFERIV alBufferiv_;
extern LPALGETBUFFERF alGetBufferf_;
extern LPALGETBUFFER3F alGetBuffer3f_;
extern LPALGETBUFFERFV alGetBufferfv_;
extern LPALGETBUFFERI alGetBufferi_;
extern LPALGETBUFFER3I alGetBuffer3i_;
extern LPALGETBUFFERIV alGetBufferiv_;


//
// ALC 1.1
//

extern LPALCCREATECONTEXT alcCreateContext_;
extern LPALCMAKECONTEXTCURRENT alcMakeContextCurrent_;
extern LPALCPROCESSCONTEXT alcProcessContext_;
extern LPALCSUSPENDCONTEXT alcSuspendContext_;
extern LPALCDESTROYCONTEXT alcDestroyContext_;
extern LPALCGETCURRENTCONTEXT alcGetCurrentContext_;
extern LPALCGETCONTEXTSDEVICE alcGetContextsDevice_;
extern LPALCOPENDEVICE alcOpenDevice_;
extern LPALCCLOSEDEVICE alcCloseDevice_;
extern LPALCGETERROR alcGetError_;
extern LPALCISEXTENSIONPRESENT alcIsExtensionPresent_;
extern LPALCGETPROCADDRESS alcGetProcAddress_;
extern LPALCGETENUMVALUE alcGetEnumValue_;
extern LPALCGETSTRING alcGetString_;
extern LPALCGETINTEGERV alcGetIntegerv_;
extern LPALCCAPTUREOPENDEVICE alcCaptureOpenDevice_;
extern LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice_;
extern LPALCCAPTURESTART alcCaptureStart_;
extern LPALCCAPTURESTOP alcCaptureStop_;
extern LPALCCAPTURESAMPLES alcCaptureSamples_;


//
// EFX 1.0
//

extern LPALGENEFFECTS alGenEffects_;
extern LPALDELETEEFFECTS alDeleteEffects_;
extern LPALISEFFECT alIsEffect_;
extern LPALEFFECTI alEffecti_;
extern LPALEFFECTIV alEffectiv_;
extern LPALEFFECTF alEffectf_;
extern LPALEFFECTFV alEffectfv_;
extern LPALGETEFFECTI alGetEffecti_;
extern LPALGETEFFECTIV alGetEffectiv_;
extern LPALGETEFFECTF alGetEffectf_;
extern LPALGETEFFECTFV alGetEffectfv_;
extern LPALGENFILTERS alGenFilters_;
extern LPALDELETEFILTERS alDeleteFilters_;
extern LPALISFILTER alIsFilter_;
extern LPALFILTERI alFilteri_;
extern LPALFILTERIV alFilteriv_;
extern LPALFILTERF alFilterf_;
extern LPALFILTERFV alFilterfv_;
extern LPALGETFILTERI alGetFilteri_;
extern LPALGETFILTERIV alGetFilteriv_;
extern LPALGETFILTERF alGetFilterf_;
extern LPALGETFILTERFV alGetFilterfv_;
extern LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots_;
extern LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots_;
extern LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot_;
extern LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti_;
extern LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv_;
extern LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf_;
extern LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv_;
extern LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti_;
extern LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv_;
extern LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf_;
extern LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv_;

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_AL_SYMBOLS_INCLUDED
