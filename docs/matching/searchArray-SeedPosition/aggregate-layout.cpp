/*
Function: searchArray::SeedPosition
Retail RVA/size: 0x000917d6 / 0x0cc2
Candidate object: build/objdiff/base/SOURCE/SEARCH.obj
Raw-link audit:
  build/link/native-current-data-adapters-no-common-stock-crt/
  audit-search-raw-frozen.json

The 29 former file-static declarations are one persistent search-phase state
record.  Their retail span is 0x00533da0..0x00533e2c.  Modeling that record as
SeedPositionState makes VC6 emit a single 0x8c-byte, eight-byte-aligned .bss
contribution with s_seedPositionState at offset zero.  The two four-byte holes
inside the span have no reviewed symbol or relocation owner and remain padding.

The canonical build keeps SeedPosition exact and reports 286 candidate and 286
retail relocations with no candidate-only identities.  Replacing only the old
SEARCH BSS-adapted input with the ordinary compiled SEARCH.obj in the raw VC6
LINK response preserves exact .text, .data, .rdata, and .rsrc bytes under the
reviewed historical timestamp/PDB-age sequence.  The complete file differs
only in the independently tracked ten Rich-header bytes.

Disposition: ACCEPTED.  Retain the semantic aggregate.  No generated symbol
name, alignment directive, input-object rewrite, or post-link byte change is
needed for SEARCH storage placement.
*/
