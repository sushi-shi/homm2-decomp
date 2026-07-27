/*
IconToBitmap, retail RVA 0x000d0570, size 0x4ed.
Greedy prefix entry-lifetime product, 2026-07-27.

The first disassembly divergence was register ownership between the selected
IconEntry pointer and its serialized x/srcOffset fields.  Six legitimate
opening structures were crossed with the clean state and all 50 top-of-TU
declaration-forest states:

  - retained repeated data + entryOffset expressions;
  - C89-style setup declarations followed by assignments;
  - separate field and entry-pointer declarations;
  - entry-pointer declaration before the field snapshots;
  - an IconEntry reference owner; and
  - one byte pointer naming the selected serialized entry record.

All 306 cells compiled in 252.31 seconds, the run was not truncated, and
source was restored:

  build/icon2b-prefix-entry-lifetimes-axes-20260727.json
  build/icon2b-prefix-entry-lifetimes-states-manifest-20260727.json
  build/match-variants/icon2b-prefix-entry-lifetimes-states-20260727/results.json

The selected-record byte lifetime is a distinct stronger family.  Clean source
improved from 80.945050% to 81.123630%.  Forest trial 1, repeated at trials 26
and 44, raised hash-scoped MAX from 81.631870% to 81.673080%.  Its object is
size 1220 with 79/83 relocations and exact 80-block flow: 63 exact plus 17
size-only block bodies.

Before retention, a typed selected-entry pointer was tested independently
against the same 50 states.  VC4.2 canonicalized it with the reference-owner
family instead: clean 79.156590%, best 79.706050%, size 1217, 79/83
relocations.  The byte lifetime is therefore not a cosmetic cast spelling; it
preserves the serialized record-base ownership visible in retail's opening
loads.

Artifacts for the typed screen and official MAX replay:

  build/tu-state-noise/icon2b-selected-entry-typed-20260727/
  build/icon2b-selected-entry-typed-summary-20260727.json
  build/tu-state-noise/
    icon2b-selected-entry-bytes-trial1-record-max-20260727/
  build/icon2b-selected-entry-bytes-trial1-summary-20260727.json

The official recorder retained 81.6731 for effective-source hash
5771c983d173.02e5f0cc8453.  No generated declaration is retained.

Disposition: keep the semantic `entryData` lifetime.  It names the selected
record within icon::m_data and removes three repeated pointer expressions; it
does not introduce fake storage or raw field offsets.  Exact closure remains
open at the same four missing ordered reloads.
*/

#if 0
u8* data = srcIcon->m_data;
i32 entryOffset = frame * sizeof(IconEntry);
u8* entryData = data + entryOffset;
i32 entryX = reinterpret_cast<IconEntry*>(entryData)->x;
i32 sourceOffset = reinterpret_cast<IconEntry*>(entryData)->srcOffset;
IconEntry* entry = reinterpret_cast<IconEntry*>(entryData);
#endif
