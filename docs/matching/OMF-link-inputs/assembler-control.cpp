// OMF link-input reconstruction control (2026-08-23)
//
// Retail evidence:
//   The Rich producer census and the linked contribution boundaries identify
//   BASE/BITS and BASE/TILE as inputs converted from OMF by VC6 LINK. Their
//   reconstructed C++ bodies are already exact, but compiling those mirrors
//   creates COFF COMDATs with different placement.
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
// The complete linked `.text`, `.data`, and `.rsrc` sections were byte-identical
// between the MASM and JWasm controls. Their only PE difference was the ordinary
// NB10/PDB timestamp word in `.rdata`. Against retail, the natural-link project
// placement census improved from 1,700/1,727 to 1,715/1,727 exact RVAs.
//
// Disposition:
//   RETAIN the MASM-syntax OMF source and the reproducible JWasm build rule.
//   REMOVE the former link-only COFF header adapter. The C++ files remain the
//   annotated analysis and objdiff mirrors; they are not the final link inputs.
