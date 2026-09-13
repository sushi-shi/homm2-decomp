// U05: retained structural recovery against6061dd78, not a spelling/state sweep.
// Actual source: include/SOURCE/KB_TYPES.h and11 SOURCE TUs.
// Complete source/initializer and70-function census: docs/reconstruction/U05.md.
// Result: all98 native objects/1826 functions/allocated sections/ordered relocs
// unchanged; all66 records/1716 bytes independently equal retail.
// Both dialects and actual VC6 layout assertions pass. Per-body retail limits
// remain explicit in U05-functions.tsv; no unknown-owner allowance was added.
// Old cost/value: a single-arm union/struct containing i16 cost and a union of
// two i32 names, fightValue/randomValue. New: i16 cost; i32 fightValue.
// Old flags: attributes over flags.{all,abilityFlags,bytes.{abilities,attributeFlags}}.
// First three all had the same MonsterFlags type; the two-byte view was unused.
// New: MonsterFlags attributes. All member operations preserve type and offset.
// Initializer example:
// before: {{20, {33}},17,12,1,FACTION_KNIGHT,2,1,1,1,1,0,"psnt",{MONSTER_FLAGS_NONE}}
// after:  {20,33,17,12,1,FACTION_KNIGHT,2,1,1,1,1,0,"psnt",MONSTER_FLAGS_NONE}
// No temporary compiler-state declarations are retained.
