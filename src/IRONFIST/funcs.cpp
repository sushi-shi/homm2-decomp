#include <IRONFIST/register.h>

#include <cstring>
#include <string>

#include <IRONFIST/callback.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/deepbinding.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/state.h>
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
    if (!MAP_TRIGGER_IS_ACTION(loc->m_triggerType)) {
        return false;
    }
    switch (MAP_TRIGGER_OBJECT(loc->m_triggerType)) {
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

static i32 CheckIndex(lua_State* L, i32 argument, i32 count, const char* description) {
    const lua_Integer index = luaL_checkinteger(L, argument);
    if (index < 0 || index >= count) {
        luaL_argerror(L, argument, description);
    }
    return static_cast<i32>(index);
}

static game& RequireGame(lua_State* L) {
    if (!gpGame)
        luaL_error(L, "no game session is active");
    return *gpGame;
}

static combatManager& RequireBattle(lua_State* L) {
    if (!state::Get().combat.IsActiveFor(gpCombatManager))
        luaL_error(L, "no battle is active");
    return *gpCombatManager;
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

static i32 LuaMessageBox(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    H2MessageBox(const_cast<char*>(msg));
    return 0;
}

static i32 LuaAdvancedMessageBox(lua_State* L) {
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

static i32 LuaQuestionBox(lua_State* L) {
    char* qst = const_cast<char*>(luaL_checkstring(L, 1));
    lua_pushboolean(L, H2QuestionBox(qst));
    return 1;
}

static i32 LuaInputBox(lua_State* L) {
    char* qst = const_cast<char*>(luaL_checkstring(L, 1));
    i32 len = static_cast<i32>(luaL_checknumber(L, 2));
    char* input = H2InputBox(qst, len);
    lua_pushstring(L, input);
    H2_FREE(input); // pushstring copies it
    return 1;
}

static i32 LuaRecruitBox(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 creature = static_cast<i32>(luaL_checknumber(L, 2));
    i16 quantity = static_cast<i16>(luaL_checknumber(L, 3));
    i16 startQ = quantity;
    gpAdvManager->ExpansionRecruitEvent(hro, CreatureTypeFromCode(creature), &quantity);

    lua_pushinteger(L, startQ - quantity);
    return 1;
}

static void RegisterDialogFunctions(lua_State* L) {
    lua_register(L, "MessageBox", LuaMessageBox);
    lua_register(L, "AdvancedMessageBox", LuaAdvancedMessageBox);
    lua_register(L, "QuestionBox", LuaQuestionBox);
    lua_register(L, "InputBox", LuaInputBox);
    lua_register(L, "RecruitBox", LuaRecruitBox);
}

/************************************************ Date ********************************/

static i32 LuaGetDay(lua_State* L) {
    lua_pushinteger(L, gpGame->m_day);
    return 1;
}

static i32 LuaGetWeek(lua_State* L) {
    lua_pushinteger(L, gpGame->m_week);
    return 1;
}

static i32 LuaGetMonth(lua_State* L) {
    lua_pushinteger(L, gpGame->m_month);
    return 1;
}

static void RegisterDateFunctions(lua_State* L) {
    lua_register(L, "GetDay", LuaGetDay);
    lua_register(L, "GetWeek", LuaGetWeek);
    lua_register(L, "GetMonth", LuaGetMonth);
}

/************************************************ Player ******************************/

static i32 LuaGetNumPlayers(lua_State* L) {
    lua_pushinteger(L, gpGame->m_playerCount);
    return 1;
}

static i32 LuaGetPlayer(lua_State* L) {
    auto& game = RequireGame(L);
    i32 n = CheckIndex(L, 1, H2EnumIndex(GAME_PLAYER_COUNT), "player index out of range");
    CheckIndex(L, 1, game.m_playerCount, "player is outside this session");
    PushBinding(L, Binding<playerData*>(&game.m_players[n]));
    return 1;
}

static i32 LuaGetCurrentPlayer(lua_State* L) {
    PushBinding(L, Binding<playerData*>(gpCurPlayer));
    return 1;
}

static i32 LuaGetPlayerColor(lua_State* L) {
    playerData* p = CheckObject<playerData>(L, 1);
    lua_pushinteger(L, p->m_color);
    return 1;
}

static i32 LuaGetNumHeroes(lua_State* L) {
    playerData* p = CheckObject<playerData>(L, 1);
    lua_pushinteger(L, p->m_heroCount);
    return 1;
}

static i32 LuaGetHero(lua_State* L) {
    playerData* p = CheckObject<playerData>(L, 1);
    i32 n = CheckIndex(L, 2, p->m_heroCount, "owned hero index out of range");
    i32 heroId = CheckStoredIndex(
        L, p->m_heroIds[n], H2EnumIndex(GAME_HERO_COUNT), "owned hero id"
    );
    PushBinding(L, Binding<hero*>(&gpGame->m_heroRecs[heroId]));
    return 1;
}

static i32 LuaGetHeroForHire(lua_State* L) {
    playerData* p = CheckObject<playerData>(L, 1);
    i32 n = CheckIndex(
        L, 2, PLAYER_AVAILABLE_HERO_COUNT, "available hero index out of range"
    );
    i32 heroId = CheckStoredIndex(
        L, p->m_availableHeroIds[n], H2EnumIndex(GAME_HERO_COUNT), "available hero id"
    );
    PushBinding(L, Binding<hero*>(&gpGame->m_heroRecs[heroId]));
    return 1;
}

static i32 LuaGiveResource(lua_State* L) {
    playerData* player = CheckObject<playerData>(L, 1);
    i32 res = static_cast<i32>(luaL_checknumber(L, 2));
    i32 val = static_cast<i32>(luaL_checknumber(L, 3));
    player->m_resources[res] += val;
    return 0;
}

static i32 LuaSetResource(lua_State* L) {
    playerData* player = CheckObject<playerData>(L, 1);
    i32 res = static_cast<i32>(luaL_checknumber(L, 2));
    i32 val = static_cast<i32>(luaL_checknumber(L, 3));
    player->m_resources[res] = val;
    return 0;
}

static i32 LuaGetResource(lua_State* L) {
    playerData* player = CheckObject<playerData>(L, 1);
    i32 res = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, player->m_resources[res]);
    return 1;
}

static i32 LuaShareVision(lua_State* L) {
    i32 sourcePlayer = static_cast<i32>(luaL_checknumber(L, 1));
    i32 destPlayer = static_cast<i32>(luaL_checknumber(L, 2));
    gpGame->ShareVision(sourcePlayer, destPlayer);
    return 0;
}

static i32 LuaCancelShareVision(lua_State* L) {
    i32 sourcePlayer = static_cast<i32>(luaL_checknumber(L, 1));
    i32 destPlayer = static_cast<i32>(luaL_checknumber(L, 2));
    gpGame->CancelVisionShare(sourcePlayer, destPlayer);
    return 0;
}

static i32 LuaSetDaysAfterTownLost(lua_State* L) {
    playerData* player = CheckObject<playerData>(L, 1);
    i32 days = static_cast<i32>(luaL_checknumber(L, 2));
    player->m_daysLeft = days;
    return 0;
}

static i32 LuaGetDaysAfterTownLost(lua_State* L) {
    playerData* player = CheckObject<playerData>(L, 1);
    lua_pushinteger(L, player->m_daysLeft);
    return 1;
}

static i32 LuaRevealMap(lua_State* L) {
    playerData* player = CheckObject<playerData>(L, 1);
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

static i32 LuaSetBarrierTentVisited(lua_State* L) {
    playerData* plyd = CheckObject<playerData>(L, 1);
    i32 tentcolor = luaL_checknumber(L, 2);
    plyd->m_barrierTents |= (1 << tentcolor);

    return 0;
}

static void RegisterPlayerFunctions(lua_State* L) {
    lua_register(L, "GetNumPlayers", LuaGetNumPlayers);
    lua_register(L, "GetPlayer", LuaGetPlayer);
    lua_register(L, "GetCurrentPlayer", LuaGetCurrentPlayer);
    lua_register(L, "GetPlayerColor", LuaGetPlayerColor);
    lua_register(L, "GetNumHeroes", LuaGetNumHeroes);
    lua_register(L, "GetHero", LuaGetHero);
    lua_register(L, "GetHeroForHire", LuaGetHeroForHire);
    lua_register(L, "GiveResource", LuaGiveResource);
    lua_register(L, "SetResource", LuaSetResource);
    lua_register(L, "GetResource", LuaGetResource);
    lua_register(L, "ShareVision", LuaShareVision);
    lua_register(L, "CancelShareVision", LuaCancelShareVision);
    lua_register(L, "SetDaysAfterTownLost", LuaSetDaysAfterTownLost);
    lua_register(L, "GetDaysAfterTownLost", LuaGetDaysAfterTownLost);
    lua_register(L, "RevealMap", LuaRevealMap);
    lua_register(L, "SetBarrierTentVisited", LuaSetBarrierTentVisited);
}

/************************************************ Heroes ******************************/

static hero* GetCurrentHero() {
    if (!gpGame || !gpCurPlayer)
        return nullptr;
    bool owned = false;
    for (auto& player : gpGame->m_players)
        owned |= &player == gpCurPlayer;
    if (!owned || gpCurPlayer->m_currentHero < 0
        || gpCurPlayer->m_currentHero >= H2EnumIndex(GAME_HERO_COUNT))
        return nullptr;
    return &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
}

static i32 LuaGetCurrentHero(lua_State* L) {
    PushBinding(L, Binding<hero*>(GetCurrentHero()));
    return 1;
}

static i32 LuaGrantSpell(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 sp = static_cast<i32>(luaL_checknumber(L, 2));
    hro->AddSpell(SpellTypeFromCode(sp), hro->Stats(HERO_PRIMARY_KNOWLEDGE));
    return 0;
}

static i32 LuaForgetSpell(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 spell = static_cast<i32>(luaL_checknumber(L, 2));
    if (spell >= 0 && spell < KB_SPELL_TABLE_CAPACITY)
        hro->m_spells[spell] = 0;
    return 0;
}

static i32 LuaHasTroop(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
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

static i32 LuaGetCreatureAmount(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
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

static i32 LuaTakeTroop(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
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

static i32 LuaTeleportHero(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 x = static_cast<i32>(luaL_checknumber(L, 2));
    i32 y = static_cast<i32>(luaL_checknumber(L, 3));

    // Working around a bug where TeleportTo will erroneously call
    // gpGame->SetVisibility(hero, viewX, viewY, heroViewingRadius)
    gpAdvManager->m_mapOriginX = x;
    gpAdvManager->m_mapOriginY = y;

    gpAdvManager->TeleportTo(hro, x, y, 0, 0);
    return 0;
}

static i32 LuaGetHeroName(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushstring(L, hro->m_name);
    return 1;
}

static i32 LuaSetHeroName(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    CopyLuaName(L, 2, hro->m_name, sizeof(hro->m_name));
    return 0;
}

static i32 LuaGetHeroInPool(lua_State* L) {
    auto& game = RequireGame(L);
    i32 n = CheckIndex(L, 1, H2EnumIndex(GAME_HERO_COUNT), "hero index out of range");
    PushBinding(L, Binding<hero*>(&game.m_heroRecs[n]));
    return 1;
}

static i32 LuaGetHeroOwner(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);

    if (hro->m_owner < 0) {
        lua_pushnil(L);
    } else {
        auto& game = RequireGame(L);
        const auto owner = CheckStoredIndex(L, hro->m_owner, H2EnumIndex(GAME_PLAYER_COUNT), "hero owner");
        PushBinding(L, Binding<playerData*>(&game.m_players[owner]));
    }

    return 1;
}

static i32 LuaGrantArtifact(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 art = static_cast<i32>(luaL_checknumber(L, 2));
    GiveArtifact(hro, ArtifactTypeFromCode(art), 1, -1);
    return 0;
}

static i32 LuaHasArtifact(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 art = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushboolean(L, hro->HasArtifact(ArtifactTypeFromCode(art)));
    return 1;
}

static i32 LuaTakeArtifact(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
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

static i32 LuaCountEmptyArtifactSlots(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 amount = 0;
    for (i32 i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        if (hro->m_artifacts[i] == ARTIFACT_NONE) {
            amount++;
        }
    }
    lua_pushinteger(L, amount);
    return 1;
}

static i32 LuaCountEmptyCreatureSlots(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 amount = 0;
    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (hro->m_army.m_creatureTypes[i] == CREATURE_NONE) {
            amount++;
        }
    }
    lua_pushinteger(L, amount);
    return 1;
}

static i32 LuaSetExperiencePoints(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 points = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_experience = points;
    hro->CheckLevel();
    return 0;
}

static i32 LuaGetExperiencePoints(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_experience);
    return 1;
}

static i32 LuaSetPrimarySkill(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    i32 amt = static_cast<i32>(luaL_checknumber(L, 3));
    hro->m_primaryStats[skill] = amt;
    return 0;
}

static i32 LuaGetPrimarySkill(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, hro->m_primaryStats[skill]);
    return 1;
}

static i32 LuaSetSpellpoints(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 points = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_spellPoints = points;
    return 0;
}

static i32 LuaGetSpellpoints(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_spellPoints);
    return 1;
}

static i32 LuaSetSecondarySkill(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    i32 level = static_cast<i32>(luaL_checknumber(L, 3));
    hro->SetSS(HeroSecondarySkillFromCode(skill), HeroSkillLevelFromCode(level));
    return 0;
}

static i32 LuaGetSecondarySkill(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 skill = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, hro->GetSSLevel(HeroSecondarySkillFromCode(skill)));
    return 1;
}

static i32 LuaGrantArmy(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 cr = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    hro->m_army.Add(CreatureTypeFromCode(cr), n, -1);
    return 0;
}

static i32 LuaGetHeroMobility(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_mobility);
    return 1;
}

