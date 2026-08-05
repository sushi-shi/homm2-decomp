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
- **[Buka] `soundManager` shrank from 0x6ae to 0x52 bytes** — everything the
  PoL layer kept past `m_musicTrack` is gone. `InitMainClasses` pushes `0x52`
  at `new soundManager`, the ctor (0x4b5bd0) writes no offset above `+0x4e`,
  the dtor (0x470df0) touches only `+0x42`, and a whole-image census of every
  `gpSoundManager` dereference tops out at `+0x4e`. The removed state resolved
  four ways: `m_samplesReady` is the byte global at 0x5348f1 that this tree
  already spelled `gSoundBackendsReady` (26 sites in ADVMGR/GAME/SMACKMGR);
  `m_currentTrack`/`m_fadeSteps` were never separate from `m_musicTrack`
  (`+0x4e`) and `m_musicFadeSteps` (`+0x4a`); `m_midiReady`/`m_midiStarted`
  became the `BASE/MusicFlags` trio and `m_cdReady`/`m_cdStarted` collapsed
  into `CDStartup()`'s bool result; the rest is read only by the `@remove`
  CD bodies and is parked as file statics in `src/BASE/soundmgr.cpp`.
- **[Buka] The `MIDI*` entry points are free `__fastcall` functions**
  (`include/BASE/Midi.h`), not `soundManager` methods: `MIDIPlay(i32&
  currentTrack, i32& fadeSteps, i32 track)`, `MIDIStop(i32& currentTrack)`,
  `MIDISetVolume(i32& fadeSteps)`, `bool MIDIStartup(void)`,
  `MIDIShutdown(void)`, `bool MIDIIsPlaying(void)`. `PlayAmbientMusic` passes
  `this+0x4e` in ecx and `this+0x4a` in edx, mirroring the Audiere family's
  `PlayAudiereMusic(i32&, i32&, ...)`. `MIDIStartup` publishes the MusicFlags
  globals directly, so those are module globals rather than MusicFlags-private
  statics.
- **[Buka] `gSoundDisabled`** — a byte at 0x5348f0, set by `SetupCDRom` when
  the CD-ROM check fails and read by `CDStartup`/`StartupMilesBackend`. It is
  NOT `gbNoSound` (a dword at 0x52661c, referenced only from `SOURCE/KB`); the
  two were conflated in the PoL-derived bodies.
- **[Buka] `SetupCDRom` lost the `savedNoSound` save/restore** and gained a
  `gpSoundManager->ShutdownSoundBackends()` in each CD-error arm.
- **[Buka] `WAVE_init_driver` is inlined into `StartupMilesBackend`** with the
  `WAVEOUTCAPSA` buffer promoted to a file static (0x5348a8, just before
  `gWaveFormat` at 0x5348e0), `AIL_set_preference(15, 1)` made unconditional,
  and `AIL_waveOutOpen` writing straight into `&m_digitalDriver`.
- **[Buka] `AppWndProc` has no `KBWIN_CUSTOM_CD_MESSAGE` (0x3b9) case** — the
  dispatch cascade tops out at 0x311 and the function carries no
  `gpSoundManager` relocation. `AppInit` still sets
  `bProcessMessage[0x3b9] = 1`.
- **[unclassified] `advManager::DoAdvCommand`'s `ADVMGR_COMMAND_HERO_VIEW` arm
  has no low-memory environment-origin dance.** The PoL body brackets
  `HeroView(...)` with `if (gbLowMemory) SetEnvironmentOrigin(-1,-1,1);` before
  and `if (gbLowMemory) SetEnvironmentOrigin(mapOriginX+7, mapOriginY+7, 1);`
  after. Retail's case is 21 straight instructions
  (`SetPointer(0); TrimLoopingSounds(0); HeroView(currentHero,0,0);
  RedrawAdvScreen(1,0); FadeScreen(FADE_IN,8,NULL);`) with no `gbLowMemory`
  relocation anywhere in the function. Removing both blocks closed the function.
- **[unclassified] `advManager::SetEnvironmentOrigin` has no `LogInt` tracing.**
  The reconstruction carried `LogInt("SEO a", edgeOffset, -999 x6)` and
  `LogInt("SEO b", ...)` inside the active-sound sweep; retail's body has zero
  `LogInt` relocations and its `for` head goes straight into the
  `m_activeSounds[i].soundId != -1` test.
- **[unclassified] `SystemOptionsHandler`'s `SYSTEM_OPTION_VIDEO` arm has no
  low-memory refusal.** PoL shows `if (gbLowMemory) { NormalDialog("You don't
  have enough memory for non-interlaced video.", ...); break; }` ahead of the
  toggle; retail goes straight to
  `gConfig.slowVideo = gConfig.slowVideo == 0;` (`xor edx,edx; cmpl $0,
  slowVideo; sete dl; movl %edx, slowVideo`) — also an assignment, not the
  PoL if/else pair of stores.
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

- **[Buka] `SetupCDDrive` (0xbf370) probes for the OGG music tracks, not a CD
  audio device.** The PoL/2.1 body drives MCI (`open %c: type cdaudio alias CD`,
  `info CD UPC wait`, `close CD`) and validates the disc through
  `%c:\heroes2\anim\voy24.smk`. Buka's body has no MCI at all: it checks the
  data files with `sprintf(gText, "%sHEROES2x.AGG", ".\\DATA\\")` (note the
  expansion aggregate, and the two-literal `%s` + tail split), and for each
  candidate drive it opens `<drive>` + a file-static
  `"\\Tracks2\\02-AudioTrack 02.ogg"` (0x11e5dc) - the ripped soundtrack Buka
  shipped in place of CD audio. The remembered `HMM2POL CDDrive` registry value
  is written from `gcRegCDRomPath` (which the body itself fills with
  `sprintf(gcRegCDRomPath, "%c:", ...)`), and `gcAnimPath` is never rewritten.
  `DriveSupportsFreeSpaceQuery` gates both the remembered-drive fast path and
  every scanned drive; the retry loop is a plain
  `for (attempt = 0; attempt < 2; ++attempt) { ...; Sleep(3000); }` returning 2
  when it runs out. Byte-exact.
