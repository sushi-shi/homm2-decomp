// OMF link-input reconstruction control (2026-08-23)
//
// Retail evidence:
//   The Rich producer census and the linked contribution boundaries identify
//   BASE/BITS and BASE/TILE as inputs converted from OMF by VC6 LINK. The
//   matching tree therefore retains only the assembler implementations.
//
// Source products:
//   src/BASE/BITS.asm
//   src/BASE/TILE.asm
//
// Independent assemblers tested:
//   Microsoft Macro Assembler 6.11, default OMF output
//   JWasm 2.20, `-Zg -omf`
//
// Both object pairs were passed to the unmodified VC6 SP5 LINK.EXE. LINK
// reported LNK4033 for each input and performed its native OMF-to-COFF
// conversion. The resulting BITS and TILE functions landed at retail RVAs:
//
//   BitTest       0x004c2ed4
//   BitSet        0x004c2f02
//   BitClear      0x004c2f22
//   TileToBitmap  0x004c2554
//
// MASM 6.11 `/coff` objects assembled from those same source products are the
// authoritative objdiff inputs. This compares the real instruction stream and
// ordered relocations without retaining a duplicate inline-assembly C++ body;
// the final link still uses ordinary OMF and therefore preserves the two MASM
// Rich-header producer records.
//
// The complete linked `.text`, `.data`, and `.rsrc` sections were byte-identical
// between the MASM and JWasm controls. Their only PE difference was the ordinary
// NB10/PDB timestamp word in `.rdata`. Against retail, the natural-link project
// placement census improved from 1,700/1,727 to 1,715/1,727 exact RVAs.
//
// Disposition:
//   RETAIN the MASM-syntax OMF source and assemble it with the pinned original
//   Microsoft MASM 6.11 media packaged in the VC6 release. JWasm remains an
//   independent control, not a final-link producer. REMOVE the former link-only
//   COFF header adapter and the misleading matching-tree C++ mirrors. Portable
//   translations live only in the clean-source generator overrides.
