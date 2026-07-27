/*
button::Main, RVA 0x000dd6d0: unchanged-source state census (2026-07-27).

The starting object was already exact in gross structure: retail size 1429,
36/36 ordered relocations, and 71/71 basic blocks with no size, target, flow,
or missing-block discrepancy.  Clean source scored 97.032120%.

The unchanged helper and Main body were compiled against clean plus all 50
declaration-forest states:

  build/button-main-baseline-states.json
  build/match-variants/button-main-baseline-states-20260727/

Trials 30 and 41 reproduced the historical 98.907920% maximum.  Trial 30 was
replayed with the official recorder and retained as paired evidence:

  build/tu-state-noise/button-main-trial30-20260727/

It has exact size, all relocations, and all 71 blocks.  Its 30 differing bytes
belong to five instances of one scheduler pattern in the inlined deselection
path: retail preloads the virtual Draw target, stores the cleared flags, sets
the this register, and calls; the candidate stores first and then loads the
virtual target.  No generated probe source was retained.
*/
