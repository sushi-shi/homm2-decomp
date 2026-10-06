# Matching gates and comparison provenance

`homm2 build` publishes a score only after its source, object and relocation
gates pass. This note records why the gates were restored, what they check and
which gates are still staged.

## What failed in the verification process

1. `scripts/homm2/cli.py` carried `AUDITS = False`. Commit `cf00fd1c8`
   disabled the gates on 2026-08-01 while the symbol inventory was empty, and the
   switch stayed off after `d468ed017` recorded 1,727/1,727 on 2026-08-25.
   `homm2 build` therefore never ran the declaration, definition, data, vtable,
   relocation or fixed-width checks.
2. Even with the switch on, only the narrower `--fields` relocation check ran.
   The ordered resolved-address check was not a build gate.
3. Schema-1 normalization stamps hashed inputs, not their output. Overwriting a
   normalized candidate with other bytes left its provenance check green, and
   deleting an intermediate stamp silently shortened the verified chain.
4. The relocation audits skipped missing raw objects or functions instead of
   rejecting a near-exact function whose inputs could not be examined.
5. Status described a normalized-object result as "byte-identical now". That
   score measures compiled comparison objects, not the linked executable; the
   linked image is checked separately by `ninja link-diff`.

These were verification defects. They do not show that comparison bytes were
ever substituted.

## Repairs

- The global audit switch is gone. A build must pass these hard gates before it
  updates the README score block:
  - `annotated_functions --check` (live source spans against the generated
    symbol inventory, and private function symbols in the candidate objects);
  - `assert_decls`, `assert_no_fake_labels`, `assert_globals_data`,
    `assert_globals_defined`, `assert_vtables`;
  - `assert_relocs --resolved` (every ordered relocation whose final retail RVA
    resolves, which subsumes the former `--fields` owner-offset check);
  - `assert_fixed_width_ints`.
- `annotated_functions --check` no longer compares line-anchored CSV manifests
  that nothing regenerated; `homm2 redelink` writes them, and the gate validates
  the live source directly. Like `source_symbols`, it tolerates Clang errors in
  VC6 system headers and fails on unreviewed errors in project files.
- Missing near-exact audit inputs now fail. Explicit target-only runtime and
  carve-out modules remain outside the source-object gate.
- Schema-2 stamps bind both output and input hashes, record the required
  intermediate stamps, reject malformed or empty chains and cycles, and notice
  same-size writes with preserved modification times. Ninja regenerates
  comparison artifacts when the stamp implementation changes.
- Status refuses self-comparisons and labels its measurement scope.
- `AudiereEffectsState` moved from `BASE/AudiereEffects.cpp` into
  `BASE/soundBackends.h` beside `AudiereSampleNode`. It is a plain data owner, so
  the move is codegen-neutral; the build still reports 1,727/1,727 and
  `link-diff` 0.

## Staged gates

These still run on every build and print their findings, but do not block
publication yet (`STAGED_GATES` in `scripts/homm2/cli.py`). Promote each one to
the hard list once its findings are resolved.

- `assert_defs_declared`: 74 free functions are not declared in their owner
  header, 65 of them in `SOURCE/KB.cpp`; five BASE units (`AudiereEffects`,
  `AudiereMusic`, `FONT`, `MilesSound`, `MiscRuntime`) do not include an owner
  header of their own name. BASE compiles with automatic precompiled headers,
  where header structure affects inline emission, so these moves need their own
  measured change.
- `assert_relocs` (the unordered identity audit): it reports anonymous string
  literals (`$SG…`) at addresses retail does not reference, for 160 functions.
  The ordered `--resolved` audit and the byte-identical `link-diff` gate both pass
  for the same functions, so the findings point at the audit's literal-identity
  resolution rather than at the source.
