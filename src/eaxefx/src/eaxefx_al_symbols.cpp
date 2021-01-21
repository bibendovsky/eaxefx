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


#include "eaxefx_al_symbols.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//
// AL 1.1
//

LPALDOPPLERFACTOR alDopplerFactor_{};
LPALDOPPLERVELOCITY alDopplerVelocity_{};
LPALSPEEDOFSOUND alSpeedOfSound_{};
LPALDISTANCEMODEL alDistanceModel_{};
LPALENABLE alEnable_{};
LPALDISABLE alDisable_{};
LPALISENABLED alIsEnabled_{};
LPALGETSTRING alGetString_{};
LPALGETBOOLEANV alGetBooleanv_{};
LPALGETINTEGERV alGetIntegerv_{};
LPALGETFLOATV alGetFloatv_{};
LPALGETDOUBLEV alGetDoublev_{};
LPALGETBOOLEAN alGetBoolean_{};
LPALGETINTEGER alGetInteger_{};
LPALGETFLOAT alGetFloat_{};
LPALGETDOUBLE alGetDouble_{};
LPALGETERROR alGetError_{};
LPALISEXTENSIONPRESENT alIsExtensionPresent_{};
LPALGETPROCADDRESS alGetProcAddress_{};
LPALGETENUMVALUE alGetEnumValue_{};
LPALLISTENERF alListenerf_{};
LPALLISTENER3F alListener3f_{};
LPALLISTENERFV alListenerfv_{};
LPALLISTENERI alListeneri_{};
LPALLISTENER3I alListener3i_{};
LPALLISTENERIV alListeneriv_{};
LPALGETLISTENERF alGetListenerf_{};
LPALGETLISTENER3F alGetListener3f_{};
LPALGETLISTENERFV alGetListenerfv_{};
LPALGETLISTENERI alGetListeneri_{};
LPALGETLISTENER3I alGetListener3i_{};
LPALGETLISTENERIV alGetListeneriv_{};
LPALGENSOURCES alGenSources_{};
LPALDELETESOURCES alDeleteSources_{};
LPALISSOURCE alIsSource_{};
LPALSOURCEF alSourcef_{};
LPALSOURCE3F alSource3f_{};
LPALSOURCEFV alSourcefv_{};
LPALSOURCEI alSourcei_{};
LPALSOURCE3I alSource3i_{};
LPALSOURCEIV alSourceiv_{};
LPALGETSOURCEF alGetSourcef_{};
LPALGETSOURCE3F alGetSource3f_{};
LPALGETSOURCEFV alGetSourcefv_{};
LPALGETSOURCEI alGetSourcei_{};
LPALGETSOURCE3I alGetSource3i_{};
LPALGETSOURCEIV alGetSourceiv_{};
LPALSOURCEPLAYV alSourcePlayv_{};
LPALSOURCESTOPV alSourceStopv_{};
LPALSOURCEREWINDV alSourceRewindv_{};
LPALSOURCEPAUSEV alSourcePausev_{};
LPALSOURCEPLAY alSourcePlay_{};
LPALSOURCESTOP alSourceStop_{};
LPALSOURCEREWIND alSourceRewind_{};
LPALSOURCEPAUSE alSourcePause_{};
LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers_{};
LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers_{};
LPALGENBUFFERS alGenBuffers_{};
LPALDELETEBUFFERS alDeleteBuffers_{};
LPALISBUFFER alIsBuffer_{};
LPALBUFFERDATA alBufferData_{};
LPALBUFFERF alBufferf_{};
LPALBUFFER3F alBuffer3f_{};
LPALBUFFERFV alBufferfv_{};
LPALBUFFERI alBufferi_{};
LPALBUFFER3I alBuffer3i_{};
LPALBUFFERIV alBufferiv_{};
LPALGETBUFFERF alGetBufferf_{};
LPALGETBUFFER3F alGetBuffer3f_{};
LPALGETBUFFERFV alGetBufferfv_{};
LPALGETBUFFERI alGetBufferi_{};
LPALGETBUFFER3I alGetBuffer3i_{};
LPALGETBUFFERIV alGetBufferiv_{};


//
// ALC 1.1
//

LPALCCREATECONTEXT alcCreateContext_{};
LPALCMAKECONTEXTCURRENT alcMakeContextCurrent_{};
LPALCPROCESSCONTEXT alcProcessContext_{};
LPALCSUSPENDCONTEXT alcSuspendContext_{};
LPALCDESTROYCONTEXT alcDestroyContext_{};
LPALCGETCURRENTCONTEXT alcGetCurrentContext_{};
LPALCGETCONTEXTSDEVICE alcGetContextsDevice_{};
LPALCOPENDEVICE alcOpenDevice_{};
LPALCCLOSEDEVICE alcCloseDevice_{};
LPALCGETERROR alcGetError_{};
LPALCISEXTENSIONPRESENT alcIsExtensionPresent_{};
LPALCGETPROCADDRESS alcGetProcAddress_{};
LPALCGETENUMVALUE alcGetEnumValue_{};
LPALCGETSTRING alcGetString_{};
LPALCGETINTEGERV alcGetIntegerv_{};
LPALCCAPTUREOPENDEVICE alcCaptureOpenDevice_{};
LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice_{};
LPALCCAPTURESTART alcCaptureStart_{};
LPALCCAPTURESTOP alcCaptureStop_{};
LPALCCAPTURESAMPLES alcCaptureSamples_{};


//
// EFX 1.0
//

LPALGENEFFECTS alGenEffects_{};
LPALDELETEEFFECTS alDeleteEffects_{};
LPALISEFFECT alIsEffect_{};
LPALEFFECTI alEffecti_{};
LPALEFFECTIV alEffectiv_{};
LPALEFFECTF alEffectf_{};
LPALEFFECTFV alEffectfv_{};
LPALGETEFFECTI alGetEffecti_{};
LPALGETEFFECTIV alGetEffectiv_{};
LPALGETEFFECTF alGetEffectf_{};
LPALGETEFFECTFV alGetEffectfv_{};
LPALGENFILTERS alGenFilters_{};
LPALDELETEFILTERS alDeleteFilters_{};
LPALISFILTER alIsFilter_{};
LPALFILTERI alFilteri_{};
LPALFILTERIV alFilteriv_{};
LPALFILTERF alFilterf_{};
LPALFILTERFV alFilterfv_{};
LPALGETFILTERI alGetFilteri_{};
LPALGETFILTERIV alGetFilteriv_{};
LPALGETFILTERF alGetFilterf_{};
LPALGETFILTERFV alGetFilterfv_{};
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots_{};
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots_{};
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot_{};
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti_{};
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv_{};
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf_{};
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv_{};
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti_{};
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv_{};
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf_{};
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv_{};

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
