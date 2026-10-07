# An initialized record of 8 bytes or more aligns `.data` to 8; `#pragma pack` does not change it

Measured with the pinned VC6 SP5 compiler (BASE flags, `/Od ... /Gy`) on
reduced objects, 2026-10-07, and on `SOURCE/EXEC`'s 604-byte executive text.

## Mechanism

VC6 gives a file-scope struct or class object of 8 bytes or more an 8-byte
storage alignment, and the object's `.data` (or `.bss`) contribution carries
`IMAGE_SCN_ALIGN_8BYTES`. Arrays of the same size, and records smaller than 8
bytes, stay at 4. `#pragma pack` changes member offsets only; it does not
lower the record's own alignment.

| Probe (one global per object) | Section | Size | Alignment |
| --- | --- | ---: | ---: |
| `struct { char a[6]; }` / `char a[7]` | `.data` | 6 / 7 | 4 |
| `struct { char a[8]; }` | `.data` | 8 | 8 |
| `struct { char a[12]; char b[20]; }`, default pack | `.data` | 0x20 | 8 |
| the same under `#pragma pack(1)` | `.data` | 0x20 | 8 |
| `char a[12]; char b[20];` | `.data` | 0x20 | 4 |
| `char g[32]`, `int g[8]` | `.data` | 0x20 | 4 |
| the 32-byte struct uninitialized | `.bss` | 0x20 | 8 |

## Signature

Every function in a unit is byte-identical, but the linked `.data` moves:
the unit's contribution starts on an 8-byte boundary in ours and on a 4-byte
one in retail (or the reverse). Function-only checks do not see it; an
all-section or linked-image comparison does.

## Use

The executive's 18 text slots (604 bytes) compiled identically as one
initialized record and as 18 `char[]` owners, but the record moved the unit's
`.data` alignment from 4 to 8. Pack 1, 2, 4 and default all kept 8. Retail's
4-byte alignment rules out a single record owner; it does not by itself say
how the original arrays were declared.

Do not add padding symbols or alignment directives to compensate. Change the
owner's kind (record versus arrays) to what the alignment allows.

See also [bss-hash-order-names-storage-spelling](bss-hash-order-names-storage-spelling.md),
where the same rule leaves holes inside a hash-ordered `.bss` run.
