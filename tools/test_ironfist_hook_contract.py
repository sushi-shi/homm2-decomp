#!/usr/bin/env python3

import pathlib
import unittest


REPOSITORY = pathlib.Path(__file__).resolve().parents[1]


def function_body(relative_path, signature):
    source = (REPOSITORY / relative_path).read_text()
    start = source.index(signature)
    opening = source.index("{", start)
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[opening + 1 : index]
    raise AssertionError(f"unterminated function {signature} in {relative_path}")


def assert_order(test, body, *tokens):
    positions = [body.index(token) for token in tokens]
    test.assertEqual(positions, sorted(positions), tokens)


class IronfistHookContractTest(unittest.TestCase):
    def test_new_map_reset_and_post_body_default_order(self):
        runtime = function_body("src/IRONFIST/runtime.cpp", "void BeginMap(const char* filename)")
        assert_order(self, runtime, "ResetAdventureState();", "script::InitializeMap(mapName);")

        game = function_body("src/SOURCE/GAME.cpp", "void game::NewMap(char* filename)")
        assert_order(
            self,
            game,
            "ironfist::runtime::BeginMap(filename);",
            "ironfist::runtime::FinishMap();",
            ".DisallowBuilding(",
        )

    def test_location_skip_preserves_normal_tail(self):
        body = function_body(
            "src/SOURCE/EVENTS.cpp", "void advManager::DoEvent(mapCell* cell, i32 x, i32 y)"
        )
        assert_order(
            self,
            body,
            "eventSample_f = NULL;",
            "if (ironfist::hooks::LocationVisit(cell, x, y))",
            "goto event_done;",
            "event_done:",
            "UpdateRadar(1, 0);",
            "CheckEndGame(END_GAME_FORCE_NONE, false);",
        )

    def test_tooltip_override_precedes_visibility_and_artifact_fallback(self):
        body = function_body(
            "src/SOURCE/ADVMGR.cpp", "void advManager::QuickInfo(i32 cellX, i32 cellY)"
        )
        self.assertEqual(body.count("ironfist::hooks::OverrideTooltip("), 1)
        assert_order(
            self,
            body,
            "ironfist::hooks::OverrideTooltip(",
            "MAP_EXTRA_AT_WFIRST",
            "case MAP_OBJECT_ARTIFACT:",
        )
        override = body[
            body.index("if (ironfist::hooks::OverrideTooltip(") : body.index("MAP_EXTRA_AT_WFIRST")
        ]
        self.assertIn("GUISetText(pWin, 1, tooltipOverride);", override)
        self.assertIn("return;", override)

    def test_script_info_text_precedes_none_fallback(self):
        body = function_body(
            "src/SOURCE/KB.cpp", "void game::ShowMoraleInfo(hero* h, i32 dialogType)"
        )
        assert_order(
            self,
            body,
            "ironfist::hooks::AppendMoraleInfo(h);",
            "MORALE_INFO_NONE",
            "NormalDialog(",
        )

    def test_town_callback_precedes_post_callback_music_refresh(self):
        body = function_body("src/SOURCE/TOWNMGR.cpp", "i32 townManager::Open(i32 id)")
        callback = body.index("ironfist::hooks::TownOpened(m_town);")
        music = body.rindex("gpSoundManager->SwitchAmbientMusic")
        self.assertLess(callback, music)

    def test_integrated_end_game_hook_remains_behind_new_game_guard(self):
        body = function_body(
            "src/SOURCE/KB.cpp",
            "void CheckEndGame(\n    CheckEndGameForcedResult forcedResult,\n    b32 dragonCityCaptured\n)",
        )
        assert_order(self, body, "if (gbInNewGameSetup)", "ironfist::hooks::CheckEndGame();")

    def test_set_town_name_preserves_upstream_result(self):
        body = function_body("src/IRONFIST/funcs.cpp", "static i32 l_setTownName(lua_State* L)")
        assert_order(self, body, "CopyLuaName(", "return 1;")

    def test_hook_layer_contains_callbacks_only(self):
        hooks = (REPOSITORY / "src/IRONFIST/hooks.cpp").read_text()
        for mechanic in (
            "ShareVision",
            "CancelVisionShare",
            "DisallowBuilding",
            "AIArmySharing",
            "ForceHeroChase",
            "PandoraBox",
            "WellGrowth",
            "HeroPool",
        ):
            self.assertNotIn(mechanic, hooks)

    def test_game_mechanics_are_owned_by_recovered_classes(self):
        game = (REPOSITORY / "src/SOURCE/GAME.cpp").read_text()
        town = (REPOSITORY / "src/SOURCE/TOWN.cpp").read_text()
        combat = function_body(
            "src/SOURCE/CMBTMGR.cpp", "void combatManager::InitNonVisualVars(void)"
        )

        self.assertIn("void game::ShareVision(", game)
        self.assertIn("void game::CancelVisionShare(", game)
        self.assertIn("void game::SetAIArmySharing(", game)
        self.assertIn("void game::ForceHeroChase(", game)
        self.assertIn("void town::DisallowBuilding(", town)
        share = function_body(
            "src/SOURCE/GAME.cpp",
            "void game::ShareVision(i32 sourcePlayer, i32 destinationPlayer)",
        )
        assert_order(
            self,
            share,
            "sharePlayerVision[sourcePlayer][destinationPlayer] = true;",
            "for (i32 source = 0; source < GAME_PLAYER_COUNT; ++source)",
            "for (i32 destination = 0; destination < GAME_PLAYER_COUNT; ++destination)",
            "for (i32 cell = 0; cell < MAP_WIDTH * MAP_HEIGHT; ++cell)",
        )
        assert_order(
            self,
            combat,
            "ironfist::hooks::BattleStarted();",
            "ApplyPandoraBox(COMBAT_ATTACKER_SIDE);",
            "ApplyPandoraBox(COMBAT_DEFENDER_SIDE);",
        )

    def test_disabled_well_preserves_post_scaling_behavior(self):
        body = function_body("src/SOURCE/GAME.cpp", "void game::PerWeek(void)")
        assert_order(
            self,
            body,
            "growth2 += CASTLE_GROWTH_SPECIAL_BONUS;",
            "WEEKLY_HARD_GROWTH_FACTOR",
            "if (ironfist::IsWellDisabled()",
            "growth2 -= castle5->m_owner >= 0",
            "m_garrison[innerIndex - WEEKLY_FIRST_DWELLING] += growth2;",
        )

    def test_source_uses_explicit_integration_namespace(self):
        for source in (REPOSITORY / "src/SOURCE").glob("*.cpp"):
            self.assertNotIn("using namespace ironfist", source.read_text(), source)

    def test_state_and_map_variables_have_owned_storage(self):
        state = (REPOSITORY / "include/IRONFIST/state.h").read_text()
        scripting = (REPOSITORY / "include/IRONFIST/scripting.h").read_text()
        self.assertIn("State& Get();", state)
        self.assertNotIn("gIronfistExtra", state)
        self.assertIn("std::string value;", scripting)
        self.assertIn("LuaTable table;", scripting)
        self.assertNotIn("std::string*", scripting)
        self.assertNotIn("LuaTable*", scripting)


if __name__ == "__main__":
    unittest.main()
