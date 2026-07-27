/*
FlipIconToBitmap, RVA 0x000d1ba0, retail size 1265.
Same-family entry ownership and row-visibility campaign, 2026-07-27.

Retail setup keeps the frame-table offset separate long enough to load fields
before publishing the selected entry.  FlipIconToBitmapColorTable also binds
the selected entry directly and snapshots entry fields independently.  The
retained setup is:

    u8* src = srcIcon->m_data;
    i32 x0 = x;
    i32 w;
    IconEntry* entry = &srcIcon->Entries()[frame];
    w = entry->w;
    i32 entryX = entry->x;
    i32 entryY = entry->y;
    i32 sourceOffset = entry->srcOffset;
    x0 = x0 - entryX;
    src += sourceOffset;
    x0 = x0 - w;

The clean 16-arm entry-owner/field-lifetime matrix compared owned Entries(),
direct Entries()[frame], a byte-cursor cast, and reference ownership against
baseline, entry-X, all-field, and subtract-before-advance lifetimes.

Artifacts:
  build/flip-icon-entry-owner-clean/
  build/flip-icon-entry-owner-clean.json
  build/flip-icon-entry-survivors-states/
  build/flip-icon-entry-survivors-states.json

Direct entry ownership plus all four field snapshots reached 87.801060% clean,
size 1251, 79/81 relocations.  Three surviving source structures were crossed
with clean plus 50 declaration-forest states (153/153 builds, not truncated).
No state exceeded that clean result.

The ColorTable sibling also centralizes the repeated vertical clip predicate
in one inline helper.  The equivalent retained mechanism is:

    static inline i32 FlipRowVisible(i32 clipTop) {
        return clipTop <= gFlipY && gFlipY <= gFlipClipB;
    }

It replaces the independent fill, dim, and literal-path currentY snapshots.
Both entry setups and both visibility structures were crossed with clean plus
50 states: 204/204 builds completed without truncation.

The matrix clean object is 86.846150% and the canonical build is 86.8727%,
both size 1251 with 79/81 relocations.  Forest trial 25 reaches a new
88.026530% island with the same size and relocation stream.  It was replayed
with tu_state_noise.py and recorded as the current hash-scoped MAX; generated
declarations were restored and not retained.

Artifacts:
  build/flip-icon-row-visible-clean/
  build/flip-icon-row-visible-states/
  build/tu-state-noise/flip-icon-row-visible-trial25/

The previously documented exact-76-block clip-bottom control was also crossed
with this new snapshot setup and clean plus 50 states.  All 51 builds completed;
best was 86.249340%, size 1244, 79/81 relocations.  It remains a structurally
coherent lower branch and is not retained.

Artifacts:
  build/flip-icon-entry-clip-control-clean/
  build/flip-icon-exact-cfg-snapshot-states/

Disposition: retain the direct entry snapshots and sibling row-visibility
helper.  Retained MAX advances from 87.0212% to 88.0265%.  Exact closure is
still blocked by the setup/clip block and two ordered relocation occurrences.
*/
