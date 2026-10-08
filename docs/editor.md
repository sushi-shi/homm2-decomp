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

A body only the editor compiles from a shared unit (an `#ifdef HOMM2_EDITOR`
variant or an editor-only method) carries `VA_AT(editor, addr, size)` instead
(`include/match.h`); the editor's labels take it ahead of placements. The game
ignores those lines, the editor's retained maxima fingerprint them as its own
`VA` markers, and the clean export drops them. Header walks (`globals-data`)
follow only the `#include`s an image compiles (`homm2.manifest.image_lines`).

EDITOR/mapcell is the first shared unit with editor bodies: the editor's
`mapCell` and `mapCellExtra` carry each object and overlay part's placement
link (20 and 15 bytes), and `fullMap` adds the undo copy, the part removal
helpers and the extra-pool compaction. All 20 bodies compile exactly; two wait
for the owners of `gMap` and `MAP_WIDTH`/`MAP_HEIGHT` (EDITOR) for their data
identities.

The object tool (EDITOR/OVERLAY) places objects from the editor's object
catalogue, `gOverlayTypes` (956 records of 0x82 bytes: an 8 x 6 grid of
part frames and bit masks per object). The catalogue and the tool's class
tables (`gObjectClassCategories`, `gObjectClassTerrains`) open EDITMGR's
.data contribution right after CLEARMGR's, so EDITMGR.cpp defines them;
OVERLAY's own .data (the class buttons, `gSelectedOverlay`, its strings)
sits between mapcell's and RANDOM's.

## Census

`homm2 --image editor audit census --write-config` writes the editor's
`functions.csv`, `functions_eh.csv`, `functions_static_libs.csv`,
`functions_imports.csv`, `absolute_relocations.tsv` and
`absolute_reference_evidence.tsv` from retail instructions alone: recursive
descent from the entry point, call targets, code addresses in operands and
data words, import thunks, C++ `FuncInfo` unwind and catch entries, switch
dispatch and index tables, unreached bodies at a `/Od` frame prologue or a
16-byte boundary after fill, and the LIBCMT/OLDNAMES contributions matched
with their relocations masked. Run on the game as a control it recovers 2,455
of the 2,474 inventory starts and 30,504 of the 30,675 reviewed DIR32 sites.
EH registration stubs are not functions (as in the game's inventory).
A LIBCMT body found entirely inside a longer member's body is that body's
tail, not a function (iswctype's six instructions end input.obj's
`_un_inc`); a body under eight bytes is placed only straight after its own
member's previous section (fpinit's one-byte `_fpclear`). Compiled static
functions are named too (input.obj's `_hextodec`, frame.obj's
`ExFilterRethrow`). Reviewed rows of functions_static_libs.csv (evidence
`reviewed...` or alternates `disambiguated...`) survive regeneration.
Data words it would admit as pointers but that are ordinary payload (packed
`gOverlayTypes` fields whose dwords happen to name image addresses) are
reviewed out in `config/retail/editor/reloc_exclusions.tsv`.
The runtime members and import thunks it identifies are the editor's
`(libcmt)` and `(imports)` carve-outs; they live in `config/retail/editor`, so
the README's function total does not depend on a generated report. A placed
body's callee whose game claim is a runtime member or an import thunk keeps
that module.

| Editor census | Count |
| --- | ---: |
| Function starts | 2,197 |
| Functions | 1,248 |
| EH funclets | 197 |
| Import thunks | 200 |
| Alignment fill | 552 |
| Absolute fields | 14,678 |

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
- a datum is placed by its code users, every pair agreeing; a content-named
  string only where the editor's cell holds the same bytes;
- a shared unit's own editor body (`VA_AT`) is a code user too once its
  compiled object equals the image; a placed datum's pointer fields place
  their pointees; and the editor's own compile of a shared unit fixes the
  rest of a data section once one member is placed (the editor's longer
  kbwin titles shift everything after them).

