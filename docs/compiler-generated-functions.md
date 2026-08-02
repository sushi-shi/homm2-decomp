# Compiler-generated and source-private functions

These functions need synthetic PDB procedure records; the stripped retail image names
none of them. Their identities come from different evidence and are kept separate.

## Source-private functions

A file-local function is ordinary reconstructed source, not compiler-generated code. When
its definition is `static` and carries `VA(address, size)`, `annotated_functions.py` reads
the Clang AST, derives its Microsoft-decorated identity from the semantic signature, and
writes `build/gen/source_private_functions.csv`. `source_symbols.py` names the same
definition in `symbol_names.csv`, so `synth_pdb.py` gives Vostok a procedure record even
though no symbol in the stripped image names it.

`homm2 build` checks this generated private-function inventory for freshness.

Ordinary public definitions also carry reviewed `VA(address, size)` spans; the claimed
inventory takes its decorated names, RVAs, and sizes from those source annotations. The
complete generated span inventory is `build/gen/source_function_spans.csv`.

## Source compiler-generated functions

MSVC's `$E<n>` names use a translation-unit counter and are not stable identities. Source
records each known generated span beside the global that causes it:

```cpp
VA_COMPGEN(0x004377b0, 0x1a, STATIC_INIT_DISPATCH, SVSearchArray)
VA_COMPGEN(0x004377ca, 0x1d, STATIC_ATEXIT, SVSearchArray)
VA_COMPGEN(0x004377e7, 0x1a, STATIC_DTOR, SVSearchArray)
VA_COMPGEN(0x00437801, 0x1a, STATIC_CTOR, SVSearchArray)
```

`canonicalize_data_symbols.py` derives a stable comparison identity from the module,
semantic role, and owner, such as
`__h2cg$SOURCE$PHILAI$static_ctor$SVSearchArray`. The macro emits no code.

The disposable COFF normalizer does not trust the `$E` number. It classifies constructor
and destructor thunks by their owner and special-member relocations, then identifies the
`atexit` registration and dispatcher through the complete local relocation graph. Sibling
functions need not have their own `VA_COMPGEN` declarations. Each claim is renamed only when
it has exactly one candidate and the proved extent has the declared size. A missing or
ambiguous candidate produces a warning and remains unrenamed; the tool never guesses. Invalid
placement, duplicate semantic identities, overlaps, and a wrong proved size remain errors.
The target object already carries the semantic identity; candidate and target therefore
compare without relying on compiler counters.
The focused and hard relocation audits follow these normalized identities, so the generated
functions receive the same relocation checks as ordinary named functions.
