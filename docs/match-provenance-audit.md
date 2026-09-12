# Battle artifact and matching provenance audit — 2026-09-12

Audit baseline: `decomp-gold-2.1-buka` at `ca2904a91`. Native rendering
investigation: PR #55 at `0660a47d9`, with a subsequent background-cache fix
held locally pending this audit. No fresh retail equality claim is made here.

## What failed in the verification process

1. `scripts/homm2/cli.py` still had `AUDITS = False`. Commit `cf00fd1c8`
   disabled the gates on 2026-08-01 while the inventory was empty; the switch
   remained disabled after `d468ed017` recorded 1,727/1,727 on 2026-08-25.
   Consequently `homm2 build` did not run the declaration, definition, data,
   vtable, relocation-field, or fixed-width checks inside that conditional.
2. Even enabling the switch would only have invoked the narrower `--fields`
   relocation check. The broader target-identity and ordered resolved-address
   checks were not build gates.
3. Schema-1 normalization stamps hashed inputs, not their output. Overwriting
   a normalized candidate with other bytes left its provenance check green.
   Deleting an intermediate stamp silently shortened the verified chain.
4. Missing raw objects/functions were skipped by the relocation audits rather
   than rejecting a near-exact function whose inputs could not be examined.
5. Status described a normalized-object result as “byte-identical now.” It
   measures compiled comparison objects, not unbuilt source edits, the final
   linked PE, or native-port behavior. Historical per-function maxima are
   weaker still: they need not coexist in a single build.

Negative controls demonstrated the disabled-gate and provenance failures before
the implementation changes. These findings establish verification defects, not
that somebody actually substituted comparison bytes or that the historical
whole-file SHA result was false.

## Repairs

- Removed the global audit-disable switch. Builds must pass the source/object
  gates and both target-identity and ordered resolved-address relocation checks
  before publishing a successful score update.
- Missing near-exact source-object audit inputs now fail. Explicit target-only
  runtime/carve-out modules remain outside this source-object gate. Existing
  compiler-generated name fallbacks remain visible in the audit implementation;
  this is not a claim that every audit consumes only raw object bytes.
- Schema-2 stamps bind both output and input hashes, record required intermediate
  stamps, reject malformed/empty chains and cycles, and notice same-size writes
  with preserved modification times. Ninja regenerates comparison artifacts
  when the stamp implementation changes. Old stamps must be rebuilt.
- Status refuses self-comparisons, labels its measurement scope, and distinguishes
  normalized-object scores from fresh linked-image SHA equality.
- Regression tests inject failed gates, swapped relocation targets with an equal
  multiset, missing inputs, overwritten comparison outputs, broken provenance
  chains, and a self-comparison configuration.
- Two pre-existing selftest fixture defects were isolated: offline CodeView
  parsing no longer depends on `MSVC_DIR` being set by the caller, and the init
  dispatch test mocks toolchain provisioning instead of attempting a download.

Source-only checks already reveal two header-discipline violations in the
baseline: local `AudiereEffectsState` and `SeedPositionState` declarations in
`BASE/AudiereEffects.cpp` and `SOURCE/SEARCH.cpp`. The fixed-width audit passes.
The declaration findings are not suppressed, nor are source declarations moved
without the compiler/retail evidence needed to verify their codegen impact.

Validation: `nix develop --command homm2 selftest` reports 879 tests, 877 passed
and two skipped because real candidate objects are not built. These tests verify
the tooling, not the unavailable retail reconstruction. Native CTest passes
23/23; the optional real-asset attack test passes separately as described below.

## Rendering provenance: established and not established

The actual captured symptom is a transient background-coloured stripe at an
outward-moving attack redraw boundary. Earlier font/mirrored-run fixes did not
resolve the user's reported symptom; they must not be described as user-confirmed.

The latest native reproduction exercises background-cache invalidation before
attack redraws. `DrawBackground` clears the whole working screen; `DrawFrame`
then restores sprites only in the dirty rectangle. `PowEffect` issues a separate
screen update with the legacy one-pixel enlargement enabled. That can expose bare
background immediately to the right/below the redrawn area. Retaining the dirty
bounds across frames moves those edges outward.

That cache-clear logic is present in the matching source (`DrawBackground`,
VA `0x438d17`; `DrawFrame`, VA `0x4395b8`; `PowEffect`, VA `0x41df4e`). This is
source-lineage evidence, not a fresh byte verification of those addresses.
Native-port commit `61a51674b` replaced `InvalidateRect`/`UpdateWindow` with
direct `IVideo::Blit`/`Present`, retaining the enlargement in
`BlitBitmapToScreenVesa` (matching-source VA `0x4d4610`). The Windows path uses
`BeginPaint`, WinG or DirectDraw, different rectangle arithmetic, and clipping.
Those backend contracts cannot be assumed interchangeable. The port boundary
is a concrete candidate for exposing an inherited buffer inconsistency; the
first retail-visible occurrence has **not** been established.

The local native fix rebuilds the terrain cache without clearing unrelated
working-screen pixels, then restores the same rectangle used for sprite redraw.
The broad cache-invalidating comparison went from 5,485 mismatching frames out
of 13,552 to zero. The smaller optional real-asset attack regression passes
800 frames, including both facings, with zero mismatches. These are native
rendering tests, not retail equivalence tests or manual user confirmation.

## Required next evidence

The original Buka `HMM2PL.exe` is absent from this audit checkout and was not
found among the available installed executables. The installed rebuilt candidate
is not an independent retail control. Supply the original with SHA-256:

```text
bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
```

Then regenerate the retail-derived inventory/objects in a fresh build directory,
build with the pinned VC6 SP5 toolchain, inspect every newly enabled gate failure,
and rerun `homm2 link --transform`. Retain the source revision, toolchain and input
identities, raw/comparison reports, transform list, and final executable hash
together. Compare the same attack sequence in that original Windows executable
and the native build before classifying the visible defect as retail-inherited
or port-introduced. Do not weaken a gate merely to recover a 100% display.