static i32 LuaSetHeroMobility(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 mobility = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_mobility = mobility;
    return 0;
}

static i32 LuaGetHeroRemainingMobility(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_remainingMobility);
    return 1;
}

static i32 LuaSetHeroRemainingMobility(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 remainingMobility = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_remainingMobility = remainingMobility;
    return 0;
}

static i32 LuaGetHeroX(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_x);
    return 1;
}

static i32 LuaGetHeroY(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_y);
    return 1;
}

static i32 LuaGetHeroLevel(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_level);
    return 1;
}

static i32 LuaGetHeroTempMoraleBonuses(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_morale);
    return 1;
}

static i32 LuaSetHeroTempMoraleBonuses(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 moraleBonus = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_morale = moraleBonus;
    return 0;
}

static i32 LuaGetHeroTempLuckBonuses(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_luck);
    return 1;
}

static i32 LuaSetHeroTempLuckBonuses(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 luckBonus = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_luck = luckBonus;
    return 0;
}

static i32 LuaGrantSpellScroll(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 sp = static_cast<i32>(luaL_checknumber(L, 2));
    GiveArtifact(hro, ARTIFACT_SPELL_SCROLL, 1, sp);
    return 0;
}

static i32 LuaGetHeroFaction(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    lua_pushinteger(L, hro->m_cursorType.value());
    return 1;
}

