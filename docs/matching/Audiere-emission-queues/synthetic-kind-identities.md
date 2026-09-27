# Synthetic function kinds recovered from C1XX names

Measured directly from the pinned C1XX.DLL in matcher-4 on 2026-09-27.
SHA-256 `f014b3bee650224adf6cb44e51f0eeac5abbf5da666fa299d5250b2f3d1937c6`.
This follows the corrected pending-kind audit and maps the internal numeric
values through the compiler's own symbol-name table, without leaked source.

## Evidence chain

`1042d804` takes a kind, subtracts one, bounds it to 0..15, and dispatches
through sixteen dwords at `1042f6f0`. Every selected arm loads one global
pointer and returns it. The read-only table beginning at `104d0480` pairs each
global slot with a literal compiler identifier. Initialization at `10446c61`
walks these eight-byte pairs, passes the string to `10401213`, and writes the
returned interned-name pointer through the paired global address.

For example, kind 4 selects `1042d825`, which returns `[104d7624]`. Table entry
`104d04d8` pairs that slot with string `104d0a14`, whose literal text is
`__dtor`. This is an actual binary name, not a label inferred from the shape of
a decompiled function.

| Kind | Compiler identifier |
| --- | --- |
| 1, 2 | `__ctor` |
| 3 | `=` |
| 4 | `__dtor` |
| 5 | `__vbaseDtor` |
| 6 | `__delDtor` |
| 7 | `__vecDelDtor` |
| 8 | `__copy_ctor_closure` |
| 9 | `__local_vftable_ctor_closure` |
| 10 | `__dflt_ctor_closure` |
| 11 | `__vec_ctor` |
| 12 | `__vec_ctor_vb` |
| 13 | `__vec_dtor` |
| 14 | `__ehvec_ctor` |
| 15 | `__ehvec_ctor_vb` |
| 16 | `__ehvec_dtor` |

Two record-conversion paths copy source-record byte `+0x57` into pending-record
byte `+0x32`, at `10404cd1` and `10404df2`. The synthetic-record factory starting
at `1042d5ae` accepts the kind as its second stack argument, asks `1042d804`
for its name, and stores the same byte at `+0x57` at `1042d656`. Another writer
at `10455c7b` copies a passed-in kind. These concrete paths connect the name
mapping to the pending-record field used by the corrected predicate.

## Consequence and limits

The retained subset 11..13 in `10425299` corresponds to vector helper names.
It is not an unexplained ordinary-destructor category. This narrows that
possible late-emission explanation without changing any source or compiler.

It does **not** establish that the current explicitly defined template node
destructor carries kind 4: the table describes synthetic function categories,
and a source-provided body can take a different path. Nor does identifying a
kind establish when its particular record was registered. The actual Node
record still requires dynamic correlation with its semantic owner and the
initializer boundary.

Artifacts: `build/link/c1xx-kind-map/{extract.py,results.json,evidence.asm}`.
The extractor reads all sixteen jump destinations from the binary, asserts each
arm's load-and-return instructions, locates its unique slot/string pair in
`.rdata`, and preserves all addresses with the binary hash. Instruction ranges
were extracted from matcher-2's complete shipped-binary disassembly; no
instrumented object supplies this static proof.
