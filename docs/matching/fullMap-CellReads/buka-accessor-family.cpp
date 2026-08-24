/*
Cross-version family: fullMap cell-read spellings

Retained PoL source
-------------------
25 explicit Row(y)[x] / Row(y) + x reads become GetCell(x, y):
  SOURCE/GAME    12 (the documented 11-site sweep plus the earlier
                     ConvertAllToLateOverlay site)
  SOURCE/EVENTS   8
  SOURCE/ADVMGR   5

PoL keeps the direct inline body:
    mapCell* GetCell(i32 x, i32 y) { return cells + width * y + x; }
It does not add Buka's Column accessor.

Whole-TU invariant check
------------------------
The complete 25-call Buka batch and the original PoL batch were rebuilt from
the same clean header state. Normalized .text is byte-identical for every
affected TU:
  ADVMGR  608c49f3a929d679f7ccf143634268e0a497cd15a54b9b0c4c6eb10ccfcbe1e3
  EVENTS  16d16b9b7d7213e7602fd8b15de479b9e5a03bf4d56267c22f4893f7f86508de
  GAME    4068dc4db0f5e330a68d2291d419c9fb352fe717f83aaa867468dc23ea5af8eb

GAME's complete text-relocation stream is raw-identical. ADVMGR/EVENTS differ
only in anonymous `$L`/`$T` counter spellings caused by the different number
of inline accessor references; after canonicalizing those disposable names,
their complete relocation streams are identical as well. All live function
scores, sizes, instruction text, and ordered external relocations are
unchanged. The source port is semantic and emits no shipped-byte change.

Shared-header isolation
-----------------------
Reviewed clean states:
  original PoL header:          1161 live exact, 98.61% fuzzy
  add unused Column only:       1171 live exact, 98.67% fuzzy
  Column + nested Buka GetCell: 1163 live exact, 98.42% fuzzy

Column-only changes 163 unrelated functions (31 exact losses, 41 gains): an
unretained compiler-state probe, not evidence for an unused declaration.
Holding that state fixed, the nested Buka GetCell body changes 18 callers;
all 18 worsen, 8 exact functions are lost, and none improve. Therefore the
nested body is compiler/version-specific and is not ported to VC4.2.

The Column-only probe did expose two audited exact target-local islands before
restoration:
  RemoteMain      1754 bytes, 81/81 blocks, ordered 143/143 relocs
  ChainLightning  865 bytes, 23/23 blocks, ordered 37/37 relocs
Artifacts: build/fullmap-header-probe/column-only/

Disposition:
  explicit call sites: invariant matching-decomp source port;
  shared nested GetCell body / Column declaration: no port.
*/
