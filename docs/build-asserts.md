# What `homm2 build` asserts

`homm2 build` is `configure.py` → `ninja` (compile every TU with wine VC6 SP5 via
`cc_wrap.py`) → **hard gates** → objdiff/README refresh. A **red gate exits non-zero and fails
the build** — they are not warnings. All gate scripts live in `scripts/homm2/build/` and run
from the repo root; each is independently runnable (`python3 -m homm2.build.<name>`).

Ordering in `cli.py`: the source-private inventory is verified first,
then compilation must succeed (ninja), followed by the source/object gates below.

## 0. Source-private function identity

The source-function gate requires every `VA(address, size)` definition to match
`build/gen/source_function_spans.csv` and `symbol_names.csv`. For `static` definitions it also
derives the identity through Clang's Microsoft ABI mangler and requires
`build/gen/source_private_functions.csv` to match. This keeps file-local procedures and reviewed
public-span overrides in the synthetic PDB without a second handwritten manifest. After
compilation, the same gate requires each derived private name to resolve to exactly one static
function definition in the owning candidate COFF.

## 1. Compile + header-dependency tracking (ninja)

Every `config/units.toml` unit must compile to `build/objdiff/base/<unit>.obj`; a compile
error fails the build. `cc_wrap.py` scans each TU's `#include` graph and writes a depfile
(`deps=gcc` in `build.ninja`; VC6's `/showIncludes` is unused) — **editing a shared
header recompiles exactly its includers**, so a header change can never leave a stale object
(this previously masked drift). See `docs/patterns/` and the `cc_wrap.py` header.

## The post-compile hard gates

### 2. `assert_decls` — header discipline (no local declarations)
No `.cpp` may carry its own `class` / `struct` / `enum` definition, `extern` declaration, or
file-scope function forward-declaration. Everything is declared in a header so two TUs cannot
drift by declaring the same entity differently (the retail already had `_open` 2-arg vs 3-arg,
`gDwellingType[][12]` vs `[20][12]`). Allowed in a `.cpp`: function **definitions** (incl.
`extern "C" T f(){}` and linkage blocks) and `#include`s.

### 3. `assert_no_fake_labels` — no invented symbols
Every external **function** symbol a `.cpp` emits (defined, `.text`) must exist in the
retained-public/recovered inventory (`build/gen/symbol_names.csv`). Catches hand-written
functions/labels that don't correspond to a reviewed retail symbol. File-local `static`
functions follow the separate source-private `VA` inventory described above.

### 4. `assert_globals_data` — DATA(VA) discipline
`DATA(0x<VA>)` rides the global's **definition** in its owner `.cpp`, not the header `extern`:
- every file-scope **definition** of an inventory data symbol carries `DATA(0x<its exact VA>)`;
- **no `DATA()` on a header `extern`**;
- every header global `extern` has an inventory symbol and owner TU;
- every `DATA()` VA is **unique** (one VA == one definition).

Rationale for def-not-extern: the VA describes *storage*, which the definition owns; a caller that
`#include`s the header sees the plain `extern` and never a duplicated address. The completed
migration emitted `DATA(VA) T g;`; its generator is archived as
`scripts/archive/gen_global_defs.py`. Anonymous or synthetic storage without a retained public
symbol is a `DATA(VA) static T g;` definition in the sole owning module. A real cross-TU external
necessarily has a retained public symbol and uses its owner header; def-less synthetic externs are
rejected.

### 5. `assert_defs_declared` — every definition has a header declaration
Every free function **defined** in a `.cpp` is **declared** in that TU's owner header
`include/<TIER>/<TU>.h`, and the `.cpp` `#include`s its own header. Member functions are
exempt (declared in their class header). Closes the loop with gate 1: a definition's prototype
lives in a header, so callers share the one canonical declaration. Owner headers are
bootstrapped by `gen_module_header.py`.

### 6. `assert_globals_defined` — link-completeness
Every global **declared** `extern` in a header has a **definition** in its owner TU's object
(symbol defined, section > 0, in `build/objdiff/base/<owner>.obj`) — so the project has no
unresolved externals and can link. Only the `_const` pseudo-unit is exempt. The completed migration
generated definitions in retail-RVA order; current definitions are maintained in source.

### 7. `assert_vtables` — source-owned vtable census
Primary vtables use `VTBL(Class, 0xVA)`, which reconstructs the MSVC identity
`??_7Class@@6B@`. A base-specific secondary vtable uses
`VTBL2(Derived, Base, 0xVA)`, which reconstructs `??_7Derived@@6BBase@@@`. Both macros
expand to nothing, but the data-manifest adapter combines their semantic identities and
retail RVAs with exact candidate COFF topology. Same-RVA aliases are accepted only when
candidate COFF proves the same owner, section coordinate, storage, and extent.

The gate requires every inventory vtable to have a matching source marker, every primary
marker to have an inventory row, every marker to have exactly one definition in its owner
candidate object, and every emitted primary or secondary vtable to be modeled. A private
secondary vtable need not have an inventory row because its source marker and candidate
definition provide the missing identity evidence.

### 8. `assert_relocs --fields` — ordered DATA-owner field offsets

The generated objdiff project uses `functionRelocDiffs=data_value`, so the normal report compares
referenced data values instead of accepting every relocation with the same type. For every live
function, this gate resolves delinker `const_<RVA>` aliases into explicitly recovered public `DATA()`
owner extents and compares order-independent owner-relative offset multisets whenever both sides
reference that owner equally often. The comparison runs at every fuzzy percentage; mismatches below
99.5% remain structural review items because a different pointer anchor can be legitimate while the
CFG is still divergent. At or above 99.5% they fail the build. A reference to `gConfig+0x1c`
therefore cannot match retail's
`gConfig+0x30`, even when every ordinary instruction byte agrees.

Owner extents live in `config/reloc_data_owners.tsv`. An entry requires all three forms of evidence:
an inventory data symbol, its source `DATA()` definition at the same RVA, and an independently
recovered storage size. Do not use `symbol_names.csv`'s data size alone; sizes there are claims,
not independently recovered extents. The generated `reviewed_delink_data.tsv` passes these owner regions to the pinned
vostok-delinker. The delinker itself selects the containing region and emits the canonical public
owner plus COFF implicit addend instead of guessing from the nearest preceding symbol.

The pinned objdiff 3.7.1 reads COFF implicit addends in
`objdiff-core/src/arch/x86.rs::relocation_override`. Its report command initializes relocation
comparison to `None`, then applies the generated project's `functionRelocDiffs=data_value` override.
On the captured `DDInitGraphics` regression, masked behavior is `100.0%` while `data_value` is
`99.947914%`. This stricter score is still not a substitute for the gate: equal-valued BSS fields can
compare alike, and objdiff has no project-specific public-owner extent map.

### 9. `assert_fixed_width_ints` — explicit game integer widths

Reconstructed code under `src/{SOURCE,BASE,EDITOR}` and
`include/{SOURCE,BASE,EDITOR}` uses `i8`/`u8` through `i64`/`u64` from `Ints.h`
instead of raw signed and unsigned integer keywords. This makes storage and APIs retain their
intended widths when the game is ported away from the 32-bit MSVC data model. Plain `char` remains
the text character type. Win32, CRT, and vendored SDK headers retain their native ABI spellings and
are outside this gate.

The retail-era MSVC compilers give `int` and `long` distinct C++ type identities even though both are 32 bits.
The compatibility aliases `i32l` and `u32l` preserve a retail long-backed declaration where that
identity affects mangling or overload resolution; ordinary 32-bit game integers use `i32`/`u32`.

The scanner is lexical and ignores comments and quoted literals, so historical assembly notes,
retail strings, and format strings do not create exceptions. `long double` is also allowed because
it is a floating-point type.

## The owner model these enforce

- A symbol defined in `<TU>.cpp` is **declared only** in `include/<TIER>/<TU>.h`; callers
  `#include` that specific header (there is no `_all.h` / `_globals.h` umbrella).
- Types come from the recovered class headers; globals from their owner-TU headers (a plain
  `extern`) + the `DATA(VA) T g;` definition in the owner `.cpp`; Win32 from
  `include/win/windows.h`; CRT from real `<io.h>`/`<string.h>`. Owner-less synthetic storage lives
  as a `static` definition in its sole module and never becomes a header dependency.

## Related checks

- **`homm2 data-relocs`** — opt-in raw-COFF whole-object data relocation census. It
  compares section/offset/type, direct symbol identity and status, owner-relative addend,
  multiplicity, and section class, then emits per-TU JSON residuals. Source `DATA()` is the
  primary HoMM2 provenance anchor and synthetic/fallback target identities are hard errors.
  See `docs/coff-data-relocations.md`.
- **`homm2 relocs`** (`assert_relocs.py`) — **opt-in broad reloc-target audit.** The hard
  owner-field subset above is always run; this wider order-independent review remains opt-in.
  Objdiff's `data_value` score compares referenced data where representable, but does not prove every
  resolved target or owner-relative addend: equal-valued fields and unresolved extern/local aliases
  can still hide or manufacture differences. This audit resolves every near-exact fn's reloc
  targets (from `symbol_names.csv` + definition `DATA()` VAs — REL32→symbol RVA, DIR32→symbol+addend,
  **signed** disp, `const_<rva>` and content-hash-collision names handled) and flags any address base
  references that retail never does, plus any base `?`-symbol resolving to neither the inventory nor a
  `DATA()` global (fabricated). It is **deliberately NOT a hard gate** because incomplete
  functions may still have legitimate relocation-shape differences. Canonical targets retain
  real folded-function identities; synthetic relocation identities are errors.
  `homm2 relocs 0x<rva>` reviews one function. Full rationale:
  memory `[[objdiff-masks-all-relocs]]`.
- **`python3 -m homm2.build.assert_relocs --pe-data`** — opt-in final-image
  `.rdata`/`.data` audit. For every unique configured function it compares the
  complete retail and candidate target-identity multisets without assuming code
  site alignment, so a shifted instruction or payload-equivalent compiler local
  at another address cannot hide a substitution. A separate context-aligned pass
  records final section-offset drift and ordered identity transpositions in
  `build/gen/linked_data_relocs.json`. IAT slots are paired semantically through
  each PE import directory by DLL plus imported name or ordinal; differing
  resolution-history order therefore needs no manual `__imp__` address claims.
  Candidate MAP owners ignore an optional library/group prefix, and one unique
  externally folded definition may satisfy another object's identical COMDAT.
  Semantic `__h2cg$...` compiler-function identities are translated back to the
  current raw linker alias through the normalization sidecar. Raw `_$E<n>` and
  `_$S<n>` names are deliberately excluded from individual final-image pairing:
  the suffix is a per-compilation counter, so those functions are checked as a
  complete unit-level relocation group instead.
  Unequal relocation counts remain explicit
  structural residuals, with candidate over-publications and identities absent
  from the corresponding retail function reported separately. This prevents a
  shape mismatch from suppressing a private-owner mapping error.
- **`homm2 link` function placement** — joins every recovered project function
  to the final MAP by semantic identity and reports raw RVA placement separately
  from object-code exactness. Ordinary decorated names are owner-scoped.
  Reviewed `__h2cg$...` helpers use the normalization sidecar. Remaining
  compiler-counter `_$E<n>` helpers are paired in normalized per-TU section
  order only when their relocation-masked bodies and relocation site/type shapes
  agree; the numeric suffix is never treated as a cross-build identity. The
  current census maps all 197 such helpers, including eight whose suffix changed,
  and leaves no missing or ambiguous project function. See
  `docs/linked-function-placement-walls.md` for the independent raw-RVA ledger.