893 functions and 940 data are placed from 53 game units. Every BASE unit
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
OutlineBrush 169 bytes) where `/O2` does not; the editor's own objects use
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
| `no-fake-labels`, `globals-data`, `globals-defined` | build gates: the editor's objects name only reviewed functions; every header extern the editor's own sources define carries its DATA claim (read from the editor's inventory, `.bss` spelling aliases resolved) and a definition. Game globals the shared headers declare but the editor never defines are not the editor's |
| `check` | tier: no function below the maximum banked in `config/match_baseline.editor.tsv` |
| `image-link-diff` | tier (and the editor's default build target): the historical link within `config/retail/editor/link_diff.tsv`; `HOMM2_IMAGE=editor python3 -m homm2.verify.link_diff --update` banks a lower count |
| `strict-allocations`, `reloc-fields` | tier |

Every census function has a source owner, so the editor links without
`/FORCE` or retail stand-ins; see [Link](#link).

## Link

`homm2 --image editor link [--rsrc|--historical]` links `EDT2PL.exe` with the
game's native-link driver (`homm2.graph.link`, the editor's `LinkProfile`);
the graph is `link_graph.emit_image_link_graph`, and the historical link and
its link-diff stamp are the editor's default build target.

- Inputs: the editor's own objects explicitly, in the retail order of their
  first functions; `OLDNAMES.LIB` searched first (its members' empty,
  16-byte-aligned `.text` is the fill before the first import thunk);
  `WINMM`, `KERNEL32`, `USER32`, `GDI32`, WinG, `ADVAPI32`, Miles and
  Audiere import libraries (WINMM leads, as in the game: nothing references
  it before BASE, so its descriptor still follows the first scan's DLLs
  while its thunks open the second block); the BASE library as one archive
  in retail member order; then `MSVCPRT` and `LIBCMT`; and `src/EDITOR/EDT2PL.rc`. LINK pulls library members in
  first-reference order
  ([pattern](patterns/library-pull-order-is-reference-fifo.md)), so source
  definition order and old-name spellings decide the BASE and CRT order.
- Resources: `src/EDITOR/EDT2PL.rc` holds the five payloads (the `EDITOR` icon and
  About dialog, `MNUDFLT`, `VERSIONINFO`), gated byte-exact by `graph/rc.py`
  with the retail-extracted icon (`--icon editor.ico`).
- Headers: LINK's defaults for stack and heap; `/DEBUG` with
  `e:\Users\igorl\VSS\HMM\HMM2\temp\release\editor\EDT2PL.pdb`, created
  fresh (age 1) by one link at the image stamp 2003-04-04 08:21:00 under
  the frozen clock. Wine's builtin `msvcrt` reproduces the IAT slot order
  (the VC6 runtime's `qsort`, measured with a native `MSVCRT.DLL`, does not).
- The editor's own objects compile without `/Gf`
  ([pattern](patterns/gf-literal-comdats-reverse-within-function.md)).
- `.bss`: editor-only names compile under `// spelling fixes .bss order`
  aliases where the readable name would break the name-hash order
  (EDITOR, EDITMGR, rumredit, OVERLAY and tile2bs; `gEditManager` compiles
  as `gpEditManager`). Retail holes are unread objects at their places:
  KB's `cOverrideMIDIDriver`/`cOverrideDigitalDriver` (exact size and hash
  window) and `gUnusedData...` storage.

- The editor's own sources (the EDITOR tier) and BASE's Misc compile with
  `/Ob2` ([pattern](patterns/ob2-places-initializer-literals-lexically.md)):
  code is unchanged at `/Od`, and a file-scope initializer's text takes its
  lexical place among the function literals, as in retail (EDITMGR's
  `gMapCodeLetters`, Misc's `gcCDTrackName`).
- Misc is one object, as in the game: every retail editor C++ object
  registers `std::ctype<wchar_t>::id` (65 sites, 65 objects), and Misc's
  registration closes its object. The editor's BASE library compiled the
  music-flag accessors as Midi's prefix (`Midi.cpp` includes
  `MusicFlags.cpp` under `HOMM2_EDITOR`; `compiled_into` in
  `config/units.toml` gives their placed identities to `BASE/Midi`); the
  game links `BASE/MusicFlags` as its own object, the one game object
  without a registration (95 sites, 96 objects).
- Retail's alias count (12 OLDNAMES objects) is reached by EDITOR's
  shipped-map test calling `stricmp`, besides the `strcmpi` REQUEST and RESMGR
  call: its alias member pulls `__stricmp`, already pulled by `strcmpi`, so
  no runtime object moves (the HoMM1 Buka game shows the same second
  spelling). LINK's zero padding after the Rich key depends only on the Rich
  entries, so with the counts the key and the PE header offset follow.

The historical link is byte-identical to retail (`config/retail/editor/link_diff.tsv`
is banked at zero in every region).

## Clean export

`homm2 clean` exports the editor with the game: the generated tree builds
`EDT2PL.exe` as its `editor` target (`build.py --target editor|all`,
`nix build .#editor`), compiling every unit whose `images` name the editor,
the shared ones with `HOMM2_EDITOR`, and `src/EDITOR/EDT2PL.rc`. The editor sources
compile in the clean tree's strict enum mode, and `homm2 clean --verify`
builds both programs ([clean source](clean-source.md)).
