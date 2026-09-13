// VC6 SP5 source family g103-setup-modem; parent292b9b73.
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/g103-setup-modem/results.json; RVA 0x93115
// Complete 2 arms; elapsed 2.126474s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// Full98 native objects/allocated sections/ordered relocations preserved after restoration.
// Nine private EH/FS site-model gaps remain explicit, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 551 / 18a80d15b4882966 / ef28afab45bac067 / 37:37 / 37,0,0,0,0 / True
// 1 / {'family': 'grouped_choices', 'candidate': 'baseline'} / 543 / 03cbfeb967ec6300 / 6de11fbe567b7a3c / 34:37 / 9,1,9,15,3 / False

// AXIS family
// BEFORE (complete exact span):
// |         case CHOICE_FOUR:
// |             gbDirectConnect = true;
// |             goto setupModem;
// |         case CHOICE_THREE:
// |             gbDirectConnect = false;
// |         setupModem:
// ALTERNATIVE grouped_choices (unified delta from BEFORE):
// | --- before
// | +++ grouped_choices
// | @@ -1,6 +1,3 @@
// |          case CHOICE_FOUR:
// | -            gbDirectConnect = true;
// | -            goto setupModem;
// |          case CHOICE_THREE:
// | -            gbDirectConnect = false;
// | -        setupModem:
// | +            gbDirectConnect = gpWindowManager->m_dialogResult == CHOICE_FOUR;
