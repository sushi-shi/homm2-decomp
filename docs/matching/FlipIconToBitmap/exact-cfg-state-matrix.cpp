/*
FlipIconToBitmap (RVA 0x000d1ba0, retail size 1265)
Exact-CFG internal spelling and TU-state matrix, 2026-07-27.

Artifacts:
  build/match-variants/iconf2b-exact-cfg-internal-axes-20260727.json
  build/match-variants/iconf2b-exact-cfg-state-matrix-20260727.json
  build/match-variants/iconf2b-exact-cfg-state-islands-20260727/results.json

The two-stage clip-bottom structure from clip-bottom-cfg-structure.cpp was
crossed with nine unclipped/clipped dim-setup ownership spellings and the clean
state plus 50 declaration-forest probes. All 459 variants compiled in 365.38
seconds. The run was complete, not wall-time-truncated, and match_variants
restored its input source.

The dim setup arms were byte-neutral within each significant TU state:
  - direct versus split dim-count assignment,
  - staged versus initialized unclipped destination, and
  - count snapshot before versus after clipped publications.

The best state was forest trial 6:
  85.864720%, size 1246, 79/81 relocations, 76/76 blocks,
  66 exact plus ten size-only blocks, and exact flow.

The clean exact-CFG branch remained 85.538460%, size 1250, with 67 exact plus
nine size-only blocks. Trials 7 and 46 reproduced its 67/9 structural
partition at 85.618034% and 85.644560%.

The state-size census covered 1246 through 1264 bytes. A one-byte-short 1264
island existed, but it scored 79.824936%, retained only 65 exact blocks, and
still had 79/81 relocations. It is a proximity clue, not evidence for source.

Disposition: keep the exact-CFG structure as a documented lower-island branch
seed. No internal spelling, generated probe declaration, generated
configuration, reconstructed source change, or MAX update is retained.
Historical/live MAX remains 86.8594%.
*/

#if 0
// Unclipped destination/count ownership arms.
u8* dp;
gFlipCnt = 0;
dp = (gFlipRow - count) + 1 + X;
i32 dimCount;
dimCount = count;
gFlipDimDst = dp;

// Clipped publication/count ownership arm.
gFlipDimLen = count;
gFlipDimDst = dp;
i32 clippedDimCount = count;
gFlipCnt = 0;
#endif
