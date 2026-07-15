# Compiler detection — PoL HEROES2W.EXE

**Conclusion: Microsoft Visual C++ 4.2 compiler and CRT** (cl 10.20), with a final
linker that stamped PE version **3.00**, and CVTRES 4.00. Compiler identity is high
confidence from static evidence and matching behavior. The exact final LINK.EXE binary
is not yet locally available for an A/B link.

## Static evidence
- `C:\MSDEV\LIB\{LIBCMT,kernel32,user32,...}.lib` — the VC4 Developer-Studio (MSDEV) layout
  (from CodeView `sstLibraries`).
- PE optional header **MajorLinkerVersion=3, MinorLinkerVersion=0**. This field is written
  by the final linker and is the authoritative in-image final-linker version evidence.
- Embedded CodeView contains 176 **`Microsoft LINK 2.60.5112 (NT)`** `S_COMPILE`
  records. Every one belongs to a DLL module that also contains an `S_THUNK32` import
  thunk. They identify retained import-thunk provenance, potentially the import-library
  producer, not the final executable linker. The one non-thunk `S_COMPILE` record is
  **`Microsoft CVTRES 4.00`** on `.\Win32_Re\heroes.res`. Game and BASE code objects
  have no `S_COMPILE` records.
- **No Rich header** (`@comp.id`) — the Rich header is VC6+, so this is pre-VC6 (rules out VC5.0/6.0).
- CRT (`LIBCMT`) exports the MBCS/locale helpers `_setmbcp`, `__initmbctable`,
  `__crtLCMapStringA/W`, `__crtGetStringTypeA/W`, `__crtGetEnvironmentStringsA/W` — the multibyte
  locale support **introduced in VC++ 4.2**; VC 4.0/4.1 CRTs predate it.
- Build date 1997-05-05 → 4.2 (the final VC4, shipped 1996) is what a mid-1997 build would use.

## Why the compiler is not 4.0/4.1

- The 4.0/4.1 CRT lacks the `_setmbcp`/`__initmbctable` MBCS layer present here.
- The PE 3.00 linker stamp does not contradict a VC4.2 compiler and CRT. Microsoft tools
  can be mixed, and the shipping build may have retained an older final linker.

## Final-linker A/B test

All locally provisioned project toolchains contain the same LINK 4.20.6164 binary; no
LINK 3.00 candidate was found under `/home/sheep`. Do not substitute an unproven download.
The required external artifact is a provenance-known Microsoft 32-bit x86 LINK.EXE whose
startup banner reports version 3.00, likely from the VC4.0-era distribution, plus any files
that binary requires to run. Record its SHA-256 before testing.

The final-link driver accepts `--linker` or `HOMM2_LINK_EXE` without changing the VC4.2
compiler, object files, SDK libraries, resource input, object order, or flags. After building
the normal link inputs, run an isolated A/B output:

```sh
python3 -m homm2.build.link_exe \
  --linker /provenance-known/VC40/BIN/LINK.EXE \
  --out build/link-link300/HEROES2W.EXE \
  --order build/link/objects.rsp \
  --resource build/link/HEROES2W.res \
  --imports build/link/vendor-imports-smack.lib \
  --imports build/link/vendor-imports-mss.lib \
  --imports build/link/vendor-imports-wing.lib
```

The isolated `.link.json` records the linker banner/path/hash and directly compares section
layout, entry point, import order and ABI, resources, public-data displacement runs, missing
CRT publics, and section-relative `.rdata` drift. This keeps linker effects separate from
source reconstruction changes.
