Doom 3 channel contribution fix
===============================

Fix for source port [DHEWM3](https://github.com/dhewm/dhewm3):
https://github.com/dhewm/dhewm3/commit/3c01757d27a22a3858737f6b51e85895d2fac887

The idea is to scale volume down in method `idSoundWorldLocal::AddChannelContribution` (file "snd_world.cpp").


Original block  
```
...
004f7155 84 db                      TEST       BL, BL
004f7157 8b 8c 24 18 81 00 00       MOV        this, dword ptr [ESP + sound]
004f715e d9 5c 24 10                FSTP       dword ptr [ESP + volume]
...
0074a794-0074a7ad 00 (26 bytes)
...
```


FIX
---
Put one-third into local variable `volume` and multiply it by ST(0).
Put extra code at address `0074a794`.
```
...
____to_patch_1:
004f7155 e9 3a 36 25 00             JMP        ___patch_1
004f715a 90
004f715b 90
004f715c 90
004f715d 90
____from_patch_1:
004f715e d9 5c 24 10                FSTP       dword ptr [ESP + volume]
...
____patch_1:
; Set `volume` to one-third.
0074a794 c7 44 24 10 ab aa aa 3e    MOV        dword ptr [ESP + 0x10], 0x3eaaaaab
; Multiplicate it.
0074a79c d8 4c 24 10                FMUL       dword ptr [ESP + 0x10]
; Execute the preserved fragment of code.
0074a7a0 84 db                      TEST       BL, BL
0074a7a2 8b 8c 24 18 81 00 00       MOV        ECX, dword ptr [ESP + 0x8118]
; Go back.
0074a7a9 e9 b0 c9 da ff             JMP        ____from_patch_1
...
```
