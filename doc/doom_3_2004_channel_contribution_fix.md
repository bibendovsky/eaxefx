Doom 3 channel contribution fix
===============================

Fix for source port [DHEWM3](https://github.com/dhewm/dhewm3):
https://github.com/dhewm/dhewm3/commit/c8321ddd1ffa4ea3fea351a362ad091201348bd9

The main idea is to clamp the volume to 1.0,
and then to scale volume down by one third
in method `idSoundWorldLocal::AddChannelContribution` (file "snd_world.cpp").


Original blocks  
```
...
004f70e6 d8 4c 24 10                FMUL       dword ptr [ESP  + volume ]
...
004f70f9 d9 5c 24 14                FSTP       dword ptr [ESP  + volume ]

...
004f720c d9 44 24 10                FLD        dword ptr [ESP  + volume ]
004f7210 8b 8c 24 1c 81 00 00       MOV        this ,dword ptr [ESP  + chan ]
...
0074a794-0074a7ca 00 (55 bytes)
...
```


FIX
---
Clamp.
Put extra code at address `0074a794`.
```
...
004f70e6 d9 5c 24 38                FSTP       dword ptr [ESP  + 0x38 ]              ; Save "s_volume" gain.
...
004f70f9 90                         NOP                                              ; Don't multiply by "s_volume" here.
004f70fa 90                         NOP
004f70fb 90                         NOP
004f70fc 90                         NOP
...
____to_patch_3:
004f720c e9 83 35 25 00             JMP        ____patch_3
004f7211 90                         NOP
004f7212 90                         NOP
004f7213 90                         NOP
004f7214 90                         NOP
004f7215 90                         NOP
004f7216 90                         NOP
____from_patch_3:
...
____patch_3:
0074a794 d9 44 24 10                FLD        dword ptr [ESP  + 0x10 ]            ; volume -> ST1
0074a798 d9 e8                      FLD1                                           ; 1.0F -> ST0
0074a79a de d9                      FCOMPP
0074a79c df e0                      FNSTSW     AX
0074a79e f6 c4 01                   TEST       AH ,0x1
0074a7a1 74 04                      JZ         ____patch_3_keep_current            ; if (1.0F >= volume)
0074a7a3 d9 e8                      FLD1                                           ; volume = 1.0F;
0074a7a5 eb 04                      JMP        ____patch_3_exit_clamp
____patch_3_keep_current                        XREF[1]:     0074a7a1 (j)   
0074a7a7 d9 44 24 10                FLD        dword ptr [ESP  + 0x10 ]            ; volume -> ST0;
____patch_3_exit_clamp                          XREF[1]:     0074a7a5 (j)   
0074a7ab c7 44 24 10 ab aa aa 3e    MOV        dword ptr [ESP  + 0x10 ],0x3eaaaaab ; 0.(3)
0074a7b3 d8 4c 24 10                FMUL       dword ptr [ESP  + 0x10 ]            ; volume /= 3;
0074a7b7 d8 4c 24 38                FMUL       dword ptr [ESP  + 0x38 ]            ; volume *= <saved "s_volume" gain>;
0074a7bb d9 54 24 10                FST        dword ptr [ESP  + 0x10 ]            ; ST0 -> volume
0074a7bf 8b 8c 24 1c 81 00 00       MOV        ECX ,dword ptr [ESP  + 0x811c ]     ; saved code
0074a7c6 e9 4c ca da ff             JMP        ____from_patch_3
...
```
