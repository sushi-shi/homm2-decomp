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
// The resulting .text, .rdata, .data, and .rsrc sections are byte-exact, and
// delete.obj lands at retail RVA 0x000d68c2.  However, Rich evidence proves that
// the first stock scan did not isolate delete.obj: the compiler records are
// ordered B(8), A(133), E(37), while retail and the one-member control are
// A(133), E(37), B(8).  With historical NB10 state held apart, ten DOS-stub/Rich
// bytes differ.  Section placement alone was too weak to prove archive ownership.
//
// Untouched VC6 LIBCPMT.LIB, LIBCP.LIB, and MSVCPRT.LIB each isolate the same
// 11-byte delete body and preserve every PE section, but their member has
// compiler ID 0x000b1fe8.  Retail requires the /MT LIBCMT member ID 0x000b1f6f;
// the resulting Rich census is B1f6f(7), A1f6f(133), E1c83(37), B1fe8(1).
// Untouched LIBC.LIB recursively resolves the single-threaded runtime and changes
// the final sections.  The local XP CRTs use later 5-byte tail-jump bodies, while
// its c932/c1032 compatibility libraries also emit different bodies.
//
// Disposition: REJECT repeated stock LIBCMT as whole-file evidence.  Retain the
// one-member ordinary VC6 LIB archive until an authentic separate archive owner
// of the exact 0x000b1f6f delete.obj is identified.  No object bytes are changed.
