# HoMM2 Buka reconstruction

Byte-matching C++ reconstruction of Buka's Heroes of Might and Magic II
(`HMM2PL.exe`, Gold 2.1 tree; `config/retail/targets.json`), built with the
pinned Visual C++ 6.0 SP5 toolchain under Wine. Retail bytes are the authority.
Every function and data byte matches and the linked executable is
byte-identical to retail; the scenario editor (`EDT2PL.exe`) is the second
image (`homm2 --image editor`, [docs/editor.md](docs/editor.md)).

## Build and gates

- Work inside `nix develop .#build`, from the assigned worktree, and confirm
  `HOMM2_DIR` resolves there. Run `homm2 build` after any source, claim, config
  or tooling change; after a claim, signature or data-owner change (including
  one brought in by a merge) run `homm2 delink` first.
- Every commit keeps `homm2 build verify` passing: every function and data
  byte exact, the linked image byte-identical (`config/link_diff.tsv`), the
  game-behaviour tests, and the gate tier. `homm2 clean --verify` keeps the
  generated tree building. Never weaken a gate to pass it.
- The README status block is generated (`homm2 verify readme`, refreshed by
  `homm2 build`); never edit it by hand. `homm2 verify bank` records maxima.
- Cap parallel work on a shared machine (`HOMM2_JOBS`).

## Evidence

- Retail bytes are authoritative. The image is stripped: names, lengths, TU
  ownership, and layouts are all reconstruction claims carried by source markers
  and reviewed manifests, and the Ghidra inventory is analysis opinion.
- Objdiff fuzzy score is prioritization evidence, not correctness proof. In particular,
  wrong `(%ebp)` displacements and relocation operands can still score highly.
- A `jmp $+0` under `/Od /Ob1` is often an inline-accessor continuation. Reconstruct the
  accessor and expression context before considering it a wall.
- TU-wide compiler state can perturb unchanged siblings. Only current raw bytes and
  relocations decide correctness.
- Retained MAX is historical evidence for unchanged effective function source, not the
  current/live score. Never lower it merely because a live object regresses. Carry it
  through hash-schema or dependency-hash migrations when the function body and all
  tracked codegen dependencies are unchanged; reset it only for a real effective-source
  change. MAX is the durable campaign memory: when a disposable island has exact target
  bytes, retail size, and complete ordered-relocation identity, record 100 for that target
  even if the same TU state perturbs siblings. Recover each sibling through its own search.
- Interpret negative optimized-code experiments narrowly. Byte-identical arms prove only
  that the compiler erased that distinction for the tested parent source and TU state;
  they do not prove which spelling was original. Likewise, a complete flat matrix rejects
  those axes only in that parent orbit. A distant ownership, helper-boundary, declaration-
  order, or compiler-state change can make the same axis observable, so retry it only when
  an evidence-backed structural parent changes.
- An exact CFG, exact block sizes, exact total size, or equal relocation count is still
  partial evidence. A relocation appearing in the wrong block often identifies the wrong
  semantic owner even when the target and total count agree. Compare ordered sites,
  identities, addends, block contents, and bytes before calling an island closed.
- Keep an ordinary source comment only when it records an enduring semantic or codegen fact.
  Do not encode queue state, scores, retained maxima, or completion claims in source comments.
- Never retain generated labels, globals, aliases, padding, or fake code in reconstructed
  source solely to improve a metric. The controlled TU-state probe is disposable compiler
  input; its best object/assembly may remain under `build/` as clue evidence. Its contract
  lives in `homm2/permute/tu_state_noise.py`'s own documentation.

## Source rules

- Ordinary C++ with real types. No decompiler output, byte blobs, naked
  assembly, dummy bodies, address masking, or compiler-state steering in
  retained source.
- One TU lives at `src/<TIER>/<TU>.cpp` with its declarations in
  `include/<TIER>/<TU>.h` (the owner-header model); define functions in
  retail-RVA order. Mark functions with `VA(...)`, globals with `DATA(...)`,
  vtables with `VTBL`/`VTBL2`.
- Compiler profiles live in `config/units.toml`; changing one needs
  retail-backed evidence.
- For a family of closely related functions, recover and preserve a consistent
  semantic phase structure, control-flow ownership, and narrow inline-helper
  boundaries. Keep real dialect differences explicit. Do not replace copied
  retail-family implementations with shared macros merely to remove source
  duplication, especially when doing so loses established byte or CFG islands.
- Class data members start with `m_`; plain struct fields need not.
- Replace `fieldN` placeholders once their meaning is known. Use real tagged layouts when
  one serialized record has multiple forms.
- C-style casts are forbidden. Prefer reconstructed types; otherwise use the appropriate
  C++ cast explicitly.
