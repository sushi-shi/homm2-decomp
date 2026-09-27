// A01, SOURCE/TOWNMGR RVA0xa5e5c. Same surrounding body in both arms:
// original: GetNumArmies()==1 && &m_swapStrip[0]==m_heroStrip
// retained: GetNumArmies()==1 && m_swapStrip==m_heroStrip
// build/a01-army-{axes,manifest}.json; build/a01-army-batch/results.json.
// Complete2/2 in3.320s, no AST/state probes/truncation, restored.
// Both884bytes/24relocs, textSHA27ddc5576236b062, relocSHA73a00a0968d3a0df;
// both strict exact,27 diagnostic exact blocks/213 non-relocation instructions.
// Independent ordinary bytes/DIR32/ordered targets pass. All36 emitted
// TOWNMGR functions/allocated sections/relocations identical before/after.
