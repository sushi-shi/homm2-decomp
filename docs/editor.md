# The scenario editor (`EDT2PL.exe`)

Buka's release ships the scenario editor `EDT2PL.exe` (659,531 bytes, LINK
6.00, linked 2003-04-04 97 seconds after `HMM2PL.exe`; pinned as `editor` in
`config/retail/targets.json`). It is a second linked program of the same
reconstruction: its own census, delink, comparison and scores, sharing the
compiler, the headers and every source proven common to both programs. Its
PDB path is `e:\Users\igorl\VSS\HMM\HMM2\temp\release\editor\EDT2PL.pdb`, and
its assertion paths (`...\Source\Base\MOUSEMGR.CPP`, `...\Source\Editor\RANDOM.CPP`,
`...\Source\Editor\wingraph.cpp`) name the same checkout root as the game's.

## Images

Every authoritative fact is keyed by `(image, rva)`. `homm2 --image editor
<command>` (exported to child processes as `$HOMM2_IMAGE`) selects the editor;
the default image is the game. Commands not yet keyed by image refuse
`--image editor` instead of answering for the game.

| Kind | Game | Editor |
| --- | --- | --- |
| Retail facts | `config/retail/` | `config/retail/editor/` |
| Graph, claims, PDB, delink, comparison | `build.ninja`, `build/` | `build/editor/build.ninja`, `build/editor/` |
| MAX ledger | `config/match_baseline.tsv` | `config/match_baseline.editor.tsv` |

`homm2 init --editor-exe PATH` stages the executable. A `[[unit]]` lists the
images it links into (`images = ["game", "editor"]`; the default is the game);
`[images.editor]` adds `HOMM2_EDITOR` to every editor compile, so a shared
source selects an editor variant with `#ifdef HOMM2_EDITOR`.

## Claim spaces

A source's `VA`, `DATA` and `VTBL` markers spell the addresses of one program
(`homm2.manifest.claim_files`): a unit linked into the game spells game
addresses even when the editor links it too; a unit linked only into the
editor spells editor addresses. The editor reads a shared unit's identities
through `config/retail/editor/placements.tsv`.

## Census

`homm2 --image editor audit census --write-config` writes the editor's
`functions.csv`, `functions_eh.csv`, `absolute_relocations.tsv` and
`absolute_reference_evidence.tsv` from retail instructions alone: recursive
descent from the entry point, call targets, code addresses in operands and
data words, import thunks, C++ `FuncInfo` unwind and catch entries, switch
dispatch and index tables, unreached bodies at a `/Od` frame prologue or a
16-byte boundary after fill, and the LIBCMT/OLDNAMES contributions matched
with their relocations masked. Run on the game as a control it recovers 2,455
of the 2,474 inventory starts and 30,504 of the 30,675 reviewed DIR32 sites.
EH registration stubs are not functions (as in the game's inventory).

| Editor census | Count |
| --- | ---: |
| Function starts | 2,208 |
| Functions | 1,259 |
| EH funclets | 197 |
| Import thunks | 200 |
| Alignment fill | 552 |
| Absolute fields | 14,566 |

## Placements

`homm2 --image editor audit placements --write-config` joins the game's
claimed inventory (`build/gen/symbol_names.csv`) to the editor:

- a function is placed where its game body, with the game's absolute fields
  and every rel32 operand masked, occurs at an editor census start; a body
  found more than once is placed at the offset its unit's other bodies share;
- a placed body's calls, tail jumps and absolute code pointers name its
  callees: a callee placed only by block offset moves to the address its
  callers reach, and an unplaced callee (an editor variant such as
  `PollSound`, or the folded `std::ctype<wchar_t>::id` initializer) is named
  where the calls land and stays an editor residual;
- a datum is placed by its code users, every pair agreeing.

890 functions and 1,026 data are placed from 53 game units. Every BASE unit
with placed bodies (44 units, from BASEMGR on) and SOURCE/kbwin, wingraph
and REQUEST link into the editor; their bodies compile from the game's
sources with the game's profiles.

## Objects

`config/retail/editor/link_order.tsv` lists the editor's objects in link
order, each closed by its `std::ctype<wchar_t>::id` registration. The
editor's own objects link in file-name order, interleaved with the shared
SOURCE units, then the BASE library and the C runtime. Names come from
assertion paths, the class names managers store, the dialog resources they
load and the Price of Loyalty editor's assertion paths (`editmgr`, `editor`,
`evntedit`, `line`, `mapcell`, `overlay`, `random`, `ridledit`, `rumredit`,
`signedit`, `specedit`, `terrain`, `wingraph`).

