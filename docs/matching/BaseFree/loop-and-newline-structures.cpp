/*
BaseFree (RVA 0x000c3f80, retail size 902)
Entry-table ownership and newline-append structures, 2026-07-27.

The live optimized function has the retail CFG family: 28/28 blocks, exact
flow, 23 exact block bodies, and five size-only block bodies. homm2 relocs
reports 52/52 sites with no base-only site, although ordered identities and
offsets are not exact. The live object is 91.333336%; the retained
hash-scoped MAX is 94.0482%.

The first repeated divergence is the log-newline append. The candidate loads
the newline word before scanning logText and computes a strlen index. Retail
scans first, loads the word afterward, and stores through the scan end pointer.
The main loop also differs in register ownership: retail holds gpMemEntry as a
table base, while the live object reloads it around indexed accesses.

Completed structural product:

  Entry ownership:
    - direct gpMemEntry[index] access,
    - one cached MemEntry* table base,
    - one MemEntry* materialized per iteration.

  Newline append:
    - direct word store through logText + strlen(logText),
    - strcat(logText, appendNewline),
    - explicit strlen result snapshot.

All nine source shapes were crossed with clean plus 50 expansive declaration-
forest states. The complete 459/459 run took 468.57 seconds, restored source,
and produced ten normalized target states without truncation.

Best state per important structural family:
  direct + word/length:       91.333336%, size 902, 55/55 raw relocs,
                              28 blocks, 23 exact + five size-only
  cached table + word/length: 90.293175%, size 898, 53/55 raw relocs,
                              28 blocks, 22 exact + six size-only
  per-entry + word/length:    89.803215%, size 886, 53/55 raw relocs,
                              28 blocks, 22 exact + six size-only
  direct + strcat:            76.160645%, size 979, 55/55 raw relocs
  cached table + strcat:      74.883540%, size 975, 53/55 raw relocs
  per-entry + strcat:         74.590360%, size 963, 53/55 raw relocs

Although the cached and per-entry versions reproduce the apparent retail
ownership more directly, they remove two retail relocations, miss retail size,
and reduce block-body agreement. They remain useful lower-orbit evidence but
are contradicted as retained source structures.

A final end-pointer hypothesis replaced all three strlen expressions with
strchr(logText, '\0'). Its separate clean-plus-50 census completed 51/51,
restored source, and emitted one 88.718870%, size-896, 58/55-relocation orbit.
It therefore does not explain retail's scan-end-pointer instruction sequence.

Artifacts:
  build/match-variants/basefree-structure-axes-20260727.json
  build/match-variants/basefree-structure-clean-20260727.json
  build/match-variants/basefree-structure-clean-islands-20260727/results.json
  build/match-variants/basefree-structure-states-20260727.json
  build/match-variants/basefree-structure-state-islands-20260727/results.json
  build/match-variants/basefree-string-end-axes-20260727.json
  build/match-variants/basefree-string-end-states-20260727.json
  build/match-variants/basefree-string-end-state-islands-20260727/results.json

Disposition: retain the current direct source and existing MAX. No source
shape or forest state exceeded the historical maximum, so no MAX replay or
source change is justified.
*/

#if 0
// Cached-table structural arm.
MemEntry* entries = gpMemEntry;
for (entryIndex = 0; entryIndex < MEMORY_ENTRY_CAPACITY; ++entryIndex) {
    if (entries[entryIndex].ptr == ptr) {
        // Existing body accessed entries[entryIndex] throughout.
    }
}

// Per-iteration ownership arm.
for (entryIndex = 0; entryIndex < MEMORY_ENTRY_CAPACITY; ++entryIndex) {
    MemEntry* entry = &gpMemEntry[entryIndex];
    if (entry->ptr == ptr) {
        // Existing body accessed entry throughout.
    }
}

// Newline structural arms; each replaced all three sites.
strcat(logText, gMiscText.log.appendNewline.text);

i32 logLength = strlen(logText);
*reinterpret_cast<u16*>(logText + logLength) =
    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);

*reinterpret_cast<u16*>(strchr(logText, '\0')) =
    *reinterpret_cast<const u16*>(gMiscText.log.appendNewline.text);
#endif
