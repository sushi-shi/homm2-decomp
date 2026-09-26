// Measured 2026-09-27; independent raw-retail and pinned-header audit.
// Worktree: .claude/worktrees/matcher-2, branch matcher/bss-audiere.
// CWD, branch and HOMM2_DIR verified in one persistent nix develop shell.
// Root replay: all three compiles, the 95-pair comparison, and the production
// owner census repeated on decomp-gold-2.1-buka with the same results.
// No production sources, headers, toolchain files, or objects changed.
//
// Corrected source ownership claim:
// docs/compiler-re-allocation-order.md section 10.1 says the `?1?` component
// of ??_B?1???id@?$ctype@G@std@@$D@@9@51 proves a source function-local
// static and therefore an earlier source ctype accessor. That inference is
// false. Pinned XLOCALE lines 453/515-516 declare and define a namespace-scope
// template static data member: `locale::id ctype<_E>::id`. locale::id at
// lines 28-39 has an implicit constructor/destructor, not an accessor or a
// function-local static. The generated `$D` initialization context explains
// this guard spelling. Candidate spelling does not prove stripped retail names.
//
// Independent retail evidence (VA, not reconstructed COFF names):
// 4cd000, size 39: guard reads/writes at +6,+19,+28 all target 539c80;
//                   REL32 operand +33 calls 4cd030.
// 4cd030, size 18: DIR32 operand +4 pushes 415a50;
//                   REL32 operand +9 calls atexit at 4d7548.
// 415a50, size 5: 55 8b ec 5d c3, no data references.
// The JNE at 4cd00f targets 4cd020, the CALL, not the epilogue. Thus the
// initialization guard does not guard atexit registration; both control-flow
// paths register the empty cleanup. No source-local accessor is present.
//
// Complete whole-retail-.text signature census finds 95 such 39-byte guarded
// initializer bodies. ALL 95 use the same three guard addresses (539c80),
// all call their corresponding 18-byte atexit wrapper, and all wrappers
// push the same cleanup 415a50 and call the same atexit 4d7548.
// Retail SHA256:
// bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
// This census is exact for the full measured pattern, not a claim that no
// other shaped initialization functions exist in the executable.
//
// Minimal ordinary compiler inputs below are diagnostic controls, not proposed
// replacement game source. Both `<string>` alone and `<audiere.h>` alone emit
// exactly three functions: _$E19 39, _$E18 18, ?id@?$ctype@G@std@@$E 5.
// All body bytes and complete ordered relocation sites/types/semantic roles
// match ALL 95 retail pairs after resolving actual addresses. Shared guard is
// COFF COMMON, size 1. No source expression mentions or uses a locale facet.
// Header chain: vendor audiere.h includes <string>; STRING includes <istream>.
// A third `<locale>`-only diagnostic emits a 71-byte combined initializer and
// separate 18-byte wrappers/5-byte cleanups for collate<char> and ctype<ushort>.
// Therefore arbitrary locale inclusion is not a shape-neutral substitute.
//
// This corrects section 10.3's claim that retail requires a real missing facet
// use or that candidate emits only a 5-byte thunk. The ordinary audiere header
// already produces the complete retail-shaped initialization trio. The actual
// node-destructor ordering problem survives; this audit does not solve it.
// The 95 repeated shared targets support one shared-header initializer family,
// not a distinct late-defined Audiere-owned static object. Exact retail type
// spelling remains inferred from the compiler/vendor source correspondence.
//
// Production owner/producer census (provenance.py, provenance.json):
// - The root manifest has 98 raw project objects: 96 C++ and two ASM.
// - Every C++ object carries @comp.id 0x000b2306. Exactly 95 of them emit
//   one measured ctype pair each; none emits multiple pairs.
// - BASE/Misc alone has no pair. It contains 23 function symbols and is
//   therefore not an empty or unextracted producer-count artifact.
// - The root historical MAP resolves all 95 pairs. 94 initializer entry VAs
//   coincide with the independent retail census. AudiereEffects alone differs:
//   native 0x004cd030 versus retail 0x004cd000.
// - This constrains the separate DIMMER split hypothesis: its narrow prefix
//   adds a producer but no pair. Simply merging two current pair-bearing TUs
//   would remove a required pair. Merging pairless Misc with one pair-bearing
//   owner can preserve 95 pairs, but says nothing by itself about correct data,
//   source ownership, or historical boundaries. The adjacent semantic candidate
//   MiscRuntime has already failed the measured named-array merge on data
//   placement; see ../Misc-track-name-data/named-owner-merge.cpp.
// - These observations do not prove one pair per original compiler input, or
//   preclude more complex real source ownership. No additional source merge
//   follows from the arithmetic alone, and none was attempted by this audit.
//
// Artifacts: build/audiere-ctype-owner-audit/
// probe.py, compare.py, minimal-results.json, retail-census.json, comparison.json;
// provenance.py, provenance.json;
// {string,locale,audiere}.{cpp,obj,log}. All 3 compiles completed without timeout.
// Commands in the verified worktree Nix shell:
//   python3 build/audiere-ctype-owner-audit/probe.py
//   python3 build/audiere-ctype-owner-audit/compare.py
//   python3 build/audiere-ctype-owner-audit/provenance.py
// Flags via unit_flags(BASE/AudiereEffects), including /Od /Ob1 /GX /Gy /MT.
// No full link needed: this is a source-ownership audit, not a retained fix.

// Diagnostic input A (entire TU):
#include <string>

// Diagnostic input B (entire independent TU):
// #include <audiere.h>

// Diagnostic input C (entire independent TU):
// #include <locale>
