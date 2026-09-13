// VC6 SP5 Overview source family g85-g86-overview-tabs; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g85-g86-overview-tabs/results.json; RVA 0x7c501
// Complete 4 arms; elapsed 3.294752s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1202 / db96af6ac0f87e83 / 9694f49bcd40cc81 / 61:61 / 60,1,0,0 / True
// 1 / {'source_family': 'towns', 'candidate': 'baseline'} / 1229 / 749f1f3b2a7c130f / 2f2bd2dcf64d71f2 / 63:61 / 33,1,5,22 / False
// 2 / {'source_family': 'heroes', 'candidate': 'baseline'} / 1229 / a49c3bff2f869100 / b4690b3f504d8267 / 63:61 / 32,1,5,23 / False
// 3 / {'source_family': 'heroes_towns', 'candidate': 'baseline'} / 1253 / 2ae7814fe02a66ec / 236995b2d665389d / 65:61 / 32,0,7,22 / False

// AXIS source_family
// BEFORE (complete exact span):
// |                         }
// |                     selectHeroes:
// |                         if (giOverviewType != OVERVIEW_HEROES) {
// |                             gpGame->SetupNewOverviewType(OVERVIEW_HEROES, 1);
// |                         }
// |                         break;
// |                     case TOWN_TAB_WIDGET:
// |                         if (quickView != 0) {
// |                             break;
// |                         }
// |                     selectTowns:
// |                         if (giOverviewType != OVERVIEW_TOWNS) {
// |                             gpGame->SetupNewOverviewType(OVERVIEW_TOWNS, 1);
// |                         }
// |                         break;
// |                     default:
// |                         done = gpGame->ProcessIconSelect(message.payload.widget.id, quickView);
// |                         break;
// |                 }
// |                 break;
// |             case WIDGET_COMMAND_DESELECT:
// |                 switch (message.payload.widget.id) {
// |                     case HERO_TITLE_WIDGET:
// |                         goto selectHeroes;
// |                     case TOWN_TITLE_WIDGET:
// |                         goto selectTowns;
// |                     case SCROLL_UP_WIDGET:
// ALTERNATIVE towns (unified source delta from BEFORE):
// | --- before
// | +++ towns
// | @@ -8,7 +8,6 @@
// |                          if (quickView != 0) {
// |                              break;
// |                          }
// | -                    selectTowns:
// |                          if (giOverviewType != OVERVIEW_TOWNS) {
// |                              gpGame->SetupNewOverviewType(OVERVIEW_TOWNS, 1);
// |                          }
// | @@ -23,5 +22,8 @@
// |                      case HERO_TITLE_WIDGET:
// |                          goto selectHeroes;
// |                      case TOWN_TITLE_WIDGET:
// | -                        goto selectTowns;
// | +                        if (giOverviewType != OVERVIEW_TOWNS) {
// | +                            gpGame->SetupNewOverviewType(OVERVIEW_TOWNS, 1);
// | +                        }
// | +                        break;
// |                      case SCROLL_UP_WIDGET:
// ALTERNATIVE heroes (unified source delta from BEFORE):
// | --- before
// | +++ heroes
// | @@ -1,5 +1,4 @@
// |                          }
// | -                    selectHeroes:
// |                          if (giOverviewType != OVERVIEW_HEROES) {
// |                              gpGame->SetupNewOverviewType(OVERVIEW_HEROES, 1);
// |                          }
// | @@ -21,7 +20,10 @@
// |              case WIDGET_COMMAND_DESELECT:
// |                  switch (message.payload.widget.id) {
// |                      case HERO_TITLE_WIDGET:
// | -                        goto selectHeroes;
// | +                        if (giOverviewType != OVERVIEW_HEROES) {
// | +                            gpGame->SetupNewOverviewType(OVERVIEW_HEROES, 1);
// | +                        }
// | +                        break;
// |                      case TOWN_TITLE_WIDGET:
// |                          goto selectTowns;
// |                      case SCROLL_UP_WIDGET:
// ALTERNATIVE heroes_towns (unified source delta from BEFORE):
// | --- before
// | +++ heroes_towns
// | @@ -1,5 +1,4 @@
// |                          }
// | -                    selectHeroes:
// |                          if (giOverviewType != OVERVIEW_HEROES) {
// |                              gpGame->SetupNewOverviewType(OVERVIEW_HEROES, 1);
// |                          }
// | @@ -8,7 +7,6 @@
// |                          if (quickView != 0) {
// |                              break;
// |                          }
// | -                    selectTowns:
// |                          if (giOverviewType != OVERVIEW_TOWNS) {
// |                              gpGame->SetupNewOverviewType(OVERVIEW_TOWNS, 1);
// |                          }
// | @@ -21,7 +19,13 @@
// |              case WIDGET_COMMAND_DESELECT:
// |                  switch (message.payload.widget.id) {
// |                      case HERO_TITLE_WIDGET:
// | -                        goto selectHeroes;
// | +                        if (giOverviewType != OVERVIEW_HEROES) {
// | +                            gpGame->SetupNewOverviewType(OVERVIEW_HEROES, 1);
// | +                        }
// | +                        break;
// |                      case TOWN_TITLE_WIDGET:
// | -                        goto selectTowns;
// | +                        if (giOverviewType != OVERVIEW_TOWNS) {
// | +                            gpGame->SetupNewOverviewType(OVERVIEW_TOWNS, 1);
// | +                        }
// | +                        break;
// |                      case SCROLL_UP_WIDGET:
