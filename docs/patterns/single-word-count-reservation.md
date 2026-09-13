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

## Packet checksum outputs

The same result is independently measured for the two-word `crc` / `crc2`
arrays in `EncodePacket` and `DecodePacket`. `calc_crc` writes exactly one
`u16`. Scalar outputs preserve the complete125/269-byte functions:

```
retail                                 scalar candidate
0048d8ec 66 c7 45 fc 00 00 mov word [ebp-4],0   +02d same bytes
0048d91d 8d 4d fc lea ecx,[ebp-4]              +05e same bytes
0048d920 e8 77 ff ff ff call calc_crc          +061 e8 00 00 00 00 ; REL32 calc_crc
0048d925 66 8b 4d fc mov cx,[ebp-4]            +066 same bytes
0048d9d1 8d 4d f4 lea ecx,[ebp-0c]             +095 same bytes
0048d9d4 e8 c3 fe ff ff call calc_crc          +098 e8 00 00 00 00 ; REL32 calc_crc
0048d9e2 8b 55 f4 mov edx,[ebp-0c]             +0a6 same bytes
0048d9e5 81 e2 ff ff 00 00 and edx,0ffffh      +0a9 same bytes
```

The last wide load is masked to the scalar's16-bit domain. It does not prove
that the source declared a second checksum. [S25](../reconstruction/S25.md)
records the complete4-arm matrix, all16 native functions/sections equal and
independent retail bytes/sites/ordered-target proof for both changed functions.
