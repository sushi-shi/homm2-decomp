// VC6 SP5 / LINK 6.00 producer-census audit, 2026-09-26.
// Worktree: .claude/worktrees/matcher-2, branch matcher/bss-audiere.
// No production source, object, library, or executable was modified.
//
// Reproducers and measured artifacts:
//   build/link/rich-tu-census/probe.py
//   build/link/rich-tu-census/archive-probe.py
//   build/link/rich-tu-census/actual.json
//   build/link/rich-tu-census/results.json
//   build/link/rich-tu-census/archive-results.json
// Both scripts use run_compile, unit_flags(BASE/AudiereEffects), and the
// pinned LINK.EXE/LIB.EXE through homm2.core.wine in the verified worktree
// Nix shell. Objects/executables, compiler logs, and linker logs are retained.
// Rich records are decoded directly from the actual PE: find Rich before the
// PE header, read its XOR key, find the encoded DanS prologue, XOR every u32,
// validate [DanS,0,0,0], then read ordered (producer,count) pairs. No metadata
// rewriting or existing normalization helper participates in the audit.
//
// Current real images, read from the root worktree:
//   retail: build/orig/HMM2PL.exe
//     SHA256 bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
//   ordinary: build/link/rsrc/HMM2PL.exe (ordinary --rsrc output)
//     SHA256 7dc0ba80187f972d3f98493a850ad431e07a052e3666a762ae3b273a52189f13
//
// Ordered producer               retail count   ordinary count
//   0x000a1f6f                          133              133
//   0x000e1c83                           37               37
//   0x000b1f6f                            8                8
//   0x000520ff                            2                2
//   0x000420ff                            2           absent
//   0x00000000                           54               58
//   0x00010000                          195              193
//   0x00131f62                           13               13
//   0x000c1c7b                           11               11
//   0x000b2306                           96               96
//   0x000606c7                            1                1
//
// Thus ordinary output already differs in other producer records, but its
// VC6 SP5 C++ count is exactly retail's 96. The current manifest contains
// exactly 96 .cpp units plus two .asm units.
//
// Complete controlled direct-input matrix:
// Baseline TU:
//   extern "C" void __cdecl entry() {}
// Additional TU arms:
//   code:     extern "C" void __cdecl extra() {}
//   data:     extern "C" { char text[] = "hello"; }
//   BSS:      extern "C" { int counter; }
//   empty:    // empty TU
//   constant: static const int value = 1;
//
// Every source compiled with the production /Gy flags. Every raw COFF,
// including the empty and erased-constant arms, contains the absolute symbol
// @comp.id = 0x000b2306. Link flags: /ENTRY:entry /NODEFAULTLIB
// /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OPT:NOREF.
//
// Baseline link: C++ producer count 1.
// Baseline + ANY of those five direct objects: C++ producer count 2.
// Code-bearing versus data-only versus no emitted program storage does not
// distinguish the compiler-count contribution. /OPT:REF repeats with empty
// and data objects also count 2; removing unused content does not erase the
// input object's producer record.
//
// Archive extraction control:
// LIB.EXE creates data.lib from the untouched data.obj above.
// - Baseline entry + data.lib: count 1 (member never extracted).
// - Referencing entry + data.lib: count 2. The entry is:
//     extern "C" char text[];
//     extern "C" int __cdecl entry() { return text[0]; }
//
// All ten links completed successfully; no truncated or inferred outcomes.
//
// Conclusion: an additional extracted data-only TU DOES increment the C++
// producer census. The older Misc dossier's unchanged-census claim cannot be
// used to exempt such a split. A semantically justified extra source owner
// can be explored for layout evidence, but full retail reproduction then
// requires recovering a compensating ownership consolidation elsewhere (or
// other independently evidenced producer identity), not altering Rich bytes.
// Merely putting an unneeded member in an archive leaves counts unchanged
// precisely because it supplies no linked storage and cannot repair placement.
