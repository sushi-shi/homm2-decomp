// S44: private RUMOUR_CATEGORY_ORDER_CAPACITY 8 -> GAME_PLAYER_COUNT(6).
// Complete GetCategoryStats/SortStats reads prove accesses0..playerCount-1.
// Full2/2 in3.620s, restored, no truncation, no AST/state probes:
// build/s44-{axes,manifest}.json and build/s44-batch/results.json.
// Both1613bytes/54relocs/textsha00278bb9f3e2b610,63blocks/433instructions.
// Both matrix strict FALSE due private doubles; ordinary bytes/DIR32/other
// targets pass. check-rumour-constants.py proves all12 missing operands and
// exact0.33/0.66 values at4ea628/4ea620, without weakening general verifier.
// Whole102-function/all-section/ordered-relocation native equivalence PASS.
