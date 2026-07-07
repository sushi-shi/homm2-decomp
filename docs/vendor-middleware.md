# Vendored middleware SDKs — PoL HEROES2W.EXE

HEROES2W.EXE dynamically imports three closed third-party middleware DLLs. Their **code**
is not in the EXE (only PE import thunks), so we do not reconstruct it — we only need the
**API headers** so the NWC translation units that CALL them compile and byte-match. Those
headers are vendored under `vendor/<sdk>-<version>/` and put on the compiler's `INCLUDE`
path by `scripts/homm2/build/cc_wrap.py` (see *Build wiring* at the end).

The retail EXE analysed here is `build/orig/HEROES2W.EXE`
(md5 `900aa22c4e88221e7ebac524cd4172c9`), byte-identical to the English "Price of Loyalty"
CD image `img-pol/HEROES2W.EXE` (dated 1997-05-05). The GOG re-release EXE
(`ea70bf65…`) is a *different* binary and must not be used to pin versions.

| Middleware | DLL | Version | Import style | Confidence |
|---|---|---|---|---|
| Miles Sound System | `mss32.dll` | **3.6** (AIL 2D 3.x) | by name, 33 fns, `__stdcall @N` | High (family), name-imported |
| Smacker Video | `smackw32.DLL` | **3.0g** | **by ordinal**, 10 fns | High |
| WinG | `WING32.dll` | **1.0** (1.0.0.37) | by name, 6 fns | Certain |

Period DLLs used as evidence come from HoMM2 disk images under
`/home/sheep/Projects/homm2/investigation/extracted/` (`img-pol` = our exact EXE but the
CD only ships installer EXEs; `img-fargus` = a localization of the same NWC retail that
keeps NWC's middleware DLLs; `demo`, `gog-win` = other lineages, cross-checks only).

## Authenticity status — all three are clean-room reconstructions

Each header here is an **original, clean-room reconstruction**: the declarations were
authored from the observable ABI (the PE import table, the `@N` stdcall byte-counts, the
mapped Smacker ordinals) plus the well-known public API shapes and standard GDI/MMSYSTEM
types. They are **not** copies of the proprietary RAD / Microsoft SDK headers.

A search for the *version-exact* official SDK headers did not turn up usable copies:

| SDK | Wanted | Authentic period source located (verification reference) | Decision |
|---|---|---|---|
| Miles | `MSS.H` v3.6 (AIL 2D 3.x) | **`AIL.H` v3.6B, 8-Mar-97** in the New World Computing source tree `gondur/mig_src` (`SRC/H/AIL.H`; Win32 superset in `SRC/H/MSSW.H`) — **same vendor, same version I pinned.** (Public *mss.h* copies elsewhere are the wrong 6.x — VBdec 6.1a, Source-engine 6.x/7.x.) | keep reconstruction (verified against it) |
| Smacker | `SMACK.H` v3.0g | **`SMACK.H` v3.0c** in `NBlood/Blood-RE` (`smacker/SMACK.H`) — the same Smacker 3.0x family. (JA2/gruntz copies are 3.2f, the version the campaign was told not to substitute.) | keep reconstruction (verified against it) |
| WinG | `WING.H` v1.0 | **authentic MS `WING.H`, "Copyright (c) 1994 Microsoft Corp"** in `templeblock/ArchiveGit` (`Clients/AG/Pm8/TPSLibs/Include/WING.H`); also the official SDK package `archive.org/details/WING10` / MS KB `Q125698` → `Wing10.exe` | keep reconstruction (verified against it) |

**Cross-verification (against the authentic sources above).** The reconstructions were
checked, function-by-function, against the period headers and found accurate:
- *Miles*: `gondur/mig_src` `AIL.H` reads `AIL_VERSION "3.6B"` / `8-Mar-97`, copyright RAD
  1991-97 — **exactly** the version pinned here (the sibling image's `mss32.dll` build
  string is `MSS V3.6B`). That `AIL.H` is the DOS variant (`cdecl`, `__pascal` callbacks)
  and declares 22 of our 33 imports; the Win32 stdcall superset (with `AIL_waveOutOpen`,
  `AIL_midiOutOpen`, `AIL_redbook_*`) is `MSSW.H`. HEROES2W.EXE links the **Win32** `stdcall
  @N` API, which `mss.h` here targets; the AIL 2D core signatures/handle types it shares
  with `AIL.H` match.
- *Smacker*: `Blood-RE` `SMACK.H` (v3.0c) confirms every imported signature —
  `SmackOpen(name,flags,extrabuf)`, `SmackToBuffer(smk,left,top,Pitch,destheight,buf,u32)`
  (7 args → `@28`), `SmackToBufferRect(smk,u32)`→`u32`, `SmackSoundUseMSS(void*)`→`u8`,
  `SmackSoundUseDirectSound(void*)`→`u8`, `SmackDoFrame/Wait`→`u32`, `SmackSummary`→`void`
  — plus the `Smack`/`SmackSum` struct field order and the flag constants
  (`SMACKTRACK1=0x2000`, `SMACKAUTOEXTRA=0xffffffffL`, `SMACKSURFACEFAST/SLOW/DIRECT`).
- *WinG*: `templeblock/ArchiveGit` `WING.H` (MS, 1994) confirms all six imported
  signatures, that `WINGAPI` = `WINAPI` (i.e. `__stdcall`, **no** `dllimport` — matching
  the retail thunk codegen), and the `WING_DITHER_TYPE` kernels
  (`WING_DISPERSED_4x4/…8x8/CLUSTERED_4x4`, which the reconstruction was corrected to use).

**Why keep the reconstructions rather than vendor the copied SDK files.** (1) The exact,
directly-usable artifacts are the *wrong* platform/version for a drop-in — the located
Miles header is the **DOS** `AIL.H` variant (`cdecl`, only 22/33 fns), not the Win32 mss32
header this EXE links; Smacker `3.0c`/`3.2f` and Miles `6.x` are not `3.0g`/`3.6B` exactly.
(2) These are third-party-**copyrighted** RAD/Microsoft SDK sources; a clean-room
reconstruction authored from the observed ABI (import table, `@N` counts, mapped ordinals)
plus verification against period references is the correct, license-clean practice for a
decompilation — and it already yields the full, correct Win32 interface that compiles under
MSVC 4.2 + clang. The reconstructions are the working, **verified** headers, not a fallback.

---

## 1. Miles Sound System — `mss32.dll` — v3.6

**Header:** `vendor/miles-3.6/mss.h` (reconstructed).

**Version evidence.** The period `mss32.dll` embeds the build string
`Miles Sound System usage script generated by MSS V3.6B`
(`img-fargus`, the same-lineage DLL; the GOG re-release ships `V3.6C`, the demo ships the
predecessor `V3.50E`), copyright `1991-97 RAD Game Tools, Inc.` All are the **AIL 2D 3.x**
API. Because the EXE imports **by name**, the exact point release is signature-neutral;
`3.6` is pinned as the companion family (specifically `3.6B` in the sibling image). The
Smacker 3.0g DLL additionally prints `Smacker needs at least version 3.50F of MSS`,
bounding the retail Miles at ≥ 3.50F, consistent with 3.6B.

**Import surface (the fingerprint).** 33 functions, each with the classic `__stdcall`
`@N` byte-count decoration → argument count `= N/4`. Every prototype in `mss.h` was
verified against that count (e.g. `AIL_set_sample_type@12` = 3 args, `AIL_startup@0` = 0):

```
AIL_startup@0            AIL_shutdown@0            AIL_serve@0
AIL_last_error@0         AIL_set_preference@8      AIL_get_preference@4
AIL_waveOutOpen@16       AIL_allocate_sample_handle@4   AIL_init_sample@4
AIL_start_sample@4       AIL_end_sample@4          AIL_sample_status@4
AIL_set_sample_address@12   AIL_set_sample_type@12  AIL_set_sample_playback_rate@8
AIL_set_sample_volume@8  AIL_sample_volume@4       AIL_set_sample_loop_count@8
AIL_midiOutOpen@12       AIL_midiOutClose@4        AIL_set_XMIDI_master_volume@8
AIL_allocate_sequence_handle@4   AIL_release_sequence_handle@4
AIL_init_sequence@12     AIL_start_sequence@4      AIL_stop_sequence@4
AIL_resume_sequence@4    AIL_sequence_status@4     AIL_set_sequence_loop_count@8
AIL_redbook_open@4       AIL_redbook_close@4       AIL_redbook_tracks@4
AIL_redbook_track_info@16
```

**Codegen note.** Retail call sites use the dllimport indirection
(`call dword ptr [__imp__AIL_*@N]`, e.g. `.text:004013A3`), so `mss.h` decorates every
function `extern "C" __declspec(dllimport) <ret> __stdcall AIL_*`.

**Header provenance.** Reconstructed minimal header. Parameter/return **types** were taken
from the authentic Miles AIL 2D API (cross-checked against a later authentic `mss.h`,
Miles 6.0c, where the AIL 2D signatures are unchanged) and pinned to the observed `@N`
arg counts. Only the 33 imported functions + their handle types (`HSAMPLE`, `HSEQUENCE`,
`HDIGDRIVER`, `HMDIDRIVER`, `HREDBOOK`) and the `AIL_waveOutOpen`/`AIL_midiOutOpen`
MMSYSTEM pass-through types are declared.

---

## 2. Smacker Video — `smackw32.DLL` — v3.0g

**Header:** `vendor/smacker-3.0g/smack.h` + `vendor/smacker-3.0g/rad.h` (reconstructed).

**Version evidence.** The retail EXE imports `smackw32.DLL` **by ordinal only** — the PE
import table carries no names. The period DLL (`img-fargus`, same NWC retail lineage)
embeds `*** Smacker Version: 3.0g ***`, copyright `1994-96 RAD Game Tools, Inc.` **Ordinal
tables are version-specific in Smacker** — the GOG-lineage `3.0r` DLL renumbers the exports
and does NOT fit this EXE (its ordinal 18 = `SmackFrameRate`, its 38 = `SmackWait`, giving a
nonsensical import set with no `SmackClose`). Only the 3.0g table yields a coherent
playback API.

**Ordinal → name mapping** (the 10 imported ordinals, from the 3.0g export table;
`(*)` = imported):

| Ord | Name (`@N`) | Args |
|----:|---|---|
| 14 | `SmackOpen@12` | 3 |
| 18 | `SmackClose@4` | 1 |
| 19 | `SmackDoFrame@4` | 1 |
| 20 | `SmackSummary@8` | 2 |
| 21 | `SmackNextFrame@4` | 1 |
| 23 | `SmackToBuffer@28` | 7 |
| 28 | `SmackToBufferRect@8` | 2 |
| 32 | `SmackWait@4` | 1 |
| 33 | `SmackSoundUseMSS@4` | 1 |
| 38 | `SmackSoundUseDirectSound@4` | 1 |

`SmackSoundUseMSS` (ord 33) confirms the game routes Smacker audio through Miles.

**Ordinal 38 disambiguation.** An earlier 38-export Smacker table (the `demo` DLL) maps
ordinal 38 to `SmackSoundUseWin@0` (0 args), whereas the 3.0g 39-export table (which adds
`SmackSoundUseDW` at ord 34, bumping everything after) maps 38 to
`SmackSoundUseDirectSound@4` (1 arg). The retail call site at `.text:004013C4` pushes
**exactly one argument** before `call dword ptr [0x53A800]` (the ord-38 IAT slot):

```
004013B8  mov  eax, [0x52990C]     ; manager object
004013BD  mov  eax, [eax+0x36]
004013C0  mov  eax, [eax+0x4C]
004013C3  push eax                 ; 1 arg  ->  @4
004013C4  call dword ptr [0x53A800]; ordinal 38
```

→ `@4` → **`SmackSoundUseDirectSound`**, confirming the 3.0g layout. (The alternative,
following `AIL_get_preference(15)` returning zero, falls back from `SmackSoundUseMSS` to
`SmackSoundUseDirectSound`.)

**Codegen note.** Like Miles, Smacker call sites use dllimport indirection
(`call dword ptr [__imp__Smack*@N]`), so `smack.h` uses RAD's `RADEXPFUNC` =
`extern "C" __declspec(dllimport)` and `RADEXPLINK` = `__stdcall` (the `__RADNT__` path).

**Header provenance.** Reconstructed. `smack.h` declares all 10 imported functions with
prototypes matching the exported `@N` byte-counts, plus the stable RAD `Smack`/`SmackSum`
structs and `SmackOpen` flag constants, and the companion core playback/sound calls that
take only `Smack*`/`SmackSum*`/scalar args. The Win32-GDI-dependent `SmackBuffer..` /
`SmackBlit..` families (not imported by the EXE) are omitted so the header needs no
`<windows.h>`. `rad.h` is a minimal Win32/MSVC slice of RAD's shared macros (`RADEXPFUNC`,
`RADEXPLINK`, `u8`/`u16`/`u32`, …); its `u8`/`u16`/`u32` are benign typedefs whose
underlying types match `include/Ints.h` so both can appear in one TU (the retail `rad.h`
`#define`s them, which would collide with `Ints.h`'s typedefs).

---

## 3. WinG — `WING32.dll` — v1.0

**Header:** `vendor/wing-1.0/wing.h` (reconstructed).

**Version evidence.** WinG shipped in exactly one version. The period `WING32.dll`
VS_VERSION resource reads `WinG Version 1.0`, `FILEVERSION 1.0.0.37`, copyright
`Microsoft Corp. 1993-1994`.

**Import surface.** 6 functions imported by name — essentially the whole public API:
`WinGCreateDC`, `WinGCreateBitmap`, `WinGRecommendDIBFormat`, `WinGSetDIBColorTable`,
`WinGStretchBlt`, `WinGBitBlt`.

**Codegen note.** Unlike Miles/Smacker, the WinG imports are reached through classic import
**thunks** (`call <thunk>` → `jmp dword ptr [__imp_]`), with **no** direct `call [IAT]`
dllimport site anywhere (verified across the whole `.text`). So `wing.h` declares them
`extern "C" <ret> WINAPI` (i.e. `__stdcall`) with **no** `__declspec(dllimport)` —
`WINGAPI` is defined empty — matching the WING32.LIB import-stub codegen.

**Header provenance.** Reconstructed, self-contained copy of the documented WinG 1.0 API.
It defines the minimal Win32/GDI types WinG needs (`HDC`, `HBITMAP`, `RGBQUAD`,
`BITMAPINFOHEADER`, `BITMAPINFO`, `BOOL`, `UINT`, …) directly rather than pulling in the
heavy MSVC `<windows.h>`, consistent with this project's minimal `include/win/windows.h`.
The full documented API surface (incl. the non-imported `WinGGetDIBPointer`,
`WinGGetDIBColorTable`, `WinGCreateHalftonePalette`, `WinGCreateHalftoneBrush`) is present.

---

## Build wiring

`scripts/homm2/build/cc_wrap.py` appends every `vendor/<sdk>/` directory to the compiler's
`INCLUDE` (after the MSVC headers and repo `include/`), mirroring the sibling Gruntz decomp.
This makes `#include <mss.h>`, `#include <smack.h>` and `#include <wing.h>` resolve like the
original toolchain's SDK dirs. These are **headers only** — never build units, and not in
`config/units.toml`. `smack.h`'s `#include "rad.h"` resolves within the same vendor dir.

**Verification.** A scratch TU including all three headers alongside `Ints.h` and
`win/windows.h` compiles cleanly under MSVC 4.2 (no typedef clashes, all signatures parse)
and parses cleanly under clang in the editor dialect (`--target=i386-pc-windows-msvc
-fms-compatibility-version=10.20 -fms-extensions`). A forced rebuild of an existing unit
(`BASE/Misc.obj`) still succeeds with the new `INCLUDE`.
