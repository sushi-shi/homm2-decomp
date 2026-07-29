/*
CatAttack damaged-wall increment forest matrix

Target: SOURCE/CMBTMGR combatManager::CatAttack, RVA 0x0009311f.

The current 4110-byte function has the retail 173-block graph, identical
relocation-masked instructions, and all 150 ordered relocations.  A fresh
unchanged-source 50-trial declaration-forest census observed eight target
states and no exact closure:

    build/cat-attack-state-summary.json
    build/tu-state-noise/cat-attack/

The best state remained the clean 99.997720% island.  Its eight differing
bytes are all at offset +0xe94..+0xe9b: the damaged-wall increment loads this
before wallIndex17, while retail loads wallIndex17 before this.

The following source spellings were crossed with the clean state and 50
declaration forests:

    m_wallStates[index]++;
    ++m_wallStates[index];
    m_wallStates[index] += 1;
    m_wallStates[index] = m_wallStates[index] + 1;

Artifacts:

    build/cat-attack-increment-axes.json
    build/cat-attack-increment-manifest.json
    build/source-variant-batch/cat-attack-increment/results.json

All 204 candidates completed without truncation.  Prefix and postfix
increments were byte-identical in every corresponding state.  The compound
and explicit addition arms did not improve the retained island.  No exact
closure appeared.

Disposition: measured residual; source remains unchanged.  The natural
prefix/postfix distinction is compiler-erased in this parent orbit, and the
more verbose addition forms provide no retail evidence.  Do not retain a
generated index local or pointer solely to steer this one load order without
independent source evidence.
*/
