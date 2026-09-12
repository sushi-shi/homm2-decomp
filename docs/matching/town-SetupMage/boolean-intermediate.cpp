// C45, SOURCE/TOWNMGR RVA0xa9c0b. Complete removal family:
// retained: spellState_c = static_cast<i16>(slot_o >= m_town->m_spellCounts[level_f + 1]);
// rejected: spellState_c = (slot_o >= m_town->m_spellCounts[level_f + 1]);
// build/c45-mage-{axes,manifest}.json; build/c45-mage-batch/results.json.
// Complete2/2 in2.324s, no AST/state probes/truncation, restored.
// Retained889bytes/34relocs/textSHA21c614f112fda4ec,26 exact diagnostic blocks.
// Rejected886bytes/34relocs/textSHAb3015068a99313cb,25 exact+one size-only block;
// fuzzy96.652540. At+123 retail0f bf c8(movsx ecx,ax), candidate89 45 e4.
// Both matrix strict false (retained private-string identity warning), but
// independent retained889-byte/DIR32/ordered-target proof passes completely.
// No fake instruction or variable added to compensate for cast removal.
