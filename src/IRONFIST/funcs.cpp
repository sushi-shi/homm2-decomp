#include <IRONFIST/register.h>

#include <cstring>
#include <string>

#include <IRONFIST/callback.h>
#include <IRONFIST/deepbinding.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/lua.h>

#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/advManager.h>
#include <SOURCE/army.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/hexcell.h>
#include <SOURCE/KB.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist::script {

enum SoundEffectWait { SND_DO_WAIT, SND_DONT_WAIT };

static bool PlaySoundEffect(std::string snd, SoundEffectWait wait, SAMPLE2* samp) {
    SAMPLE2 res = NULL;
    if (!snd.empty()) {
        char* src;
        snd += ".82m";
        src = strdup(snd.c_str());
        res = LoadPlaySample(src);
        if (samp != NULL)
            *samp = res;
        if (wait == SND_DO_WAIT)
            WaitEndSample(&res, -1);
        free(src);
        return true;
    }
    return false;
}

static bool CheckLocationItem(mapCell* loc) {
    if (!(loc->m_triggerType & H2EnumIndex(MAP_TRIGGER_ACTION_FLAG))) {
        return false;
    }
    switch (loc->m_triggerType ^ H2EnumIndex(MAP_TRIGGER_ACTION_FLAG)) {
        case MAP_OBJECT_ANCIENT_LAMP:
        case MAP_OBJECT_ARTIFACT:
        case MAP_OBJECT_RESOURCE:
        case MAP_OBJECT_CAMPFIRE:
        case MAP_OBJECT_TREASURE_CHEST:
        case MAP_OBJECT_SHIPWRECK_SURVIVOR:
        case MAP_OBJECT_FLOTSAM:
        case MAP_OBJECT_SEA_CHEST:
            return true;
        default:
            return false;
    }
}

static bool CheckBoolean(lua_State* L, i32 pos) {
    if (lua_isboolean(L, pos)) {
        return lua_toboolean(L, pos);
    }
    const char* msg = lua_pushfstring(
        L, "%s expected, got %s", lua_typename(L, LUA_TBOOLEAN), luaL_typename(L, pos)
    );
    luaL_argerror(L, pos, msg);
    return false;
}

static i32 StackIndexOfArg(i32 argNumber, i32 numArgs) {
    return (numArgs - (argNumber - 1));
}

static i32 CheckIndex(lua_State* L, i32 argument, i32 count, const char* description) {
    i32 index = static_cast<i32>(luaL_checkinteger(L, argument));
    if (index < 0 || index >= count) {
        luaL_argerror(L, argument, description);
    }
    return index;
}

static i32 CheckStoredIndex(lua_State* L, i32 index, i32 count, const char* description) {
    if (index < 0 || index >= count) {
        luaL_error(L, "%s is out of range", description);
    }
    return index;
}

static void CopyLuaName(lua_State* L, i32 argument, char* destination, size_t capacity) {
    size_t length;
    const char* source = luaL_checklstring(L, argument, &length);
    const size_t copyLength = length < capacity - 1 ? length : capacity - 1;
    memcpy(destination, source, copyLength);
    destination[copyLength] = '\0';
}

/************************************************ Dialogs *****************************/

static i32 l_msgBox(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    H2MessageBox(const_cast<char*>(msg));
    return 0;
}

static i32 l_AdvancedMessageBox(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    i32 dialogType = luaL_checkinteger(L, 2);
    i32 horizontal = luaL_checkinteger(L, 3);
    i32 vertical = luaL_checknumber(L, 4);
    i32 img1type = luaL_checknumber(L, 5);
    i32 img1arg = luaL_checknumber(L, 6);
    i32 img2type = luaL_checknumber(L, 7);
    i32 img2arg = luaL_checknumber(L, 8);
    i32 writeOr = luaL_checknumber(L, 9);
    i32 a10 = luaL_checknumber(L, 10);

    std::string msgCopy = msg;
    NormalDialog(
        &msgCopy[0], dialogType, horizontal, vertical, img1type, img1arg, img2type, img2arg,
        writeOr, a10
    );

    if (dialogType == DIALOG_LEARN_CHOICE) { // learn dialog
        lua_pushboolean(L, gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_SEVEN);
    } else if (dialogType == DIALOG_CANCEL_ALT) {
        lua_pushboolean(L, gbFunctionComplete);
    } else if (dialogType == DIALOG_YES_NO) {
        lua_pushboolean(L, gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE);
    } else {
        lua_pushboolean(L, true);
    }

    return 1;
}

static i32 l_questionBox(lua_State* L) {
    char* qst = const_cast<char*>(luaL_checkstring(L, 1));
    lua_pushboolean(L, H2QuestionBox(qst));
    return 1;
}

static i32 l_inputBox(lua_State* L) {
    char* qst = const_cast<char*>(luaL_checkstring(L, 1));
    i32 len = static_cast<i32>(luaL_checknumber(L, 2));
    char* input = H2InputBox(qst, len);
    lua_pushstring(L, input);
    H2_FREE(input); // pushstring copies it
    return 1;
}

static i32 l_recruitBox(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 creature = static_cast<i32>(luaL_checknumber(L, 2));
    i16 quantity = static_cast<i16>(luaL_checknumber(L, 3));
    i16 startQ = quantity;
    gpAdvManager->ExpansionRecruitEvent(hro, static_cast<CreatureType>(creature), &quantity);

    lua_pushinteger(L, startQ - quantity);
    return 1;
}

