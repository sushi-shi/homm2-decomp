# Compiler detection — PoL HEROES2W.EXE

**Conclusion: Microsoft Visual C++ 4.2 compiler** (cl 10.20), with the older
Visual C++ 4.0 final-link component: LINK **3.00.5270**, its matching CVPACK/PDB
tools, and its static `LIBCMT.LIB`. Compiler identity is high confidence from
matching behavior. The final-linker identity comes from the retail PE stamp, and
the runtime-library identity comes from exact private CRT literals selected from
the checksum-verified VC 4.0 archive.

The closest evidence-backed shipping pipeline is:

```text
CL:   /Od /Ob1 /MT /Gr ...          probably without /Z7 or /Zi
LINK: /DEBUG:NOTMAPPED,MINIMAL
      /DEBUGTYPE:CV
      /PDB:NONE
```

The final LINK/CVPACK pass packed a complete minimal NB09 payload into the executable.
`/PDB:NONE` is consistent with embedding the payload rather than retaining only an external
PDB reference; `NOTMAPPED` is consistent with the CodeView debug entry's `AddressOfRawData`
being zero and its `PointerToRawData` naming payload appended outside the loaded image; and
`MINIMAL` explains the publics-only symbol inventory.
The PE also sets `IMAGE_FILE_LOCAL_SYMS_STRIPPED` and `IMAGE_FILE_LINE_NUMS_STRIPPED`.

Two build histories remain possible. The game objects may have been compiled without `/Z7` or
`/Zi`, so private information never existed, or richer object/PDB debug information may have
existed internally and then been removed by the minimal shipping link. The executable alone does
not distinguish those histories. It does prove that no private procedure/type/line information
survives in the shipped NB09 stream.

## Matching consequences

Most game units use `/Od /Ob1 /MT /Gr /G5 /QIfdiv`; the per-unit exceptions and optimized
BASE units are authoritative in `config/units.toml`. `/Gr` makes undeclared free functions
fastcall by default, so CRT declarations must come from the period headers and retain their
`__cdecl` convention. The same applies to the global `operator new` and `operator delete`
entry points; do not redeclare them under the `/Gr` default. `/Ob1` remains active under `/Od`
and explains the characteristic inline continuation jumps. The retail image has no C++
exception-handler or RTTI references, so the configured units do not enable `/GX` or RTTI.
These flags are supported by disassembly and matching behavior, not by private compiler records
in the minimal NB09 stream.

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
- All 3,541 retained named symbols are `S_PUB32` records with type index zero. There are no
  game `S_GPROC32`, `S_LPROC32`, labels, locals, types, or source lines; consequently the
  shipping stream does not provide function lengths.
- **No Rich header** (`@comp.id`) — the Rich header is VC6+, so this is pre-VC6 (rules out VC5.0/6.0).
- Build date 1997-05-05 → 4.2 (the final VC4, shipped 1996) is what a mid-1997 build would use.
- Repeated object-level A/B comparisons reproduce retail instruction selection,
  `/Od` local-slot behavior, mangling, and TU-state effects with cl 10.20. This is
  compiler evidence independent of the separately selected runtime archive.

## Why the compiler is not 4.0/4.1

- The matching cl 10.20 code-generation fingerprints and object-level A/B results
  identify the compiler. CRT symbol inventory is not valid compiler-version
  evidence here because the final link demonstrably selected the older library.
- The PE 3.00 linker stamp and VC 4.0 runtime do not contradict a VC 4.2 compiler.
  Microsoft compiler, headers, linker, and libraries can be mixed; the shipping
  build retained the older final-link installation for both LINK and `LIBCMT`.

## Final-linker identification

Archive.org item `msvc4x`, file `MSVC40.iso`, was verified against its published
MD5 `772b1bbd7d7ff95399145f02d719587b` and SHA-1
`81e139ac41d76740a6ba6d474355b37ed2e46c66`. Its LINK.EXE reports
`3.00.5270`, has SHA-256
`81109c8cb534debc0c5645db7c3a1b99dd646d982b0fd545070ceb1c77f9cb6c`,
and writes the same PE linker version 3.00 as retail.

The valid A/B must also select the sibling VC 4.0 CVPACK, MSPDB40, and
`LIBCMT.LIB`. Mixing LINK 3.00 with VC 4.2 CVPACK fails with LNK4027. CVTRES is
byte-identical between the verified VC 4.0 and VC 4.2 media. The VC 4.0
`testfdiv.obj` supplies 17 exact retail private-literal identities missing from
the VC 4.2 runtime archive. The pinned combined run returns success, emits a
minimal NB09 stream, and matches the resource tree and vendor import ABI.

The final-link driver accepts `--linker` or `HOMM2_LINK_EXE` without changing the
VC 4.2 compiler, object files, non-CRT SDK libraries, resource input, object
order, or flags. When the selected linker's sibling `LIB` directory contains the
pinned VC 4.0 `LIBCMT.LIB`, that directory is deliberately placed first. After
building the normal link inputs, run an isolated A/B output:

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