| Unit | Functions | HoMM1 counterpart |
| --- | ---: | --- |
| EDITOR/CLEARMGR | 9 | CLEARMGR (seeded; see below) |
| EDITOR/EDITMGR | 90 | EDITMGR |
| EDITOR/EDITOR | 37 | EDITOR (start-up, KB/NOOPT copies) |
| EDITOR/EVENTMGR | 16 | EVENTMGR (cell, monster, artifact dialogs) |
| EDITOR/evntedit | 5 | none (event dialog) |
| EDITOR/heroedit | 5 | EVENTMGR's hero dialog |
| EDITOR/line | 15 | none (`lineManager`) |
| EDITOR/mapcell | 20 | none; 3 bodies equal the game's mapcell |
| EDITOR/OVERLAY | 23 | OVERLAY |
| EDITOR/RANDOM | 17 | EDITMGR's random-map generator |
| EDITOR/ridledit, rumredit, signedit, specedit, setup, townedit, x_spedit | 5-15 each | none, or EVENTMGR's town dialog (townedit) |

## Compiler profile

The shared units' bodies equal the game's under the game's profiles (`/Od
/Ob1 /Gr /G5`, BASE with `/Gy /YX`). 297 of the 299 editor-only functions keep a full `/Od`
frame (the other two are a `ret` and a thunk), and the CLEARMGR seed compiled with the game's `base` profile
takes the retail sizes (Open 312, Close 184, UpdateBrushButtons 160,
SelectBrush 169 bytes) where `/O2` does not; the editor's own objects use
`base`.

## Comparison

`homm2 --image editor delink` and `homm2 --image editor build` produce the
editor's report and its README section. The first numbers: 510 of 1,121
functions exact, 509 of 523 in the shared units; the editor-only code is in
`(unmatched)` until its units are reconstructed.

## Build and gates

`homm2 build` builds the game and then every other staged image (each in its
own process with `$HOMM2_IMAGE`); `homm2 --image editor build` builds the
editor alone. `homm2 build verify` runs the game's tier and then the
editor's. The editor's gates:

| Gate | Role for the editor |
| --- | --- |
| `vtables` | build gate: every vtable the editor's inventory names has a source marker (own or placed) |
| `no-fake-labels`, `globals-data`, `globals-defined` | staged (advisory): their findings are the editor variants and owner units not yet written - REQUEST's editor `Main`/`Open`, EDITMGR's globals, the KB data the editor's own objects define |
| `check` | tier: no function below the maximum banked in `config/match_baseline.editor.tsv` |
| `image-link-diff` | tier: pending until the editor links; the first `--update` banks `config/retail/editor/link_diff.tsv` and the gate holds it from then on |
| `strict-allocations`, `reloc-fields` | tier |

The editor's link graph waits for a link that can resolve: every census
function needs a source owner first (no `/FORCE`, no retail stand-ins).

## Open work

- Reconstruct the editor-only units in link order, starting from EDITMGR,
  whose identities (`gEditManager`, the selection rectangle, the edit
  manager's methods) the CLEARMGR seed waits for; `include/EDITOR/editManager.h`
  declares them provisionally.
- The two BASE-library objects the game does not link (0x39b00, 0x39fb0).
- The shared units' 14 residuals: AudiereMusic's compiler-generated static
  initializers, OLDNAMES aliases (`_lseek`/`__lseek`, `_access`, `_strrev`),
  one string identity in Misc and one data identity in SAMPLE.
- Data: the editor's data bytes (31%) wait for its own units' `DATA` claims.
- The editor's link graph and `link_diff.tsv` (when it can link), and its
  clean export.