static void register_dialog_funcs(lua_State* L) {
    lua_register(L, "MessageBox", l_msgBox);
    lua_register(L, "AdvancedMessageBox", l_AdvancedMessageBox);
    lua_register(L, "QuestionBox", l_questionBox);
    lua_register(L, "InputBox", l_inputBox);
    lua_register(L, "RecruitBox", l_recruitBox);
}

/************************************************ Date ********************************/

static i32 l_getDay(lua_State* L) {
    lua_pushinteger(L, gpGame->m_day);
    return 1;
}

static i32 l_getWeek(lua_State* L) {
    lua_pushinteger(L, gpGame->m_week);
    return 1;
}

static i32 l_getMonth(lua_State* L) {
    lua_pushinteger(L, gpGame->m_month);
    return 1;
}

static void register_date_funcs(lua_State* L) {
    lua_register(L, "GetDay", l_getDay);
    lua_register(L, "GetWeek", l_getWeek);
    lua_register(L, "GetMonth", l_getMonth);
}

/************************************************ Player ******************************/

static i32 l_getNumPlayers(lua_State* L) {
    lua_pushinteger(L, gpGame->m_playerCount);
    return 1;
}

static i32 l_getPlayer(lua_State* L) {
    i32 n = CheckIndex(L, 1, gpGame->m_playerCount, "player index out of range");
    PushBinding(L, Binding<playerData*>(&gpGame->m_players[n]));
    return 1;
}

static i32 l_getCurrentPlayer(lua_State* L) {
    PushBinding(L, Binding<playerData*>(gpCurPlayer));
    return 1;
}

static i32 l_getPlayerColor(lua_State* L) {
    playerData* p = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, p->m_color);
    return 1;
}

static i32 l_getNumHeroes(lua_State* L) {
    playerData* p = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, p->m_heroCount);
    return 1;
}

static i32 l_getHero(lua_State* L) {
    playerData* p = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 n = CheckIndex(L, 2, p->m_heroCount, "owned hero index out of range");
    i32 heroId = CheckStoredIndex(
        L, p->m_heroIds[n], H2EnumIndex(GAME_HERO_COUNT), "owned hero id"
    );
    PushBinding(L, Binding<hero*>(&gpGame->m_heroRecs[heroId]));
    return 1;
}

static i32 l_getHeroForHire(lua_State* L) {
    playerData* p = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 n = CheckIndex(
        L, 2, PLAYER_AVAILABLE_HERO_COUNT, "available hero index out of range"
    );
    i32 heroId = CheckStoredIndex(
        L, p->m_availableHeroIds[n], H2EnumIndex(GAME_HERO_COUNT), "available hero id"
    );
    PushBinding(L, Binding<hero*>(&gpGame->m_heroRecs[heroId]));
    return 1;
}

static i32 l_giveResource(lua_State* L) {
    playerData* player = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 res = static_cast<i32>(luaL_checknumber(L, 2));
    i32 val = static_cast<i32>(luaL_checknumber(L, 3));
    player->m_resources[res] += val;
    return 0;
}

static i32 l_setResource(lua_State* L) {
    playerData* player = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 res = static_cast<i32>(luaL_checknumber(L, 2));
    i32 val = static_cast<i32>(luaL_checknumber(L, 3));
    player->m_resources[res] = val;
    return 0;
}

static i32 l_getResource(lua_State* L) {
    playerData* player = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 res = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, player->m_resources[res]);
    return 1;
}

static i32 l_shareVision(lua_State* L) {
    i32 sourcePlayer = static_cast<i32>(luaL_checknumber(L, 1));
    i32 destPlayer = static_cast<i32>(luaL_checknumber(L, 2));
    gpGame->ShareVision(sourcePlayer, destPlayer);
    return 0;
}

static i32 l_cancelShareVision(lua_State* L) {
    i32 sourcePlayer = static_cast<i32>(luaL_checknumber(L, 1));
    i32 destPlayer = static_cast<i32>(luaL_checknumber(L, 2));
    gpGame->CancelVisionShare(sourcePlayer, destPlayer);
    return 0;
}

static i32 l_setDaysAfterTownLost(lua_State* L) {
    playerData* player = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 days = static_cast<i32>(luaL_checknumber(L, 2));
    player->m_daysLeft = days;
    return 0;
}

static i32 l_getDaysAfterTownLost(lua_State* L) {
    playerData* player = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, player->m_daysLeft);
    return 1;
}

static i32 l_revealMap(lua_State* L) {
    playerData* player = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 4)));
    i32 x = static_cast<i32>(luaL_checknumber(L, 2));
    i32 y = static_cast<i32>(luaL_checknumber(L, 3));
    i32 radius = static_cast<i32>(luaL_checknumber(L, 4));

    for (i32 i = 0; i < gpGame->m_playerCount; i++) {
        if (&gpGame->m_players[i] == player) {
            gpGame->SetVisibility(x, y, i, radius);
            break;
        }
    }
    return 1;
}

static i32 l_SetBarrierTentVisited(lua_State* L) {
    playerData* plyd = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 tentcolor = luaL_checknumber(L, 2);
    plyd->m_barrierTents |= (1 << tentcolor);

    return 0;
}

