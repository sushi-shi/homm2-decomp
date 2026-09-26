// Native Audiere import-library producer recovery, 2026-09-26.
// Worktree matcher-2, branch matcher/bss-audiere.
//
// Evidence/artifacts:
//   build/native-import-producers/census.py
//   build/native-import-producers/archives.json
//   build/native-import-producers/audiere.lib
//   build/native-import-producers/link-native.py
//   build/native-import-producers/{link.rsp,link.log,HMM2PL.exe,HMM2PL.map}
//   build/native-import-producers/results.json
//
// Real input discrepancy:
// - root build/link/generic-imports/audiere.lib had regular COFF members with
//   no @comp.id, including its import descriptor and null thunk.
// - root build/link/audiere.lib (native Microsoft LINK output) had three
//   descriptor members bearing @comp.id=0x000420ff plus short imports.
// - Both root native-historical/generic_imports.rsp and the supposedly
//   contrasting retail_abi_imports.rsp still selected the generic Audiere
//   archive. Their identical census was therefore not a native/regular test.
//
// Only two Audiere functions are linked. The generic library contributes four
// versionless inputs: descriptor, null thunk, two regular function members.
// The shared null import descriptor was already supplied by an earlier SDK
// library, so Audiere's own null descriptor is not selected. Native Audiere
// contributes two0x420ff descriptors and two short-import records instead.
//
// Measured replacement delta, exactly explaining the retail census gap:
//   0x000420ff: absent ->2
//   0x00000000:     58 ->54
//   0x00010000:    193 ->195
// No assembler or source-TU change is involved.
//
// The accepted normal generator uses the complete reviewed ABI, not the
// diagnostic retail-derived filler exports in the older native archive:
//
// python3 -m homm2.build.import_lib \
//   --definition imports/audiere.def --dll audiere.dll \
//   --out build/native-import-producers/audiere.lib
//
// It compiles exactly the15 actual declared API functions as disposable C
// export stubs, invokes untouched Microsoft LINK /DLL /NOENTRY /NODEFAULTLIB,
// and retains LINK's import library. No COFF fields, symbols, archive hints,
// Rich records, or PE bytes are patched. No additional exports are invented.
// All15 native hint entries are verified against sorted declared ABI names.
// The two actual retail imports independently agree:
//   _AdrOpenDevice@8        hint10
//   _AdrOpenSampleSource@4  hint11
//
// Full stock-link control replaces only the generic Audiere archive in the
// root combined Misc/Midi response. All11 ordered Rich records now exactly
// equal retail (results.json contains both complete lists). Whole .text,
// .data and .rsrc are unchanged from the prior historical control. Section
// RVAs and virtual sizes are unchanged. .rdata differs only because this
// diagnostic link uses a fresh timestamp/PDB rather than replaying the root's
// four-pass historical state. Remaining retail mismatches in program sections
// are501 .text bytes,2 .data bytes and0 .rsrc bytes; not full image closure.
//
// The implementation adds --definition mutually exclusive with --exe;
// diagnostic image-derived mode remains available separately. It reuses the
// existing strict DEF parser through a deferred import, avoiding a circular
// module import and avoiding a second interpretation of the ABI manifest.
//
// Validation:14 focused import-library/parser tests passed, including tests
// that definition mode does not read retail hints and that the complete ABI
// emits exactly15 declared exports with the evidenced native hint positions.
// Full selftest result is retained in build/native-import-producers/selftest.log.
// Full selftest ran953 tests (6 skipped), with one unrelated expected lane
// discrepancy: test_ordinary's fixed edge count expects107, while this lane's
// Midi merge alone produces106. Root's paired Misc split restores that missing
// source owner. No import-library test failed. Recheck the full suite after
// integrating both ownership changes; do not weaken the edge-count test here.
