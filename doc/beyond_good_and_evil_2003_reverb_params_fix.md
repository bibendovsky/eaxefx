Beyond Good & Evil (2003) reberb parameters fix
===============================================

Problem:
The game may pass EAXLISTENERPROPERTIES structure with some invalid parameters.
- Environment Diffusion: 85.0F or 100.0F
- Decay LF Ratio: 0.0F

Solution:
Adjust invalid values.
- Environment Diffusion: 85.0F => 0.85F; 100.0F => 1.0F
- Decay LF Ratio: 0.0F => 1.0F (default)


GOG
===

Unpatched block #1
------------------
```
0041fe75 ff 51 10                 CALL   dword ptr [ECX + 0x10]
0041fe78 8b 46 04                 MOV    EAX, dword ptr [ESI + 0x4]
```

Unpatched block #2
------------------
```
00b36c9f 00
...      00
00b36cdb 00
```

Patched block #1
----------------
```
____to_patch_1:
0041fe75 e9 25 6e 71 00           JMP    ____patch_1
0041fe7a 90                       NOP
____from_patch_1:
```

Patched block #2
----------------
```
____patch_1:
00b36c9f 8b 42 08                 MOV    EAX, dword ptr [EDX + 0x8]    ; EAX <= flEnvironmentDiffusion
00b36ca2 3d 00 00 aa 42           CMP    EAX, 0x42aa0000               ; 85.0F
00b36ca7 74 09                    JZ     ____patch_1_envdiff_85
00b36ca9 3d 00 00 c8 42           CMP    EAX, 0x42c80000               ; 100.0F
00b36cae 74 09                    JZ     ____patch_1_envdiff_100
00b36cb0 eb 0c                    JMP    ____patch_1_envdiff_update
____patch_1_envdiff_85:
00b36cb2 b8 9a 99 59 3f           MOV    EAX, 0x3f59999a               ; 0.85F
00b36cb7 eb 05                    JMP    ____patch_1_envdiff_update
____patch_1_envdiff_100:
00b36cb9 b8 00 00 80 3f           MOV    EAX, 0x3f800000               ; 1.0F
____patch_1_envdiff_update:
00b36cbe 89 42 08                 MOV    dword ptr [EDX + 0x8], EAX    ; flEnvironmentDiffusion <= EAX
00b36cc1 8b 42 20                 MOV    EAX, dword ptr [EDX + 0x20]   ; EAX <= flDecayLFRatio
00b36cc4 83 f8 00                 CMP    EAX, 0x0
00b36cc7 75 05                    JNZ    ____patch_1_decaylfr_update
00b36cc9 b8 00 00 80 3f           MOV    EAX, 0x3f800000               ; 1.0F
____patch_1_decaylfr_update:
00b36cce 89 42 20                 MOV    dword ptr [EDX + 0x20], EAX   ; flDecayLFRatio <= EAX
____patch_1_exec_original_code:
00b36cd1 ff 51 10                 CALL   dword ptr [ECX + 0x10]
00b36cd4 8b 46 04                 MOV    EAX, dword ptr [ESI + 0x4]
00b36cd7 e9 9f 91 8e ff           JMP    ____from_patch_1
```

Ubisoft Connect
===============

Unpatched block #1
------------------
```
004455d1 ff 51 10                 CALL   dword ptr [ECX + 0x10]
004455d4 8b 46 04                 MOV    EAX, dword ptr [ESI + 0x4]
```

Unpatched block #2
------------------
```
00b69787 00
...      00
00b697c3 00
```

Patched block #1
----------------
```
____to_patch_1:
004455d1 e9 b1 41 72 00           JMP    ____patch_1
004455d6 90                       NOP
____from_patch_1:
```

