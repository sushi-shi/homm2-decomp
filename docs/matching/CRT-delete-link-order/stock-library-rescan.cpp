// Link-order experiment; this is evidence, not reconstructed source.
//
// Baseline response:
//   build/link/native-current-data-adapters-no-common/HMM2PL.rsp
// Source/BSS control:
//   build/link/native-source-bss-stock-delete/
//
// The former link input extracted VC6 LIBCMT's delete.obj and wrapped that
// untouched object in a one-member archive at the point immediately after
// BASE-suffix.lib.  This probe replaced that archive with the ordinary stock
// LIBCMT.LIB and retained the later ordinary LIBCMT.LIB occurrence.
//
// VC6 LINK's first archive scan selected only delete.obj at retail RVA
// 0x000d68c2.  The later scan selected the remaining runtime members.  The
// resulting .text, .data, and .rsrc sections were byte-exact; .rdata differed
// only at the volatile four-byte link/debug timestamp and one PDB-path byte.
// The full-file audit was 99.990649% because the diagnostic output used the
// current timestamp and a different output/PDB pathname.
//
// Disposition: ACCEPT the ordinary repeated-library input as the stronger
// historical model.  A stock LIBCMT rescan produces the same delete.obj
// placement without a derived archive.  This result is isolated from the
// other still-active data/COMDAT/CRT-order adapters in the response file and
// does not claim that the complete link is transformation-free.
