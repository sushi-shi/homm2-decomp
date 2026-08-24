/*
Cross-version local-name batch 1
================================

Functions (PoL RVA / Buka RVA):
  playerData::NumOfGivenArtifact  0x70fda / 0x4c6b4
  game::InitializePasswords       0x78a49 / 0x53a95
  game::RandomizeBarrier          0x78aea / 0x53b1a
  hero::GetSSLevel                0x705c2 / 0x64a03
  advManager::GetMoveShowIt       0x0e473 / 0x3403f
  advManager::DemobilizeCurrHero  0x63f95 / 0x0e03c
  StopOnTrigger                   0x6c318 / 0x159ab
  CanBuy                          0x99d21 / 0x69085

Complete clean VC4.2 products
------------------------------
  GetMoveShowIt, 4/4:
    directionX + directionY  100.000000%
    dx + directionY          100.000000%  RETAINED
    directionX + dy           99.920000%
    dx + dy                   99.920000%
    build/slot1-getmoveshowit-manifest.json

  GetSSLevel, 4/4:
    shrineAndArtifactBonus + baseLevel  99.966100%
    bonus + baseLevel                    99.966100%  RETAINED
    either ssLevel arm                   99.762710%
    build/slot1-getsslevel-manifest.json

  DemobilizeCurrHero, 4/4:
    currentHero + currentCell  100.000000%
    currentHero + cell         100.000000%  RETAINED
    hp + currentCell           100.000000%
    hp + cell                   99.834950%
    build/slot1-demobilize-manifest.json

  StopOnTrigger, 2/2:
    trigger  100.000000%  RETAINED
    special   99.853660%
    build/slot1-stopontrigger-manifest-complete.json

  CanBuy, 2/2:
    idx  100.000000%  RETAINED
    r     99.854164%
    build/slot1-canbuy-manifest.json

Structural products
-------------------
  NumOfGivenArtifact, complete 2 x 2 structure/name product:
    inner j       100.000000%
    inner jj      100.000000%
    hoisted j     100.000000%  RETAINED
    hoisted jj     99.809525%
    build/slot1-numgiven-structure-manifest.json
    build/slot1-numgiven-inner-names-manifest.json
    build/slot1-numgiven-hoisted-names-manifest.json

  InitializePasswords, 2/2 compare-order product:
    table[j] == table[i]  99.953490%
    table[i] == table[j]  99.953490%  RETAINED
    build/slot1-password-mirror-manifest.json
    Both arms have 15/15 exact blocks and ordered 4/4 relocations. The live
    instruction/slot residual predates and is independent of this donor.

  RandomizeBarrier, complete 2 structures x 4 name products:
    PoL `color | 0`, idx + pass  100.000000%  RETAINED
    PoL `color | 0`, index + p    99.823530%
    Buka direct store, best arm   91.411766%
    Buka direct store, index + p  91.235290%
    build/slot1-randomize-structure-manifest.json
    build/slot1-randomize-names-manifest.json
    build/slot1-randomize-direct-names-manifest.json

Retained normal build
---------------------
  NumOfGivenArtifact   exact, 151 bytes, 12/12 blocks, 1/1 relocs
  InitializePasswords  99.953490%, 161 bytes, 15/15 blocks, 4/4 relocs
  RandomizeBarrier     exact, 100 bytes, 2/2 blocks, 1/1 relocs
  GetSSLevel           99.966100%, 195 bytes, 14/14 blocks, 3/3 relocs
  GetMoveShowIt        exact, 172 bytes, 8/8 blocks, 9/9 relocs
  DemobilizeCurrHero   exact, 364 bytes, 9/9 blocks, 7/7 relocs
  StopOnTrigger        exact, 133 bytes, 10/10 blocks, 1/1 relocs
  CanBuy               exact, 154 bytes, 8/8 blocks, 3/3 relocs

Clean-source state audit
------------------------
  InitializePasswords: forest/top trial 1 exact, ordered 4/4 relocs
    build/slot1-initialize-passwords-state-summary.json
  GetSSLevel: forest/top trial 4 exact, ordered 3/3 relocs
    build/slot1-getsslevel-state-summary.json
  Both audited exact maxima are retained for the final source hashes; generated
  probe declarations are absent from reconstructed source.

Disposition: matching-decomp port of `dx`, `bonus`, `cell`, the hoisted
NumOfGivenArtifact loop index, and InitializePasswords's Buka table-read
mirror. Retain the other PoL spellings and RandomizeBarrier's `| 0`; literal
Buka combinations are compiler/version-specific. No gameplay change.
*/