static void register_player_funcs(lua_State* L) {
    lua_register(L, "GetNumPlayers", l_getNumPlayers);
    lua_register(L, "GetPlayer", l_getPlayer);
    lua_register(L, "GetCurrentPlayer", l_getCurrentPlayer);
    lua_register(L, "GetPlayerColor", l_getPlayerColor);
    lua_register(L, "GetNumHeroes", l_getNumHeroes);
    lua_register(L, "GetHero", l_getHero);
    lua_register(L, "GetHeroForHire", l_getHeroForHire);
    lua_register(L, "GiveResource", l_giveResource);
    lua_register(L, "SetResource", l_setResource);
    lua_register(L, "GetResource", l_getResource);
    lua_register(L, "ShareVision", l_shareVision);
    lua_register(L, "CancelShareVision", l_cancelShareVision);
    lua_register(L, "SetDaysAfterTownLost", l_setDaysAfterTownLost);
    lua_register(L, "GetDaysAfterTownLost", l_getDaysAfterTownLost);
    lua_register(L, "RevealMap", l_revealMap);
    lua_register(L, "SetBarrierTentVisited", l_SetBarrierTentVisited);
}

/************************************************ Heroes ******************************/

static hero* GetCurrentHero() {
    return &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
}

static i32 l_getCurrentHero(lua_State* L) {
    PushBinding(L, Binding<hero*>(GetCurrentHero()));
    return 1;
}

static i32 l_grantSpell(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 sp = static_cast<i32>(luaL_checknumber(L, 2));
    hro->AddSpell(static_cast<SpellType>(sp), hro->Stats(HERO_PRIMARY_KNOWLEDGE));
    return 0;
}

static i32 l_forgetSpell(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 spell = static_cast<i32>(luaL_checknumber(L, 2));
    if (spell >= 0 && spell < KB_SPELL_TABLE_CAPACITY)
        hro->m_spells[spell] = 0;
    return 0;
}

static i32 l_hasTroop(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 creature = static_cast<i32>(luaL_checknumber(L, 2));
    i32 quantity = static_cast<i32>(luaL_checknumber(L, 3));
    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (hro->m_army.m_creatureTypes[i].value() == creature
            && hro->m_army.m_quantities[i] >= quantity) {
            lua_pushboolean(L, true);
            return 1;
        }
    }
    lua_pushboolean(L, false);
    return 1;
}

static i32 l_getCreatureAmount(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 creature = static_cast<i32>(luaL_checknumber(L, 2));
    i32 quantity = 0;

    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (hro->m_army.m_creatureTypes[i].value() == creature) {
            quantity += hro->m_army.m_quantities[i];
        }
    }

    lua_pushinteger(L, quantity);
    return 1;
}

static i32 l_takeTroop(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 creature = static_cast<i32>(luaL_checknumber(L, 2));
    i32 quantity = static_cast<i32>(luaL_checknumber(L, 3));

    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (hro->m_army.m_creatureTypes[i].value() == creature) {
            if (hro->m_army.m_quantities[i] > quantity) {
                hro->m_army.m_quantities[i] -= quantity;
                break;
            } else {
                quantity -= hro->m_army.m_quantities[i];
                hro->m_army.m_creatureTypes[i] = CREATURE_NONE;
                hro->m_army.m_quantities[i] = 0;
            }
        }
    }
    return 0;
}

static i32 l_teleportHero(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 x = static_cast<i32>(luaL_checknumber(L, 2));
    i32 y = static_cast<i32>(luaL_checknumber(L, 3));

    // Working around a bug where TeleportTo will erroneously call
    // gpGame->SetVisibility(hero, viewX, viewY, heroViewingRadius)
    gpAdvManager->m_mapOriginX = x;
    gpAdvManager->m_mapOriginY = y;

    gpAdvManager->TeleportTo(hro, x, y, 0, 0);
    return 0;
}

static i32 l_getHeroName(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushstring(L, hro->m_name);
    return 1;
}

static i32 l_setHeroName(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    CopyLuaName(L, 2, hro->m_name, sizeof(hro->m_name));
    return 0;
}

static i32 l_getHeroInPool(lua_State* L) {
    i32 n = CheckIndex(L, 1, H2EnumIndex(GAME_HERO_COUNT), "hero index out of range");
    PushBinding(L, Binding<hero*>(&gpGame->m_heroRecs[n]));
    return 1;
}

static i32 l_getHeroOwner(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));

    if (hro->m_owner < 0) {
        lua_pushnil(L);
    } else {
        PushBinding(L, Binding<playerData*>(&gpGame->m_players[hro->m_owner]));
    }

    return 1;
}

static i32 l_grantArtifact(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 art = static_cast<i32>(luaL_checknumber(L, 2));
    GiveArtifact(hro, static_cast<ArtifactType>(art), 1, -1);
    return 0;
}

static i32 l_hasArtifact(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 art = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushboolean(L, hro->HasArtifact(static_cast<ArtifactType>(art)));
    return 1;
}

static i32 l_takeArtifact(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 art = static_cast<i32>(luaL_checknumber(L, 2));
    for (i32 i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        if (hro->m_artifacts[i].value() == art) {
            GiveTakeArtifactStat(hro, hro->m_artifacts[i], 1);
            hro->m_artifacts[i] = ARTIFACT_NONE;
            break;
        }
    }
    return 0;
}

static i32 l_countEmptyArtifactSlots(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    i32 amount = 0;
    for (i32 i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        if (hro->m_artifacts[i] == ARTIFACT_NONE) {
            amount++;
        }
    }
    lua_pushinteger(L, amount);
    return 1;
}

static i32 l_countEmptyCreatureSlots(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    i32 amount = 0;
    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (hro->m_army.m_creatureTypes[i] == CREATURE_NONE) {
            amount++;
        }
    }
    lua_pushinteger(L, amount);
    return 1;
}

static i32 l_setExperiencePoints(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 points = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_experience = points;
    hro->CheckLevel();
    return 0;
}

