// VC6 SP5 source product g107-events-recruit-tails; RVA 0x3b640.
// Parent: 3a80a103 (unmodified).
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.230433s; no compile failure or timeout; source restored.
// build/source-variant-batch/g107-events-recruit-tails/results.json and build/g107-events-recruit-tails-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / f00329f724dc8285 / 557:561 / 556,0,0,1,4 / False
// 1 / {'family': 'recovered', 'candidate': 'baseline'} / 24586 / 6fab73b3a9057f5c / 1c5842f78772c7aa / 557:561 / 553,0,0,4,4 / False

// AXIS family
// BEFORE (complete exact span):
// | goto recruitTroll;
// OPTION keep
// OPTION recovered
// | --- before
// | +++ recovered
// | @@ -1 +1 @@
// | -goto recruitTroll;
// | +RecruitEvent(eventHero2, CREATURE_TROLL, cell);
// EXTRA BEFORE:
// |                 recruitTroll:
// EXTRA REPLACEMENT:
// EXTRA BEFORE:
// | goto recruitLich;
// EXTRA REPLACEMENT:
// | RecruitEvent(eventHero2, CREATURE_POWER_LICH, cell);
// EXTRA BEFORE:
// |                 recruitLich:
// EXTRA REPLACEMENT:
// EXTRA BEFORE:
// | goto recruitDragon;
// EXTRA REPLACEMENT:
// | RecruitEvent(eventHero2, CREATURE_RED_DRAGON, cell);
// EXTRA BEFORE:
// |                 recruitDragon:
// EXTRA REPLACEMENT:
