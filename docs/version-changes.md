# Functional changes: PoL 2.0 → Gold 2.1 → Buka HMM2PL.exe

Living ledger of every functional difference between the Price of Loyalty 2.0
retail build (the `decomp-pol-2.0` line) and the Buka release reconstructed on
this branch. A matcher that closes a function whose semantics differ from the
PoL body adds the entry in the same change (`.claude/agents/matcher.md`).

Classification: entries marked **[2.1]** are upstream NWC Gold 2.1 changes,
**[Buka]** are localization-era changes, **[unclassified]** have not yet been
diffed against the Gold 2.1 GOG binary
(`investigation/extracted/gog-win/app/HEROES2W.EXE`). The image itself was
built from the `e:\Users\igorl\VSS\HMM\HMM2` tree (the five surviving assert
path strings) with VC6 SP5 — PoL 2.0 used VC 4.2.

## Build-level changes

- **[Buka] Whole-image /Od.** Frame census over all 95 game TUs: every retail
  function opens with the full `push ebp; mov ebp,esp` frame and /Od codegen
  (redundant reloads, no CSE). The PoL build compiled ~40 of these TUs /O2.
  Only `BASE/BITS` stays optimized (/O1 /Oy-). `config/units.toml` records the
  per-TU truth.
- **[Buka] Debug allocation layer abandoned.** `BaseAlloc`/`BaseFree` (the
  PoL file/line-tracking allocator behind `H2_ALLOC`/`H2_FREE`) are still
  compiled into `BASE/Misc` but have **zero** callers; all 435 allocation and
  524 deallocation sites call CRT `operator new`/`operator delete` directly.
  The per-TU source-path string tables PoL carried in `.data` are gone.
- **[Buka] Asserts survive in five TUs only.** `ProcessAssert(cond, file,
  line)` is live in MOUSEMGR, RESMGR, netwin, dpnetwin, and wingraph with
  `e:\Users\igorl\...` path strings; every other TU's asserts were compiled
  out. (Our MOUSEMGR reconstruction is still missing its retail asserts.)
- **[Buka] No /Gi line machinery.** The PoL-era `*SourceLineBase` statics
  (VC4.2 `/Gi` __LINE__Var lowering) have no VC6 counterpart; retail passes
  literal line numbers where file/line machinery survives at all. All 76
  statics folded to literals and removed.
- **[Buka] /QIfdiv off.** Retail game code carries raw `fdiv` instructions
  (hundreds of sites; PHILAI alone has ~90) and links no `_adj_fdiv_*`
  helpers; the PoL-era `/QIfdiv` flag made VC6 emit `__adj_fdiv_m64` calls
  and was removed from every profile.
- **[Buka] C++ exception state EVERYWHERE.** The whole game build compiles
  /GX: EH-registered prologues appear in every TU that constructs an object
  with a throwing path (ADVMGR, SETUP, TOWNMGR, RESMGR, KB, the widget and
  audio TUs...); the image's `(funclets)` region is their unwind bodies.
  PoL had no /GX anywhere. Both base profiles carry /Gi- /GX.
- **[Buka] VC6 STL exposure.** `KB.h` includes `soundManager.h`, which
  includes `<audiere.h>` — dragging `<string>`/`<vector>` into 90 of the 95
  game TUs; each emits the ctype-facet guard stub trio ($E19/$E18 +
  `?id@?$ctype@G@std@@` dtor thunk, the dtor COMDAT-folded to ADVMGR's copy
  at 0x415a50). Only BITS, TILE, X_GLOBAL, MusicFlags, and the emptied TUs
  stay clean. **Resolved 2026-08-03**: eleven of our TUs did not reach the
  chain and so emitted no stubs, leaving their claimed `_$E18`/`_$E19`
  spans unpaired. Adding `#include <SOURCE/KB.h>` to Icon2b, Icond2b,
  Icondf2b, Iconf2b, Iconm2b, Iconmf2b, icon2bc, icon2bs, iconf2bc,
  bmap2 and RESOURCE emits them, byte-matching retail (+22 exact,
  units-with-progress 84 -> 94, 0 drops).

## Removed (present in PoL 2.0, absent from this image)

- **[Buka] The MCI/redbook CD-audio subsystem** — replaced by the Audiere
  layer. `src/BASE/soundmgr.cpp` keeps the PoL bodies under `// @remove`:
  `HandleMCIError`, `soundManager::{ValidatePreviousPosition, CDStop,
  CDIsPlaying, CDShutdown, CDSetVolume, CDPlay, CDPoll, AllocateSampleHandles,
  StartSample, ForcePollSound, GetNumberCDDrives}`, `SetReady2Poll`,
  `UpdateTimers`, and the Miles `WAVE_init_driver` sample path.
- **[Buka] The CD verification TU** — all six `CDTest_*` functions of
  `SOURCE/x_cdtest` (`Init`, `Cleanup`, `VerifyCDQuick`, `VerifyCDThorough`,
  `VerifyTrack`, `GenerateTable`). The TU still links, contributing only its
  STL init tail.
- **[Buka] `soundManager::MIDIPoll`** (`src/BASE/Midi.cpp`) — the empty MCI
  poll body.
- ~~The Y-modify icon decoder pair~~ — **not removed after all**: retail
  relocated both decoders to the BASE tier tail (`FlipIconToBitmapYModify`
  at 0x4d5270, `IconToBitmapYModify` at 0x4d5a50) with rewritten bodies;
  reclaimed 2026-08-02.

## Added (no PoL counterpart)

- **[Buka] The Audiere backend** (`vendor/audiere-1.9.2/audiere.h`, dynamic
  `audiere.dll`): `BASE/AudiereEffects` (12 sample functions around an
  `AudiereSampleNode` list), `BASE/AudiereMusic` (7 music functions over
  class-static stream/source RefPtrs with guarded atexit teardowns), and the
  refactored Miles wrapper `BASE/MilesSound` (11 functions).
- **[Buka] `BASE/MusicFlags`** — a new three-function /Od TU between EXEC
  and Midi (`GetMusicFlagA`, `MusicFlagsActive`, `GetMusicFlagB`).
- **[Buka] The Cyrillic text engine** in `BASE/FONT`:
  `RemapCyrillicCharacter` (CP1251 → glyph range) and the 4KB
  `font::ExtractLine` — a line extractor that word-wraps and hyphenates
  over-long words at syllable boundaries using a Latin+CP1251 `IsVowel`
  test, writing `-` at break points.
- **[Buka] `TranslatePolishInputCharacter`** (`BASE/INPUTMGR`, 0xbcad0) — the
  Polish-release input remap this build inherited.
- **[Buka] `GetMonsterPluralName`** (`SOURCE/KB`, 0x68cf0) — plural creature
  names for the localized UI.
- **[unclassified] `DriveSupportsFreeSpaceQuery`** (`BASE/Misc`, 0xbf2f0).
- **[Buka] soundManager backend plumbing** — `ShutdownSoundBackends`,
  `StartupMilesBackend`, `ConvertVolumeFloat` (see the class rework below).
- **[Buka] EDITOR/mapcell gained the STL init pair** — the only editor-tier
  TU now carries the ctype-facet guard stub and atexit registration at its
  tail (the "two new functions" vs 2.0), meaning Buka's mapcell.cpp include
  chain reaches <audiere.h>; our TU does not emit them yet.
- **[Buka] User-declared empty destructors** `game::~game` and
  `soundManager::~soundManager`, defined at the tail of `SOURCE/KB` — PoL
  relied on the implicit destructors.

## Changed

