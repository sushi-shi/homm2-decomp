/*
Unit: BASE/AudiereMusic
Retail functions: _$E15 at 0x004cd108, _$E20 at 0x004cd1f8
Source: src/BASE/AudiereMusic.cpp

Evidence
--------
VC6 emits one four-byte BSS initialization guard for the two static RefPtr
members of AudiereMusic. Candidate COFF identifies it as local `_$S18` at
.bss+0x8. Retail placement and the exit-thunk relocations identify the same
allocation at VA 0x005395e4; bit 0 guards `stream` and bit 1 guards `source`.

The candidate exit thunks are counter-named _$E17 and _$E23, while retail calls
the corresponding bodies _$E15 and _$E20. Both 70-byte bodies are
instruction-identical and their ordered relocations are exact. The source
retains the generated RefPtr semantics and names the guard with:

    DATA_COMPGEN_GUARD(
        0x005395e4, audiereMusicInitializationGuard, AudiereMusic
    )

The class name is private and absent from the stripped image. VC6 hashes the
decorated class-static identities into BSS buckets; the concise semantic name
`AudiereMusic` places `source` in bucket 0xfb and `stream` in 0x142, before
the generated guard's 0x3c1 bucket. An explicit zero initializer then places
the positions array at +0xc. These are the retail offsets, and substituting the
ordinary compiled object into the raw LINK input preserves exact `.text` and
`.data` without adapting the object.

Tooling disposition
-------------------
`data_manifest_adapter` previously recognized decorated `_$S5$...` guards but
not VC6's plain ordinal `_$S<n>` form. Plain ordinal symbols are now eligible
only when candidate COFF classifies them as BSS. The explicit source claim,
retail payload/storage, and candidate topology must still select one allocation.
A focused test rejects the same spelling in initialized data.

After redelink/build, the generated manifest binds the semantic guard to
BASE/AudiereMusic.c .bss+0xf4. All 19 functions in the unit are exact, including
both exit thunks, and the newly comparable state contribution raises reviewed
exact data by 252 bytes. The 27 focused compiler-data tests pass.
*/