Patched block #2
----------------
```
____patch_1:
00b69787 8b 42 08                 MOV    EAX, dword ptr [EDX + 0x8]    ; EAX <= flEnvironmentDiffusion
00b6978a 3d 00 00 aa 42           CMP    EAX, 0x42aa0000               ; 85.0F
00b6978f 74 09                    JZ     ____patch_1_envdiff_85
00b69791 3d 00 00 c8 42           CMP    EAX, 0x42c80000               ; 100.0F
00b69796 74 09                    JZ     ____patch_1_envdiff_100
00b69798 eb 0c                    JMP    ____patch_1_envdiff_update
____patch_1_envdiff_85:
00b6979a b8 9a 99 59 3f           MOV    EAX, 0x3f59999a               ; 0.85F
00b6979f eb 05                    JMP    ____patch_1_envdiff_update
____patch_1_envdiff_100:
00b697a1 b8 00 00 80 3f           MOV    EAX, 0x3f800000               ; 1.0F
____patch_1_envdiff_update:
00b697a6 89 42 08                 MOV    dword ptr [EDX + 0x8], EAX    ; flEnvironmentDiffusion <= EAX
00b697a9 8b 42 20                 MOV    EAX, dword ptr [EDX + 0x20]   ; EAX <= flDecayLFRatio
00b697ac 83 f8 00                 CMP    EAX, 0x0
00b697af 75 05                    JNZ    ____patch_1_decaylfr_update
00b697b1 b8 00 00 80 3f           MOV    EAX, 0x3f800000               ; 1.0F
____patch_1_decaylfr_update:
00b697b6 89 42 20                 MOV    dword ptr [EDX + 0x20], EAX   ; flDecayLFRatio <= EAX
____patch_1_exec_original_code:
00b697b9 ff 51 10                 CALL   dword ptr [ECX + 0x10]
00b697bc 8b 46 04                 MOV    EAX, dword ptr [ESI + 0x4]
00b697bf e9 13 be 8d ff           JMP    ____from_patch_1
```


Retail
======

Unpatched block #1
------------------
```
00420505 ff 51 10                 CALL   dword ptr [ECX + 0x10]
00420508 8b 46 04                 MOV    EAX, dword ptr [ESI + 0x4]
```

Unpatched block #2
------------------
```
00b384cf 00
...      00
00b3850b 00
```

Patched block #1
----------------
```
____to_patch_1:
00420505 e9 c5 7f 71 00           JMP    ____patch_1
0042050a 90                       NOP
____from_patch_1:
```

Patched block #2
----------------
```
____patch_1:
00b384cf 8b 42 08                 MOV    EAX, dword ptr [EDX + 0x8]    ; EAX <= flEnvironmentDiffusion
00b384d2 3d 00 00 aa 42           CMP    EAX, 0x42aa0000               ; 85.0F
00b384d7 74 09                    JZ     ____patch_1_envdiff_85
00b384d9 3d 00 00 c8 42           CMP    EAX, 0x42c80000               ; 100.0F
00b384de 74 09                    JZ     ____patch_1_envdiff_100
00b384e0 eb 0c                    JMP    ____patch_1_envdiff_update
____patch_1_envdiff_85:
00b384e2 b8 9a 99 59 3f           MOV    EAX, 0x3f59999a               ; 0.85F
00b384e7 eb 05                    JMP    ____patch_1_envdiff_update
____patch_1_envdiff_100:
00b384e9 b8 00 00 80 3f           MOV    EAX, 0x3f800000               ; 1.0F
____patch_1_envdiff_update:
00b384ee 89 42 08                 MOV    dword ptr [EDX + 0x8], EAX    ; flEnvironmentDiffusion <= EAX
00b384f1 8b 42 20                 MOV    EAX, dword ptr [EDX + 0x20]   ; EAX <= flDecayLFRatio
00b384f4 83 f8 00                 CMP    EAX, 0x0
00b384f7 75 05                    JNZ    ____patch_1_decaylfr_update
00b384f9 b8 00 00 80 3f           MOV    EAX, 0x3f800000               ; 1.0F
____patch_1_decaylfr_update:
00b384fe 89 42 20                 MOV    dword ptr [EDX + 0x20], EAX   ; flDecayLFRatio <= EAX
____patch_1_exec_original_code:
00b38501 ff 51 10                 CALL   dword ptr [ECX + 0x10]
00b38504 8b 46 04                 MOV    EAX, dword ptr [ESI + 0x4]
00b38507 e9 9f 91 8e ff           JMP    ____from_patch_1
```
