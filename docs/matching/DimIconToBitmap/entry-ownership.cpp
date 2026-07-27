/*
DimIconToBitmap, retail RVA 0x004cfd50, size 0x26e.
Same-family frame-entry ownership campaign, 2026-07-27.

The clean classifier tested the current owned Entries() table, a direct selected
entry, a direct entry with all field snapshots, and an entry-reference version.
The direct selected-entry arm was structurally credible at the retail-nearer
608-byte size, so it and the current arm were each crossed with the clean state
plus all 50 forest states. The survivor matrix completed all 102 builds in
68.74 seconds and restored source.

Artifacts:
  build/icond2b-entry-owner-axes-20260727.json
  build/icond2b-entry-owner-clean-20260727/results.json
  build/icond2b-entry-owner-survivors-axes-20260727.json
  build/icond2b-entry-owner-survivors-states-20260727.json
  build/icond2b-entry-owner-survivors-states-20260727/results.json

Clean results:
  owned Entries() table       83.6437%, 611 bytes, 35/37 relocations
  direct selected entry       83.4713%, 608 bytes, 35/37 relocations
  all-field snapshots         77.6954%, 614 bytes, 35/37 relocations
  entry-reference snapshots   77.6954%, 614 bytes, 35/37 relocations

The survivor census reproduced the unchanged arm's durable 86.2299% MAX at
trial 20. The direct selected-entry family peaked at 85.9138% at trial 26.
Neither ownership recovered the two missing predicate reloads or exact ordered
relocations.

Disposition: preserve the current owned-table setup. The 608-byte direct-entry
version remains a credible lower island, but it did not improve MAX and is kept
only as the documented branch seed below.
*/

#if 0
IconEntry* const entry = &srcIcon->Entries()[frame];
i32 entryX = entry->x;
u8* const srcData =
    reinterpret_cast<u8*>(srcIcon->Entries()) + entry->srcOffset;
gDimEntry = entry;
const i32 entryY = entry->y;
gDimSrc = srcData;
#endif