static i32 LuaSetHeroFaction(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 newFaction = static_cast<i32>(luaL_checknumber(L, 2));
    hro->m_cursorType = FactionTypeFromCode(newFaction);
    return 0;
}

static void RegisterHeroFunctions(lua_State* L) {
    lua_register(L, "GetCurrentHero", LuaGetCurrentHero);
    lua_register(L, "GrantSpell", LuaGrantSpell);
    lua_register(L, "ForgetSpell", LuaForgetSpell);
    lua_register(L, "HasTroop", LuaHasTroop);
    lua_register(L, "GetCreatureAmount", LuaGetCreatureAmount);
    lua_register(L, "TakeTroop", LuaTakeTroop);
    lua_register(L, "TeleportHero", LuaTeleportHero);
    lua_register(L, "GetHeroName", LuaGetHeroName);
    lua_register(L, "SetHeroName", LuaSetHeroName);
    lua_register(L, "GetHeroInPool", LuaGetHeroInPool);
    lua_register(L, "GetHeroOwner", LuaGetHeroOwner);
    lua_register(L, "GrantArtifact", LuaGrantArtifact);
    lua_register(L, "HasArtifact", LuaHasArtifact);
    lua_register(L, "TakeArtifact", LuaTakeArtifact);
    lua_register(L, "CountEmptyArtifactSlots", LuaCountEmptyArtifactSlots);
    lua_register(L, "CountEmptyCreatureSlots", LuaCountEmptyCreatureSlots);
    lua_register(L, "SetExperiencePoints", LuaSetExperiencePoints);
    lua_register(L, "GetExperiencePoints", LuaGetExperiencePoints);
    lua_register(L, "SetPrimarySkill", LuaSetPrimarySkill);
    lua_register(L, "GetPrimarySkill", LuaGetPrimarySkill);
    lua_register(L, "SetSpellpoints", LuaSetSpellpoints);
    lua_register(L, "GetSpellpoints", LuaGetSpellpoints);
    lua_register(L, "SetSecondarySkill", LuaSetSecondarySkill);
    lua_register(L, "GetSecondarySkill", LuaGetSecondarySkill);
    lua_register(L, "GrantArmy", LuaGrantArmy);
    lua_register(L, "GetHeroMobility", LuaGetHeroMobility);
    lua_register(L, "SetHeroMobility", LuaSetHeroMobility);
    lua_register(L, "GetHeroRemainingMobility", LuaGetHeroRemainingMobility);
    lua_register(L, "SetHeroRemainingMobility", LuaSetHeroRemainingMobility);
    lua_register(L, "GetHeroX", LuaGetHeroX);
    lua_register(L, "GetHeroY", LuaGetHeroY);
    lua_register(L, "GetHeroLevel", LuaGetHeroLevel);
    lua_register(L, "GetHeroTempMoraleBonuses", LuaGetHeroTempMoraleBonuses);
    lua_register(L, "SetHeroTempMoraleBonuses", LuaSetHeroTempMoraleBonuses);
    lua_register(L, "GetHeroTempLuckBonuses", LuaGetHeroTempLuckBonuses);
    lua_register(L, "SetHeroTempLuckBonuses", LuaSetHeroTempLuckBonuses);
    lua_register(L, "GrantSpellScroll", LuaGrantSpellScroll);
    lua_register(L, "GetHeroFaction", LuaGetHeroFaction);
    lua_register(L, "SetHeroFaction", LuaSetHeroFaction);
}

