# cp1251-fold-first-letter

**Symptom.** A whole ~40-instruction block of retail code with no counterpart in
ours: three range compares against `0x41/0x5a`, `0xc0/0xdf` and `0xa8`, each arm
storing one byte into a frame slot, then that byte copied back into a string
buffer. Ours has a single `add byte ptr [buf], 0x20`.

This is Buka's localisation folding the leading letter of a resource /
creature name to lower case in **CP1251**, where the Cyrillic capitals are
`0xc0..0xdf` (`+0x20` to lower-case) and `Ё` `0xa8` maps to `ё` `0xb8`.

    retail   DoEvent+0x2b70                       ours   sphinxAnswer_a[0] += ' ';
    -----------------------------------------------------------------------------
    2b70  mov  ecx, [ebp-0x64]                    2b55  mov al, [ebp-0x40]
    2b73  and  ecx, 0xff                          2b58  add al, 0x20
    2b79  cmp  ecx, 0x41                          2b5a  mov [ebp-0x40], al
    2b7c  jl   0x2b9f
    2b7e  ... cmp edx, 0x5a ; jg 0x2b9f
    2b8c  ... add eax, 0x20 ; mov [ebp-0x320], al
    2b9d  jmp  0x2bf7
    2b9f  ... cmp ecx, 0xc0 ; jl 0x2bd4
    2bb0  ... cmp edx, 0xdf ; jg 0x2bd4
    2bc1  ... add eax, 0x20 ; mov [ebp-0x320], al
    2bd2  jmp  0x2bf7
    2bd4  ... cmp ecx, 0xa8 ; jne 0x2bee
    2be5  movb $0xb8, [ebp-0x320]
    2bec  jmp  0x2bf7
    2bee  mov  dl, [ebp-0x64] ; mov [ebp-0x320], dl
    2bf7  mov  al, [ebp-0x320] ; mov [ebp-0x64], al

## The source

Byte-identical to the already-exact `SetupRecruitWin` (`SOURCE/RECRUIT.cpp`),
which is the reference spelling — one `char` variable, an if/else-if chain, and
a write-back. The dword read + `and 0xff` is what
`static_cast<u8>(buf[0])` lowers to here; a nested `?:` instead produces THREE
byte temps and copy chains and does not match:

```cpp
if (static_cast<u8>(buf[0]) >= 'A' && static_cast<u8>(buf[0]) <= 'Z')
    ch = static_cast<char>(static_cast<u8>(buf[0]) + 0x20);
else if (static_cast<u8>(buf[0]) >= 0xc0 && static_cast<u8>(buf[0]) <= 0xdf)
    ch = static_cast<char>(static_cast<u8>(buf[0]) + 0x20);
else if (static_cast<u8>(buf[0]) == 0xa8)
    ch = static_cast<char>(0xb8);
else
    ch = buf[0];
buf[0] = ch;
```

**Where to look for it.** Every `sprintf`/`strcpy` of a `gResourceNames[]` /
`GetMonsterPluralName()` value that is then interpolated mid-sentence. The PoL
2.0 body has `name[0] += ' '` there; Buka replaced it. Classify as **[Buka]** in
`docs/version-changes.md`.

**Closes.** `advManager::DoEvent`: three sites (`MAP_OBJECT_CAMPFIRE`,
`ARTIFACT_EVENT_MODE_RESOURCE_3`, `ARTIFACT_EVENT_MODE_RESOURCE_5`), ~120
instructions.
