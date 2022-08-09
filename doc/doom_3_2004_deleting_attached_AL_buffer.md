Doom 3 deleting attached AL buffer fix
======================================

Sometimes the engine tries to delete OpenAL buffer while it is attached to a source.
In that case a message appears in the console "idSoundCache: error unloading data from OpenAL hardware buffer".

The fix iterates over all allocated OpenAL sources in method `idSoundSample::PurgeSoundSample` (snd_cache.cpp) and stops it if the sample is:
1) in use
2) has a channel
3) the channel has a leading sample
4) the leading sample's OpenAL buffer is not null
5) the leading sample's OpenAL buffer is equal to the current one

Fixed source code fragment:
```
	...
	if ( hardwareBuffer && idSoundSystemLocal::useOpenAL ) {
// <<< FIX BEGIN
		if (openalBuffer != AL_NONE)
		{
			for (ALsizei i = 0; i < soundSystemLocal.openalSourceCount; ++i)
			{
				openalSource_t& openalSource = soundSystemLocal.openalSources[i];

				if (!openalSource.inUse ||
					openalSource.chan == NULL ||
					openalSource.chan->leadinSample == NULL ||
					openalSource.chan->leadinSample->openalBuffer == AL_NONE ||
					openalSource.chan->leadinSample->openalBuffer != openalBuffer)
				{
					continue;
				}

				openalSource.chan->ALStop();
			}
		}
// >>> FIX END
		alGetError();
		...
	}
```

Original blocks:
```
...
004ea758 ff 15 00 a0 70 01        CALL       dword ptr [idalGetError]
...
0074a7cc 00
...      00
0074a820 00
...
```

Modified blocks:
```
...
004ea758 e9 6f 00 26 00           JMP        ____patch_1
004ea75d 90                       NOP
____from_patch_1:
...
____patch_1:
0074a7cc 8b 4e 48                 MOV        ECX,dword ptr [ESI + 0x48]
0074a7cf 85 c9                    TEST       ECX,ECX
0074a7d1 74 43                    JZ         ____patch_1_exit_2         ; if (openalBuffer == AL_NONE)
0074a7d3 55                       PUSH       EBP
0074a7d4 bd 54 ca 6b 01           MOV        EBP,0x16bca54              ; &soundSystemLocal.openalSourceCount
0074a7d9 8b 7d 00                 MOV        EDI,dword ptr [EBP]        ; soundSystemLocal.openalSourceCount
0074a7dc 83 c5 04                 ADD        EBP,0x4                    ; &soundSystemLocal.openalSources[0]
0074a7df c1 e7 04                 SHL        EDI,0x4                    ; soundSystemLocal.openalSourceCount * sizeof(openalSource_t)
0074a7e2 01 ef                    ADD        EDI,EBP                    ; &soundSystemLocal.openalSources[soundSystemLocal.openalSourceCount]
____patch_1_loop_1:
0074a7e4 39 ef                    CMP        EDI,EBP
0074a7e6 74 2d                    JZ         ____patch_1_exit_1         ; if (openalSource == &soundSystemLocal.openalSources[soundSystemLocal.openalSourceCount])
0074a7e8 80 7d 0c 00              CMP        byte ptr [EBP + 0xc],0x0
0074a7ec 74 22                    JZ         ____patch_1_continue_1     ; if (!openalSource.inUse)
0074a7ee 8b 45 08                 MOV        EAX,dword ptr [EBP + 0x8]
0074a7f1 85 c0                    TEST       EAX,EAX
0074a7f3 74 1b                    JZ         ____patch_1_continue_1     ; if (openalSource.chan == NULL)
0074a7f5 8b 40 24                 MOV        EAX,dword ptr [EAX + 0x24]
0074a7f8 85 c0                    TEST       EAX,EAX
0074a7fa 74 14                    JZ         ____patch_1_continue_1     ; if (openalSource.chan->leadinSample == NULL)
0074a7fc 8b 40 48                 MOV        EAX,dword ptr [EAX + 0x48]
0074a7ff 85 c0                    TEST       EAX,EAX
0074a801 74 0d                    JZ         ____patch_1_continue_1     ; if (openalSource.chan->leadinSample->openalBuffer == AL_NONE)
0074a803 2b 46 48                 SUB        EAX,dword ptr [ESI + 0x48]
0074a806 75 08                    JNZ        ____patch_1_continue_1     ; if (openalSource.chan->leadinSample->openalBuffer != openalBuffer)
0074a808 8b 4d 08                 MOV        ECX,dword ptr [EBP + 0x8]
0074a80b e8 40 31 da ff           CALL       idSoundChannel::ALStop
____patch_1_continue_1:
0074a810 83 c5 10                 ADD        EBP,0x10                   ; openalSource += 1
0074a813 eb cf                    JMP        ____patch_1_loop_1
____patch_1_exit_1:
0074a815 5d                       POP        EBP
____patch_1_exit_2:
0074a816 ff 15 00 a0 70 01        CALL       dword ptr [idalGetError]
0074a81c e9 3d ff d9 ff           JMP        ____from_patch_1
...
```
