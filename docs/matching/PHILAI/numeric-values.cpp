// C51, parent 7da3d7e8, full structural numeric families, no TU-state probes.
// ValueOfTown: sum=(i32)(expr) versus sum=static_cast<i32>(expr); retain latter.
// TurnCostResource: retain explicit i32 result cast; independently choose
// static_cast<T>(operand) or implicit conversion at these six original sites:
//   (double)gResourceBaseValue[res]
//   (double)(pAI->m_income[res]*5)
//   (double)gpGame->m_players[player].m_resources[res]
//   (float)(sum/AI_PURCHASE_RESOURCE_COUNT)
//   (float)resValue[res]
//   (float)(gResourceBaseValue[res]/(frac[res]/2.0f+0.5))
// All64 combinations plus original; retain implicit choices without changing
// integer division or grouping. Original/complete code in build/c51-resource-axes.json.
// TurnValueOfObelisk: both i32 casts C-style/static (2x2) crossed with return
// (float)value/static_cast<float>(value)/value (3). Retain explicit i32/implicit return.
// NetValueOfArtifact: all2^3 keep/remove choices for float casts on table[a1],
// a2 and a4, final static_cast<i32> unchanged; retain all three removed.
//
// build/c51-{town,resource,obelisk,artifact}-{axes,manifest}.json and
// corresponding -batch/results.json. Counts2/65/12/8, all complete/restored,
// elapsed3.624/61.980/12.080/9.005s. Sizes319/270/307/54 unchanged; all arms in
// each family share text+ordered normalized relocs. First three have generic
// private-numeric relocation warnings, resolved by fixed-site supplement for
// final retail proof; artifact passes strict directly. No verifier relaxation.
// Final full build plus all89-function native/allocated-section/relocation
// comparison pass; no fake local, padding, probe or source arm retained.
