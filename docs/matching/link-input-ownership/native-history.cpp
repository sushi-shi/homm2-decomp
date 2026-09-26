// Native linked-image checkpoint, 2026-09-26.
// Source products: Misc/MiscRuntime ownership split and MusicFlags/Midi merge.
// See ../Misc-track-name-data/owner-split.cpp and
// ../Midi-owner-boundary/music-state-prefix.cpp for their semantic evidence.
// No compiler object, library member or linked executable is corrected.
//
// Reproduction in the root reconstruction checkout:
//   homm2 redelink
//   homm2 build
//   homm2 selftest
//   homm2 link --historical
//   ninja link-audit
// The last command intentionally fails until the native image is exact.
// Artifacts:
//   build/native-ownership-redelink.log
//   build/native-final-{build,selftest,historical,audit}.log
//   build/native-ownership-retail-review.json
//   build/link/historical/HMM2PL.{exe,map,rsp,link.json}
//
// Native inputs, independently recovered:
// - The source/header split and merge leave 96 C++ compiler inputs.
// - Audiere's complete fifteen-export DEF is compiled into a disposable DLL;
//   stock LINK supplies its import library. Both used hints (10 and 11), the
//   two descriptor producer records and the short-import census match retail.
//   See ../Audiere-import-library/native-definition.cpp.
// - The ordinary PDB path is
//   e:\Users\igorl\VSS\HMM\HMM2\temp\release\game\HMM2PL.pdb.
//   Start with no PDB, link once with process clock 2003-02-26 14:51:33,
//   then three times at 2003-04-04 08:19:23. LINK emits the NB10 signature,
//   age four and PE/debug timestamps itself. No metadata bytes are rewritten.
//
// Final current evidence:
// - homm2 build: 1727/1727 exact functions, 291995/291995 data bytes.
// - homm2 selftest: 955 tests pass, six skips.
// - SetupCDDrive and the three music-state accessors plus MIDIStartup pass
//   focused relocation review (47, 1, 2, 1 and 7 ordered sites respectively).
// - All sixty claimed function/helper spans owned by Misc, MiscRuntime and
//   Midi equal raw retail bytes at their actual retail RVAs. The entire
//   3928-byte Misc initialized-data interval equals retail.
// - Both executable sizes are 1208393 bytes. PE headers, complete ordered
//   Rich producer records, resource bytes and NB10 tail are byte-identical.
// - 506 bytes still differ: .text 501, .rdata 3, .data 2.
//   Five instruction operand bytes and cFRDummy's initialized pointer byte
//   are REQUEST's six-cell BSS identity rotation. The other 500 bytes arise
//   from the still-unrecovered AudiereEffects/DIMMER contribution placement
//   and references to those functions. This is NOT whole-image closure.
// Candidate SHA256:
//   9b61ebb58edc19038b59c7ebdcd466fb64e1f52a2b025726f55a576902239c9b
// Retail SHA256:
//   bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
//
// Rejected metadata diagnostic, preserved to avoid repeating it:
//   build/native-asm-probe.py; build/native-asm/results.json
//   BITS.asm and TILE.asm, each /nologo /c /Zd or /nologo /c /Zi, ordinary
//   OMF objects and native LIB/LINK. Both complete arms leave every linked
//   section and the producer census unchanged. No assembler flag retained.
//   The missing 0x000420ff records belonged to Audiere import descriptors,
//   not these assembly sources. Actual artifact records decide attribution.
