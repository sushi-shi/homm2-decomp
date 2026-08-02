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
- **[Buka] /QIfdiv off.** Retail game code carries raw `fdiv` instructions
  (hundreds of sites; PHILAI alone has ~90) and links no `_adj_fdiv_*`
  helpers; the PoL-era `/QIfdiv` flag made VC6 emit `__adj_fdiv_m64` calls
  and was removed from every profile.
- **[Buka] C++ exception state on the new audio/bitmap TUs.** AudiereEffects,
  AudiereMusic, and BITMAP compile /GX; their EH registration funclets are the
  bulk of the image's `(funclets)` region. PoL had no /GX anywhere.
- **[Buka] VC6 STL exposure.** `KB.h` includes `soundManager.h`, which
  includes `<audiere.h>` — dragging `<string>`/`<vector>` into 90 of the 95
  game TUs; each emits the ctype-facet guard stub trio ($E19/$E18 +
  `?id@?$ctype@G@std@@` dtor thunk, the dtor COMDAT-folded to ADVMGR's copy
  at 0x415a50). Only BITS, TILE, X_GLOBAL, MusicFlags, and the emptied TUs
  stay clean.

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
- **[Buka] User-declared empty destructors** `game::~game` and
  `soundManager::~soundManager`, defined at the tail of `SOURCE/KB` — PoL
  relied on the implicit destructors.

## Changed

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
- **[Buka] Music configuration** routed through the new `MusicFlags` TU and
  the config musicSource/musicVolume paths in `GAME`/`ADVMGR`/`SMACKMGR`
  (fade timing via `glTimers` slots).

## Reconstruction infrastructure notes (not version deltas)

- The stripped image's DIR32 sites are recovered by donation
  (`homm2.audit.reloc_donation`): masked-identical claimed functions
  transfer their compiled relocation sites onto retail bytes; unanimous
  (symbol, addend) votes name the data owners and alias interior fields as
  owner+addend. Re-run with `--write` after each matching wave.
- VC6 /Od slot-order model (`homm2/core/od_slots.py`, solved on cl 10.20)
  re-validated on VC6 SP5 by probe: order rule identical; slot offsets
  round each local to 4 bytes.

## Bookkeeping

- The 36 no-PoL-counterpart functions harvested from the attempt-1 remap are
  enumerated in `docs/buka-va-queue.tsv` (empty `pol_va` column); the
  additions above cover all of them plus this branch's own recoveries.
- `// @remove` markers in `src/` are the authoritative removed-body list;
  the bitmap ctors left that list when their rewritten forms were recovered.