static i32 l_getExperiencePoints(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_experience);
    return 1;
}

static i32 l_setPrimarySkill(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    i32 amt = static_cast<i32>(luaL_checknumber(L, 3));
    hro->m_primaryStats[skill] = amt;
    return 0;
}

static i32 l_getPrimarySkill(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, hro->m_primaryStats[skill]);
    return 1;
}

static i32 l_setSpellpoints(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 points = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_spellPoints = points;
    return 0;
}

static i32 l_getSpellpoints(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_spellPoints);
    return 1;
}

static i32 l_setSecondarySkill(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    i32 level = static_cast<i32>(luaL_checknumber(L, 3));
    hro->SetSS(static_cast<HeroSecondarySkill>(skill), static_cast<HeroSkillLevel>(level));
    return 0;
}

static i32 l_getSecondarySkill(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, hro->GetSSLevel(static_cast<HeroSecondarySkill>(skill)));
    return 1;
}

static i32 l_grantArmy(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 cr = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    hro->m_army.Add(static_cast<CreatureType>(cr), n, -1);
    return 0;
}

static i32 l_getHeroMobility(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_mobility);
    return 1;
}

static i32 l_setHeroMobility(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 mobility = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_mobility = mobility;
    return 0;
}

static i32 l_getHeroRemainingMobility(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_remainingMobility);
    return 1;
}

static i32 l_setHeroRemainingMobility(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 remainingMobility = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_remainingMobility = remainingMobility;
    return 0;
}

static i32 l_getHeroX(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_x);
    return 1;
}

static i32 l_getHeroY(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_y);
    return 1;
}

static i32 l_getHeroLevel(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_level);
    return 1;
}

static i32 l_getHeroTempMoraleBonuses(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_morale);
    return 1;
}

static i32 l_setHeroTempMoraleBonuses(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 moraleBonus = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_morale = moraleBonus;
    return 0;
}

static i32 l_getHeroTempLuckBonuses(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_luck);
    return 1;
}

static i32 l_setHeroTempLuckBonuses(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 luckBonus = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_luck = luckBonus;
    return 0;
}

static i32 l_grantSpellScroll(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 sp = static_cast<i32>(luaL_checknumber(L, 2));
    GiveArtifact(hro, ARTIFACT_SPELL_SCROLL, 1, sp);
    return 0;
}

static i32 l_getHeroFaction(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, hro->m_cursorType.value());
    return 1;
}

static i32 l_setHeroFaction(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 newFaction = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_cursorType = static_cast<HeroCursorType>(newFaction);
    return 0;
}

static void register_hero_funcs(lua_State* L) {
    lua_register(L, "GetCurrentHero", l_getCurrentHero);
    lua_register(L, "GrantSpell", l_grantSpell);
    lua_register(L, "ForgetSpell", l_forgetSpell);
    lua_register(L, "HasTroop", l_hasTroop);
    lua_register(L, "GetCreatureAmount", l_getCreatureAmount);
    lua_register(L, "TakeTroop", l_takeTroop);
    lua_register(L, "TeleportHero", l_teleportHero);
    lua_register(L, "GetHeroName", l_getHeroName);
    lua_register(L, "SetHeroName", l_setHeroName);
    lua_register(L, "GetHeroInPool", l_getHeroInPool);
    lua_register(L, "GetHeroOwner", l_getHeroOwner);
    lua_register(L, "GrantArtifact", l_grantArtifact);
    lua_register(L, "HasArtifact", l_hasArtifact);
    lua_register(L, "TakeArtifact", l_takeArtifact);
    lua_register(L, "CountEmptyArtifactSlots", l_countEmptyArtifactSlots);
    lua_register(L, "CountEmptyCreatureSlots", l_countEmptyCreatureSlots);
    lua_register(L, "SetExperiencePoints", l_setExperiencePoints);
    lua_register(L, "GetExperiencePoints", l_getExperiencePoints);
    lua_register(L, "SetPrimarySkill", l_setPrimarySkill);
    lua_register(L, "GetPrimarySkill", l_getPrimarySkill);
    lua_register(L, "SetSpellpoints", l_setSpellpoints);
    lua_register(L, "GetSpellpoints", l_getSpellpoints);
    lua_register(L, "SetSecondarySkill", l_setSecondarySkill);
    lua_register(L, "GetSecondarySkill", l_getSecondarySkill);
    lua_register(L, "GrantArmy", l_grantArmy);
    lua_register(L, "GetHeroMobility", l_getHeroMobility);
    lua_register(L, "SetHeroMobility", l_setHeroMobility);
    lua_register(L, "GetHeroRemainingMobility", l_getHeroRemainingMobility);
    lua_register(L, "SetHeroRemainingMobility", l_setHeroRemainingMobility);
    lua_register(L, "GetHeroX", l_getHeroX);
    lua_register(L, "GetHeroY", l_getHeroY);
    lua_register(L, "GetHeroLevel", l_getHeroLevel);
    lua_register(L, "GetHeroTempMoraleBonuses", l_getHeroTempMoraleBonuses);
    lua_register(L, "SetHeroTempMoraleBonuses", l_setHeroTempMoraleBonuses);
    lua_register(L, "GetHeroTempLuckBonuses", l_getHeroTempLuckBonuses);
    lua_register(L, "SetHeroTempLuckBonuses", l_setHeroTempLuckBonuses);
    lua_register(L, "GrantSpellScroll", l_grantSpellScroll);
    lua_register(L, "GetHeroFaction", l_getHeroFaction);
    lua_register(L, "SetHeroFaction", l_setHeroFaction);
}