/************************************** Map *******************************************/

static i32 LuaMapSetObject(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    i32 obj = static_cast<i32>(luaL_checknumber(L, 3));

    mapCell* cell = gpAdvManager->GetCell(x, y);
    // Ironfist's Lua placement contract uses overlay slot 3 for map objects.
    cell->m_overlayIndex = 3;
    cell->m_objectIndex = obj;
    cell->m_triggerType = MAP_PASSIVE_TRIGGER(MapObjectTypeFromCode(obj & 0x7f));
    return 0;
}

static i32 LuaMapPutArmy(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    i32 monIdx = static_cast<i32>(luaL_checknumber(L, 3));
    i32 monQty = static_cast<i32>(luaL_checknumber(L, 4));

    mapCell* loc = gpAdvManager->GetCell(x, y);
    loc->m_objectIndex = monIdx;
    loc->m_objectMetadata = monQty;
    loc->SetObjectTileset(TILESET_MONS32);
    loc->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
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
static i32 LuaMapEraseSquare(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    mapCell* cell = gpAdvManager->GetCell(x, y);
    gpAdvManager->EraseObj(cell, x, y);
    gpAdvManager->CompleteDraw(0);
    return 0;
}

static i32 LuaMapFizzle(lua_State* L) {
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

static i32 LuaMapSetTileTerrain(lua_State* L) {
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

static void RegisterMapFunctions(lua_State* L) {
    lua_register(L, "MapSetObject", LuaMapSetObject);
    lua_register(L, "MapPutArmy", LuaMapPutArmy);
    lua_register(L, "MapEraseSquare", LuaMapEraseSquare);
    lua_register(L, "MapFizzle", LuaMapFizzle);
    lua_register(L, "MapSetTileTerrain", LuaMapSetTileTerrain);
}

/************************************** Town ******************************************/

static i32 LuaGetCurrentTown(lua_State* L) {
    PushBinding(L, Binding<town*>(gpTownManager ? gpTownManager->m_town : nullptr));
    return 1;
}

static i32 LuaHasVisitingHero(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    lua_pushboolean(L, twn->m_occupyingHeroId >= 0);
    return 1;
}

static i32 LuaGetVisitingHero(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
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

static i32 LuaBuildInCurrentTown(lua_State* L) {
    i32 obj = static_cast<i32>(luaL_checknumber(L, 1));
    gpTownManager->BuildObj(BuildingSlotTypeFromCode(obj));
    return 0;
}

static i32 LuaGetTown(lua_State* L) {
    auto& game = RequireGame(L);
    i32 index = CheckIndex(L, 1, H2EnumIndex(GAME_TOWN_COUNT), "town index out of range");
    PushBinding(L, Binding<town*>(&game.m_castleRecs[index]));
    return 1;
}

static i32 LuaGetTownName(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    lua_pushstring(L, twn->m_name);
    return 1;
}

static i32 LuaSetTownName(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    CopyLuaName(L, 2, twn->m_name, sizeof(twn->m_name));
    // Upstream returns the still-on-stack name argument as the single result.
    return 1;
}

static i32 LuaGetTownByName(lua_State* L) {
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

static i32 LuaGetPlayerTown(lua_State* L) {
    playerData* player = CheckObject<playerData>(L, 1);
    i32 index = CheckIndex(L, 2, player->m_townCount, "owned town index out of range");
    i32 townId = CheckStoredIndex(
        L, player->m_townIds[index], H2EnumIndex(GAME_TOWN_COUNT), "owned town id"
    );
    PushBinding(L, Binding<town*>(&gpGame->m_castleRecs[townId]));
    return 1;
}

static i32 LuaBuildInTown(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    i32 building = static_cast<i32>(luaL_checknumber(L, 2));
    twn->BuildBuilding(BuildingSlotTypeFromCode(building));
    return 0;
}

static i32 LuaGetTownFaction(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    lua_pushinteger(L, twn->m_type.value());
    return 1;
}

static i32 LuaSetTownFaction(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    i32 faction = static_cast<i32>(luaL_checknumber(L, 2));
    twn->SetFaction(FactionTypeFromCode(faction));
    return 0;
}

static i32 LuaGetCreatureCost(lua_State* L) {
    i32 creature = static_cast<i32>(luaL_checknumber(L, 1));
    i32 cost[H2EnumIndex(RES_COUNT)];
    GetMonsterCost(CreatureTypeFromCode(creature), cost);
    for (i32 i = 0; i < H2EnumIndex(RES_COUNT); i++) {
        lua_pushinteger(L, cost[i]);
    }
    return H2EnumIndex(RES_COUNT);
}

static i32 LuaGetTownOwner(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    lua_pushinteger(L, twn->m_owner);
    return 1;
}

static i32 LuaSetTownOwner(lua_State* L) {
    i32 townIdx = static_cast<i32>(luaL_checknumber(L, 1));
    i32 playerIdx = static_cast<i32>(luaL_checknumber(L, 2));
    gpGame->ClaimTown(townIdx, playerIdx, 0);
    return 0;
}

static i32 LuaGetTownX(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    lua_pushinteger(L, twn->m_x);
    return 1;
}

static i32 LuaGetTownY(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    lua_pushinteger(L, twn->m_y);
    return 1;
}

static i32 LuaGetTownIdFromPos(lua_State* L) {
    i32 x = static_cast<i32>(luaL_checknumber(L, 1));
    i32 y = static_cast<i32>(luaL_checknumber(L, 2));
    lua_pushinteger(L, gpGame->GetTownId(x, y));
    return 1;
}

static i32 LuaSetNumberOfCreatures(lua_State* L) {
    town* cstle = CheckObject<town>(L, 1);
    i32 dwllng = static_cast<i32>(luaL_checknumber(L, 2));
    i32 numcrtrs = static_cast<i32>(luaL_checknumber(L, 3));
    cstle->m_garrison[dwllng] = numcrtrs;
    return 0;
}

static i32 LuaSetNumGuildSpells(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    i32 l = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    twn->m_spellCounts[l + TOWN_MAGE_GUILD_FIRST_LEVEL] = n;
    twn->GiveSpells(NULL);
    return 0;
}

static i32 LuaSetGuildSpell(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    i32 l = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    i32 s = static_cast<i32>(luaL_checknumber(L, 4));
    twn->m_spells[l][n] = SpellTypeFromCode(s);
    twn->GiveSpells(NULL);
    return 0;
}

static i32 LuaGetGuildSpell(lua_State* L) {
    town* twn = CheckObject<town>(L, 1);
    i32 l = static_cast<i32>(luaL_checknumber(L, 2));
    i32 n = static_cast<i32>(luaL_checknumber(L, 3));
    lua_pushinteger(L, twn->m_spells[l][n].value());
    return 1;
}

static i32 LuaDisallowBuilding(lua_State* L) {
    i32 townIdx = static_cast<i32>(luaL_checknumber(L, 1));
    i32 building = static_cast<i32>(luaL_checknumber(L, 2));
    if (townIdx >= 0 && townIdx < GAME_TOWN_COUNT) {
        gpGame->m_castleRecs[townIdx].DisallowBuilding(building);
    }
    return 0;
}

static void RegisterTownFunctions(lua_State* L) {
    lua_register(L, "GetCurrentTown", LuaGetCurrentTown);
    lua_register(L, "HasVisitingHero", LuaHasVisitingHero);
    lua_register(L, "GetVisitingHero", LuaGetVisitingHero);
    lua_register(L, "BuildInCurrentTown", LuaBuildInCurrentTown);
    lua_register(L, "GetTown", LuaGetTown);
    lua_register(L, "GetTownName", LuaGetTownName);
    lua_register(L, "SetTownName", LuaSetTownName);
    lua_register(L, "GetTownByName", LuaGetTownByName);
    lua_register(L, "GetPlayerTown", LuaGetPlayerTown);
    lua_register(L, "BuildInTown", LuaBuildInTown);
    lua_register(L, "GetTownFaction", LuaGetTownFaction);
    lua_register(L, "SetTownFaction", LuaSetTownFaction);
    lua_register(L, "GetCreatureCost", LuaGetCreatureCost);
    lua_register(L, "GetTownOwner", LuaGetTownOwner);
    lua_register(L, "SetTownOwner", LuaSetTownOwner);
    lua_register(L, "GetTownX", LuaGetTownX);
    lua_register(L, "GetTownY", LuaGetTownY);
    lua_register(L, "GetTownIdFromPos", LuaGetTownIdFromPos);
    lua_register(L, "SetNumberOfCreatures", LuaSetNumberOfCreatures);
    lua_register(L, "SetNumGuildSpells", LuaSetNumGuildSpells);
    lua_register(L, "SetGuildSpell", LuaSetGuildSpell);
    lua_register(L, "GetGuildSpell", LuaGetGuildSpell);
    lua_register(L, "DisallowBuilding", LuaDisallowBuilding);
}

/************************************* Battle *****************************************/

static i32 LuaBattleSummonCreature(lua_State* L) {
    auto& battle = RequireBattle(L);
    i32 side = CheckIndex(L, 1, COMBAT_SIDE_COUNT, "battle side out of range");
    i32 hex = static_cast<i32>(luaL_checknumber(L, 2));
    i32 creature = static_cast<i32>(luaL_checknumber(L, 3));
    i32 quantity = static_cast<i32>(luaL_checknumber(L, 4));

    battle.AddArmy(
        CombatSideFromCode(side), CreatureTypeFromCode(creature), quantity, hex,
        MONSTER_FLAGS_NONE, 1
    );
    return 0;
}

static i32 LuaIsHexEmpty(lua_State* L) {
    auto& battle = RequireBattle(L);
    i32 hexno = static_cast<i32>(luaL_checknumber(L, 1));
    if (!ValidHex(hexno)) {
        lua_pushboolean(L, 0);
    } else {
        hexcell* cell = &battle.m_hexCells[hexno];
        lua_pushboolean(L, cell->m_occupantSide.value() == -1 && !cell->m_blocked);
    }

    return 1;
}

static i32 LuaBattleHasHero(lua_State* L) {
    auto& battle = RequireBattle(L);
    i32 side = CheckIndex(L, 1, COMBAT_SIDE_COUNT, "battle side out of range");
    lua_pushboolean(L, battle.m_heroes[side] != NULL);
    return 1;
}

static i32 LuaBattleGetHero(lua_State* L) {
    auto& battle = RequireBattle(L);
    i32 side = CheckIndex(L, 1, COMBAT_SIDE_COUNT, "battle side out of range");
    PushBinding(L, Binding<hero*>(battle.m_heroes[side]));
    return 1;
}

static i32 LuaBattleMessage(lua_State* L) {
    auto& battle = RequireBattle(L);
    char* message = const_cast<char*>(luaL_checkstring(L, 1));
    battle.CombatMessage(message, 1, 0, 0);
    return 0;
}

static i32 LuaBattleNumStacksForSide(lua_State* L) {
    auto& battle = RequireBattle(L);
    i32 side = CheckIndex(L, 1, COMBAT_SIDE_COUNT, "battle side out of range");
    lua_pushinteger(L, battle.m_armyCount[side]);
    return 1;
}

static i32 LuaBattleGetStack(lua_State* L) {
    auto& battle = RequireBattle(L);
    i32 side = CheckIndex(L, 1, COMBAT_SIDE_COUNT, "battle side out of range");
    i32 idx = CheckIndex(L, 2, COMBAT_ARMY_SLOT_COUNT, "battle stack index out of range");
    CheckIndex(L, 2, battle.m_armyCount[side], "stack is outside this battle");
    PushBinding(L, Binding<army*>(&battle.m_armies[side][idx]));
    return 1;
}

static i32 LuaGetStackSide(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_side.value());
    return 1;
}

static i32 LuaGetStackType(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_monsterType.value());
    return 1;
}

static i32 LuaGetStackQuantity(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_quantity);
    return 1;
}

static i32 LuaSetStackQuantity(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 quantity = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_quantity = quantity;
    return 0;
}

static i32 LuaGetStackInitialQuantity(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_initialQuantity);
    return 1;
}

static i32 LuaSetStackInitialQuantity(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 initialQuantity = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_initialQuantity = initialQuantity;
    return 0;
}

static i32 LuaGetStackHex(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_hex);
    return 1;
}

