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
// Exhaustive ordinary-library scheduling probes:
//
//   build/link/stock-position-matrix/results.json
//   build/link/defaultlib-matrix/results.json
//
// Ten placements of an untouched first LIBCMT scan were tested: before the
// sources, after source 1 and 10, around OLDNAMES, around each BASE archive,
// after BASE-suffix, and immediately before the final scan.  No position emits
// retail Rich order.  The late positions retain exact .text/.data but produce
// B(8), A(133), E(37); early scans also reorder linked sections.  /INCLUDE on
// operator delete does not isolate the member.
//
// The objects' embedded /DEFAULTLIB:LIBCMT path was tested both with and without
// the explicit final LIBCMT, while independently enabling embedded OLDNAMES and
// LIBCPMT.  Every LIBCMT-resolved arm again emits B,A,E.  Allowing all embedded
// defaults selects one LIBCPMT member and is also non-retail.  Thus
// /NODEFAULTLIB suppression is not the missing developer link option.
//
// A local archive census covered VC6 RTM/SP3/SP5, VC5, LIBCP/LIBCPMT/MSVCPRT,
// debug variants, CTL framework archives, and the preserved XP CRT families.
// Only SP5 LIBCMT supplies the exact 11-byte body with compiler ID 0x000b1f6f.
// VC6 RTM/SP3 and the C++ libraries stamp 0x000b1fe8; VC5's delete.obj has a
// different 16-byte body; CTL and XP candidates have different allocator/code
// semantics.  No authentic second local archive currently supplies the retail
// member.

// Direct-object and combined-archive probes:
//
//   build/link/rich-order-natural-probes/results.json
//   build/link/combined-crt-archive/results.json
//
// Passing the untouched delete.obj directly is not equivalent to an archive
// scan: LINK processes it in the direct-object phase, 210,513 bytes differ, and
// the B(8) Rich producer moves behind the linker/import records.  Adding the
// member to BASE-suffix.lib, with BASE's exact physical member order preserved,
// gets the retail A(133),E(37),B(8) Rich order but resolves operator delete as
// soon as that archive opens.  It lands in the pre-BITMAP gap at 0x004c5cc2
// instead of 0x004d68c2, changing 1,113 call-site bytes while .rdata, .data, and
// .rsrc remain exact.  Member-first and member-last forms behave identically.
// Merging the complete stock LIBCMT into BASE instead pulls the runtime in the
// wrong scan phase and changes 164,043 bytes.  The separate post-BASE archive
// boundary is therefore required by LINK resolution semantics, not merely by
// the current response-file spelling.
//
// Disposition: REJECT repeated stock LIBCMT as whole-file evidence.  Retain the
// one-member ordinary VC6 LIB archive until an authentic separate archive owner
// of the exact 0x000b1f6f delete.obj is identified.  No object bytes are changed.