- **[unclassified] `WritePrefsToFile` (0xbed60) has no zero-scratch prologue.**
  The whole body is `sprintf(gText, "%s", "HEROES2.CFG");
  fd = open(gText, ...); if (fd == -1) return; write(fd, &gConfig, 0x19d);
  close(fd);` - one 4-byte frame slot (`push ecx`), no `i32 zeroBuffer[25]`
  clearing loop. Byte-exact.
- **[unclassified] `SetGameDefaults` (0xbe0a0) seeds the map offset and the
  unique system id from `Random`, not `rand`.** `gConfig.firstMapOffset =
  Random(0, 32000)` (fastcall, not `rand() % 32001`), and each of the three
  `uniqueSystemID` characters advances one accumulator with
  `seed += Random(1, 999999) + KBTickCount();` before indexing a local
  `"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"` pointer. The id bytes are cleared
  with `memset(gConfig.uniqueSystemID, 0, 4)`. The graphics defaults run as a
  real `for (i = 0; i < 2; ++i)` over `gConfig.gfx[i]`, and
  `gConfig.blackoutComputer` is written 0 twice (once before the loop, once
  after). Byte-exact.
- **[unclassified] `executive::DoDialog` (0xc50d0) builds a real nested
  `executive` on the stack.** The body declares `executive ex;` (its default
  ctor runs first, before anything else), adds the mouse/window managers and
  the dialog manager to `ex`, runs `ex.MainLoop()`, and returns `ex.m_result`.
  The outer executive's list is saved into three parallel
  `baseManager*[20]` arrays and restored afterwards with a single counter.
  Byte-exact.