static i32 LuaGetStackMorale(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_morale);
    return 1;
}

static i32 LuaSetStackMorale(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 morale = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_morale = morale;
    return 0;
}

static i32 LuaGetStackLuck(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_luck);
    return 1;
}

static i32 LuaSetStackLuck(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 luck = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_luck = luck;
    return 0;
}

static i32 LuaGetStackAttack(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_monster.attack);
    return 1;
}

static i32 LuaSetStackAttack(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 attack = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.attack = attack;
    return 0;
}

static i32 LuaGetStackDefense(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_monster.defense);
    return 1;
}

static i32 LuaSetStackDefense(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 defense = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.defense = defense;
    return 0;
}

static i32 LuaGetStackSpeed(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_monster.speed);
    return 1;
}

static i32 LuaSetStackSpeed(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 speed = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.speed = speed;
    return 0;
}

static i32 LuaGetStackShots(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_monster.shots);
    return 1;
}

static i32 LuaSetStackShots(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 shots = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_monster.shots = shots;
    return 0;
}

static i32 LuaGetStackHp(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    lua_pushinteger(L, creat->m_monster.hitPoints - creat->m_hitPointsLost);
    return 1;
}

static i32 LuaSetStackHp(lua_State* L) {
    army* creat = CheckObject<army>(L, 1);
    i32 hp = static_cast<i32>(luaL_checknumber(L, 2));
    creat->m_hitPointsLost = creat->m_monster.hitPoints - hp;
    return 0;
}

