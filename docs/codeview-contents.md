# What's in HEROES2W.EXE's CodeView (NB09) — and what isn't

Exhaustive byte-level mine of the embedded CodeView NB09 debug stream (the source of
`build/gen/symbol_names.csv`). Recorded here so we don't re-derive it: the stream is a
**linker-merged, publics-only** image — rich in names/addresses/ownership, but with **no type
information whatsoever**.

## Static-link manifest (from `sstLibraries`)

The 19 libraries linked into the retail EXE — this is the game's whole dependency stack:

| Category | Libraries |
| :--- | :--- |
| **Win32 imports** | `kernel32` `user32` `gdi32` `winmm` `winspool` `comdlg32` `advapi32` `shell32` `ole32` `oleaut32` `uuid` `wsock32` `netapi32` |
| **Middleware (vendor, NOT NWC source)** | `basewin.lib` (NWC UI framework, `/O2`) · `Mss32.lib` (Miles Sound System) · `Wing32.lib` (WinG graphics) · `Smkw32ms.lib` (Smacker video) |
| **CRT / misc** | `LIBCMT.lib` (static CRT) · `oldnames.lib` |

**Static vs dynamic (from each lib's `.text` contribution to the EXE):** only three things are
real static code *in the EXE* — the directly-linked NWC objects (`Win32_Re`/`winextra`, ~797 KB),
`basewin.lib` (NWC's own UI framework, ~108 KB), and `LIBCMT.lib` (static MS CRT, ~51 KB). Every
other lib — the Win32 system libs **and the middleware `Mss32`(Miles) / `Smkw32ms`(Smacker) /
`Wing32`(WinG)** — is an **import library → DLL**: it contributes only 6-byte `jmp [__imp__X]`
thunk stubs (30–510 bytes each), while the actual code lives in the DLL, loaded at runtime.

**Consequences:** (1) the middleware is **dynamically linked and closed** — its code is *not in
this binary* (hence not in CodeView beyond the import thunks), and the devs had only the DLL +
SDK headers, never source. We never reconstruct it; we only need its SDK **headers** so calling
NWC TUs compile (`vendor/<sdk>/`, Gruntz-style — header-only). (2) The reconstruction targets are
NWC's own code + `basewin`; `LIBCMT` is static MS CRT (compile from the real CRT headers, not
reconstructed). Tier placement keys off `basewin`; this list is the map for telling NWC code from
DLL-import middleware.

## Subsection inventory (what the stream holds)

| Subsection | Content | Use |
| :--- | :--- | :--- |
| `sstGlobalPub` (201 KB, 3541 `S_PUB32`) | symbol name + address; **type index = 0 on all** | → `symbol_names.csv` (funcs, data, `??_7` vtables, `??_C@` literals) |
| `sstModule` (498) | per-compiland name + segment ranges + lib index | → TU ownership + tier |
| `sstAlignSym` | `S_COMPILE` (linker-stamped), `S_OBJNAME`, `S_THUNK32`, `S_END` | thunks below |
| `sstLibraries` | the 19 libs above | tier / vendor ID |
| `sstSegMap` (7 segs) | segment RWX flags + sizes (seg1 RX=.text, seg3 RW=.data, …) | corroborates PE sections |
| `sstGlobalTypes` | **`cType = 0` — EMPTY** | nothing |
| `sstGlobalSym` / `sstStaticSym` | header-only (`cbSymbol` 8 / 0) | empty |

Two details worth remembering:
- **`S_COMPILE` is the *linker's* record** — every one says *"Microsoft LINK 2.60.5112 (NT)"*,
  machine 0x386. There is **no per-compiland compiler record**, which is exactly why the opt
  level (`/Od` vs `/O2`) is absent from CodeView and `gen_manifest.py` infers it from the
  prologue (FPO vs full `ebp` frame).
- **`S_THUNK32` = 182 import thunks** — the Win32/multimedia/WinG IAT jump-stubs
  (`_waveOutGetNumDevs@0`, `_WinGBitBlt@32`, `_GetFileAttributesA@4`, … each a 6-byte
  `jmp [__imp__…]`). **Already fully captured**: the linker emits an `S_PUB32` public for each
  thunk stub, so they're already in `symbol_names.csv` as decorated `func` rows (`_X@N`,
  attributed to their DLL/`.def`), alongside the `__imp__X@N` IAT `data` entries. So `sstAlignSym`
  holds nothing we don't already have — a direct `call <thunk>` resolves to the named stub.
  (Miles' `AIL_*` have no thunk — called indirectly via `__imp__` — also already in the CSV.)

## Decisively ABSENT (checked every byte)

**No types, no `LF_CLASS`/`LF_STRUCTURE` records, no field lists, no member offsets, no member
widths/types, no member visibility, no class sizes, no locals (`S_BPREL32`), no line numbers.**
The `type` index on all 3541 publics is `0`.

**Consequence:** class **field layouts / sizes / member visibility have zero source in
CodeView** and cannot be extracted. What *is* CodeView-authoritative: symbol names + addresses,
TU ownership, vtable slot order (`??_7` + relocs), and **method** interface + **method**
visibility (decoded from the MSVC mangling access chars `E/F/M/N/U/V/Q…`). Data-member layouts
must be **reconstructed from disassembly** (Ghidra `FillOutStructureHelper` over each class's
`this+offset` accesses — see the Ghidra struct-recovery work) and pinned exactly during
byte-matching; member *visibility* is a source-only concept with no binary source at all.
