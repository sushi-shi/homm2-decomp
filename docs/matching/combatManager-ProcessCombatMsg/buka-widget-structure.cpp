/*
combatManager::ProcessCombatMsg cross-version widget structure
==============================================================

PoL 2.0 RVA 0x2bb26; Buka 2.1 RVA 0x2c958.

Complete structural product: 32/32
----------------------------------
  build/process-combat-msg-buka-manifest.json
  build/source-variant-batch/process-combat-msg-buka/results.json

Axes:
  - right-click help switch: PoL help cases first / Buka MAIN_BUTTON first
  - DESELECT switch: PoL ATTACK,WAIT,DISABLE,OPTIONS /
                     Buka DISABLE,WAIT,ATTACK,OPTIONS
  - right-button ownership: PoL if/else / Buka guarded arm with break
  - selectedHex/member relational mirror
  - currentCommand/previousCommand relational mirror

Measured clean VC4.2 states:
  PoL structure                                      97.887400%
  + Buka right-button guard                          97.887955%
  + Buka DESELECT order                              97.918770%
  + Buka help order                                  99.041460%
  + Buka help + DESELECT                             99.072830%
  + all three Buka structural axes                   99.073390%  RETAINED

Both relational mirrors are byte-invariant in every structural state. PoL
adopts the Buka local-first spellings.

Complete outer-name product: 8/8
--------------------------------
  build/process-combat-msg-buka-names-manifest.json
  build/source-variant-batch/process-combat-msg-buka-names/results.json

  pendingMessage_2 only                              99.073390%  RETAINED
  selectedHex_0 arms                         99.059944..99.061066%
  unusedResult arms                          99.059390..99.060500%

Final normal build
------------------
  99.11% live fuzzy, 2281 bytes, 109/109 relocation count
  nine owner-relative switch-table addends remain mismatched
  the wider non-donor residual remains live; this dossier does not claim
  function closure

Disposition: matching-decomp port of all three Buka structural axes, both
relational mirrors, and byte-invariant `pendingMessage_2`. Retain PoL
`unusedResult_14` and `selectedHex_36`. No gameplay change.
*/
