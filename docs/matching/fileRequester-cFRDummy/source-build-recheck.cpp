// REQUEST source/build ownership recheck, 2026-09-26.
// Baseline: decomp-gold-2.1-buka at 299514f88900c0cf30ba03422c72830a38fc1cb7.
// All twelve pinned toolchain artifacts were independently verified before
// compiling. Retail SHA256:
// bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a.
// Reconstructed source and the ordinary linker inputs remain unchanged.
//
// Fresh baseline:
//   homm2 redelink; homm2 build; homm2 link --rsrc
//   python3 -m homm2.build.link_exe --audit-existing \
//     --out build/link/rsrc/HMM2PL.exe --map build/link/rsrc/HMM2PL.map \
//     --report build/link/rsrc/HMM2PL.link.json
// The build reports 1727/1727 exact functions and 291995/291995 data bytes.
// The ordinary linked image is 99.706803% equal: 3543 differing file bytes.
// .data differs in 2744/217088 bytes; .rdata differs in 8/20480 bytes.
// Imports, raw import order, resources and section geometry agree. Both
// images have a 94248-byte virtual loader-zero tail. This is object closure,
// not ordinary final-image closure. No corrective input was used in this link.
//
// Direct current-object/retail placement evidence:
//   build/bss-request-accessor/retail_refs.py
//   build/bss-request-accessor/retail-refs.json
// The script reads raw REQUEST.obj relocations and the retail operands at
// their claimed function/data offsets. Every listed site is independently in
// config/delink_relocs.tsv. REQUEST's three named BSS globals anchor +0,+4,+8
// at retail 0x00533d78. The six private cells are:
//
// retail operand RVA   owner               candidate BSS   retail BSS
// 0x00116ae0           cFRDummy             +0x0c           +0x20
// 0x0008eaf4           InitializeFiles      +0x10           +0x0c
// 0x0008eb1c           InitializeFiles      +0x14           +0x10
// 0x0008f294           Open                 +0x18           +0x14
// 0x0008f7ef           Main                 +0x1c           +0x18
// 0x0008fefc           Main                 +0x20           +0x1c
//
// GetFilename's 0x12b-byte function span retains its twelve ordered
// relocations. The semantic disassembler currently includes the following
// two compiler initialization helpers in the base-side end boundary: its
// 13 exact target blocks plus four trailing helper blocks must not be read
// as a mismatch inside GetFilename.
//
// 1. Actual accessor ownership, complete four-arm structural matrix.
// Reproducer, full source snapshots, logs and objects:
//   build/bss-request-accessor/probe.py
//   build/bss-request-accessor/results.json
// All four arms use unit_flags(SOURCE/REQUEST), compile successfully, and
// finish without timeouts. No compiler-state probes are implied.
//
// A. Unchanged global declaration:
//      DATA(0x00516ae0) H2_CONST char* cFRDummy = "";
//    GetFilename uses its existing `return cFRDummy;`.
// B. Remove that global definition; insert at the start of GetFilename:
//      static H2_CONST char* emptyFilename = "";
//    Change the existing return to `return emptyFilename;`.
// C. B with `inline` on the existing tail definition of GetFilename.
// D. C with the complete inline definition placed before GetMapHeader.
// All other source and owner declarations are unchanged in the snapshots.
//
// A emits the pointer at ordinary .data+4 and its literal at .bss+0x0c.
// B emits the desired literal at .bss+0x20, but moves the pointer to
// .data+0x244. C and D instead emit the pointer in a separate four-byte
// COMDAT and the literal in a separate one-byte BSS COMDAT. Neither retains
// a standalone GetFilename definition; the body's calls are expanded.
// Disposition: reject these accessor forms. A correct literal offset alone
// does not repair the pointer owner or the required function definition.
//
// 2. Real precompiled-header creation/restoration, complete ten-build matrix.
//   build/bss-request-pch/probe.py
//   build/bss-request-pch/results.json
// For each boundary below, insert only `#pragma hdrstop`, compile the full
// source using /Yc /Fp<arm.pch>, then recompile it using /Yu /Fp<arm.pch>:
//   after includes, before private enum declarations;
//   immediately before the first function (GetMapHeader);
//   after InitializeFiles, immediately before the constructor;
//   after Main, immediately before DoKnob;
//   after GetFilename, immediately before the global definitions.
// Production flags are otherwise unchanged. Each pair uses the same source,
// generated localization view and PCH path. All ten compiles succeeded with
// no timeouts. All ten retain pointer .data+4 -> literal .bss+0x0c.
// The last two /Yu objects also omit the constructor definition; this is
// additional evidence against them, not a successful placement state.
// Disposition: reject these PCH histories as an explanation of the rotation.
// This is stronger than merely enabling /YX, but not a claim about every
// possible historical precompiled-header input.
//
// 3. Real accessor TU boundary, with explicitly modeled sentinel storage.
//   build/bss-request-owner/probe.py
//   build/bss-request-owner/link.py
//   build/bss-request-owner/{results.json,link.json,HMM2PL.map}
//
// Hypothesis only: GetFilename and its empty-name backing byte could belong
// to a subsequent filename-result source owner. A disposable shared header
// declares `extern char gEmptyRequesterFilename[1];`. The prefix retains all
// other REQUEST functions/globals and changes only the pointer initializer:
//   H2_CONST char* cFRDummy = gEmptyRequesterFilename;
// The suffix retains GetFilename's unmodified body and defines:
//   char gEmptyRequesterFilename[1];
// Both TUs initially use REQUEST's existing includes and production flags.
// The ordinary --rsrc response substitutes these two raw objects, in order,
// for REQUEST.obj; all other inputs are unchanged. LINK is unmodified.
//
// Native LINK does place the backing byte at retail 0x00533d98 and keeps
// cFRDummy at 0x00516ae0. The five function literal cells now precede it.
// But this does NOT establish an acceptable reconstruction:
// - The prefix emits the 57-byte ctype init/atexit pair before GetFilename;
//   the suffix emits another pair. GetFilename moves from retail 0x00491554
//   to 0x00491590, and SEARCH's first method moves to 0x00491700.
// - The suffix's ordinary .text section has native 16-byte alignment.
//   Even removing unnecessary prefix header initialization would leave
//   GetFilename's retail address (4 modulo 16) incompatible with that new
//   ordinary contribution boundary. This rejects this particular boundary;
//   it is not pruning on the lower 70.899801% whole-file score.
// - A new extracted C++ TU also changes the already-equal compiler producer
//   count. See ../link-input-ownership/rich-producer-census.cpp.
// - A separately named backing array is still an ownership hypothesis, not
//   evidence that the original developer named or split this storage.
// Disposition: rejected; no extra array, TU, pragma or build flag retained.
// An isolated data-only backing TU would avoid the code-boundary conflict
// but merely recreates the previously rejected artificial storage owner.
//
// These matrices narrow concrete source/build families. They neither prove
// that native recovery is impossible nor justify retaining the BSS adapter
// as the final reconstruction. The correction machinery was removed separately
// in commit 20bf4a60. Exact native closure still requires a source/build owner
// satisfying both the storage references and the surrounding native
// contribution/code evidence.