static void RegisterBattleFunctions(lua_State* L) {
    lua_register(L, "BattleSummonCreature", LuaBattleSummonCreature);
    lua_register(L, "IsHexEmpty", LuaIsHexEmpty);
    lua_register(L, "BattleHasHero", LuaBattleHasHero);
    lua_register(L, "BattleGetHero", LuaBattleGetHero);
    lua_register(L, "BattleMessage", LuaBattleMessage);
    lua_register(L, "BattleNumStacksForSide", LuaBattleNumStacksForSide);
    lua_register(L, "BattleGetStack", LuaBattleGetStack);
    lua_register(L, "GetStackSide", LuaGetStackSide);
    lua_register(L, "GetStackType", LuaGetStackType);
    lua_register(L, "GetStackQuantity", LuaGetStackQuantity);
    lua_register(L, "SetStackQuantity", LuaSetStackQuantity);
    lua_register(L, "GetStackInitialQuantity", LuaGetStackInitialQuantity);
    lua_register(L, "SetStackInitialQuantity", LuaSetStackInitialQuantity);
    lua_register(L, "GetStackHex", LuaGetStackHex);
    lua_register(L, "GetStackMorale", LuaGetStackMorale);
    lua_register(L, "SetStackMorale", LuaSetStackMorale);
    lua_register(L, "GetStackLuck", LuaGetStackLuck);
    lua_register(L, "SetStackLuck", LuaSetStackLuck);
    lua_register(L, "GetStackAttack", LuaGetStackAttack);
    lua_register(L, "SetStackAttack", LuaSetStackAttack);
    lua_register(L, "GetStackDefense", LuaGetStackDefense);
    lua_register(L, "SetStackDefense", LuaSetStackDefense);
    lua_register(L, "GetStackSpeed", LuaGetStackSpeed);
    lua_register(L, "SetStackSpeed", LuaSetStackSpeed);
    lua_register(L, "GetStackShots", LuaGetStackShots);
    lua_register(L, "SetStackShots", LuaSetStackShots);
    lua_register(L, "GetStackHp", LuaGetStackHp);
    lua_register(L, "SetStackHp", LuaSetStackHp);
}

