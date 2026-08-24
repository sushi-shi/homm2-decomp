/*
game::MakeAllWaterVisible and fullMap::Column
=============================================

PoL 2.0 RVA 0x80d94; Buka 2.1 RVA 0x5b944.

The first four-arm read/write product exposed a missing type member: Buka's
`fullMap::Column` call did not compile in PoL. The exact Buka inline accessor
was added to `fullMap` and the complete product was rerun over clean plus 50
forest/top states.

Complete accessor-aware product: 204/204
-----------------------------------------
  build/make-all-water-visible-state-manifest.json
  build/source-variant-batch/make-all-water-visible-state/results.json

Best per call-site family:
  Row + PoL subscript:       100.000000%, trial 16, 216 bytes
  Row + Buka flat write:      96.949150%
  Column + PoL subscript:     86.796610%
  Column + Buka flat write:   83.813560%

The retained trial-16 island has exact 13/13 CFG and complete ordered 7/7
relocations. Trials 32 and 49 also close the same PoL call-site family.

Disposition: matching-decomp port of the real `fullMap::Column(i32)` inline
accessor only. Keep PoL's `Row(y)[x]` read and `mapExtra[y * MAP_WIDTH + x]`
write; Buka's call-site family is compiler/version-specific, not gameplay.
The integrated header state raises live exact functions from 1162 to 1172.
*/
