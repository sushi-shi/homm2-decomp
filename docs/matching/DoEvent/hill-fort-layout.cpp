/*
DoEvent, RVA 0x000a8530: Hill Fort branch-layout experiment (2026-07-27).

The clean candidate and retail both use an 0x350-byte frame and have 972
relocations.  The initial block census was 595 candidate blocks versus 615
retail blocks.  The first branch-kind divergence was the Hill Fort test:

  candidate B48: cmp / jne, with the empty dialog as fallthrough
  retail    B48: cmp / je,  with the upgrade body as fallthrough

The structural arm below expresses the retail branch layout directly.  A
complete clean plus 50-trial declaration-forest matrix is recorded in:

  build/doevent-hill-fort-layout-manifest-20260727.json
  build/match-variants/doevent-hill-fort-layout-20260727/

It improved the clean score from 96.0874% to 96.207640%.  Trials 15, 33, and
42 reached 96.207810%, above the retained 96.0876% MAX.  This was nevertheless
rejected: the candidate function shrank from the retail 23259 bytes to 22899
bytes, and the ordinary normalization build reported that the 0x9f-byte
doEventHillFortAnUnusualAllianceOf claim had been paired with a physical
extent of only 0x84.  Moving DATA_COMPGEN changed compiler-generated data
topology, so the fuzzy improvement is not admissible MAX evidence.
*/

#if 0
if (firstUpgrade_e != CREATURE_NONE) {
    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
    eventHero2->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
    eventHero2->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
    eventHero2->UpgradeCreatures(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
    if (thirdUpgrade == CREATURE_NONE) {
        if (secondUpgrade1 == CREATURE_NONE) {
            sprintf(
                gText,
                DATA_COMPGEN(0x00517b70, doEventHillFortAllOfTheS, "{Hill Fort}\n\nAll of the %s you have in your army have been trained "
                "by the battle masters of the fort.  Your army now contains %s."),
                gArmyNamesPlural[IDX(firstUpgrade_e)],
                gArmyNamesPlural[IDX(firstUpgrade_e) + 1]
            );
        } else {
            sprintf(
                gText,
                DATA_COMPGEN(0x00517ae0, doEventHillFortAllOfTheS2, "{Hill Fort}\n\nAll of the %s and %s you have in your army have been "
                "trained by the battle masters of the fort.  Your army now contains %s "
                "and %s."),
                gArmyNamesPlural[IDX(firstUpgrade_e)],
                gArmyNamesPlural[IDX(secondUpgrade1)],
                gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                gArmyNamesPlural[IDX(secondUpgrade1) + 1]
            );
        }
    } else {
        sprintf(
            gText,
            DATA_COMPGEN(0x00517a44, doEventHillFortAllOfTheS3, "{Hill Fort}\n\n All of the %s, %s and %s you have in your army have been "
            "trained by the battle masters of the fort.  Your army now contains %s, "
            "%s, and %s."),
            gArmyNamesPlural[IDX(firstUpgrade_e)],
            gArmyNamesPlural[IDX(secondUpgrade1)],
            gArmyNamesPlural[IDX(thirdUpgrade)],
            gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
            gArmyNamesPlural[IDX(secondUpgrade1) + 1],
            gArmyNamesPlural[IDX(thirdUpgrade) + 1]
        );
    }
    EventWindow(
        -1,
        NORMAL_DIALOG_INFO,
        gText,
        NORMAL_DIALOG_MONSTER,
        IDX(firstUpgrade_e) + 1,
        secondUpgrade1 == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
        IDX(secondUpgrade1) + 1,
        -1
    );
} else {
    EventWindow(
        -1,
        NORMAL_DIALOG_INFO,
        DATA_COMPGEN(0x00517bf4, doEventHillFortAnUnusualAllianceOf, "{Hill Fort}\n\nAn unusual alliance of Orcs, Ogres, and Dwarves offer to train "
        "(upgrade) any such troops brought to them.  Unfortunately, you have none with "
        "you."),
        -1,
        0,
        -1,
        0,
        -1
    );
}
#endif

/*
A second arm tried to keep the empty-dialog DATA_COMPGEN lexically before the
upgrade body while using the structural arm above:

  (void)DATA_COMPGEN(0x00517bf4, doEventHillFortAnUnusualAllianceOf,
      "{Hill Fort}...");

The eventual empty EventWindow used the same plain string literal.  That made
the Hill Fort claim pass, but emitted an extra compiler string and shifted the
next claim: normalization then paired the 0x13a-byte
doEventFreemanSFoundryABlacksmithWorking claim with a 0xa0-byte extent.  This
metadata-order-preserving spelling was also rejected.  The reconstructed
source was restored, and neither result was recorded as MAX.
*/
