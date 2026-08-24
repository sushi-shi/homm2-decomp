/*
Cross-version local-name batch 3
================================

Function                         PoL RVA     Retained / rejected Buka names
ExpCampaign::HandleVictory       0x0bcc6e    keep mapIndex / reject map
dpSendMessage                    0x01f3a4    keep result / reject status
philAI::DetermineHeroToMove      0x03b748    keep val,idx / reject value,best
philAI::GetTurnAttentionValue    0x03e6b0    keep factor / reject scale
resourceManager::Expunge         0x0c87a0    keep currentResource / port next
army::ValidPath                  0x0bdd3a    keep reserved / reject extra
combatManager::ValidSpellTarget  0x02107b    keep unusedSpellWord5 / reject unused

Complete clean products
-----------------------
Function                    Shapes   PoL/current       Literal Buka
HandleVictory                 3/3     99.888390%        99.848210%
dpSendMessage                 3/3    100.000000%        99.863014%
DetermineHeroToMove          16/16    89.948715%        89.884610%
GetTurnAttentionValue         3/3    100.000000%        99.714290%
Expunge                      16/16   100.000000%        99.800000%
ValidPath                     3/3    100.000000%        99.960785%
ValidSpellTarget              3/3     99.381190%        99.366330%

Manifests/results:
  build/handle-victory-map-name-manifest.json
  build/dp-send-message-status-name-manifest.json
  build/determine-hero-to-move-buka-names-manifest.json
  build/get-turn-attention-scale-name-manifest.json
  build/expunge-buka-names-manifest.json
  build/valid-path-extra-name-manifest.json
  build/valid-spell-target-unused-name-manifest.json
  build/source-variant-batch/<matching stem>/results.csv

Expunge isolates the mixed result: `nextResource -> next` alone is exact at
139 bytes, 7/7 blocks, ordered 1/1 relocs; `currentResource -> cur` alone and
the full Buka pair are both 99.800000%. The retained normal build is exact.

HandleVictory's clean permuter baseline differs from its exact normal TU
state, but the paired Buka arm is lower. DetermineHeroToMove and
ValidSpellTarget remain non-exact for unrelated source-shape/value reasons;
their Buka name arms are lower than the paired current-name arms and do not
resolve those residuals.

Disposition: matching-decomp port of Expunge `next` only. All other Buka
names are compiler/version-specific. No gameplay or shipped-byte change.
*/
