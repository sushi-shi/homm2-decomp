// Question: can an older VC6 C++ front end, with the pinned SP5 back end,
// recover DIMMER's independent function alignment without /Gy?
//
// Measured 2026-09-27 in:
//   /home/sheep/Projects/homm2/homm2-buka/.claude/worktrees/matcher-1
//   branch matcher/bss-misc
// pwd, branch, and HOMM2_DIR were verified in one persistent nix develop .#build
// shell before compiling. Production source/toolchain and root remained untouched.
//
// Artifacts: build/link/dimmer-older-frontend-nogy/
//   inputs.json: exact compiler component paths, SHA-256 hashes, PE versions
//   run.py, run.log, results.json: six package-label/source-form cells
//   audit.py, audit.json: all seven raw bodies and complete ordered relocations
//   {rtm,sp3,sp5}-{ordinary,inline}/: disposable source/header, object, phase log
//   additional-local-search.json: bounded named Nix-store package search
//
// Prior evidence gap:
// comdat-order-matrix.cpp describes older/mixed VC6 phases with /Gy, while
// native-alignment-matrix.cpp tests no-/Gy only on pinned SP5. Referenced old
// mixed-vc6-phases and compiler-version-probes directories were absent from
// root at audit time; their full matrices were not recreated. Root's retained
// dimmer-backend-alignment/{ordinary,inline}/control.obj supply the two SP5
// controls, reused after exact disposable source/header and flags verification.
//
// Compiler provenance:
// driver CL.EXE 12.0.8804.0, SHA256
//   1bf99f206271ecdbd13da2829192ea2c02e2a44c740b8d72935d5d9cb753b156
// back end C2.DLL 12.0.8966.0, SHA256
//   d50100ac2380d58f3f6f756961fb1319d35f5248e5fa6cafb866ca657e5dda4a
// RTM C1XX.DLL 12.0.8472.0, SHA256
//   28c355499c2f8090910624734faff31fe719a69a8aac7dd44b2ce64addf96ee2
// SP5 C1XX.DLL 12.0.8964.0, SHA256
//   f014b3bee650224adf6cb44e51f0eeac5abbf5da666fa299d5250b2f3d1937c6
//
// IMPORTANT: locally available homm3-toolchain-vc6-sp3 directories contain
// a C1XX.DLL identical to RTM, including version and full SHA256. Thus the
// `sp3` artifact key denotes a package label, NOT a distinct SP3 front end.
// A bounded search of /tmp, the HoMM2 trees, the local cache, and named
// VC6/SP3 Nix-store packages found no distinct VC6 SP3 front end. Actual
// distinct-SP3 coverage remains UNTRIED/unavailable. The six labeled cells
// cover only four unique version/source-form cells, and that available-input
// matrix is complete. The duplicate package-label cells are retained as
// measured provenance evidence rather than counted as additional coverage.
//
// Exact build state:
//   unit_flags(BASE/DIMMER), removing only /Gy:
//   /nologo /c /Od /MT /Gr /G5 /Ob1 /Gf /Gi- /GX /DNO_STRICT
// Fresh older-front-end compiles add /Bd solely to record actual loaded phase
// paths and arguments; no source/output patching or alignment directive is used.
// Private copies of the SP5 bin directory replace only C1XX.DLL. C2 and the
// driver remain pinned SP5. SP5 include files remain unchanged. Four fresh
// compiles and two previously measured controls all completed successfully.
//
// Both declaration forms use unchanged out-of-line destructor bodies:
// ordinary: virtual ~dimmerWidget(void) OVERRIDE;
// inline:   inline virtual ~dimmerWidget(void) OVERRIDE;
// The disposable header changes its relative widget.h include to the equivalent
// owner-qualified include, matching the reused controls exactly.
//
// Result, for all six labeled cells:
// - @comp.id = 0x000b2306, retaining the expected SP5 producer identity.
// - All seven method bodies have EXACT RAW bytes at their retail lengths:
//   default constructor 0x2b, argument constructor 0x3f, Read 0x77,
//   Main 0x19, Draw 0x13, deleting destructor 0x2e, destructor 0x1c.
// - Every ordered relocation site, type, symbolic target identity, and raw
//   owner-relative addend equals build/delink/BASE/DIMMER.c.obj. No relocation
//   masking was needed. Full records and retail comparison are in audit.json.
// - The five ordinary functions still occupy one .text contribution at
//   0x0, 0x2b, 0x6a, 0xe1, 0xfa. Retail requires 0x0, 0x30, 0x70, 0xf0, 0x110.
// - Ordinary declaration: destructor is at section 3 +0x10d, then the deleting
//   wrapper at section 5 +0. Inline declaration: deleting wrapper is at section
//   5 +0, then destructor at section 6 +0. Neither form fixes the first five
//   boundaries. Available front ends do not change this topology.
//
// Disposition: reject the available older-front-end/no-/Gy alignment hypothesis.
// It preserves bodies and producer identity but not retail placement. No flags,
// source changes, or modified compiler output are retained. Distinct SP3 remains
// an explicitly unavailable input, not an inferred negative measurement.