/**************************************** Campaign ************************************/

static const SCampaignChoice* CurrentCampaignChoice(lua_State* L) {
    const auto* definition = Campaigns().Find(xCampaign.m_campaignId);
    const i32 map = H2EnumIndex(xCampaign.m_currentMap);
    if (!xIsPlayingExpansionCampaign || !definition || map < 0
        || static_cast<size_t>(map) >= definition->scenarios.size()
        || xCampaign.m_bonusChoices[map] >= EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT) {
        luaL_error(L, "No active campaign choice");
        return nullptr;
    }
    return &definition->Scenario(map).choices[xCampaign.m_bonusChoices[map]];
}

static i32 LuaGetCampaignChoiceType(lua_State* L) {
    lua_pushinteger(L, H2EnumIndex(CurrentCampaignChoice(L)->type));
    return 1;
}

static i32 LuaGetCampaignChoiceField(lua_State* L) {
    lua_pushinteger(L, CurrentCampaignChoice(L)->value);
    return 1;
}

static i32 LuaGetCampaignChoiceAmount(lua_State* L) {
    lua_pushinteger(L, CurrentCampaignChoice(L)->amount);
    return 1;
}

static i32 LuaGetCampaignChoice(lua_State* L) {
    PushBinding(L, Binding<SCampaignChoice>(*CurrentCampaignChoice(L)));
    return 1;
}