- **[Buka] The preferences registry moved to Buka's own key and value
  namespace.** `BASE/Misc`'s three registry functions
  (`ReadPrefsFromRegistry` 0xbe410, `WritePrefsToRegistry` 0xbedd0,
  `SetupCDDrive` 0xbf370) open
  `SOFTWARE\Buka\3DO\Heroes of Might and Magic Platinum\1.000` instead of
  `SOFTWARE\New World Computing\Heroes of Might and Magic 2\1.0`, and every
  value name is prefixed and de-spaced: `Music Volume` -> `HMM2POL
  MusicVolume`, `Sound Volume` -> `HMM2POL FXVolume`, `Quick Combat Level` ->
  `HMM2POL AutoCombat`, `Combat Shade Level` -> `HMM2POL CombatGridLevel`,
  `Combat Army Info Level` -> `HMM2POL CombatViewArmyLevel`,
  `Network Default Name` -> `HMM2POL NetName`, `Autosave` -> `HMM2POL
  UseAutosave`, `Main Game *` -> `HMM2POL Game*`, `Editor *` -> `HMM2POL
  Editor*`, `Editor Screen Animation` -> `HMM2POL EditorAnimateScreen`,
  `CDDrive` -> `HMM2POL CDDrive`. The application-path value is renamed
  outright: `AppPath` -> `PathPL2` (the Polish-release lineage this build
  inherits). Full pair list with retail addresses in
  `docs/buka-literal-divergences.tsv`; all 46 names are byte-pinned by the
  `$anon_str_<sha256 of content+NUL>` cells the delinker names.
- **[unclassified] `ReadPrefsFromFile` / `ReadPrefsFromRegistry` default-path
  bodies call the shared helpers.** Both reset paths run
  `SetInstallDefaults(); SetGameDefaults(); WritePrefs();` (the registry one
  preceded by an explicit `memset(&gConfig, 0, 0x19d)` and `RegCloseKey`)
  rather than inlining the memset/strcpy/`musicSource` block and the
  `UpdateSystemOptionsMenu(); WritePrefsToRegistry();` pair. Byte-pinned at
  0xbe371 and 0xbe4a3.
- **[Buka] `resourceManager::m_lastFileName` grew from 60 to 1000 bytes**
  (`RESOURCE_MANAGER_FILENAME_CAPACITY` 0x3c -> 0x3e8, sizeof 0xa2 ->
  0x44e). Byte-pinned three ways: `m_lastFileId` reads at `this+0x44a`
  (PointToFile, GetFileSize sprintf args) while `m_lastFileName` stays at
  `this+0x62`, and the single retail `push 0x44e` at the KB
  `new resourceManager` site. Longer localized path/name headroom.
- **[build] String pooling (`/Gf`) is on for most of the build but off for
  eight TUs.** Retail KB/dpnetwin/ADVMGR blocks hold one cell per
  literal content (duplicate literals in our reconstruction resolve to a
  single retail cell), while Bzip, Misc, EVENTS, Modem, SETUP, SPELLS,
  wingraph and COMMAND keep adjacent duplicate cells of the same content
  inside their unit block - their compiles ran without pooling
  (`base_nogf`). COMMAND's block carries seven `smalfont.fnt` cells, three
  `pickup%02d.82M`, three `mons32.icn`, two `wincmbt.bin` and four
  `Баллиста` (0xf1188/0xf11b8/0xf12d4/0xf1354/0xf1380/0xf1394/0xf13c0;
  0xf1198/0xf12a4/0xf133c, ...), one per source occurrence; recompiling the
  unit without `/Gf` leaves its `.text` byte-identical (28,432 bytes both
  ways) and clears 25 `homm2 relocs only-base` rows, so the flip to
  `base_nogf` in `config/units.toml` is pending. Pooled
  literals are `??_C@` COMDATs the linker folds image-wide, so cross-unit
  references are expected.
