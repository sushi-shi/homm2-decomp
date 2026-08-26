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
        hook = function_body("src/IRONFIST/hooks.cpp", "void Ironfist_NewMap(char* filename)")
        assert_order(self, hook, "Ironfist_ResetGameState();", "ScriptingInit(mapName);")

        game = function_body("src/SOURCE/GAME.cpp", "void game::NewMap(char* filename)")
        assert_order(
            self,
            game,
            "Ironfist_NewMap(filename);",
            "Ironfist_NewMapReady();",
            "Ironfist_DisallowBuilding",
        )

    def test_location_skip_preserves_normal_tail(self):
        body = function_body(
            "src/SOURCE/EVENTS.cpp", "void advManager::DoEvent(mapCell* cell, i32 x, i32 y)"
        )
        assert_order(
            self,
            body,
            "eventSample_f = NULL;",
            "if (Ironfist_LocationVisit(cell, x, y))",
            "goto event_done;",
            "event_done:",
            "UpdateRadar(1, 0);",
            "CheckEndGame(END_GAME_FORCE_NONE, false);",
        )

    def test_tooltip_override_precedes_visibility_and_artifact_fallback(self):
        body = function_body(
            "src/SOURCE/ADVMGR.cpp", "void advManager::QuickInfo(i32 cellX, i32 cellY)"
        )
        self.assertEqual(body.count("Ironfist_TooltipText("), 1)
        assert_order(
            self,
            body,
            "Ironfist_TooltipText(",
            "MAP_EXTRA_AT_WFIRST",
            "case MAP_OBJECT_ARTIFACT:",
        )
        override = body[body.index("if (Ironfist_TooltipText(") : body.index("MAP_EXTRA_AT_WFIRST")]
        self.assertIn("GUISetText(pWin, 1, tooltipOverride);", override)
        self.assertIn("return;", override)

    def test_script_info_text_precedes_none_fallback(self):
        body = function_body(
            "src/SOURCE/KB.cpp", "void game::ShowMoraleInfo(hero* h, i32 dialogType)"
        )
        assert_order(
            self,
            body,
            "Ironfist_AppendMoraleInfo(h);",
            "MORALE_INFO_NONE",
            "NormalDialog(",
        )

    def test_town_callback_precedes_post_callback_music_refresh(self):
        body = function_body("src/SOURCE/TOWNMGR.cpp", "i32 townManager::Open(i32 id)")
        callback = body.index("Ironfist_TownOpened(m_town);")
        music = body.rindex("gpSoundManager->SwitchAmbientMusic")
        self.assertLess(callback, music)

    def test_integrated_end_game_hook_remains_behind_new_game_guard(self):
        body = function_body(
            "src/SOURCE/KB.cpp",
            "void CheckEndGame(\n    CheckEndGameForcedResult forcedResult,\n    b32 dragonCityCaptured\n)",
        )
        assert_order(self, body, "if (gbInNewGameSetup)", "Ironfist_CheckEndGame();")

    def test_set_town_name_preserves_upstream_result(self):
        body = function_body("src/IRONFIST/funcs.cpp", "static i32 l_setTownName(lua_State* L)")
        assert_order(self, body, "CopyLuaName(", "return 1;")


if __name__ == "__main__":
    unittest.main()
