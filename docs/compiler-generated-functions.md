# Compiler-generated and runtime-library functions

These functions have executable retail spans but do not have ordinary source
definitions. Their identities come from different evidence and are kept separate.

## Runtime-library functions

The runtime FID database is `config/library_labels.csv`. All CRT identities live there, not in
`config/delink_procedures.csv`. `runtime_fid.py` compares each reviewed retail RVA against
the same-named compiland in the pinned VC 4.0 `LIBCMT.LIB`. It masks only relocation
operands and accepts exactly one signature identity. A reviewed procedure span can be a
proved prefix when the library symbol extent continues into embedded data or a separately
modeled alternate entry. An assembly entry without a standalone COFF symbol is named as
`$entry_0x<owner-relative-offset>` after the containing library function is proved.

Run `python3 -m homm2.build.runtime_fid --check` to verify the tracked database. A normal
`homm2 build` runs the same check as a hard gate.

## Source compiler-generated functions

MSVC's `$E<n>` names use a translation-unit counter and are not stable identities. Source
records each known generated span beside the global that causes it:

```cpp
VA_COMPGEN(0x004377b0, 0x1a, STATIC_INIT_DISPATCH, SVSearchArray)
VA_COMPGEN(0x004377ca, 0x1d, STATIC_ATEXIT, SVSearchArray)
VA_COMPGEN(0x004377e7, 0x1a, STATIC_DTOR, SVSearchArray)
VA_COMPGEN(0x00437801, 0x1a, STATIC_CTOR, SVSearchArray)
```

`gen_manifest.py` derives a stable comparison identity from the module, semantic role,
and owner, such as
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