/************************************** Map *******************************************/

static i32 l_mapSetObject(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    i32 obj = static_cast<i32>(luaL_checknumber(L, 3));

    mapCell* cell = gpAdvManager->GetCell(x, y);
    // Ironfist's Lua placement contract uses overlay slot 3 for map objects.
    cell->m_overlayIndex = 3;
    cell->m_objectIndex = obj;
    cell->m_triggerType = obj & 0x7F;
    return 0;
}

static i32 l_mapPutArmy(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    i32 monIdx = static_cast<i32>(luaL_checknumber(L, 3));
    i32 monQty = static_cast<i32>(luaL_checknumber(L, 4));

    mapCell* loc = gpAdvManager->GetCell(x, y);
    loc->m_objectIndex = monIdx;
    loc->m_objectMetadata = monQty;
    loc->m_objectTileset = TILESET_MONS32;
    loc->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER;
    loc->m_overlayIndex = -1;
    loc->m_objectLayerBit0 = 0;
    loc->m_objectLayerBit1 = 0;
    lua_pushinteger(L, 0);
    return 1;
}

/* Note: This doesn't really work, at least not in the way you want.
 * EraseObj has special casing for a number of objects which are normally
 * deleted in the game; see the upstream Ironfist comment for the details.
 */
static i32 l_mapEraseObj(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    mapCell* cell = gpAdvManager->GetCell(x, y);
    gpAdvManager->EraseObj(cell, x, y);
    gpAdvManager->CompleteDraw(0);
    return 0;
}

static i32 l_mapFizzleObj(lua_State* L) {
    SAMPLE2 res;
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    bool snd = CheckBoolean(L, 3);
    mapCell* cell = gpAdvManager->GetCell(x, y);
    gpAdvManager->CompleteDraw(0);
    gpWindowManager->SaveFizzleSource(
        CURSOR_FIZZLE_X, CURSOR_FIZZLE_Y, CURSOR_FIZZLE_WIDTH, CURSOR_FIZZLE_HEIGHT
    );
    if (snd) {
        if (!PlaySoundEffect(
                (!CheckLocationItem(cell) ? "killfade"
                                          : ("pickup0" + std::to_string(Random(1, 7)))),
                SND_DONT_WAIT, &res
            )) {
            snd = false;
        }
    }
    gpAdvManager->EraseObj(cell, x, y);
    gpAdvManager->CompleteDraw(0);
    PollSound();
    gpWindowManager->FizzleForward(
        CURSOR_FIZZLE_X, CURSOR_FIZZLE_Y, CURSOR_FIZZLE_WIDTH, CURSOR_FIZZLE_HEIGHT, -1, 0, 0
    );
    if (snd) {
        WaitEndSample(&res, -1);
    }
    return 0;
}

static i32 l_mapSetTerrainTile(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    i32 tileno = static_cast<i32>(luaL_checknumber(L, 3));

    i8 flip = 0;
    if (lua_gettop(L) >= 4) {
        flip = static_cast<i8>(luaL_checknumber(L, 4));
    }

    mapCell* cell = gpAdvManager->GetCell(x, y);
    cell->m_terrainImageIndex = tileno;
    cell->m_flags = (cell->m_flags & ~0x3) | flip;
    gpAdvManager->CompleteDraw(0);
    return 0;
}

static void register_map_funcs(lua_State* L) {
    lua_register(L, "MapSetObject", l_mapSetObject);
    lua_register(L, "MapPutArmy", l_mapPutArmy);
    lua_register(L, "MapEraseSquare", l_mapEraseObj);
    lua_register(L, "MapFizzle", l_mapFizzleObj);
    lua_register(L, "MapSetTileTerrain", l_mapSetTerrainTile);
}

/************************************** Town ******************************************/

static i32 l_getCurrentTown(lua_State* L) {
    PushBinding(L, Binding<town*>(gpTownManager->m_town));
    return 1;
}

static i32 l_hasVisitingHero(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushboolean(L, twn->m_occupyingHeroId >= 0);
    return 1;
}

static i32 l_getVisitingHero(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    if (twn->m_occupyingHeroId < 0) {
        lua_pushnil(L);
        return 1;
    }
    i32 heroId = CheckStoredIndex(
        L, twn->m_occupyingHeroId, H2EnumIndex(GAME_HERO_COUNT), "visiting hero id"
    );
    PushBinding(L, Binding<hero*>(&gpGame->m_heroRecs[heroId]));
    return 1;
}

static i32 l_buildInCurrentTown(lua_State* L) {
    i32 obj = static_cast<i32>(luaL_checknumber(L, 1));
    gpTownManager->BuildObj(static_cast<BuildingSlotType>(obj));
    return 0;
}

static i32 l_getTown(lua_State* L) {
    i32 index = CheckIndex(L, 1, H2EnumIndex(GAME_TOWN_COUNT), "town index out of range");
    PushBinding(L, Binding<town*>(&gpGame->m_castleRecs[index]));
    return 1;
}

static i32 l_getTownName(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushstring(L, twn->m_name);
    return 1;
}

static i32 l_setTownName(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    CopyLuaName(L, 2, twn->m_name, sizeof(twn->m_name));
    // Upstream returns the still-on-stack name argument as the single result.
    return 1;
}

static i32 l_getTownByName(lua_State* L) {
    char* name = const_cast<char*>(luaL_checkstring(L, 1));
    for (i32 i = 0; i < H2EnumIndex(GAME_TOWN_COUNT); i++) {
        if (strcmp(gpGame->m_castleRecs[i].m_name, name) == 0) {
            PushBinding(L, Binding<town*>(&gpGame->m_castleRecs[i]));
            return 1;
        }
    }
    lua_pushinteger(L, -1);
    return 1;
}

