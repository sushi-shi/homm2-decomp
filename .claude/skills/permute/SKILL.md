---
name: permute
description: THE way this campaign tests matching hypotheses - one bounded Cartesian matrix through `homm2 permute`, never a ladder of hand edits. Use when a function is a COMPLETE, correct reconstruction (right types, right control-flow shape) but sits below 100% on VC6 /Od residue - operand evaluation order, od-slot layout, accessor dialect, spelling-sensitive lowering - AND for certifying an unchanged body via island search. Trigger phrases: "climb to 100%", "permute", "spelling orbit", "which arm", "test both spellings", "slot order", "island search". NOT for wrong structure/types/control flow - fix those by hand first (byte-pinned mechanical fixes stay hand-applied).
version: 0.1.0
---

# Permuter — one bounded matrix, byte-proof judging, banked by MAX

**Never test hypotheses as a ladder of one-off edit-compile cycles** (user
directive; violated twice this campaign, both times the ladder measured worse
and burned context: ElementalStorm's two arms, SystemOptions' rename guess).
Localize the divergence first, enumerate every suspected site and every legal
spelling per site, put the whole family in ONE manifest, and let the engine
compile and score the Cartesian product in a single run.

Manual edits are reserved for exactly two cases (CLAUDE.md):
1. integrating the winning arm of a measured matrix, and
2. mechanical fixes pinned directly by byte/relocation evidence (a wrong
   constant, field, call target, or a relational mirror READ OFF the bytes).

## The run

Inside `nix develop .#build`, after a current `homm2 build`:

```
homm2 permute <src.cpp> <rva> \
    [--min-depth 1 --max-depth 2 --families <fams>] \
    [--axes-from /tmp/<fn>-axes.json] \
    [--state-trials 32 --state-families forest --state-seed 1] \
    --limit 512 -o /tmp/<fn>.json --run --top 12
```

Three independent dimensions, crossed into one product:

- **Generated AST forests** (`--families`, from libclang - precedence-correct,
  semantics-preserving by construction): `commutative_order`,
  `relational_order`, `independent_statement_order`, `terminal_return_order`,
  `declaration_split`, `declaration_merge`, `declaration_hoist`,
  `inline_expression`, `inline_read_advance`, `inline_nested_expression`,
  `inline_member_access`, `identifier_rename`, `inline_global_read`.
  `identifier_rename` is the automated od-slot axis (`--rename-identifier`
  scopes it, `--rename-candidate` adds whole spellings - feed it the
  bucket-solver's natural-name pool, never gibberish).
- **Hand-authored exact-span axes** (`--axes-from`, schema below) for
  hypotheses the generator cannot express - accessor dialects, cast trees,
  guard-vs-wrap shapes. Put the FULL candidate family per site in one file;
  never ladder it across runs.
- **TU-state islands** (`--state-trials`, `tu_state_noise` families, default
  `forest`): deterministic parser-visible declarations/includes inserted
  before the target. /Od is TU-state-volatile, so islands are a STANDARD
  dimension of every /Od matrix (user directive) - cross them with the
  spelling axes rather than running spellings dry. They are also THE
  mechanism to certify a finished body without touching it: a 100 under
  some island means the body is done and never needs editing again.

## Exact axes-file schema (validation kills the run otherwise)

```json
{ "schema": 1, "source": "src/SOURCE/Campaign.cpp", "rva": "0x44f00",
  "axes": [ { "name": "priority_compare",
              "find": "<byte-exact span, must occur EXACTLY ONCE in the file>",
              "options": [ {"name": "keep"},
                           {"name": "mirror", "replace": "..."},
                           {"name": "guard", "replace": "...",
                            "extra_edits": [ {"insert_after": "<unique anchor>",
                                              "text": "..."} ] } ] } ] }
```

Traps (all real): options are OBJECTS (`{"name","replace"}`), not strings; an
option without `replace` is the identity arm; `source`/`rva` must match the
command's arguments; every `find`/anchor must occur exactly once in the file
bytes; `--max-depth 0` (exact-span-only runs) is the right default when the
arms come from disassembly - a generated tree on top is an extra multiplier
you opt into with reason.

## Judging and banking

- Survivors are judged by **block topology and ordered relocations**, not
  fuzzy score alone. The repo's byte-proof standard is the donation-grade
  masked compare (`homm2.audit.reloc_donation.masked_equal` + in-image dword
  check); objdiff percent is queue guidance, not proof.
- **FIRST EXACT WINS.** Integrate the winning arm by hand, then run the
  cadence: `homm2 build` → `python3 -m homm2.audit.reloc_donation --write` →
  `homm2 redelink` → `homm2 build` → regression check → `homm2 status update`
  → `homm2 audit harvest-max` after banked runs → commit.
- **PRIME SOURCE RULE**: we write code the way the original devs wrote it.
  A grotesque spelling that scores 100 is WORSE than a clean 99.9 - when the
  winning arm is ugly, bank the MAX (ledger keeps the observed score for the
  hash) and keep the cleanest spelling that preserves the win, or record the
  island that certifies the clean body.
- **Cross-version rule**: any winning spelling that diverges from the PoL 2.0
  spelling adds an OPEN row to `docs/cross-version-spellings.md` in the same
  change. (Verification against 2.0 is deferred until 2.1 is complete.)

## Known VC6 /Od levers to seed axes from (measured this campaign)

- Guard-clause vs wrapping-if: retail's je+jmp early-return stub
  (docs/patterns/, mouseManager::Close class).
- Pointer-side-first evaluation of `*(P+I)` UNLESS P contains an /Ob1-inlined
  accessor - then index-first (`Column(x)[y * width]`,
  docs/patterns/inline-call-operand-index-first.md).
- a1-moffs order: `local + global` loads the local first (3+6 bytes); the
  global-first spelling emits the 5-byte `mov eax, moffs32` (WGAppPaint,
  SystemOptions class).
- Zero-arm ternary lowers branchless sub/neg/sbb/and; `!= 0` bool add lowers
  neg/sbb/neg (TeleportTo).
- Relational mirrors: `if (a < b)` vs `if (b > a)` swap the cmp operand load
  order (InitCampaignMap class).
- od-slot layout: sort by (bucket(name), -decl_index), offsets by
  align4(size); `homm2.core.od_slots` bucket() + natural-name pools solve
  scalar frames; aggregate frames (structs in locals) need decl-order axes
  too. The scalar model MISPREDICTS frames with aggregate interior refs -
  verify model==observed before trusting a rename solution.

## When NOT to reach for it

Wrong control-flow shape, wrong types, cast-hacked views, missing calls -
fix by hand first; the permuter does not restructure. And never respell
`BASE/Bzip.cpp` bodies except where both spellings are a proven
compiler-neutral mirror pair (upstream bzip-0.21 text is invariant).