- Prefer repository naming over secondary projects. Use named constants instead of magic
  numbers.
- Use the fixed-width aliases from `Ints.h` throughout reconstructed game code: `i8`/`u8`
  through `i64`/`u64`. Plain `char` remains the text character type. Reserve `i32l`/`u32l`
  for proven retail `long` declarations whose distinct C++ type identity affects mangling or
  overload resolution, and retain SDK aliases such as `DWORD`, `WPARAM`, and `U32` at external
  ABI boundaries. The fixed-width build assertion enforces this scope.
- Put type declarations and shared enum domains in headers. A `typedef enum` used by
  exactly one TU is private and lives in that `.cpp`. For known serialized numeric
  domains, prefer a `typedef enum` with explicit values while preserving storage width
  and packed layout.
- Preserve proven layouts with packed records and `SIZE` evidence. The retail MSVC build
  keeps `SIZE` byte-neutral; do not turn it into emitted declarations.
- Use inline accessors where retail `/Ob1` traces prove them. Do not replace modeled fields
  with `reinterpret_cast<unsigned char *>(this)[offset]` merely for a local score gain.
- In the optimized icon-decoder family, begin from the
  `FlipIconToBitmapYModify` structural idiom: semantic file-static decoder
  state and comparable declaration order, direct compact cursor operations,
  few incidental state-copying locals, shallow early-exit/continue flow, and a
  scoped `do { ... } while (0)` candidate for small two-join goto clusters that
  plausibly came from a multiline macro. Preserve normal semantic gotos and
  treat every family resemblance as a hypothesis requiring byte, relocation,
  and CFG evidence. Enumerate uncertain declaration orders rather than editing
  them manually one by one.
- Do not search for unavailable original source. Secondary references are for
  naming/semantic guidance only; adapt useful names to this repository.

## Data and linking

- Candidate objects supply reconstructed COFF symbol spelling and topology; retail PE
  bytes and the reviewed relocation-site manifest supply placement evidence. Vostok
  emits the reviewed model; it does not discover private identities automatically.
- The canonical data manifest is generated from source `DATA(...)`,
  `DATA_COMPGEN(...)`, `VTBL(...)`, and `VTBL2(...)` annotations. Candidate COFF
  supplies physical topology; source supplies semantic identity. Missing or ambiguous
  private placement warns normally and fails strict assembly instead of falling back to
  a synthetic name or a second hand-maintained ledger.
- Preserve storage class and section alignment. Padding is not a symbol. Do not insert
  giant arrays to reproduce final-image gaps, and do not classify `.data`/`.bss` solely
  from the PE raw-size boundary.
- Compare relocation source site, target identity, and owner-relative addend. Never hide
  an interior-field mismatch by aliasing overlapping storage.
- Anonymous `$SG`, `$T`, and compiler-counter names are normalized only in disposable
  comparison copies. Never link or derive layout from those normalized objects.

Detailed contracts live in `docs/data-symbol-normalization.md`,
`docs/coff-data-relocations.md`, and `docs/static-storage-link-audit.md`. The
compiler-generated data contract lives in `docs/candidate-data-topology.md`.

## Layout

| Path | Contents |
| --- | --- |
| `src/{BASE,SOURCE,EDITOR}`, `include/` | reconstructed source and owner headers |
| `res/`, `imports/`, `locales/` | resources, reviewed import ABI, text catalogs |
| `vendor/` | third-party SDK headers (Audiere, Miles, Smacker, WinG) |
| `config/`, `config/retail/`, `config/reviews/` | build contracts; retail facts by kind and image; review ledgers |
| `scripts/homm2/` | tooling; keep `homm2.core.usage.logged` on entry points (`homm2 audit usage`) |
| `docs/`, `docs/patterns/` | reference docs; measured compiler mechanisms |
| `build/` (ignored) | retail images, toolchain, Wine prefix, generated files |

## Branches

`decomp-gold-2.1-buka` (this branch) generates `source-gold-2.1-buka` and
`classic-gold-2.1-buka` with `homm2 clean`; never edit generated branches.
`port` (the cross-platform port) builds on the generated source branch.

## Git

- Never revert user or concurrent-agent changes.
- Commit focused units of work with messages such as
  `match: reconstruct EVENTS EraseObj` or `tools: verify relocation addends`.
- Stage only declared source, header, config and docs plus generated status
  files; leave build probes, worktrees and queues unstaged.

## Skills

`.agents/skills/` (linked from `.claude/skills`): `matcher` for
reconstruction, `permute` for compiler-state experiments, `orchestrator` for
parallel lanes with serial integration. The matching loop is
[docs/workflow.md](docs/workflow.md); the tools are
[docs/tooling.md](docs/tooling.md) and [docs/tooling-map.md](docs/tooling-map.md).