static i32 l_getPlayerTown(lua_State* L) {
    playerData* player = static_cast<playerData*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 index = CheckIndex(L, 2, player->m_townCount, "owned town index out of range");
    i32 townId = CheckStoredIndex(
        L, player->m_townIds[index], H2EnumIndex(GAME_TOWN_COUNT), "owned town id"
    );
    PushBinding(L, Binding<town*>(&gpGame->m_castleRecs[townId]));
    return 1;
}

static i32 l_buildInTown(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 building = static_cast<i32>(luaL_checknumber(L, 2));
    twn->BuildBuilding(static_cast<BuildingSlotType>(building));
    return 0;
}

static i32 l_getTownFaction(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, twn->m_type.value());
    return 1;
}

static i32 l_setTownFaction(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 faction = static_cast<i32>(luaL_checknumber(L, 2));
    twn->SetFaction(static_cast<FactionType>(faction));
    return 0;
}

static i32 l_getCreatureCost(lua_State* L) {
    i32 creature = static_cast<i32>(luaL_checknumber(L, 1));
    i32 cost[H2EnumIndex(RES_COUNT)];
    GetMonsterCost(static_cast<CreatureType>(creature), cost);
    for (i32 i = 0; i < H2EnumIndex(RES_COUNT); i++) {
        lua_pushinteger(L, cost[i]);
    }
    return H2EnumIndex(RES_COUNT);
}

static i32 l_getTownOwner(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, twn->m_owner);
    return 1;
}

static i32 l_setTownOwner(lua_State* L) {
    i32 townIdx = static_cast<i32>(luaL_checknumber(L, 1));
    i32 playerIdx = static_cast<i32>(luaL_checknumber(L, 2));
    gpGame->ClaimTown(townIdx, playerIdx, 0);
    return 0;
}

static i32 l_getTownX(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, twn->m_x);
    return 1;
}

static i32 l_getTownY(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, twn->m_y);
    return 1;
}

static i32 l_getTownIDFromPos(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, gpGame->GetTownId(x, y));
    return 1;
}

static i32 l_setNumberOfCreatures(lua_State* L) {
    town* cstle = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 dwllng = static_cast<i32>(luaL_checknumber(L, 2));
    i32 numcrtrs = static_cast<i32>(luaL_checknumber(L, 3));
    cstle->m_garrison[dwllng] = numcrtrs;
    return 0;
}

static i32 l_setNumGuildSpells(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 l = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    twn->m_spellCounts[l + TOWN_MAGE_GUILD_FIRST_LEVEL] = n;
    twn->GiveSpells(NULL);
    return 0;
}

static i32 l_setGuildSpell(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 4)));
    i32 l = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    i32 s = static_cast<i32>(luaL_checknumber(L, 4));
    twn->m_spells[l][n] = static_cast<SpellType>(s);
    twn->GiveSpells(NULL);
    return 0;
}

static i32 l_getGuildSpell(lua_State* L) {
    town* twn = static_cast<town*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    i32 l = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    lua_pushinteger(L, twn->m_spells[l][n].value());
    return 1;
}

static i32 l_disallowBuilding(lua_State* L) {
    i32 townIdx = static_cast<i32>(luaL_checknumber(L, 1));
    i32 building = static_cast<i32>(luaL_checknumber(L, 2));
    if (townIdx >= 0 && townIdx < GAME_TOWN_COUNT) {
        gpGame->m_castleRecs[townIdx].DisallowBuilding(building);
    }
    return 0;
}

static void register_town_funcs(lua_State* L) {
    lua_register(L, "GetCurrentTown", l_getCurrentTown);
    lua_register(L, "HasVisitingHero", l_hasVisitingHero);
    lua_register(L, "GetVisitingHero", l_getVisitingHero);
    lua_register(L, "BuildInCurrentTown", l_buildInCurrentTown);
    lua_register(L, "GetTown", l_getTown);
    lua_register(L, "GetTownName", l_getTownName);
    lua_register(L, "SetTownName", l_setTownName);
    lua_register(L, "GetTownByName", l_getTownByName);
    lua_register(L, "GetPlayerTown", l_getPlayerTown);
    lua_register(L, "BuildInTown", l_buildInTown);
    lua_register(L, "GetTownFaction", l_getTownFaction);
    lua_register(L, "SetTownFaction", l_setTownFaction);
    lua_register(L, "GetCreatureCost", l_getCreatureCost);
    lua_register(L, "GetTownOwner", l_getTownOwner);
    lua_register(L, "SetTownOwner", l_setTownOwner);
    lua_register(L, "GetTownX", l_getTownX);
    lua_register(L, "GetTownY", l_getTownY);
    lua_register(L, "GetTownIdFromPos", l_getTownIDFromPos);
    lua_register(L, "SetNumberOfCreatures", l_setNumberOfCreatures);
    lua_register(L, "SetNumGuildSpells", l_setNumGuildSpells);
    lua_register(L, "SetGuildSpell", l_setGuildSpell);
    lua_register(L, "GetGuildSpell", l_getGuildSpell);
    lua_register(L, "DisallowBuilding", l_disallowBuilding);
}

/************************************* Battle *****************************************/

