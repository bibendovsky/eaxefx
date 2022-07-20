Doom 3 reverb properties initialization fix
===========================================

The engine allocates an _uninitialized_ block of memory for structure `EAXREVERBPROPERTIES` inside `idEFXFile::ReadEffect` (file "snd_efxfile.cpp").

EAX will reject the call with `EAXREVERB_ALLPARAMETERS` if any value is out of range.

For example, the official add-on "Resurrection Of Evil" does not provide a value for parameter "air absorption hf" in reverb files (*.efx).

The fix sets all default parameters for reverb.

Original blocks:
```
...
004ecf7f 8d 54 24 10                 LEA        EDX => local_b0, [ESP + 0x10]
004ecf83 52                          PUSH       EDX
...

0074a7ae 00
...      00
0074a7cb 00
...
007ce558 00
...      00
007ce5c7 00
...
```

Modified blocks:
```
...
____to_patch_1:
004ecf7f e9 2a d8 25 00              JMP        ____patch_1
...
____patch_1:
; Preserve registers
0074a7ae 56                          PUSH       ESI
0074a7af 57                          PUSH       EDI
0074a7b0 51                          PUSH       ECX
; Copy default reverb properties.
0074a7b1 be 58 e5 7c 00              MOV        ESI, 0x7ce558
0074a7b6 89 ef                       MOV        EDI, EBP
; 28 (0x1c) properties.
0074a7b8 b9 1c 00 00 00              MOV        ECX, 0x1c
0074a7bd f3 a5                       MOVSD.REP  ES:EDI, ESI
; Restore registers.
0074a7bf 59                          POP        ECX
0074a7c0 5f                          POP        EDI
0074a7c1 5e                          POP        ESI
; Execute preserved code fragment.
0074a7c2 8d 54 24 10                 LEA        EDX, [ESP + 0x10]
0074a7c6 52                          PUSH       EDX
; Go back.
0074a7c7 e9 b8 27 da ff              JMP        idEFXFile::ReadEffect::____from_patch_1
...
____dat_default_reverb:
007ce558 00 00 00 00                 ulong      0       ; ulEnvironment
007ce55c 00 00 f0 40                 float      7.5     ; flEnvironmentSize
007ce560 00 00 80 3f                 float      1.0     ; flEnvironmentDiffusion
007ce564 18 fc ff ff                 long       -1000   ; lRoom
007ce568 9c ff ff ff                 long       -100    ; lRoomHF
007ce56c 00 00 00 00                 long       0h      ; lRoomLF
007ce570 52 b8 be 3f                 float      1.49    ; flDecayTime
007ce574 e1 7a 54 3f                 float      0.83    ; flDecayHFRatio
007ce578 00 00 80 3f                 float      1.0     ; flDecayLFRatio
007ce57c d6 f5 ff ff                 long       -2602   ; lReflections
007ce580 42 60 e5 3b                 float      0.007   ; flReflectionsDelay
007ce584 00 00 00 00                 float      0.0     ; vReflectionsPan[0]
007ce588 00 00 00 00                 float      0.0     ; vReflectionsPan[1]
007ce58c 00 00 00 00                 float      0.0     ; vReflectionsPan[2]
007ce590 c8 00 00 00                 long       C8h     ; lReverb
007ce594 58 39 34 3c                 float      0.011   ; flReverbDelay
007ce598 00 00 00 00                 float      0.0     ; vReverbPan[0]
007ce59c 00 00 00 00                 float      0.0     ; vReverbPan[1]
007ce5a0 00 00 00 00                 float      0.0     ; vReverbPan[2]
007ce5a4 00 00 80 3e                 float      0.25    ; flEchoTime
007ce5a8 00 00 00 00                 float      0.0     ; flEchoDepth
007ce5ac 00 00 80 3e                 float      0.25    ; flModulationTime
007ce5b0 00 00 00 00                 float      0.0     ; flModulationDepth
007ce5b4 00 00 a0 c0                 float      -5.0    ; flAirAbsorptionHF
007ce5b8 00 40 9c 45                 float      5000.0  ; flHFReference
007ce5bc 00 00 7a 43                 float      250.0   ; flLFReference
007ce5c0 00 00 00 00                 float      0.0     ; flRoomRolloffFactor
007ce5c4 3f 00 00 00                 ulong      3Fh     ; ulFlags
...
```