- **[Buka] In-code UI strings translated to Russian (CP1251).** 33 string
  literals compiled into code (not the resource text) carry Buka's Russian
  translation where PoL/Gold had English: the ADVMGR bottom-bar date words
  (`Month`/`Week`/`Day` → `Месяц`/`Неделя`/`День`), combat confirmations and
  the Sphere-of-Negation notice (COMMAND), hero dismissal (HERO), building
  production and out-of-memory text (KB), the whole Modem/SETUP dial and
  hosting dialog set, garrison-full (RECRUIT), five combat-spell rejection
  messages plus the Holy Word / Holy Shout and Death-spell damage reports
  and the plural/singular resurrection reports (SPELLS), the tavern rumor
  lead-in (TOWNMGR), the whole combat win/lose panel set (COMMAND:
  the necromancy skeleton report singular+plural, the Eagle Eye
  spell-learned line, the casualty panel `Потери`/`Нападавшие:`/
  `Оборонявшиеся:`/`Нет`, and the ballista/turret info block
  `Баллиста`/`Левая башня`/`Правая башня` with their destroyed and
  fire-strength sentences), network
  hosting/waiting texts (Wsnetwin/dpnetwin), and the 256-color requirement
  (wingraph). Found mechanically: for every byte-proven function, the retail
  dword at each donated string site names a cell whose retail content is
  compared with the compiled literal. Source now carries the CP1251 bytes as
  hex escapes with the Russian rendering in a trailing comment. The same
  translation reshapes `SExecutiveText` (BASE/EXEC): eleven of its eighteen
  inline `char[]` messages are Russian, so every slot size changed
  (struct 0x214 → 0x25c; each slot is align4(strlen+1)); the list-format
  strings and `Terminated` stay English.
- **[Buka] `soundManager` class rework.** The class gained a backend-state
  head at offset 0x36: backend/savedBackend kinds, the Miles `_DIG_DRIVER*`,
  an `audiere::AudioDevicePtr` (a real RefPtr — its inlined ref/unref
  sequences and the compiler-generated dtor at 0x470df0 prove the type), and
  music fade/track fields. `PlayAmbientMusic`/`PollSound`/`SwitchAmbientMusic`
  were rewritten around it; `CDStartup` now opens an Audiere device.
- **[2.1?/unclassified] `bitmap` ctor/dtor rewrite** (`BASE/BITMAP`): the
  ctors allocate with plain `new` under /GX (EH-framed), the dtor `delete`s
  and null-checks; `CopyTo`/`CopyToCareful` bodies differ substantially from
  PoL (rewrite still being matched).
- **[Buka] FONT line breaking.** `DrawBoundedString` and `LineLength` now
  drive `font::ExtractLine` over a heap scratch buffer instead of PoL's
  in-place truncation loop; centering gained a `+1` bias; the last visible
  line is drawn unwrapped (clip-truncated). `LineWidth` keeps the PoL body,
  including its infinite-loop-on-`'\n'` defect.
- **[Buka] `sample` grew 0x30 → 0x34** (Audiere stream handle).
- **[2.1] Gold 2.1 upstream content**: Archmagi/armageddon strings, cheat
  notices in `advManager::LoadRemote` (established when the branch was
  seeded; classify further entries against the GOG binary).
- **[2.1?/unclassified] Environment sound loop count 0 → 1.**
  `advManager::InsertSound` sets `m_playbackData.loopCount = 1` where the
  2.0 body (PoL, byte-pinned) sets 0. Both sides proven by bytes; classify
  against the Gold 2.1 GOG binary. The same function's bounds guard checks
  `x` against `MAP_HEIGHT` and `mapY` against `MAP_WIDTH` - swapped axes,
  harmless on square maps, present in retail. Companion site:
  `army::LoadResources` writes `loopCount = 0` in this image where the
  PoL line wrote 1 (byte-pinned both sides, 2026-08-03) - the two sites
  moved in opposite directions between versions.