static i32 l_battleSummonCreature(lua_State* L) {
    i32 side = static_cast<i32>(luaL_checknumber(L, 1));
    i32 hex = static_cast<i32>(luaL_checknumber(L, 2));
    i32 creature = static_cast<i32>(luaL_checknumber(L, 3));
    i32 quantity = static_cast<i32>(luaL_checknumber(L, 4));

    gpCombatManager->AddArmy(
        static_cast<CombatSide>(side), static_cast<CreatureType>(creature), quantity, hex,
        static_cast<MonsterFlags>(0), 1
    );
    return 0;
}

static i32 l_isHexEmpty(lua_State* L) {
    i32 hexno = static_cast<i32>(luaL_checknumber(L, 1));
    if (!ValidHex(hexno)) {
        lua_pushboolean(L, 0);
    } else {
        hexcell* cell = &gpCombatManager->m_hexCells[hexno];
        lua_pushboolean(L, cell->m_occupantSide.value() == -1 && !cell->m_blocked);
    }

    return 1;
}

static i32 l_battleHasHero(lua_State* L) {
    i32 side = static_cast<i32>(luaL_checknumber(L, 1));
    lua_pushboolean(L, gpCombatManager->m_heroes[side] != NULL);
    return 1;
}

static i32 l_battleGetHero(lua_State* L) {
    i32 side = static_cast<i32>(luaL_checknumber(L, 1));
    PushBinding(L, Binding<hero*>(gpCombatManager->m_heroes[side]));
    return 1;
}

static i32 l_battleMessage(lua_State* L) {
    char* message = const_cast<char*>(luaL_checkstring(L, 1));
    gpCombatManager->CombatMessage(message, 1, 0, 0);
    return 0;
}

static i32 l_battleGetNumStacks(lua_State* L) {
    i32 side = static_cast<i32>(luaL_checknumber(L, 1));
    lua_pushinteger(L, gpCombatManager->m_armyCount[side]);
    return 1;
}

static i32 l_battleGetStack(lua_State* L) {
    i32 side = static_cast<i32>(luaL_checknumber(L, 1));
    i32 idx = static_cast<i32>(luaL_checknumber(L, 2));
    PushBinding(L, Binding<army*>(&gpCombatManager->m_armies[side][idx]));
    return 1;
}

static i32 l_getStackSide(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_side.value());
    return 1;
}

static i32 l_getStackType(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_monsterType.value());
    return 1;
}

static i32 l_getStackQuantity(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_quantity);
    return 1;
}

static i32 l_setStackQuantity(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 quantity = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_quantity = quantity;
    return 0;
}

static i32 l_getStackInitialQuantity(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_initialQuantity);
    return 1;
}

static i32 l_setStackInitialQuantity(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 initialQuantity = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_initialQuantity = initialQuantity;
    return 0;
}

static i32 l_getStackHex(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_hex);
    return 1;
}

static i32 l_getStackMorale(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_morale);
    return 1;
}

static i32 l_setStackMorale(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 morale = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_morale = morale;
    return 0;
}

static i32 l_getStackLuck(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_luck);
    return 1;
}

static i32 l_setStackLuck(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 luck = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_luck = luck;
    return 0;
}

static i32 l_getStackAttack(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_monster.attack);
    return 1;
}

static i32 l_setStackAttack(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 attack = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.attack = attack;
    return 0;
}

static i32 l_getStackDefense(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_monster.defense);
    return 1;
}

static i32 l_setStackDefense(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 defense = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.defense = defense;
    return 0;
}

static i32 l_getStackSpeed(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_monster.speed);
    return 1;
}

static i32 l_setStackSpeed(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 speed = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.speed = speed;
    return 0;
}

static i32 l_getStackShots(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_monster.shots);
    return 1;
}

static i32 l_setStackShots(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 shots = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.shots = shots;
    return 0;
}

static i32 l_getStackHp(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 1)));
    lua_pushinteger(L, creat->m_monster.hitPoints - creat->m_hitPointsLost);
    return 1;
}

static i32 l_setStackHp(lua_State* L) {
    army* creat = static_cast<army*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 2)));
    i32 hp = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_hitPointsLost = creat->m_monster.hitPoints - hp;
    return 0;
}

static void register_battle_funcs(lua_State* L) {
    lua_register(L, "BattleSummonCreature", l_battleSummonCreature);
    lua_register(L, "IsHexEmpty", l_isHexEmpty);
    lua_register(L, "BattleHasHero", l_battleHasHero);
    lua_register(L, "BattleGetHero", l_battleGetHero);
    lua_register(L, "BattleMessage", l_battleMessage);
    lua_register(L, "BattleNumStacksForSide", l_battleGetNumStacks);
    lua_register(L, "BattleGetStack", l_battleGetStack);
    lua_register(L, "GetStackSide", l_getStackSide);
    lua_register(L, "GetStackType", l_getStackType);
    lua_register(L, "GetStackQuantity", l_getStackQuantity);
    lua_register(L, "SetStackQuantity", l_setStackQuantity);
    lua_register(L, "GetStackInitialQuantity", l_getStackInitialQuantity);
    lua_register(L, "SetStackInitialQuantity", l_setStackInitialQuantity);
    lua_register(L, "GetStackHex", l_getStackHex);
    lua_register(L, "GetStackMorale", l_getStackMorale);
    lua_register(L, "SetStackMorale", l_setStackMorale);
    lua_register(L, "GetStackLuck", l_getStackLuck);
    lua_register(L, "SetStackLuck", l_setStackLuck);
    lua_register(L, "GetStackAttack", l_getStackAttack);
    lua_register(L, "SetStackAttack", l_setStackAttack);
    lua_register(L, "GetStackDefense", l_getStackDefense);
    lua_register(L, "SetStackDefense", l_setStackDefense);
    lua_register(L, "GetStackSpeed", l_getStackSpeed);
    lua_register(L, "SetStackSpeed", l_setStackSpeed);
    lua_register(L, "GetStackShots", l_getStackShots);
    lua_register(L, "SetStackShots", l_setStackShots);
    lua_register(L, "GetStackHp", l_getStackHp);
    lua_register(L, "SetStackHp", l_setStackHp);
}