static void RegisterCampaignFunctions(lua_State* L) {
    lua_register(L, "GetCampaignChoiceType", LuaGetCampaignChoiceType);
    lua_register(L, "GetCampaignChoiceField", LuaGetCampaignChoiceField);
    lua_register(L, "GetCampaignChoiceAmount", LuaGetCampaignChoiceAmount);
    lua_register(L, "GetCampaignChoice", LuaGetCampaignChoice);
}

/************************************** Uncategorized *********************************/

static i32 LuaPlaySoundEffect(lua_State* L) {
    std::string snd = std::string(luaL_checkstring(L, 1));
    PlaySoundEffect(snd, SND_DO_WAIT, NULL);
    return 0;
}

static i32 LuaGetInclinedToJoin(lua_State* L) {
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

static i32 LuaSetInclinedToJoin(lua_State* L) {
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

static i32 LuaStartBattle(lua_State* L) {
    hero* hro = CheckObject<hero>(L, 1);
    i32 mon1 = static_cast<i32>(luaL_checknumber(L, 2));
    i32 mon1quantity = static_cast<i32>(luaL_checknumber(L, 3));
    i32 switchSides = static_cast<i32>(luaL_checknumber(L, 4));
    mapCell* mapcell = gpAdvManager->GetCell(hro->m_x, hro->m_y);
    CombatResult winningSide = gpAdvManager->CombatMonsterEvent(
        hro, CreatureTypeFromCode(mon1), mon1quantity, mapcell, hro->m_x, hro->m_y,
        switchSides, hro->m_x, hro->m_y, CREATURE_NONE, 0, 0, CREATURE_NONE, 0, 0
    );
    lua_pushinteger(L, H2EnumIndex(winningSide));
    return 1;
}

static i32 LuaToggleAIArmySharing(lua_State* L) {
    bool toggle = CheckBoolean(L, 1);
    gpGame->SetAIArmySharing(toggle);
    return 0;
}

static i32 LuaForceComputerPlayerChase(lua_State* L) {
    hero* src = CheckObject<hero>(L, 1);
    hero* dst = CheckObject<hero>(L, 2);
    bool force = CheckBoolean(L, 3);
    gpGame->ForceHeroChase(src->m_id, dst->m_id, force);
    return 0;
}

static void RegisterUncategorizedFunctions(lua_State* L) {
    lua_register(L, "PlaySoundEffect", LuaPlaySoundEffect);
    lua_register(L, "GetInclinedToJoin", LuaGetInclinedToJoin);
    lua_register(L, "SetInclinedToJoin", LuaSetInclinedToJoin);
    lua_register(L, "StartBattle", LuaStartBattle);
    lua_register(L, "ToggleAIArmySharing", LuaToggleAIArmySharing);
    lua_register(L, "ForceComputerPlayerChase", LuaForceComputerPlayerChase);
}

/**************************************************************************************/

void RegisterFunctions(lua_State* L) {
    RegisterDialogFunctions(L);
    RegisterDateFunctions(L);
    RegisterPlayerFunctions(L);
    RegisterHeroFunctions(L);
    RegisterMapFunctions(L);
    RegisterTownFunctions(L);
    RegisterBattleFunctions(L);
    RegisterCampaignFunctions(L);
    RegisterUncategorizedFunctions(L);
    RegisterBindings(L);
}

} // namespace ironfist::script
