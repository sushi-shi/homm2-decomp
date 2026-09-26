// VC6 SP5 source product g108-events-shrine-tail; RVA 0x3b640.
// Parent: 3a80a103 (unmodified).
// Alternatives are relative to BEFORE; not sequential edits or a cross-family product.
// Historical prefixes o08/a04 correspond to final finding IDs O09/A05.
// Complete 2 arms, 3.172451s; no compile failure or timeout; source restored.
// build/source-variant-batch/g108-events-shrine-tail/results.json and build/g108-events-shrine-tail-axes.json
// Disposition: retain original. Equality here is text only; final strict native census
// separately proves all98 objects/1826 functions/all allocated sections and ordered relocations.
// Four preexisting EH/FS retail model gaps remain explicit; they are not waived.
// No TU-state census was performed in these products.
// trial / choices / bytes / text SHA / normalized reloc SHA / blocks base:target / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 24521 / 8da6864f2c51a3de / f00329f724dc8285 / 557:561 / 556,0,0,1,4 / False
// 1 / {'family': 'recovered', 'candidate': 'baseline'} / 25158 / b554247741b68c9b / 44305185339f6608 / 568:561 / 287,23,112,139,7 / False

// AXIS family
// BEFORE (complete exact span):
// |         case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
// |             sprintf(
// |                 gText,
// |                 "%s'%s'.  ",
// |                 "{\xd1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5 1-\xe3\xee \xca\xf0\xf3\xe3\xe0}\n\n\xc2\xfb \xed\xe0\xe1\xf0\xe5\xeb\xe8 "
// |                 "\xed\xe0 \xec\xe0\xeb\xe5\xed\xfc\xea\xee\xe5 \xf1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5, \xe3\xe4\xe5 "
// |                 "\xf1\xeb\xf3\xe6\xe0\xf2 \xec\xee\xeb\xee\xe4\xfb\xe5 \xef\xee\xf1\xeb\xf3\xf8\xed\xe8\xea\xe8."
// |                 "  \xc2 \xee\xe1\xec\xe5\xed \xed\xe0 \xe7\xe0\xf9\xe8\xf2\xf3 \xee\xed\xe8 \xf1\xee\xe3\xeb\xe0\xf1\xe8\xeb\xe8\xf1\xfc "
// |                 "\xed\xe0\xf3\xf7\xe8\xf2\xfc \xe2\xe0\xf1 \xef\xf0\xee\xf1\xf2\xee\xec\xf3 \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xfe "
// |                     "- ",
// |                 gSpellNames[cell->m_objectMetadata - 1]
// |             );
// |             goto shrineSpell;
// OPTION keep
// OPTION recovered
// | --- before
// | +++ recovered
// | @@ -10,4 +10,41 @@
// |                      "- ",
// |                  gSpellNames[cell->m_objectMetadata - 1]
// |              );
// | -            goto shrineSpell;
// | +            if (eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
// | +                if (IDX(gsSpellInfo[cell->m_objectMetadata - 1].level)
// | +                    <= static_cast<i32>(eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
// | +                           + SHRINE_WISDOM_BONUS) {
// | +                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// | +                    eventHero2->AddSpell(
// | +                        static_cast<SpellType>(cell->m_objectMetadata - 1),
// | +                        eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE)
// | +                    );
// | +                    EventWindow(
// | +                        -1,
// | +                        NORMAL_DIALOG_INFO,
// | +                        gText,
// | +                        NORMAL_DIALOG_SPELL,
// | +                        cell->m_objectMetadata - 1,
// | +                        -1,
// | +                        0,
// | +                        -1
// | +                    );
// | +                } else {
// | +                    strcat(
// | +                        gText,
// | +                        "\xca \xf1\xee\xe6\xe0\xeb\xe5\xed\xe8\xfe, \xf3 \xe2\xe0\xf1 \xed\xe5 \xf5\xe2\xe0\xf2\xe0\xe5\xf2 "
// | +                            "\xec\xf3\xe4\xf0\xee\xf1\xf2\xe8, \xf7\xf2\xee\xe1\xfb \xef\xee\xed\xff\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5,"
// | +                            " \xe8 \xe2\xfb \xed\xe5 \xec\xee\xe6\xe5\xf2\xe5 \xe2\xfb\xf3\xf7\xe8\xf2\xfc \xe5\xe3\xee."
// | +                    );
// | +                    EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
// | +                }
// | +            } else {
// | +                strcat(
// | +                    gText,
// | +                    "\xca \xf1\xee\xe6\xe0\xeb\xe5\xed\xe8\xfe, \xf3 \xe2\xe0\xf1 \xed\xe5\xf2 \xc2\xee\xeb\xf8\xe5\xe1\xed\xee\xe9 "
// | +                        "\xea\xed\xe8\xe3\xe8, \xf7\xf2\xee\xe1\xfb \xe7\xe0\xef\xe8\xf1\xe0\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 "
// | +                        "\xe2 \xed\xe5\xe5."
// | +                );
// | +                EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
// | +            }
// | +            break;
// EXTRA BEFORE:
// |         case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
// |             sprintf(
// |                 gText,
// |                 "%s'%s'.  ",
// |                 "{\xd1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5 2-\xe3\xee \xca\xf0\xf3\xe3\xe0}\n\n\xc2\xfb \xed\xe0\xe1\xf0\xe5\xeb\xe8 "
// |                 "\xed\xe0 \xe1\xee\xe3\xe0\xf2\xee \xf0\xe0\xf1\xef\xe8\xf1\xe0\xed\xed\xee\xe5 \xf1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5,"
// |                 " \xe3\xe4\xe5 \xf1\xeb\xf3\xe6\xe0\xf2 \xef\xee\xeb\xed\xee\xef\xf0\xe0\xe2\xed\xfb\xe5 "
// |                 "\xe1\xf0\xe0\xf2\xfc\xff \xe2\xe5\xf0\xfb.  \xc2 \xee\xe1\xec\xe5\xed \xed\xe0 \xe7\xe0\xf9\xe8\xf2\xf3 "
// |                 "\xee\xed\xe8 \xf1\xee\xe3\xeb\xe0\xf1\xe8\xeb\xe8\xf1\xfc \xed\xe0\xf3\xf7\xe8\xf2\xfc \xe2\xe0\xf1 "
// |                     "\xed\xee\xe2\xee\xec\xf3 \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xfe - ",
// |                 gSpellNames[cell->m_objectMetadata - 1]
// |             );
// |             goto shrineSpell;
// EXTRA REPLACEMENT:
// |         case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
// |             sprintf(
// |                 gText,
// |                 "%s'%s'.  ",
// |                 "{\xd1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5 2-\xe3\xee \xca\xf0\xf3\xe3\xe0}\n\n\xc2\xfb \xed\xe0\xe1\xf0\xe5\xeb\xe8 "
// |                 "\xed\xe0 \xe1\xee\xe3\xe0\xf2\xee \xf0\xe0\xf1\xef\xe8\xf1\xe0\xed\xed\xee\xe5 \xf1\xe2\xff\xf2\xe8\xeb\xe8\xf9\xe5,"
// |                 " \xe3\xe4\xe5 \xf1\xeb\xf3\xe6\xe0\xf2 \xef\xee\xeb\xed\xee\xef\xf0\xe0\xe2\xed\xfb\xe5 "
// |                 "\xe1\xf0\xe0\xf2\xfc\xff \xe2\xe5\xf0\xfb.  \xc2 \xee\xe1\xec\xe5\xed \xed\xe0 \xe7\xe0\xf9\xe8\xf2\xf3 "
// |                 "\xee\xed\xe8 \xf1\xee\xe3\xeb\xe0\xf1\xe8\xeb\xe8\xf1\xfc \xed\xe0\xf3\xf7\xe8\xf2\xfc \xe2\xe0\xf1 "
// |                     "\xed\xee\xe2\xee\xec\xf3 \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xfe - ",
// |                 gSpellNames[cell->m_objectMetadata - 1]
// |             );
// |             if (eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
// |                 if (IDX(gsSpellInfo[cell->m_objectMetadata - 1].level)
// |                     <= static_cast<i32>(eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
// |                            + SHRINE_WISDOM_BONUS) {
// |                     EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
// |                     eventHero2->AddSpell(
// |                         static_cast<SpellType>(cell->m_objectMetadata - 1),
// |                         eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE)
// |                     );
// |                     EventWindow(
// |                         -1,
// |                         NORMAL_DIALOG_INFO,
// |                         gText,
// |                         NORMAL_DIALOG_SPELL,
// |                         cell->m_objectMetadata - 1,
// |                         -1,
// |                         0,
// |                         -1
// |                     );
// |                 } else {
// |                     strcat(
// |                         gText,
// |                         "\xca \xf1\xee\xe6\xe0\xeb\xe5\xed\xe8\xfe, \xf3 \xe2\xe0\xf1 \xed\xe5 \xf5\xe2\xe0\xf2\xe0\xe5\xf2 "
// |                             "\xec\xf3\xe4\xf0\xee\xf1\xf2\xe8, \xf7\xf2\xee\xe1\xfb \xef\xee\xed\xff\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5,"
// |                             " \xe8 \xe2\xfb \xed\xe5 \xec\xee\xe6\xe5\xf2\xe5 \xe2\xfb\xf3\xf7\xe8\xf2\xfc \xe5\xe3\xee."
// |                     );
// |                     EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
// |                 }
// |             } else {
// |                 strcat(
// |                     gText,
// |                     "\xca \xf1\xee\xe6\xe0\xeb\xe5\xed\xe8\xfe, \xf3 \xe2\xe0\xf1 \xed\xe5\xf2 \xc2\xee\xeb\xf8\xe5\xe1\xed\xee\xe9 "
// |                         "\xea\xed\xe8\xe3\xe8, \xf7\xf2\xee\xe1\xfb \xe7\xe0\xef\xe8\xf1\xe0\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 "
// |                         "\xe2 \xed\xe5\xe5."
// |                 );
// |                 EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
// |             }
// |             break;
// EXTRA BEFORE:
// |         shrineSpell:
// EXTRA REPLACEMENT:
