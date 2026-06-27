# Compiler detection — PoL HEROES2W.EXE

**Conclusion: Microsoft Visual C++ 4.2** (cl 10.20, link 3.10, cvtres 4.00) — high
confidence from static evidence; empirical byte-match confirmation pending a VC4.2 cl.exe.

## Static evidence
- `C:\MSDEV\LIB\{LIBCMT,kernel32,user32,...}.lib` — the VC4 Developer-Studio (MSDEV) layout
  (from CodeView `sstLibraries`).
- PE optional header **MajorLinkerVersion=3, MinorLinkerVersion=10** → link 3.10.
- CodeView `S_COMPILE`: **`Microsoft LINK 2.60.5112 (NT)`** + **`Microsoft CVTRES 4.00`**
  (on the linker-synthesized import-thunk modules; the game's own modules were stripped to publics).
- **No Rich header** (`@comp.id`) — the Rich header is VC6+, so this is pre-VC6 (rules out VC5.0/6.0).
- CRT (`LIBCMT`) exports the MBCS/locale helpers `_setmbcp`, `__initmbctable`,
  `__crtLCMapStringA/W`, `__crtGetStringTypeA/W`, `__crtGetEnvironmentStringsA/W` — the multibyte
  locale support **introduced in VC++ 4.2**; VC 4.0/4.1 CRTs predate it.
- Build date 1997-05-05 → 4.2 (the final VC4, shipped 1996) is what a mid-1997 build would use.

## Why not 4.0/4.1
- 4.0 shipped link **3.00** (here it's 3.10).
- 4.0/4.1 CRT lacks the `_setmbcp`/`__initmbctable` MBCS layer present here.

## Empirical confirmation (pending toolchain)
Compile one leaf function (e.g. a small `BASE/` accessor of known RVA+size) with cl 10.20
`/c /O2 /MT` and objdiff against the delinked retail object. A byte-match pins 4.2 exactly;
a near-miss would send us to test 4.1. This needs the VC4.2 `cl.exe`/`c1.dll`/`c2.exe` +
`include/` + `lib/` (LIBCMT + the era Win32 SDK), packaged as a self-contained toolchain tree.
