# A two-byte count can occupy a four-byte aligned reservation

In Buka `resourceManager::LoadAggregateHeader` (RVA0xb89b0), all accesses to
the scratch owner are a two-byte CRT read and one signed-word load. The
reconstructed `i16 fpCountBuffer[2]` falsely implied a second meaningful
element. `i16 fpCountBuffer` with `&fpCountBuffer` at the read call produces
the same frame and instructions:

```
retail                                 scalar candidate
004b89b3 83 ec 14 sub esp,14             +003 83 ec 14
004b8a59 6a 02 push 2                   +0a9 6a 02
004b8a5b 8d 4d f8 lea ecx,[ebp-8]        +0ab 8d 4d f8
004b8a6d e8 de fe 01 00 call _read       +0bd e8 00 00 00 00 ; REL32 _read
004b8a75 0f bf 45 f8 movsx eax,[ebp-8]   +0c5 0f bf 45 f8
```

Both complete raw312-byte functions and all14 ordered relocation operands
match genuine retail; before/after all34 functions and all allocated sections
in the TU agree. [S01](../reconstruction/S01.md) preserves the four-arm
matrix and reproduction commands. This proves scalar sufficiency, not the
original spelling. Do not infer array extent from an aligned slot gap alone,
and do not generalize this to wider reads or pointer escapes.