- **[2.1?/unclassified] Visions no longer doubles the join fee.**
  `advManager::DoVisions` computes the diplomacy joining price as
  `gMonsterDatabase[type].cost * count` in this image; the 2.0 body (PoL,
  byte-pinned) multiplies that by 2. Retail bytes at RVA 0x154ac read
  `movsx ecx,word[eax+gMonsterDatabase]; imul ecx,[ebp-0xc];
  mov [ebp-0xf8],ecx` with no `shl ecx,1` - the 2.0 build emits the shift.
  The same function evaluates its strength ratio in **double** precision in
  2.1 (`fild;fild;fdivp`) where 2.0 divides in float (`fidiv`), and
  multiplies `count * fightValue` rather than `fightValue * count`.
  Classify against the Gold 2.1 GOG binary.
- **[Buka] Necromancy eligibility excludes SKELETON, not GHOST.**
  `combatManager::DoVictory`'s stack-count chain compares
  `m_monsterType` against creature 47 (SKELETON) where the PoL 2.0
  bytes compare 59 (GHOST) - the last term of the elemental-exclusion
  chain (62/63/64/65 prefix identical). Byte-pinned both sides
  (2026-08-03). Reads as a dev logic fix: skeletons killed in combat
  no longer feed necromancy.
- **[Buka] The RESMGR read helpers lost their error branches.**
  `ReadByte`/`ReadWord`/`ReadLong` in 2.1 are assert + read + return
  with NO `if (bytesRead == 0)` debug-trap block (retail bodies are
  0x59/0x5c/0x5c bytes; the PoL branch would add 41). Assert line
  numbers also moved (703/732/760/816 -> 675/700/723/775), so the
  source file itself shrank ~30-40 lines between versions.
- **[Buka] The wingraph palette family is `__cdecl`, not `/Gr` fastcall.**
  `UpdatePalette`, `WGUpdatePalette` and `DDUpdatePalette` read their
  `i8*` argument from `[ebp+8]` with no ecx spill, and `UpdatePalette`
  calls the other two with `push`/`add esp,4`. Declaring all three
  `__cdecl` reproduces retail's exact sizes (0x28 / 0x1c1 / 0xee).
  Suspect a wider class - any /Gr TU function whose retail prologue
  lacks the ecx spill is a `__cdecl` candidate.
- **[Buka] `bitmap::CopyTo` / `CopyToCareful` were rewritten as plain
  indexed loops.** PoL's running-offset `do/while` (with `rowCount`,
  `sourceRowOffset`, `destinationRowOffset`) is gone; 2.1 spells both as
  `for (row = 0; row < height; row++)` with the address recomputed each
  iteration as `pixels + x + (y + row) * stride`, and `CopyToCareful`
  opens with a `if (width < 1) return;` guard. `CopyTo` keeps the
  `width != COPY_STRIDE` fast-path test in that polarity (loop first,
  whole-block memcpy in the else). Both byte-exact 2026-08-03.
- **[Buka] `font::GetCharacterWidth` became Cyrillic-aware.** The 2.1
  body routes any code above 0x7f through `RemapCyrillicCharacter`
  (which our source had but NEVER CALLED, so VC6 dropped the
  unreferenced static and the claimed 0xc37a0 span went unpaired), and
  rejects the 0x80..0xbf gap - except 0xa8 / 0xb8, the two stray Yo
  glyphs - to the 0x7f fallback. It also measures `'.'` (0x2e) as the
  underscore glyph where the draw path still uses `FONT_SPACER_CHAR`
  (0x1f). Both functions byte-exact 2026-08-03.
- **[Buka] Rainbow luck bonus applies before the clamps.**
  `game::GetLuck` in 2.1 adds the Sorceress Rainbow bonus BEFORE the
  MIN/MAX clamps and the Battle Garb override; the PoL 2.0 order
  applied it after, letting Rainbow push luck past MAXIMUM. Byte-pinned
  (2026-08-03) - the reorder closed the whole function exact.
- **[2.1?/unclassified] Combat hex validation tightened.** 2.0's
  `ValidHex` accepts hexes 0..125 (PoL byte-proven, `<= 125`); this image
  bounds the grid at `hex < COMBAT_HEX_COUNT` (117). Out-of-grid hexes
  116..125 were previously "valid".
- **[2.1?/unclassified] Event artifact transfer excludes the whole
  ultimate range plus the Golden Goose.** 2.0 keeps artifacts above
  `ARTIFACT_ULTIMATE_WAND` (3) transferable - ultimate shield/staff/crown
  and the goose moved between heroes (PoL byte-proven); this image bounds
  at `<= ARTIFACT_GOLDEN_GOOSE` (7).
- **[Buka] Music configuration** routed through the new `MusicFlags` TU and
  the config musicSource/musicVolume paths in `GAME`/`ADVMGR`/`SMACKMGR`
  (fade timing via `glTimers` slots).
- **[2.1?/unclassified] `SAMPLE2` shrank to one pointer and `NULL_SAMPLE2`
  is gone.** PoL 2.0 initializes every event-sound local by copying the
  8-byte global (`mov eax,NULL_SAMPLE2; mov ecx,NULL_SAMPLE2+4; mov -0x20,eax;
  mov -0x1c,ecx`, e.g. BarrierEvent 0x4aea92, GenericSiteEvent 0x4b0810).
  This image stores a single immediate zero (`c7 45 f8 00000000`) into a
  4-byte slot and never reads a `NULL_SAMPLE2` global anywhere; PoL's
  `InitVars` writes `NULL_SAMPLE2.pSample = NULL` as its first statement,
  retail 2.1's `InitVars` (0x46af49) starts at `gGameCommand = -1`. Modeled
  as `SAMPLE2 sample = {NULL};`. Byte-pinned in BarrierEvent (0x41fbc) and
  JailEvent (0x42b6c); the same delta is pending in the other EVENTS
  sound sites (PasswordEvent, GenericSiteEvent, RecruitSiteEvent, DoEvent,
  FizzleCenter) and in every other TU that still copies `NULL_SAMPLE2`.
- **[2.1?/unclassified] `advManager::CombatMonsterEvent` stack-split
  counter is a scalar, not the array's spare element.** PoL's frame carries
  `placement[MONSTER_ARMY_SLOTS + 1]` and uses `placement[5]` as the running
  army index; 2.1's frame has `placement[5]` (at -0x14) plus a separate int
  at -0x20 that the `+= stackCount` walks. Same total frame, different local
  set. Byte-pinned 2026-08-03 (0x4403b).
- **[2.1?/unclassified] `advManager::ComputerMonsterInteract` combat tail is
  an if/else, and the flee arm returns.** 2.1 spells
  `if (result != 0) *handled = 1; else cell->m_objectMetadata = ...;` and
  ends the flee arm with an explicit `return;` (jump straight to the
  epilogue at 0xce36); the PoL body uses an early `return` in the result arm
  and lets the metadata update fall through. Byte-pinned (0x48730).
- **[2.1?/unclassified] `advManager::DoCombat` defender-side owner is an
  if/else chain.** 2.1 writes `secondPlayer` directly in three arms (no
  ternary temp: only the attacker side keeps the `-0x88` ternary temp);
  PoL nests two ternaries and burns two extra temps (frame 0x9c vs 0x94).
  Byte-pinned (0x48c95).

### SOURCE/TOWNMGR (town screen)