/**************************************** Campaign ************************************/

static SCampaignChoice* CurrentCampaignChoice() {
    i32 curMapID = H2EnumIndex(xCampaign.m_currentMap);
    return &xCampaignChoices[H2EnumIndex(xCampaign.m_campaignId)][curMapID]
                            [xCampaign.m_bonusChoices[curMapID]];
}

static i32 l_getCampaignChoiceType(lua_State* L) {
    lua_pushinteger(L, H2EnumIndex(CurrentCampaignChoice()->type));
    return 1;
}

static i32 l_getCampaignChoiceField(lua_State* L) {
    lua_pushinteger(L, CurrentCampaignChoice()->value);
    return 1;
}

static i32 l_getCampaignChoiceAmount(lua_State* L) {
    lua_pushinteger(L, CurrentCampaignChoice()->amount);
    return 1;
}

static i32 l_getCampaignChoice(lua_State* L) {
    PushBinding(L, Binding<SCampaignChoice*>(CurrentCampaignChoice()));
    return 1;
}

static void register_campaign_funcs(lua_State* L) {
    lua_register(L, "GetCampaignChoiceType", l_getCampaignChoiceType);
    lua_register(L, "GetCampaignChoiceField", l_getCampaignChoiceField);
    lua_register(L, "GetCampaignChoiceAmount", l_getCampaignChoiceAmount);
    lua_register(L, "GetCampaignChoice", l_getCampaignChoice);
}

/************************************** Uncategorized *********************************/

static i32 l_playsoundeffect(lua_State* L) {
    std::string snd = std::string(luaL_checkstring(L, 1));
    PlaySoundEffect(snd, SND_DO_WAIT, NULL);
    return 0;
}

static i32 l_getinclinedtojoin(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    i32 inclinedToJoin = 0;
    if ((x >= 0) && (y >= 0) && (x < gpGame->m_worldMap.width)
        && (y < gpGame->m_worldMap.height)) {
        mapCell* cell = gpGame->m_worldMap.GetCell(x, y);
        if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
            inclinedToJoin = (cell->m_objectMetadata & 0x1000);
        }
    }
    lua_pushinteger(L, inclinedToJoin ? 1 : 0);
    return 1;
}

static i32 l_setinclinedtojoin(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    bool inclinedToJoin = CheckBoolean(L, 3);
    if ((x >= 0) && (y >= 0) && (x < gpGame->m_worldMap.width)
        && (y < gpGame->m_worldMap.height)) {
        mapCell* cell = gpGame->m_worldMap.GetCell(x, y);
        if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
            if (inclinedToJoin) {
                cell->m_objectMetadata |= 0x1000;
            } else {
                cell->m_objectMetadata &= ~0x1000;
            }
        }
    }
    return 0;
}

static i32 l_startbattle(lua_State* L) {
    hero* hro = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 4)));
    i32 mon1 = static_cast<i32>(luaL_checknumber(L, 2));
    i32 mon1quantity = static_cast<i32>(luaL_checknumber(L, 3));
    i32 switchSides = static_cast<i32>(luaL_checknumber(L, 4));
    mapCell* mapcell = gpAdvManager->GetCell(hro->m_x, hro->m_y);
    CombatResult winningSide = gpAdvManager->CombatMonsterEvent(
        hro, static_cast<CreatureType>(mon1), mon1quantity, mapcell, hro->m_x, hro->m_y,
        switchSides, hro->m_x, hro->m_y, CREATURE_NONE, 0, 0, CREATURE_NONE, 0, 0
    );
    lua_pushinteger(L, H2EnumIndex(winningSide));
    return 1;
}

static i32 l_toggleAIArmySharing(lua_State* L) {
    bool toggle = CheckBoolean(L, 1);
    gpGame->SetAIArmySharing(toggle);
    return 0;
}

static i32 l_forceComputerPlayerChase(lua_State* L) {
    hero* src = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(1, 3)));
    hero* dst = static_cast<hero*>(PointerFromLuaClassTable(L, StackIndexOfArg(2, 3)));
    bool force = CheckBoolean(L, 3);
    gpGame->ForceHeroChase(src->m_id, dst->m_id, force);
    return 0;
}

static void register_uncategorized_funcs(lua_State* L) {
    lua_register(L, "PlaySoundEffect", l_playsoundeffect);
    lua_register(L, "GetInclinedToJoin", l_getinclinedtojoin);
    lua_register(L, "SetInclinedToJoin", l_setinclinedtojoin);
    lua_register(L, "StartBattle", l_startbattle);
    lua_register(L, "ToggleAIArmySharing", l_toggleAIArmySharing);
    lua_register(L, "ForceComputerPlayerChase", l_forceComputerPlayerChase);
}

/**************************************************************************************/

void RegisterFunctions(lua_State* L) {
    register_dialog_funcs(L);
    register_date_funcs(L);
    register_player_funcs(L);
    register_hero_funcs(L);
    register_map_funcs(L);
    register_town_funcs(L);
    register_battle_funcs(L);
    register_campaign_funcs(L);
    register_uncategorized_funcs(L);
}

} // namespace ironfist::script
