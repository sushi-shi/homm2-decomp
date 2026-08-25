#ifndef HOMM2_IRONFIST_HEROES_H
#define HOMM2_IRONFIST_HEROES_H

#include <string>
#include <vector>

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

/*
 * Ironfist's hero globals: the editor-placed hero overlay tables (overlay
 * object index to faction and color, with the Cyborg rows 42-47) and the
 * hero name list covering the base 54, the campaign heroes, and the Cyborg
 * hero. TOTAL_AVAILABLE_HEROES spans them all.
 */

enum IronfistHeroConstant {
    IRONFIST_TOTAL_AVAILABLE_HEROES = 73,
    IRONFIST_FIRST_CYBORG_HERO_ID   = 72,
    // Cybernetics rides one row past the retail secondary-skill table:
    // row 15 in the dialog image groups, frame 16 in secskill.icn,
    // frame 14 in miniss.icn.
    CYBERNETICS_SKILL_ROW           = 15,
    CYBERNETICS_SKILL_FRAME         = 16,
    CYBERNETICS_MINI_SKILL_FRAME    = 14
};

FactionType GetHeroOverlayFaction(i32 overlayObjectIdx);
i32 GetHeroOverlayColor(i32 overlayObjectIdx);
extern std::vector<std::string> heroNames;

// Fixed spells a Cyborg hero picks up at set levels.
SpellType GetCyborgLevelSpell(i32 level);

#endif