- **[Buka] The garrison crest reads out the calendar.**
  `townManager::SetCommandAndText` (0xa61d0) has a `case 0x74`
  (`TOWN_WIDGET_GARRISON_CREST`, the crest widget that owns the garrison
  strip's first border id) that PoL does not: it prints
  `sprintf(m_statusText, "%s: %d, %s: %d, %s: %d", "\xcc\xe5\xf1\xff\xf6",
  gpGame->m_month, "\xcd\xe5\xe4\xe5\xeb\xff", gpGame->m_week,
  "\xc4\xe5\xed\xfc", gpGame->m_day)` ("Месяц/Неделя/День"). Its presence
  raises the 0x74..0x7a arm from a compare chain to a 7-entry jump table.
- **[Buka] CP1251-aware first-letter upcase.** Both `townManager::SetupWell`
  (0xaacfd) and `townManager::SetupThievesGuild` (0xab344) replace PoL's
  `gText[0] -= ' ';` with a four-way classifier written through one `char`
  local: `'a'..'z'` and CP1251 `0xe0..0xff` subtract 0x20, `0xb8` ("ё") maps
  to `0xa8` ("Ё"), anything else passes through. The reads are all
  zero-extended, i.e. the source compares `(u8)gText[0]`.
- **[unclassified] `townManager::ChangeTown` (0xa5163) reordered and gained a
  MIDI settle.** The ambient-music switch now runs AFTER
  `SetupExtraStuff()/SetupTown()/SetCommandAndText()`, is guarded by
  `gConfig.useOpera != 0 || gConfig.musicSource == MIDI`, and calls
  `Sleep(100)` before `SwitchAmbientMusic` when the source is MIDI. The
  reconstruction previously had the switch first and no sleep.
- **[unclassified] `townManager::RecruitHero` (0xaa0d8) keeps two visit
  flags.** Placing the recruited hero does
  `m_recruitHero->m_eventFlags &= HERO_EVENT_SIRENS | HERO_EVENT_ARENA;`
  (mask 0x600000) instead of clearing the field to `HERO_EVENT_NONE`.
- **[Buka] Seventeen user-facing strings are CP1251 Russian**, verified by
  content hash against the image: the two spell-book dialogs, the mage-guild
  no-room dialog, the boat dialogs, the split-army prompts, the thieves-guild
  column headers ("Ат./Защ./Маг./Зн."), "Доступно:", "\n\nНеобходимо:",
  "Гильдия магов, 2 этаж", "%d этаж Гильдии магов", the hero-summary format
  "%s %d уровня %s. Артефактов: %d.", and the strip labels "героя" /
  "гарнизона".

## Reconstruction infrastructure notes (not version deltas)

- `BASE/Bzip` is Julian Seward's bzip 0.21 (25 Aug 1996) adapted by NWC
  (EncodeData/DecodeData/spotBlock/endsInBz/compress/uncompress wrappers and
  the Free*Structures split are the adaptations; the codec bodies are
  upstream text). Reference source: bzip-0.21/bzip.c (archived copy in
  github.com/simenbkr/CTF, ncsc18/for170). The reconstruction restores the
  original local names; the codec bodies are invariant across PoL and Buka.

- The stripped image's DIR32 sites are recovered by donation
  (`homm2.audit.reloc_donation`): masked-identical claimed functions
  transfer their compiled relocation sites onto retail bytes; unanimous
  (symbol, addend) votes name the data owners and alias interior fields as
  owner+addend. Re-run with `--write` after each matching wave.
- VC6 /Od slot-order model (`homm2/core/od_slots.py`, solved on cl 10.20)
  re-validated on VC6 SP5 by probe: order rule identical; slot offsets
  round each local to 4 bytes.

## Scoring-surface note

objdiff's per-function fuzzy undercounts proof-vocabulary exactness on this
branch: functions whose bytes, claimed size, and ordered relocation rows all
agree can still read 99.9x (e.g. philAI::CombatMonsterEvent, byte- and
reloc-identical, $fnpad boundary present, 99.973). The graded census
(masked-identical class) is the truthful exact count; treat sub-100 rows in
that class as display artifacts, not residuals.

## Bookkeeping

- The 36 no-PoL-counterpart functions harvested from the attempt-1 remap are
  enumerated in `docs/buka-va-queue.tsv` (empty `pol_va` column); the
  additions above cover all of them plus this branch's own recoveries.
- `// @remove` markers in `src/` are the authoritative removed-body list;
  the bitmap ctors left that list when their rewritten forms were recovered.
