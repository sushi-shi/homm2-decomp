/*
DecodePacket cross-version audit, 2026-08-24.

Targets:
  PoL 2.0: RVA 0x000a3aa7, 314 bytes, 7 blocks.
  Buka:    RVA 0x0008d93c, 269 bytes, 7 blocks.

Retained PoL result: 99.973335%, 314/314 bytes, exact 7/7 CFG, aligned
224-byte frame, and complete ordered 17/17 relocations. Raw delta: two stack
displacement bytes at the CRC comparison; retail loads received CRC before
calculated CRC, while the candidate loads them in the reverse order.

Buka declaration-family matrix (2/2 clean):
  build/decode-packet-cross-version-manifest.json
  build/source-variant-batch/decode-packet-cross-version/results.json

The exact Buka crc/res/crc2/text/len family emits 293 bytes under VC4.2 and is
structurally incompatible with the 314-byte PoL target. Retain PoL's bucket-
pinned names and declaration family.

PoL spelling matrices:
  build/decode-packet-compare-manifest.json                    2/2
  build/source-variant-batch/decode-packet-compare/results.json
  build/decode-packet-guard-manifest.json                      5/5
  build/source-variant-batch/decode-packet-guard/results.json
  build/decode-packet-array-read-manifest.json                 4/4
  build/source-variant-batch/decode-packet-array-read/results.json

Both != operand orders, negated equality, and [0]/pointer-dereference forms
are byte-identical. Empty-positive-arm equality forms grow to 319 bytes and
contradict retail size.

Unchanged-source census:
  build/decode-packet-state-summary.json
  build/tu-state-noise/decode-packet-cross-version/

All 50/50 forest/top trials completed and collapsed to one byte/relocation
state; no exact island. Disposition: bounded matching residual, no source/MAX
change. A future attempt needs new evidence, not more compare spellings.
*/
