// C25: build/c25-axes.json, c25-owner-manifest.json, c25-owner-batch/results.json.
// 2/2 cast/direct arms with recovered b8 game::m_cheated; 3.91s, restored,
// no truncation or state probes. Both530bytes/55relocs/19matchingblocks.
// Whole98-unit native before/after bytes and ordered relocations pass.
// Standalone retail check still rejects four pre-existing private local-static
// identities (cheatWarned/debugWarned), not the field load or ordinary bytes.
// No standalone full-function closure claim. See reconstruction/C25-C32.md.
if (static_cast<i8>(gpGame->m_cheated)) {  // previous spelling
if (gpGame->m_cheated) {                  // retained: typed signed-byte owner
