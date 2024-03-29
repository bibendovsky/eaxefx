Doom 3 deleting attached AL buffer fix
======================================

Sometimes the engine tries to delete OpenAL buffer while it is attached to a source.
In that case a message appears in the console "idSoundCache: error unloading data from OpenAL hardware buffer".

The fix iterates over all allocated OpenAL sources in method `idSoundSample::PurgeSoundSample` (snd_cache.cpp) and stops it if the sample is:
1) in use
2) has a channel
3) attached AL buffer to the source is equal to the sample's one

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

				if (!openalSource.inUse || openalSource.chan == NULL)
				{
					continue;
				}

				ALint al_current_buffer = AL_NONE;
				alGetSourcei(openalSource.handle, AL_BUFFER, &al_current_buffer);

				if (openalBuffer != static_cast<ALuint>(al_current_buffer))
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
0074a82f 00
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
0074a7cc 8b 4e 48                 MOV        ECX ,dword ptr [ESI  + 0x48 ]
0074a7cf 85 c9                    TEST       ECX ,ECX
0074a7d1 74 52                    JZ         ____patch_1_exit_2            ; if (openalBuffer == AL_NONE)
0074a7d3 55                       PUSH       EBP
0074a7d4 bd 54 ca 6b 01           MOV        EBP ,0x16bca54                ; &soundSystemLocal.openalSourceCount
0074a7d9 8b 7d 00                 MOV        EDI ,dword ptr [EBP ]         ; soundSystemLocal.openalSourceCount
0074a7dc 83 c5 04                 ADD        EBP ,0x4                      ; &soundSystemLocal.openalSources[0]
0074a7df c1 e7 04                 SHL        EDI ,0x4                      ; soundSystemLocal.openalSourceCount * sizeof(openalSource_t)
0074a7e2 01 ef                    ADD        EDI ,EBP                      ; &soundSystemLocal.openalSources[soundSystemLocal.openalSourceCount]
0074a7e4 50                       PUSH       EAX                           ; ALint al_current_buffer;
____patch_1_loop_1:
0074a7e5 39 ef                    CMP        EDI ,EBP
0074a7e7 74 3a                    JZ         ____patch_1_exit_1            ; if (openalSource == &soundSystemLocal.openalSources[soundSystemLocal.openalSourceCount])
0074a7e9 80 7d 0c 00              CMP        byte ptr [EBP  + 0xc ],0x0
0074a7ed 74 2f                    JZ         ____patch_1_continue_1        ; if (!openalSource.inUse)
0074a7ef 8b 45 08                 MOV        EAX ,dword ptr [EBP  + 0x8 ]
0074a7f2 85 c0                    TEST       EAX ,EAX
0074a7f4 74 28                    JZ         ____patch_1_continue_1        ; if (openalSource.chan == NULL)
0074a7f6 31 c0                    XOR        EAX ,EAX
0074a7f8 89 04 24                 MOV        dword ptr [ESP ],EAX          ; al_current_buffer = AL_NONE;
0074a7fb 8b 45 00                 MOV        EAX ,dword ptr [EBP ]
0074a7fe 54                       PUSH       ESP                           ; &al_current_buffer
0074a7ff 68 09 10 00 00           PUSH       0x1009                        ; AL_BUFFER
0074a804 50                       PUSH       EAX                           ; openalSource.handle
0074a805 ff 15 10 a0 70 01        CALL       dword ptr [idalGetSourcei ]   ; alGetSourcei(openalSource.handle, AL_BUFFER, &al_current_buffer);
0074a80b 83 c4 0c                 ADD        ESP ,0xc
0074a80e 8b 46 48                 MOV        EAX ,dword ptr [ESI  + 0x48 ]
0074a811 2b 04 24                 SUB        EAX ,dword ptr [ESP ]
0074a814 75 08                    JNZ        ____patch_1_continue_1        ; if (openalBuffer != static_cast<ALuint>(al_current_buffer))
0074a816 8b 4d 08                 MOV        ECX ,dword ptr [EBP  + 0x8 ]
0074a819 e8 32 31 da ff           CALL       idSoundChannel::ALStop        ; openalSource.chan->ALStop();
____patch_1_continue_1:
0074a81e 83 c5 10                 ADD        EBP ,0x10                     ; openalSource += 1;
0074a821 eb c2                    JMP        ____patch_1_loop_1
____patch_1_exit_1:
0074a823 58                       POP        EAX
0074a824 5d                       POP        EBP
____patch_1_exit_2:
0074a825 ff 15 00 a0 70 01        CALL       dword ptr [idalGetError ]     ; alGetError();
0074a82b e9 2e ff d9 ff           JMP        ____from_patch_1
...
```
