// HISTORICAL EXPERIMENT: the source-first review rejected retaining this storage
// or split-owner arrangement as recovered developer structure. The measured bytes
// remain useful diagnostics. Current source and complete follow-up products are
// described in ../source-first-ownership/two-passes.cpp.
//
// Full native-link follow-up to nested-template-split.cpp, 2026-09-27.
// Root: decomp-gold-2.1-buka. Production source/build inputs remain unchanged.
//
// Real semantic source family: heroWindow::DimmerWidget<BaseWidget>, with the
// first five primary-template definitions instantiated by a narrow prefix TU,
// and the explicit empty destructor instantiated by a suffix TU retaining KB.h.
// See nested-template-split.cpp for complete source shape and raw-object proof.
// WINDOW uses the same nested type via a typedef in a disposable header overlay.
// The actual heroWindow fields, layout, and public functions are unchanged.
//
// Root WINDOW recompile: all 18 raw function spans exactly match production.
// Complete ordered relocation comparison changes only the derived ctor/Read
// identities in heroWindow's resource constructor and a compiler-counter EH
// label in SaveBackground. Raw spans and all relocation records are retained
// in WINDOW-audit.json; final linked-byte checks below resolve their destinations.
//
// Native input alternatives, complete historical four-LINK history in each:
// 1. Existing BASE-suffix archive, prefix then destructor object in LIB inputs:
//    13281 image bytes differ; destructor extracted late at VA 0x004d6860.
//    Swapping these two LIB inputs produces the identical image. Neither fixes
//    native archive extraction scheduling. Prefix's six methods and vtable do
//    already occupy their required retail addresses.
// 2. Direct BASE-suffix objects, ordered by the unchanged MAP's ordinary-function
//    arrival addresses: unchanged-source control differs from production by
//    250826 bytes; candidate differs from retail by 251085 bytes. Direct objects
//    are processed before the preceding archives. Reject this build family.
// 3. Three ordinary archives partition the existing BASE-suffix members into
//    pre-DIMMER owners, DIMMER, and post-DIMMER owners. Member order within each
//    partition remains the configured order. No section or symbol is rewritten.
//    This separates the pending destructor dependency from later widget inputs.
//
// Third-family unchanged-source control is BYTE-FOR-BYTE production-identical:
//   1208393 bytes; SHA256
//   c0eacb837c0d8bdec918962df5bce11270d05247908f4be34badd4015255a1b8
// Its retail mismatch remains exactly 500 bytes. Thus archive grouping itself
// is independently controlled before evaluating the new source topology.
//
// Third-family split-source candidate:
//   1208393 bytes; SHA256
//   54ead2bb5e88c4f01607cbc381c644284bbc982bd1b3f5930883d2a0df5ade06
//   468 differing bytes: headers 264, .text 203, .rdata 0, .data 1, .rsrc 0.
// Retail SHA256:
//   bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a
//
// Native code/data closure for this local structural family:
// - All 464 bytes at RVA 0xd3310..0xd34df equal retail, including the seven
//   methods, both ctype initializer helpers, every resolved relocation operand,
//   and native interfunction alignment padding. BUTTON still begins at d34e0.
// - The complete 12-byte vtable at RVA 0xeaa04 equals retail.
// - Exactly 296 prior nonheader mismatches are corrected. No new nonheader
//   mismatch occurs anywhere in the entire executable. The remaining 204 are
//   the pre-existing Audiere placement bytes; output size and NB10 tail agree.
// - The three-archive control proves this is native source/build output, not
//   a corrected object/image. Both candidate objects are unmodified CL output;
//   LIB and LINK perform ordinary archive extraction and COMDAT selection.
//
// Unresolved producer provenance prevents retention as a complete fix:
// - Retail/production Rich C++ producer 0x000b2306 has count 96; candidate 97.
//   Every other decoded ordered producer/count pair equals retail.
// - Changed Rich checksum changes native DOS/PE-header placement: retail PE
//   offset 0x100; candidate 0xf8. The complete 408-byte PE/section-header body
//   is still byte-identical when read at each image's own declared PE offset.
//   These are metadata consequences, not a reason to patch output bytes.
// - No authentic compensating source-owner consolidation has been recovered.
//   The three archive partitions likewise remain a measured sufficient build
//   arrangement, not a claim about the historical project's library names.
// Disposition: retain compiled clue artifacts and this measured source family;
// do not integrate a 97-input production build or claim executable equality.
//
// Artifacts under build/dimmer-split-native/:
//   probe.py, probe.log, WINDOW-audit.json, WINDOW.obj, include/BASE/*.h
//   DIMMERPrefix.obj, DIMMERDestructor.obj, BASE-{prefix,suffix}.{lib,rsp,log}
//   reversed_archive.py, reversed-archive/{HMM2PL.*,results.json,link-*.log}
//   direct.py, direct-member-order.json, direct-results.json
//   grouped_archives.py, grouped-results.json, grouped-{control,split}/
//   section-audit.json, nonregression.json
// Each linked arm preserves response files, all four link logs, native MAP and
// executable. No /FORCE option is present in these complete full-game links.
