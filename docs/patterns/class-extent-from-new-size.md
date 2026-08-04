# The `new` push, the ctor/dtor span and the deref census bound a class

**Trigger.** A `new X` site is a fixed number of bytes off and the diff shows only the
pushed immediate: `6a 52` (push 0x52) in retail against `68 ae 06 00 00` (push 0x6ae) in
ours. `operator new`'s argument *is* `sizeof(X)`, so the retail push is the class extent,
full stop — every member modelled past it is a fabrication.

Measured on `InitMainClasses` (RVA 0x65d1c) / `soundManager`, 2026-08-04.

## The three channels that bound the extent

```
InitMainClasses  ours                          InitMainClasses  retail
---------------------------------------------- ----------------------------------------------
68 ae 06 00 00   pushl $0x6ae                   6a 52            pushl $0x52
e8 .. .. .. ..   calll ??2@YAPAXI@Z             e8 .. .. .. ..   calll ??2@YAPAXI@Z
```

1. **`new` size.** 0x52 — the upper bound, exactly.
2. **Ctor and dtor writes.** `??0soundManager@@QAE@XZ` (RVA 0xb5bd0) writes
   `[this]`, `+0x32` (the base's `m_active`), `+0x36`, `+0x3a`, `+0x3e`, `+0x42`,
   `+0x46`, `+0x4a`, `+0x4e` and nothing else; `??1soundManager@@QAE@XZ` (RVA 0x70df0)
   touches only `+0x42`. A dword at `+0x4e` ends at 0x52 — the two agree.
3. **Whole-tree deref census.** Every `mov <global>, %reg` with a DIR32 to the object
   pointer, followed by any `disp(%reg)` in the next dozen instructions, across all of
   `build/delink/**/*.obj`:

```
('OFF', 0)    EXEC.c.obj, KB.c.obj          (%eax)          vtable call
('OFF', 0x36) ADVMGR.c.obj, SMACKMGR.c.obj  0x36(%edx) ...
('OFF', 0x3e) ADVMGR.c.obj, SMACKMGR.c.obj  0x3e(%ecx) ...
('OFF', 0x42) ADVMGR.c.obj, SMACKMGR.c.obj  0x42(%eax) ...
('OFF', 0x4e) ADVMGR.c.obj, GAME.c.obj      0x4e(%edx) ...
```

Max 0x4e, and the owning TU's own object (`soundmgr.c.obj`) shows the same ceiling in a
plain displacement census: `{0x4, 0xc, 0x10, 0x32, 0x36, 0x3a, 0x3e, 0x42, 0x46, 0x4a, 0x4e}`.

## Where the "members" actually went

A field modelled past the boundary is one of four things; all four appeared here.

| what it was | how to tell |
| :-- | :-- |
| a **global** | the use sites carry a DIR32 to a fixed address instead of `disp(%reg)`. `m_samplesReady` (26 sites in ADVMGR/GAME/SMACKMGR) is the byte at 0x5348f1 — the reloc count per function matched our member-read count exactly, 5/20/1. |
| an **existing member under another name** | the reference args of a free function pin it: `PlayAmbientMusic` passes `this+0x4e` in ecx and `this+0x4a` in edx to `MIDIPlay`, so `m_currentTrack`/`m_fadeSteps` were never separate from `m_musicTrack`/`m_musicFadeSteps`. |
| **replaced by other machinery** | `m_midiReady` reads are `call ?GetMusicFlagA@@YIEXZ`; `m_midiReady == 0` is `call ?MusicFlagsActive@@YIEXZ`; `m_cdReady == 0`/`m_cdStarted == 0` collapsed into `if (!CDStartup())`. |
| **dead with its subsystem** | only `// @remove` bodies read it. Park those as file statics in the owning `.cpp`; nothing retail references them. |

## What made it match

Trimming the class to the seven real members (`SIZE(soundManager, 0x52)`) closed
`InitMainClasses` to EXACT and, with the four dispositions above, took
`BASE/soundmgr` from 11/26 to 22/26 masked-identical with no regression anywhere in the
tree.

Do not accept a smaller class on the `new` push alone: run the ctor/dtor and the
whole-tree deref census too. A class whose only construction site is a placement `new`
or a static instance has no push to read, and a union-ish tail can be written by exactly
one function you have not looked at yet.