- **[unclassified] `dropListWidget`'s four press/track flags are unsigned
  bytes.** `m_scrollUpPressed`, `m_scrollDownPressed`, `m_scrollThumbDragging`
  and `m_itemSelectionTracking` (0xac..0xaf) are read with `/G5`'s
  `xor r32,r32` + `mov r8` zero-extension in `DrawDropStuff` and
  `ProcessSelectDialog`, which pins them as `u8` rather than `char`
  (`docs/patterns/unsigned-byte-flag-zero-extend.md`).

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
  strings and `Terminated` stay English. Seven more ADVMGR literals joined
  the list on the 2026-08-04 harvest band: `SummonBoat`'s failure notice
  (0xf0094 `Не удалось призвать корабль!!!`), `CPanelHandler`'s three
  restart/load/quit confirmations (0xef408/0xef44c/0xef494) and
  `SystemOptionsHandler`'s MIDI+Redbook, digital-sound and CD-stereo notices
  (0xf0260/0xf029c/0xf02c8). The restart confirmation has two retail cells
  (`_0` and `_1`); ADVMGR's own second user of that text is the
  `INPUT_SCAN_N` cheat arm of `advManager::Main`, still English in source.
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
- **[Buka] `ShutDown` no longer stops the CD.** PoL 2.0 calls
  `gpSoundManager->CDStop()` between `ShutDownSmacker()` and
  `ClearMapExtra()`; 2.1/Buka drops the call entirely (`homm2 relocs 0x6cc61`
  reported it as the unit's only only-base reference). The farewell string is
  localized: `"Bye!"` -> `"\xcf\xee\xea\xe0!"` ("Пока!"). Byte-pinned
  (0x6cc61).
- **[Buka] `CheckEndGame` lost two dead 100-byte text buffers.** PoL's frame
  carries three `char[100]` locals; this image has exactly one (the artifact
  name) plus the `char[20]` campaign save name, and the frame is 0xfc where
  PoL's is far larger. Same local COUNT (33), different composition. All 23
  user-facing strings are CP1251 Russian. Byte-pinned (0x69899).
- **[Buka] End-game / player-exit / net-box user text is localized.**
  `CheckEndGame` (23 strings), `ReceiveHostReportsPlayerExit` (6),
  `ReceiveRemotePlayerExit` (5), `PopNetBox` (1), `ShutDown` (1) and
  `AddScoreToHighScore` (1) all carry CP1251 Russian text where PoL has
  English. Two are semantic, not just translated: the save-game name
  `"PLYREXIT"` became `"\xc8\xe3\xf0\xee\xea \xc2\xfb\xf8\xe5\xeb"`
  ("Игрок Вышел"), and the campaign autosave prefix `"WIN_"` became
  `"\xcf\xce\xc1\xc5\xc4\xc0_"` ("ПОБЕДА_"), so both save files are named
  in Russian. `ReceiveHostReportsPlayerExit`'s timed-out and exited host
  reports were also collapsed onto the same sentence (two identical
  duplicate cells in the unit's literal block).
- **[Buka] `soundManager` is 82 bytes (0x52).** `InitMainClasses` allocates
  `new soundManager` with `push $0x52`, and the retail constructor
  (`??0soundManager@@QAE@XZ`, 0x4b5bd0) writes no offset above 0x4e. Every
  member our `include/BASE/soundManager.h` models past `m_musicTrack` (the
  Miles sample-handle tables, MIDI track positions, CD/aux state) is
  therefore NOT in this image's class; those fields live elsewhere. Pinned by
  `InitMainClasses` (0x65d1c), which is otherwise byte-identical and is held
  at 3 bytes by the wrong `sizeof`.

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

- **[Buka] Combat damage messages rebuilt around CP1251 case folding**
  (`army::SpecialAttack` 0x1a1d9, `army::DoAttack` 0x1bddb,
  `army::DoHydraAttack` 0x1b858). PoL's `"%s %s %d %s.\n%d %s %s."` with a
  singular/plural attacker name and a `gText[0] -= 32` capitalisation became
  `"%s %s %s %d %s.\n%d %s %s."` — a leading literal "Атака", the attacker's
  name **always** plural (`gArmyNamesPlural[m_monsterType]`, no
  `m_quantity > 1` select), "наносит", the damage, "ед. урона", the kill count,
  and a defender name/verb pair selected on `killed <= 1`. Two new helpers
  replace the bare `-= 32`: inline CP1251 `ToUpperCp1251` / `ToLowerCp1251`
  fold `a`-`z`, `а`-`я` (0xE0-0xFF) and `ё`/`Ё` (0xB8/0xA8) — each expansion
  gets its own byte slot in the caller frame. The defender's singular name is
  lower-cased into a new 100-byte file-static scratch buffer at VA 0x00524038
  (`gTargetName`, referenced only by `SOURCE/ARMY`, 22 sites). The genie-half
  message keeps a `m_quantity <= 1` select but pairs the *singular* attacker
  name with the *plural* verb "уничтожают" (and vice-versa) — a translation
  slip preserved in the bytes.
- **[Buka] `SOURCE/ARMY` combat text buffers are 800 bytes**, not 200:
  `army::SpecialAttack`, `army::DoAttack` and `army::DoHydraAttack` each
  reserve `char[800]` for the `strcpy(text, gText)` copy (frames 0x43c, 0x38c,
  0x358).
- **[Buka] `toupper` (`SOURCE/KB` 0x67c38) folds CP1251 Cyrillic.** Beyond
  `'a'..'z'` it uppercases 0xE0..0xFF (а..я) by subtracting 0x20 and maps the
  single 0xB8 (ё) to 0xA8 (Ё). The parameter stays `char`; the body reads it
  through `(u8)` casts, which is what widens its /Od home slot to a dword
  (`docs/patterns/uchar-cast-widens-param-spill.md`).
- **[Buka] A second, inline copy of the same fold lives in a header.**
  `/Ob1` expands it in `oldmain`, `game::GetSideDesc` (twice), `ShowCongrats`,
  `SOURCE/GAME`, `BASE/FONT`, `SOURCE/ARMY`, `Overview`, `REQUEST`, `SPELLS`
  and `TOWNMGR`; only `InterpretCommandLine` calls the out-of-line `toupper`.
  The inline copy differs in one detail — it subtracts from the masked value
  (`(u8)c - ' '`), the out-of-line one from the sign-extended byte. Modelled
  as `CyrillicToUpper` in `include/SOURCE/KB.h`; the sites it replaced in our
  reconstruction had been written as `x -= 'a' - 'A'`, which is not the retail
  fold at all (it corrupts every non-ASCII first letter).
- **[Buka] `InterpretCommandLine` (0x67ca8) dropped five switches.** The
  `/M`, `/R`, `/D`, `/S` and `/X` options (gbDontTryMIDI, gbDontTryRedbook,
  gbDontTryDigital, gbNoSound, xSmackFromNetwork) are gone from the option
  switch, and with `/S` gone the `gbNoSound = false` initialiser at the top of
  the function went too. `/Z /W /V /N /I /P /T` remain.
- **[Buka] `oldmain` (0x66767) plays two extra Buka videos.** The intro is a
  four-term chain `PlaySmacker(0x49) && PlaySmacker(0x42) && PlaySmacker(1)`
  guarding `PlaySmacker(0x41)` (PoL had one guard and two videos), and the
  credits case plays 0x48, 0x24 **and** 0x4a.
- **[unclassified] `oldmain`'s remote player re-match keys on
  `SNetPlayerInfo::uniqueSystemID`, not `.name`.** Both the `strcmp` against
  `game::m_defaultPlayerNames[i*4]` and the `strcpy` back into it use offset 0
  of the 34-byte record — the 4-byte, 3-character system id whose length the
  guard `strlen(...) == 3` checks — while our reconstruction had used the
  21-byte `.name` at offset 4.

### SOURCE/GAME (turn/map setup, army and spell views)

- **[Buka] `GenerateStandardFileName` (0x44d202) folds CP1251 Cyrillic.** The
  PoL body only upper-cased `a`-`z` and only accepted `A`-`Z`, `0`-`9`, `_`
  into the eight-character save base name. The Buka body adds the CP1251
  Russian alphabet: it upper-cases 0xE0-0xFF by 0x20 and maps 0xB8 (`ё`) to
  0xA8 (`Ё`), and accepts 0xC0-0xDF and 0xA8 as name characters. The character
  local is `unsigned char` (retail reads it with the `/G5` `and reg,0xff`
  widening), which PoL's `char` was not.
- **[Buka] `game::ViewArmy` (0x455529) upper-cases the army name's first
  letter through the same CP1251 fold** instead of PoL's unconditional
  `armyName[0] -= 'a' - 'A'`; `game::DoNewTurn` (0x45e8fe) lower-cases the
  creature name through the mirrored fold (0xC0-0xDF += 0x20, 0xA8 -> 0xB8) in
  both the new-month and new-week branches.
- **[Buka] `game::ViewArmy` drops the low-memory art path.** PoL chose
  `sprintf(filename, "monh%04d.icn", type)` under `gbLowMemory`, forced the
  widget frame to 0, and overrode the monster origin with the low-memory
  coordinates. Buka always uses `cMonFilename[type]`, always passes the walk
  frame, and has no low-memory origin override. `ViewArmyHandler` (0x456231)
  likewise animates on `glTimers[0] < KBTickCount()` alone, with no
  `!gbLowMemory` guard.
- **[Buka] `game::RandomizeEvents` (0x4513cf) reorders the artifact-guardian
  table.** PoL filled `arr[6..9]` with paladin/crusader/cyclops/genie and
  `arr[0..5]` with the dragons, then indexed minor guards at `+6` and major at
  `+0`. Buka fills `arr[0..3]` with the small guards and `arr[4..9]` with the
  dragons, indexing minor at `+0` and major at `+4`. Semantics are unchanged;
  only the layout and the two base constants moved.
- **[2.1?] `game::RandomizeEvents` castle metadata footprint is one row
  taller** — the ownership stamp loop runs `y-3 .. y+1` instead of PoL's
  `y-2 .. y+1` (`CASTLE_METADATA_TOP_OFFSET` 2 -> 3). [unclassified]
- **[Buka] `game::GetRandomArtifactId` (0x45a9e8) gates the Anduran set on the
  expansion campaign.** PoL rejected `ARTIFACT_BREASTPLATE_ANDURAN`,
  `BATTLE_GARB`, `HELMET_ANDURAN`, `SWORD_ANDURAN` and `SPHERE_NEGATION`
  unconditionally; Buka rejects them only when `xIsPlayingExpansionCampaign`
  is set, so ordinary maps can now roll them.
- **[Buka] `game::CountShrines` (0x460c8c) counts necromancer taverns under a
  hero as well as on the tile.** PoL only inspected cells whose trigger is
  `MAP_OBJECT_CASTLE`. Buka additionally resolves
  `MAP_OBJECT_HERO_INTERACTION` cells through
  `gpGame->GetHero(cell->m_objectMetadata)` and uses the hero's
  `m_locationType`/`m_occupiedTown` when the hero is standing in a town.
- **[Buka] `game::SetupNewRumour` (0x45fdd7) guards the ultimate-artifact
  rumour.** When `m_ultimateArtifactId == -1` the final branch falls back to
  `cRandomTavernText[(giCurTurn / 7) % 8]` instead of naming artifact -1.
- **[Buka] `game::UpdateSpellWidgets` (0x4548cf) clears the message struct and
  re-stamps `type` per broadcast.** The body opens with
  `msg.type = MESSAGE_WIDGET; ...; memset(&msg, 0, sizeof(msg));` and then sets
  `msg.type` again before *every* one of its fifteen `BroadcastMessage` calls;
  it also drops PoL's read-only spell-icon flag branch, so the icon and text
  widgets are always enabled + drawn.
- **[Buka] `ViewSpellsHandler` (0x454d5e) refreshes the window even when the
  spell page is already at the top** — the `VIEW_SPELL_PREVIOUS_ID` case calls
  `UpdateSpellWidgets()` + `MoveWindow(0,0)` in both arms.
### SOURCE/X_CAMPGN (expansion campaign screen)

- **[Buka] The bonus/award panel text is CP1251 Russian.**
  `ExpCampaign::UpdateInfo` (0xb3e05) spells all fifteen campaign-bonus
  artifact names, the spell-scroll suffix, the puzzle/experience bonus names
  and the "no awards" placeholder in Russian: "Малый свиток", "Кольцо мага",
  "Щлем защитника", "Топор силы", "Драконий меч", "Доспехи", "Символ
  неудачи", "Громовая палица", "Перчатки", "Свиток высш. зн.", "Баллиста",
  "Незримый щит", "Башмаки кочевника", "Башмаки путника", "Призвать земляных
  эл.", "Обрывки карты", "Опыт", "н/д", "Свиток", "Нет". Two are semantic,
  not just translated: **Major Scroll and Minor Scroll share one cell**
  ("Малый свиток" is pushed for both `ARTIFACT_MINOR_SCROLL` and
  `ARTIFACT_MAJOR_SCROLL`), and the resource bonus is formatted `"%s: %d"`
  with the resource name first, not `"%d %s"`.
- **[Buka] The creature bonus is not capitalised.** `UpdateInfo`'s
  `CAMPAIGN_CHOICE_CREATURES` arm is `strcpy(armyName, gArmyNamesPlural[...])`
  followed straight by `sprintf(gText, "%d %s", ...)`; the `armyName[0] -=
  'a' - 'A'` upper-casing step the reconstruction carried has no bytes in
  this image (the Russian plural names are already capitalised).
- **[Buka] Abbreviated secondary-skill levels.** The special-bonus arm of
  `UpdateInfo` indexes a three-entry X_CAMPGN-local table at VA 0x0051b800
  ("1 ступ.", "2 ступ.", "3 ступ.") rather than the game-wide
  `gSecondarySkillLevels` ("1 ступени"...), which is what the ordinary arm
  still uses. The abbreviation exists because the Russian names no longer fit
  the bonus widget.
- **[Buka] No low-memory replay button.** `ExpCampaign::ShowInfo` (0xb3aba)
  enables only `CAMPAIGN_DIALOG_RESTART` (widget 0x385, under
  `viewOnly == 0`). The `gbLowMemory`-guarded `CAMPAIGN_DIALOG_REPLAY`
  (widget 0x386) block the reconstruction carried is absent, and both dialog
  prompts are Russian ("Вы действительно хотите начать сначала сценарий?",
  "Выбранная карта - плохой выбор для вашего следующего сценария.").

### BASE/INPUTMGR (keyboard/mouse input)

- **[Buka] Enter is force-mapped to its scan code.** Both `WM_KEYDOWN` and
  `WM_KEYUP` arms of `KeyboardMessageHandler` (0xbbf40) open with
  `if (virtualKey == VK_RETURN) event->payload.keyboard.keyCode =
  INPUT_SCAN_ENTER; else <HIWORD(lParam) & 0xff>` — the wParam virtual key
  overrides the lParam scan byte for Enter only.
- **[Buka] Cyrillic layout remap on the ASCII result.**
  `TranslateInputCharacterCp1251` (0xbcad0, a file static) rewrites
  `event.payload.keyboard.keyCode` through a 128-byte table at VA 0x0051e51c
  whenever the code is in `[0, 0x80)`, and `AsciiConvert` calls it whenever
  no control-class modifier is held. The table is identity below 0x22 and
  maps the ASCII letter/punctuation range onto the CP1251 codes of the
  standard ЙЦУКЕН layout (`A`->0xd4 "Ф", `B`->0xc8 "И", `"`->0xdd "Э"). No
  PoL counterpart.
- **[unclassified] `SetKeyCodeType` calls `Flush()`.** 0xbcaa0 stores
  `m_keyCodeType` and then makes a real `__thiscall` to
  `inputManager::Flush`, not an inline ring reset.
- **[unclassified] `ForceMouseMove` tests its re-entrancy flag twice.**
  0xbd300 emits `cmp gpInputManager->m_mouseMessageActive, 0 / je / jmp`
  twice in a row before claiming the flag - a duplicated guard preserved in
  the bytes.
- **[unclassified] `AsciiConvert` shift-map falls through to the Polish
  remap.** Each shifted-symbol case `break`s out of the switch; retail then
  still runs the `(modifiers & MESSAGE_MODIFIER_CONTROL_KEYS) == 0` test and
  calls `TranslatePolishInputCharacter`. The reconstruction's `return` in
  every case skipped it.

### SOURCE/SPELLS + SOURCE/CMBTMGR (combat spells and the combat manager)

- **[unclassified] `CombatSpecialHandler` (0x98bc1) does not call
  `heroWindowManager::ConvertToHover`.** The spell-book hover handler goes
  straight from `message.type == SPELL_MESSAGE_HOVER` to the
  `m_lastHoverId` compare; PoL's body converts the message first. Pinned by
  the two ours-only instructions (`push message; mov ecx,[gpWindowManager];
  call ConvertToHover`) that the retail span has no room for.
- **[unclassified] `combatManager::KeepAttack` (0x29990) has no trailing
  `LogStr("KA2")`.** The opening `LogStr("KA1")` is present; the retail body
  ends `WaitEndSample(&sample,-1); mov esp,ebp; pop ebp; ret 4`. The sibling
  `CatAttack` keeps both `"CA1"` and `"CA2"`.
- **[unclassified] `combatManager::ShowSpellMessage` (0xa162e) always uses
  `gArmyNamesPlural`.** There is no `m_quantity > 1` selection: the body is
  `if (target != NULL) sprintf(targetName, gArmyNamesPlural[type]);`. The
  singular/plural choice IS present in the two sibling messages
  (`KeepAttack`, `CastSpell`), so this is a behaviour difference, not a
  reconstruction shortcut.
- **[unclassified] `combatManager::CastSpell` (0x99653) guards the
  `SPELL_MAGIC_ARROW` case body with a literal-true condition.** Retail emits
  `mov eax,1; test eax,eax; je <case end>` before the case body, i.e. a build
  switch the shipping tree left permanently on. See
  `docs/patterns/constant-true-case-guard.md`.
- **[unclassified] `combatManager::ChainLightning` (0x9df96) passes both bolt
  angle limits as `firstBolt ? A : A` ternaries whose arms are equal.** The
  two `DoBolt` angle arguments are computed with `neg/sbb/and $0/add K` from
  the `firstBolt` flag; the intended first-bolt/branch-bolt split exists in
  the source and was neutralised by giving both arms the same value. See
  `docs/patterns/identical-arm-ternary.md`.
- **[unclassified] `combatManager::MirrorImage` (0x9f7c0) skips six
  (facing, direction, sourcePart, distance) combinations.** Between computing
  `searchDirection` and walking the ray, retail runs six four-term guards that
  `continue` the direction loop for
  (RIGHT, EAST, 0, 1), (RIGHT, WEST, 0, 1), (RIGHT, WEST, 1, <=2),
  (LEFT, WEST, 0, 1), (LEFT, EAST, 0, 1) and (LEFT, EAST, 1, <=2) - the hex
  cells a wide creature's own body would occupy. 62 instructions the
  reconstruction was missing outright.
- **[unclassified] `SOURCE/CURSOR` dropped the walk-sample bookkeeping.** PoL
  2.0's `advManager::DrawCursor` kept the last two footstep voices in
  `hOldWalkSample` / `hNewWalkSample` (`if (m_cursorFrameCount == 0)
  hOldWalkSample = hNewWalkSample;` and `hNewWalkSample =
  gpSoundManager->MemorySample(...)`), and `StopCursor` cleared both. In this
  image none of the three statements exists: `DrawCursor` (0x334da) passes the
  sample straight into `MemorySample` (`mov edx, 0x352(%ecx,%eax,4); push edx;
  call ?MemorySample@soundManager@@QAEXPAVsample@@@Z` — note the `X` return,
  i.e. `void`, where PoL's returned a voice id) and `StopCursor` (0x333f4)
  stops after `EveryOther = 0`. Both globals are now write-only in the TU.
- **[unclassified] `EveryOther` narrowed from `int` to `unsigned char` and is
  toggled with `!`.** Every retail reference is a byte access
  (`xor ecx,ecx; mov cl, byte ptr [EveryOther]` at 0x334da+0x4a4 and
  0x340ca+0x81a, `movb $0x0` in `StopCursor`), and the toggle is
  `neg ecx; sbb ecx,ecx; inc ecx` — logical NOT, not PoL's `1 - EveryOther`
  (`mov eax,1; sub eax,...`). Declared `u8 EveryOther` in
  `include/SOURCE/advManager.h` on this branch.
- **[unclassified] `advManager::MoveHero`'s south-west deferred-draw arm adds
  the Y delta.** In the `directionX == -1 && directionY == -1` arm retail reads
  `add eax, -0x44(%ebp)` (directionY) where PoL 2.0 reads directionX
  (`giDeferObjDrawX = movingHero->m_x + directionX_b;`). Both deltas are -1
  inside that arm, so the runtime result is unchanged; the source line differs.
- **[unclassified] `searchArray::SeedCombatPosition` gates on `shots`, not
  `speed`.** Both of its "already surrounded" tests read
  `movsbl 0xca(%ecx), %edx` = `m_monster.shots` (monsterInfo + 0x10), where PoL
  2.0 reads `m_monster.speed` (0xc5 = monsterInfo + 0xb): retail is
  `if (unit->m_monster.shots > 0 && GetAttackMask(...) == ATTACK_MASK_SURROUNDED)`.
  A shooter, not a fast unit, is what makes the enemy hex reachable without a
  path.
- **[unclassified] `CastleHandler`'s quick-view test reads the RIGHT MOUSE
  BUTTON, not left shift.** Retail masks `0x200`
  (`MESSAGE_MODIFIER_RIGHT_BUTTON`) where the PoL 2.0 reconstruction masks
  `0x2` (`MESSAGE_MODIFIER_LEFT_SHIFT`): `andl $0x200, %ecx` at the
  `WIDGET_COMMAND_SELECT` arm of `CastleHandler` (0x25230). The whole
  castle-screen "quick info" path is therefore a right-click in this build.
- **[unclassified] `PlaySmacker` no longer runs the video speed test.** In the
  `gConfig.slowVideo == VIDEO_SPEED_TEST` branch retail clears the flag, calls
  `WritePrefs()`, stores `0` into one dword global and goes straight to the
  `smksum` threshold test: the PoL sequence `bSmackNum = SMACK_EARTH;
  bTesting = 1; SmackManagerMain(); bTesting = 0;` is gone, so `smksum` is
  whatever the previous playback left behind. The threshold test also lost its
  `|| gbLowMemory` term (retail's chain is two `||` terms, ours was three).
- **[unclassified] `searchArray::SeedPosition` keeps a second, dead
  `FindAdjacentMonster` call.** After the guard
  `if (!findAdjacentMonster || s_currentNode.rvFlag1) goto point_complete;`
  retail tests `s_currentNode.rvFlag1` again and, in the (unreachable) true
  arm, calls `FindAdjacentMonster(x, y, &ax, &ay,
  s_currentNode.adjacentMonsterX, s_currentNode.adjacentMonsterY)` and jumps to
  `point_complete` when it succeeds; the false arm is the `-1, -1` call the PoL
  reconstruction has. 33 instructions of retail code with no reachable effect.
- **[unclassified] `searchArray::SeedPosition` reads the object tileset
  bitfield.** `(m_objTypeBits & 0xfc) != 0xbc` in the PoL reconstruction is
  `m_objectTileset != TILESET_DUMMY` here (`shrb $2 / andb $0x3f / cmp 0x2f`),
  matching `TestPossibleDirections`. `SEARCH_OBJECT_TYPE_MASK` and
  `SEARCH_BLOCKING_OBJECT_TYPE` lose their last users.
- **[unclassified] Thieves' guild personality widget geometry.** The
  personality `textWidget` in `townManager::SetupThievesGuild` is 28 pixels
  high at y=393 (`pushl $0x1c` / `pushl $0x189`); the PoL reconstruction used
  16 at y=397. The 397 constant survives in the same function as the value of a
  dead `i16` local, so the widget geometry, not the constant, moved.

### SOURCE/REQUEST (file requester) + SOURCE/DRAWING (combat drawing)

- **[Buka] `fileRequester::Main` (0x8f737) accepts CP1251 Cyrillic in the
  filename field.** The character-class chain that sanitises the typed name
  gained four terms between the ASCII digit test and the `'_'` test:
  `c >= 0xC0 && c <= 0xDF` (А-Я), `c >= 0xE0 && c <= 0xFF` (а-я),
  `c == 0xA8` (Ё) and `c == 0xB8` (ё). Every read of the buffer is
  zero-extended (`xor eax,eax; mov al,[ebp+edx-0x160]`, and
  `mov edx,[ebp-0x160]; and edx,0xff` for index 0), i.e. the buffer is
  `unsigned char` in retail — the trailing-space trim loop and the
  `newNameData[0] > ' '` test read it unsigned too. 30 instructions the
  reconstruction was missing outright.
- **[Buka] Five `fileRequester::Main` dialog strings are Russian.**
  0x516b74 "Выберите из списка или нажмите кнопку отмены.",
  0x516ba4 / 0x516bd4 the two "no maps of that size" variants,
  0x516c2c "Выбранная вами игра рассчитана только на %d человек. ...",
  0x516c90 "Выбранная игра начнется с %d игроками-людьми. ...". The PoL
  English wording is kept as a comment beside each escape sequence.
- **[unclassified] `fileRequester::Main` binds the scroll-gutter and
  scroll-knob widget ids the other way round from the reconstruction.**
  Retail's dispatch table sends id 3 (`FILE_REQUESTER_SCROLL_GUTTER`) to the
  "jump the list to the clicked pixel" body and id 14
  (`FILE_REQUESTER_SCROLL_KNOB`) to `DoKnob()`. See
  `docs/patterns/jump-table-entry-swap-names-case-labels.md`.
- **[unclassified] `fileRequester::InitializeFiles` (0x8e836) keeps the
  extension in a 5-byte buffer.** The frame gives `extension` exactly 8 bytes
  between `findFileData` (-0x454) and `indexData` (-0x460), i.e.
  `char extension[FILE_REQUESTER_EXTENSION_SIZE]` (5, padded to 8) — the same
  width as `m_extensions[i].text`, not the 208-byte scratch the
  reconstruction had.
- **[unclassified] `combatManager::CombatMessage(char*, int, int, int)`
  (0x37e48) copies its argument into a 400-byte file-static first.** The body
  opens `strcpy(gCombatMessageText, message);` and every later use — the
  `NoShowCombatLog` call, both `strcpy`s into the member lines, `FindToken`,
  the `newlinePtr > gCombatMessageText` bound — reads the static, never the
  parameter. The static lives at 0x005242fc and is referenced only from this
  function. The wrapper writes a temporary NUL over the embedded newline, so
  the caller's string literal could not be used in place.
- **[unclassified] `combatManager::CombatMessage(CombatMessageCommand)`
  (0x381ef) handles `COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL` (15) with the
  same body as `COMBAT_MESSAGE_COMMAND_SHOOT` (3), and both the ATTACK and
  SHOOT messages format `gArmyNamesPlural`, not `gArmyNames` (the VIEW_INFO
  message still uses the singular table). The retail dispatch table is 16
  entries (`cmp ...,0xf; ja`), the reconstruction's was 14.
- **[Buka] `SetupRecruitWin` (`SOURCE/RECRUIT` 0x8c330) recruits by plural name,
  lower-cased.** It calls `GetMonsterPluralName`, not `GetMonsterName`, and then
  folds the leading letter DOWN (`'A'..'Z'` and CP1251 `0xc0..0xdf` by +0x20,
  `0xa8` (Ё) to `0xb8` (ё)) before `sprintf(label, "%s %s", "\xcd\xe0\xed\xff\xf2\xfc"
  /* Нанять */, name)`. PoL 2.0 raised the first letter (`name[0] -= 32`) for the
  English "Recruit %s". The fold reads the buffer through `(u8)` casts and stores
  the result through a `char` local, so it is an if/else chain over a byte slot,
  not a ternary; the monster-name buffer is `char[40]`, not `char[20]`.
- **[Buka] `game::ProcessIconSelect` (`SOURCE/Overview` 0x7c9b3) opens the artifact
  viewer.** The kingdom-overview artifact click calls
  `hero::ViewArtifact(artifact, quickView, m_artifactExtra[slot])` where the PoL
  body called `NormalDialog(gArtifactDesc[artifact], ...)`. The magic-book special
  case (`ViewSpells`) is unchanged.
- **[Buka] `game::ShowScenInfo` / `game::InitNewGameWindow` race-name width is a
  three-way select on player count.** `playerCount < 5 ? 26 : playerCount < 6 ? 16
  : 0`, lowered as a branchy outer `?:` over a `setge/dec/and 0x10` inner one, and
  the race-name text widget is 24 pixels high (the reconstruction had 12).
- **[Buka] `oldmain` (`SOURCE/KB` 0x66767) re-dispatches a pending menu command.**
  After `game_setup_complete` the retail body is `if (giMenuCommand != -1) goto
  process_menu_command;` (jumping back INTO the `if (giMenuCommand != -1)` guard,
  at the switch itself), then `if (!quit) { ... }` and `if (gbGameOver) { ... }` as
  structured ifs; the two campaign-win arms `goto` a label inside the gbGameOver
  block, so a won campaign scenario reaches the victory sequence whether or not
  `gbGameOver` is set. The reconstruction had `goto game_finished` early exits.

- **[unclassified] `advManager::LoadRemote` (0x412c7a) ends with two one-shot
  warning dialogs the reconstruction did not have.** After
  `gSoundBackendsReady = 1;` retail adds
  `if ((i8)gpGame->m_cheated) { static i32 once = 0; if (!once) { once = 1;
  sprintf(gText, "\xc8\xf1\xef\xee\xeb\xfc\xe7\xf3\xfe\xf2\xf1\xff \xf7\xe8\xf2-\xea\xee\xe4\xfb!\n");
  NormalDialog(gText, NORMAL_DIALOG_INFO, ...); } }` and the same shape for
  `giDebugLevel > 0` with `"Someone has their debug level set!\n"`. The two
  guard flags are the unnamed .data cells 0x123ffc and 0x124000 (function
  statics: no mangled name, so the delinker calls them `const_...`). Byte-exact.
- **[unclassified] `advManager::Main` (0x4027cf) drops two things the
  reconstruction carried.** The `WM_TIMER`-style forced-music guard is
  `if (gConfig.musicVolume != CONFIG_VOLUME_MUTED && giForceSwitchMusic > 0
  && KBTickCount() - giForceSwitchMusic > FORCED_MUSIC_DELAY)` — there is no
  `!gbNoSound` term — and the F9 resource cheat is one compound assignment
  `gpCurPlayer->m_resources[c] += (c == IDX(RES_GOLD) ? CHEAT_GOLD_AMOUNT
  : CHEAT_RESOURCE_AMOUNT);` (a `neg`/`sbb` select), not an `if`/`else` pair.
  `giCheatSeq` accumulates as
  `giCheatSeq * CHEAT_SEQUENCE_RADIX % CHEAT_SEQUENCE_MODULUS + c`.
- **[unclassified] `advManager::ProcessDeSelect` (0x404308) has no
  `gbLowMemory` handling in its `PANEL_OVERVIEW` arm.** All three
  `if (gbLowMemory) SetEnvironmentOrigin(...)` calls the reconstruction had
  (before `TrimLoopingSounds`, inside the hero-return arm, and as the trailing
  `else if`) are absent; the arm is just
  `TrimLoopingSounds(0); gpGame->Overview();` then the two return-action arms.
- **[unclassified] `Process1WindowsMessage` (SOURCE/kbwin, 0x4718f9) services
  sound unconditionally.** Retail has no `if (gbNoSound == 0)` around
  `gpSoundManager->ServiceSound()`.
- **[unclassified] `AppWndProc` (SOURCE/kbwin, 0x471248) `WM_TIMER` does not
  call `UpdateTimers`.** The arm is
  `lTemp = KBTickCount(); if (lTemp > lLastGTimerTickCount + 5)
  lLastGTimerTickCount = lTemp; return 0;` — the `UpdateTimers(0)` call the
  reconstruction had is not in the image.
- **[unclassified] `advManager::QuickInfo` (0x409363) carries several arms the
  reconstruction lacked.** The terrain arm prints
  `"%s\n%s"` with a can-dig/cannot-dig annotation chosen from the cell's
  object/overlay/ground state; the barrier and traveller-tent arm is guarded by
  `HAS(m_triggerType, MAP_TRIGGER_ACTION_FLAG)` and falls back to the terrain
  name; the mine arms print only `"%s"` (no `" Mine"` suffix); the resource arm
  indexes `gResourceNames[m_objectIndex / 2]` with no `& ~1` mask; and the
  saved-text buffer is 800 bytes, not 200.
- **[Buka] User-facing text in `advManager::QuickInfo`, `ProcessSelect`,
  `LoadRemote` and `Main` is CP1251 Russian**, not the English of the
  reconstruction: "Border"/"Uncharted Territory"/"Artifact"/"Unknown"/"Reefs",
  the two "(already/not visited)" strings, the `"\n\nguarded by %s %s"` guard
  suffix, the world-map and status-window quick-help texts, and the
  restart/load confirmation prompts. All verified by content hash against the
  image (`sha256(bytes + NUL)` = the delinker's `$anon_str_<hash>` name).

- **[Buka] `game::ShowCampaignInfo` (0x226d2) has no low-memory REPLAY
  disable.** PoL 2.0 broadcasts a second `CAMPAIGN_MESSAGE_DESELECT` for
  `CAMPAIGN_DIALOG_REPLAY` under `if (gbLowMemory)`; retail goes straight from
  the `if (!viewOnly)` RESTART deselect to `SwitchAmbientMusic`. The whole
  guard and its body are gone (the eight instructions between the first
  `BroadcastMessage` and the ambient-music `neg/sbb` select).
- **[Buka] `game::CampaignInfoUpdate` (0x22adc) is localized and its bonus
  strings were rewritten.** The resource line is `sprintf(gText, "%s: %d",
  gResourceNames[...], choice->amount)` - a different format string AND the
  two arguments swapped relative to PoL's `"%d %s"`. The nine named-artifact
  `case` labels are emitted in a different SOURCE order (jump table decoded
  from the retail index/jump tables at +0x87f/+0x8a3: MINOR_SCROLL,
  MAGE_RING, DEFENDER_HELM, POWER_AXE, DRAGON_SWORD, DIVINE_BREASTPLATE,
  FIZBIN, THUNDER_MACE, ARMORED_GAUNTLETS) and all nine literals, plus the
  Summon-Earth, Puzzle-Pieces, Experience and `"n/a"` strings, are CP1251
  Russian. The CREATURES case no longer capitalizes the copied army name -
  PoL's `armyName[0] -= 'a' - 'A';` has no retail counterpart (three
  instructions absent).
- **[unclassified] `CampaignHandler` (0x233b8) indexes
  `m_campaignMapEnabled` with the scenario first.** On the switching-map
  accept path retail computes `m_campaignScenario * 12 + IDX(m_campaignType)`
  (`movsbl 0x4(%ecx),%edx; imul $0xc; lea 0xa2(%eax,%edx)`), i.e.
  `m_campaignMapEnabled[m_campaignScenario][IDX(m_campaignType)]`, where PoL
  2.0 has the indices the other way round. Every other use of the array in
  both builds is `[side][map]`, so the 2.1 line writes out of the declared
  `[2][12]` bounds here; the two adjacent statements
  (`m_campaignScenarioBonus`, `m_campaignChoice`) keep `[type][scenario]`.
- **[unclassified] `game::SetupNetworkGame2` (0x92b64) probes for
  DPLAYX.DLL.** After the Windows-NT check (which in retail dims only
  `CHOICE_THREE`, not PoL's `CHOICE_ONE` *and* `CHOICE_THREE`), the body does
  `hLib = NULL; hLib = LoadLibraryA("DPLAYX.DLL"); if (hLib == NULL) { ...
  dim CHOICE_ONE ... }` with its own `tag_message` in the guarded block.
  Retail's frame is 0x20 bytes larger than PoL's for exactly that handle and
  message. The library is never freed.
- **[unclassified] The widget dispatch family stopped snapshotting `m_flags`.**
  Every PoL `Main` override opens `H2EnumStorage<WidgetFlag,i16> flags =
  m_flags;` and then writes `m_flags = flags | WIDGET_FLAG_SELECTED` /
  `m_flags = flags & ~WIDGET_FLAG_SELECTED`. Retail keeps no such slot in any
  of them (`border::Main` 0xcb390 has a 0x10 frame: two coordinates, `this`,
  one switch temp) and performs read-modify-write on the member in place -
  `movw 0x16(%ecx),%dx; orb $1,%dl; movw %dx,0x16(%eax)`. `widget::Main`
  (0xd4180) likewise re-reads `m_flags` after `m_flags |= ...` instead of
  testing a `flags` local. Affects `widget::Main`, `border::Main`,
  `iconWidget::Main`, `button::Main`.
- **[unclassified] `widget::Main` (0xd4180) moved the un-dimmable-kind test
  into `widget::Dim`.** PoL guards both call sites with `m_kind !=
  WIDGET_KIND_UNDIMMED && m_kind != WIDGET_KIND_TEXT` and calls the file-static
  `DimWidgetArea(this)`; the PoL `WIDGET_COMMAND_DRAW` arm also returns
  `MESSAGE_DISPATCH_CONTINUE` from inside that guard. Retail has neither test
  at the call sites (`movswl 0x16(%eax),%ecx; and $8; test; je; call
  ?Dim@widget@@QAEXXZ` and nothing else) and no `DimWidgetArea` body anywhere
  in `.text`; the kind test lives only in `widget::Dim` (0xd4540). The DRAW arm
  falls through to the switch's `break`.
- **[unclassified] `widget::Main` `WIDGET_COMMAND_SET_FLAGS` writes the member
  directly.** PoL builds `flags = m_flags | (i16)value; m_flags = flags;` and
  then tests the local. Retail is `m_flags |= value & 0xffff;` followed by a
  fresh `movswl 0x16(%edx),%eax` of the member. `WIDGET_COMMAND_CLEAR_FLAGS`
  keeps its `flags` local, but its initialiser masks (`and edx,0xffff`) where
  PoL casts, and the `WIDGET_COMMAND_DIMMED` fast path assigns
  `flags = WIDGET_FLAG_DIMMED` and re-uses the shared `m_flags &= ~flags`
  instead of naming the enumerator.
- **[unclassified] `iconWidget::Main` (0xbba10) sets a right-button modifier on
  a message it has already retyped.** The button-up arm ends
  `msg.type = MESSAGE_WIDGET; ...command = WIDGET_COMMAND_DESELECT;
  ...id = m_id; if (msg.type == MESSAGE_RIGHT_BUTTON_UP)
  msg.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;` - retail
  really emits `cmpl $0x40,(%ecx); jne; movl $0x200,0xc(%edx)` at 0x64d after
  the `movl $0x200,(%edx)` at 0x630, so the test can never be true. PoL has no
  such statement, and Buka's own `border::Main` (the same arm, same file
  family) does not either.
- **[unclassified] `iconWidget::Read` (0xbb890) masks the orientation byte.**
  Retail is `movswl %ax,%edx; andl $0xff,%edx; movb %dl,0x26(%eax)` - i.e.
  `m_orientation = (IconDrawOrientation)(ReadWord() & 0xff)`, matching the
  `m_fillColor` line two statements later. PoL stores the `ReadWord()` result
  unmasked.
- **[unclassified] `border::Read` (0xcb250) uses two name buffers and no `kind`
  local.** Retail's 0x24 frame holds `char[16]` at -0x10 and a second
  `char[16]` at -0x20, one declared inside each of the bitmap and icon
  branches, and `m_kind` is written straight from `ReadWord()` before the two
  NULL stores and re-read (`movswl 0x14(%edx),%eax`) for each test. PoL keeps a
  single function-scope `resourceName` and a `kind` local.
- **[unclassified] `button::DeselectSelected` is gone; `button::Deselect`
  (0xd3e60) carries the body.** PoL routes all six deselect sites through a
  private `inline DeselectSelected`; retail has the full flag-clear / `Draw()`
  / `UpdateScreenRegion` / message-fill body inside `Deselect` and five REL32
  `call ?Deselect@button@@QAEFAAUtag_message@@@Z` sites in `button::Main`.
  The message fill order is `type`, `command`, `id` in both.
- **[unclassified] `button::Main` (0xd3890) drag loop returns
  `MESSAGE_DISPATCH_FORWARD` literally.** On loop exit retail is
  `call ?Deselect@...; movl $0x2,%eax` - the deselect result is discarded -
  where PoL returns it (`movswl %ax,%eax`). The three flag guards on the
  keyboard arms are separate early-exit `break`s (`jne <next>; jmp <switch
  end>` per term), not one `&&` chain, and the KEY_UP arm's hotkey test has the
  same polarity as KEY_DOWN's (`if (m_hotkey != NO_HOTKEY && m_hotkey ==
  keyCode) return Deselect(msg);`) rather than PoL's inverted `||` guard.
- **[unclassified] `button::button(char*, ...)` (0xd3640) and `button::Draw`
  (0xd3f20) drop their one local.** Both open `push ecx` (a single `this`
  slot): the ctor assigns `m_iconId = MakeId(...)` and then reads the member
  back for `GetIcon`, and `Draw` addresses `m_owner->m_posX` directly instead
  of caching a `heroWindow* win`. `iconWidget::Draw` (0xbbd70) likewise has no
  `widgetWidth`/`iconWidth` pair - a 0x14 frame with only `x`, `y`, the
  `IconEntry*`, `this` and the switch temp.

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
