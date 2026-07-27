/*
BaseFree, retail RVA 0x000c3f80, size 902.

This file records rejected inline-helper structures. It is evidence, not
compiled game source.

The three newline append regions are each one instruction longer than retail,
and retail discovers the string end before loading the newline word. Four
shared boundaries were therefore crossed with the clean state and 50 top
forest states:

  - the open strlen expression;
  - an inline AppendLogNewline(char*) helper;
  - an inline LogTextEnd(char*) helper;
  - an inline manual character-scan helper.

The complete 4 x 51 matrix is retained at:

  build/match-variants/basefree-inline-newline-20260727/

The open expression, append helper, and end-pointer helper were byte-identical
in every corresponding useful state: best 91.333336%, size 902, 55/55 raw
relocations, with 23 exact and five size-only blocks. The manual scan formed a
contradictory 85.771090%, size-911 orbit and destroyed block alignment. Thus
the compiler already canonicalizes the credible inline boundaries, and none
explains the three retail newline bodies.

The remaining untested table-owner mechanism wrapped gpMemEntry[index] in
either an inline reference accessor or an inline pointer accessor at every
BaseFree access. All 3 x 51 candidates completed:

  build/match-variants/basefree-inline-entry-accessor-20260727/

The two helper spellings were byte-identical and peaked at 89.947790%, size
907, 55/55 raw relocations. They reduced aligned body agreement instead of
creating retail's table-base lifetime. The direct indexed source remains
91.333336% live and retains the 94.0482% historical MAX. No helper or generated
state was retained.
*/
