// Unit: BASE/Misc
// Retail BSS span: 0x00536088..0x005360d1
// Candidate object: build/objdiff/base/BASE/Misc.obj
// Raw-link probe: build/link/native-source-bss-probes/
//
// The former declarations mixed tentative definitions from the start and end
// of the TU.  VC6 consequently emitted giFindMid after four DataEntry globals
// and emitted gBlitRight after the remaining DataEntry globals.  Giving the
// zero-initialized state its ordinary explicit initializers in retail address
// order produces the complete raw BSS topology directly:
//
//   giFindMid          +0x00
//   bDataEntryTime     +0x04
//   inBoxY             +0x08
//   inBoxX             +0x0c
//   gBlitBottom        +0x10
//   gBlitRight         +0x14
//   DataEntryWin       +0x18
//   cDEDest            +0x1c
//   iDEMaxLen          +0x20
//   iMemEntries        +0x24
//   gpMemEntry         +0x28
//   giTotalMemAllocated +0x2c
//
// The seven compiler empty-string allocations remain naturally at
// +0x30..+0x48.  The resulting raw .bss section is 0x49 bytes with four-byte
// alignment and matches the reviewed retail placement without an input-object
// BSS rewrite.  The isolated raw-link probe retains only the pre-existing,
// independent Misc track-name .data-contribution adapter; with that adapter,
// .text, .data, and .rsrc remain byte-exact and only link metadata differs in
// .rdata.
//
// Disposition: RETAIN the explicit zero-initializer declaration order.  The
// track-name .data contribution remains a separate source-recovery problem.
