from __future__ import annotations

import contextlib
import importlib.util
import io
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock


SCRIPT = Path(__file__).with_name("clean_source.py")
SPEC = importlib.util.spec_from_file_location("clean_source", SCRIPT)
clean_source = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = clean_source
SPEC.loader.exec_module(clean_source)


def git(repo: Path, *args: str) -> str:
    return subprocess.run(
        ("git", *args),
        cwd=repo,
        check=True,
        capture_output=True,
        text=True,
    ).stdout.strip()


class CleanSourcePatchTests(unittest.TestCase):
    def test_patch_covering_fewer_sites_than_declared_fails_loudly(self):
        text = "case MAP_ONE + 1:\n" * 5
        with self.assertRaisesRegex(SystemExit, r"hit 5 site\(s\).*expected 52"):
            clean_source.apply_patches("src/SOURCE/X_CAMPGN.cpp", text)

    def test_patch_covering_more_sites_than_declared_fails_loudly(self):
        text = "case MAP_ONE + 1:\n" * 53
        with self.assertRaisesRegex(SystemExit, r"hit 53 site\(s\).*expected 52"):
            clean_source.apply_patches("src/SOURCE/X_CAMPGN.cpp", text)

    def test_campaign_cases_keep_the_enum_domain(self):
        text = "case MAP_ONE + 1:\n" * 52
        result = clean_source.apply_patches("src/SOURCE/X_CAMPGN.cpp", text)
        self.assertEqual(
            result.count(
                "case static_cast<ExpansionCampaignMap>("
                "H2EnumIndex(MAP_ONE) + 1):"
            ),
            52,
        )

    def test_gold_source_needs_only_the_remaining_audited_patches(self):
        self.assertEqual(
            set(clean_source.GENERATED_PATCHES),
            {
                "include/BASE/Misc.h",
                "src/BASE/INPUTMGR.cpp",
                "src/BASE/Misc.cpp",
                "src/BASE/TEXTWDGT.cpp",
                "src/BASE/Textntry.cpp",
                "src/BASE/droplist.cpp",
                "src/SOURCE/ADVMGR.cpp",
                "src/SOURCE/ARMY.cpp",
                "src/SOURCE/COMMAND.cpp",
                "src/SOURCE/EVENTS.cpp",
                "src/SOURCE/FINDPATH.cpp",
                "src/SOURCE/GAME.cpp",
                "src/SOURCE/HERO.cpp",
                "src/SOURCE/PHILAI.cpp",
                "src/SOURCE/X_CAMPGN.cpp",
                "vendor/audiere-1.9.2/audiere.h",
            },
        )

    def test_menu_handles_use_the_windows_type(self):
        header = (clean_source.REPO / "include/SOURCE/KB.h").read_text()
        source = (clean_source.REPO / "src/SOURCE/KB.cpp").read_text()
        self.assertIn("#include <windows.h>", header)
        for name in ("hmnuAdv", "hmnuCmbt", "hmnuDflt", "hmnuTown"):
            self.assertIn(f"extern HMENU {name};", header)
            self.assertIn(f"HMENU {name} = NULL;", source)

    def test_gold_text_widget_conversions_are_explicit(self):
        source = (
            "SetColorIndex(msg.payload.widget.data.value);\n"
            "(H2EnumIndex((m_flags) & (WIDGET_FLAG_DIMMED))) "
            "? FONT_DRAW_DIMMED : m_color,\n"
        )
        result = clean_source.apply_patches("src/BASE/TEXTWDGT.cpp", source)
        self.assertIn("static_cast<FontDrawMode>(msg.payload.widget.data.value)", result)
        self.assertIn(": static_cast<FontDrawMode>(m_color),", result)

    def test_audiere_declares_the_c_string_api_it_uses(self):
        source = "#include <vector>\n#include <string>\n"
        result = clean_source.apply_patches(
            "vendor/audiere-1.9.2/audiere.h",
            source,
        )
        self.assertIn("#include <string.h>", result)

    def test_gold_enum_zeroes_use_domain_values(self):
        input_source = (
            "(event.payload.keyboard.modifiers & MESSAGE_MODIFIER_CONTROL_KEYS) == 0"
        )
        self.assertIn(
            "== MESSAGE_MODIFIER_NONE",
            clean_source.apply_patches("src/BASE/INPUTMGR.cpp", input_source),
        )

        misc_source = (
            "u32l MAKEFILEID(char* text) {\n"
            "H2SteppedEnumStorage<DataEntryPhase, i32> bDataEntryTime = 0;\n"
        )
        self.assertIn(
            "bDataEntryTime = ENTRY_PHASE_IMMEDIATE;",
            clean_source.apply_patches("src/BASE/Misc.cpp", misc_source),
        )

    def test_makefileid_does_not_modify_literals(self):
        declaration = clean_source.apply_patches(
            "include/BASE/Misc.h",
            "u32l MAKEFILEID(char* text);",
        )
        self.assertEqual(declaration, "u32l MAKEFILEID(const char* text);")

        source = """u32l MAKEFILEID(char* text) {
    char buf[260];
    strcpy(buf, text);
    return buf[0];
}
H2SteppedEnumStorage<DataEntryPhase, i32> bDataEntryTime = 0;"""
        result = clean_source.apply_patches("src/BASE/Misc.cpp", source)
        self.assertIn("u32l MAKEFILEID(const char* text)", result)
        self.assertIn("strcpy(buf, text);", result)
        self.assertNotIn("text[i] &=", result)


class CleanSourceCurrentEnumTests(unittest.TestCase):
    def test_clean_override_expands_enum_index_at_the_call_site(self):
        text = (
            clean_source.OVERRIDE_DIR / "include/Ints.h"
        ).read_text()
        self.assertIn(
            "#define H2EnumIndex(value) static_cast<i32>(value)",
            text,
        )
        self.assertNotIn("constexpr i32 H2EnumIndex(", text)

    def test_verify_rejects_enum_index_symbol(self):
        built = subprocess.CompletedProcess((), 0)
        symbols = subprocess.CompletedProcess(
            (), 0, stdout="00000000 T H2EnumIndex<int>(int)\n"
        )
        with (
            mock.patch("subprocess.run", side_effect=(built, symbols)),
            contextlib.redirect_stderr(io.StringIO()),
        ):
            self.assertEqual(clean_source.verify(Path("generated")), 1)

    def test_output_has_one_final_newline(self):
        self.assertEqual(clean_source.clean("int value;\n\n"), "int value;\n")

    def test_comments_are_removed_without_joining_tokens(self):
        text = 'int/* note */value; // tail\nconst char* text = "//";\n'
        self.assertEqual(
            clean_source.clean(text),
            'int value;\nconst char* text = "//";\n',
        )

    def test_windows_intrinsic_collisions_are_renamed(self):
        self.assertEqual(
            clean_source.clean("BitTest(bits, index);\n"),
            "H2BitTest(bits, index);\n",
        )

    def test_windows_calling_conventions_are_preserved(self):
        self.assertEqual(
            clean_source.clean("void __stdcall callback();\n"),
            "void __stdcall callback();\n",
        )

    def test_multiline_comment_preserves_line_breaks(self):
        self.assertEqual(
            clean_source.strip_comments("left/* one\ntwo */right"),
            "left \nright",
        )

    def test_current_strict_condition_selects_typed_arm(self):
        text = (
            "#if H2_STRICT_ENUMS\n"
            "typed\n"
            "#else\n"
            "retail\n"
            "#endif\n"
            "#if !H2_STRICT_ENUMS\n"
            "retail\n"
            "#endif\n"
        )
        self.assertEqual(clean_source.resolve_strict_conditionals(text), "typed\n")

    def test_clear_flag_expands_to_typed_bit_removal(self):
        self.assertEqual(
            clean_source.rewrite("H2_ENUM_CLEAR_FLAG(flags, FLAG_A)"),
            "((flags) &= ~(FLAG_A))",
        )

    def test_flag_operators_stay_compact(self):
        self.assertEqual(
            clean_source.rewrite("H2_ENUM_FLAGS(MessageType)"),
            "ENABLE_ENUM_FLAGS(MessageType)",
        )

    def test_step_operators_stay_compact(self):
        self.assertEqual(
            clean_source.rewrite("H2_ENUM_STEPPED(CreatureType)"),
            "ENABLE_ENUM_STEPS(CreatureType)",
        )

    def test_size_annotations_are_removed(self):
        self.assertEqual(
            clean_source.clean(
                "struct Record { int value; };\n"
                'SIZE(Record, 4);\n'
            ),
            "struct Record { int value; };\n",
        )

    def test_decode_masked_expands_to_typed_assignment(self):
        result = clean_source.rewrite(
            "H2_ENUM_DECODE_MASKED(Kind, value, packed.field, KIND_MASK)"
        )
        self.assertIn(
            "(value) = static_cast<Kind>((packed.field) & (KIND_MASK));",
            result,
        )

    def test_residue_rejects_unknown_enum_scaffolding(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            (output / "unknown.h").write_text("H2_ENUM_FUTURE(Value)\n")
            self.assertEqual(
                clean_source.residue(output),
                {"H2_ENUM_FUTURE": 1},
            )

    def test_residue_rejects_comments(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            (output / "comment.h").write_text("int value; // note\n")
            self.assertEqual(clean_source.residue(output), {"comment": 1})


class ClassicSourceTests(unittest.TestCase):
    DOMAINS = {
        "CombatSide": "i32",
        "PackedKind": "u8",
    }

    def test_classic_domains_follow_retail_macro_storage(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "include").mkdir()
            (root / "src").mkdir()
            (root / "include/types.h").write_text(
                "H2_ENUM_CLASS_BEGIN(CombatSide)\n"
                "H2_ENUM_CLASS_END(CombatSide)\n"
                "H2_ENUM_CLASS_BEGIN_T(PackedKind, u8)\n"
                "H2_ENUM_CLASS_END_T(PackedKind, u8)\n"
                "H2_ENUM_CLASS_BEGIN_SPLIT(SplitKind, i8)\n"
                "H2_ENUM_CLASS_END_SPLIT(SplitKind, i8)\n"
            )
            self.assertEqual(
                clean_source.classic_domains(root),
                {
                    "CombatSide": "i32",
                    "PackedKind": "u8",
                    "SplitKind": "i32",
                },
            )

    def test_classicize_demotes_generated_enum_domains(self):
        source = (
            "enum class CombatSide : i32 {\n"
            "    COMBAT_ATTACKER,\n"
            "    COMBAT_DEFENDER\n"
            "};\n"
            "using enum CombatSide;\n"
            "ENABLE_ENUM_STEPS(CombatSide)\n"
            "inline CombatSide& operator^=(CombatSide& side, i32 mask) {\n"
            "    return side;\n"
            "}\n"
        )
        self.assertEqual(
            clean_source.classicize(source, self.DOMAINS),
            "enum {\n"
            "    COMBAT_ATTACKER,\n"
            "    COMBAT_DEFENDER\n"
            "};\n"
            "typedef i32 CombatSide;\n",
        )

    def test_classicize_demotes_new_platform_enum_classes(self):
        source = (
            "enum class HostKey { Escape, Enter };\n"
            "HostKey key = HostKey::Escape;\n"
        )
        self.assertEqual(
            clean_source.classicize(source, self.DOMAINS),
            "enum { HostKey_Escape, HostKey_Enter };\n"
            "typedef int HostKey;\n"
            "HostKey key = HostKey_Escape;\n",
        )

    def test_classicize_qualifies_nested_enum_alias_constants(self):
        enums = {"Type": ("int", ("None", "KeyDown"))}
        source = (
            "using Type = platform::Event::Type;\n"
            "return Type::KeyDown;\n"
        )
        self.assertEqual(
            clean_source.classicize(
                source,
                self.DOMAINS,
                modern_enums=enums,
            ),
            "using Type = platform::Event::Type;\n"
            "return platform::Event::Type_KeyDown;\n",
        )

    def test_classicize_qualifies_imported_enum_constants(self):
        enums = {"Key": ("int", ("Escape",))}
        source = "using platform::Key;\nreturn Key::Escape;\n"
        self.assertEqual(
            clean_source.classicize(
                source,
                self.DOMAINS,
                modern_enums=enums,
            ),
            "using platform::Key;\nreturn platform::Key_Escape;\n",
        )

    def test_classicize_replaces_forward_storage_and_index_helpers(self):
        source = (
            "enum class CombatSide : i32;\n"
            "H2EnumStorage<CombatSide, i16> side;\n"
            "H2SteppedEnumStorage<CombatSide, i8> phase;\n"
            "i32 value = H2EnumIndex(side) + H2EnumIndex(COMBAT_ATTACKER);\n"
            "i32 menu = H2EnumIndex(platform::SystemMenuCommand::Help);\n"
        )
        self.assertEqual(
            clean_source.classicize(source, self.DOMAINS),
            "typedef i32 CombatSide;\n"
            "i16 side;\n"
            "i8 phase;\n"
            "i32 value = (side) + (COMBAT_ATTACKER);\n"
            "i32 menu = (platform::SystemMenuCommand::Help);\n",
        )

    def test_classic_ints_drops_strict_support(self):
        source = (
            "#ifndef HOMM2_INTS_H\n"
            "#define HOMM2_INTS_H\n"
            "#include <type_traits>\n"
            "typedef int i32;\n"
            "template <typename T>\n"
            "inline constexpr bool H2IsMaskLike = true;\n"
            "#define H2EnumIndex(value) value\n"
            "#endif\n"
        )
        result = clean_source.classicize(source, self.DOMAINS, "include/Ints.h")
        self.assertEqual(
            result,
            "#ifndef HOMM2_INTS_H\n"
            "#define HOMM2_INTS_H\n"
            "typedef int i32;\n"
            "#endif\n",
        )

    def test_cp1251_literals_are_readable_without_changing_bytes(self):
        source = (
            'const char* text = "\\xcf\\xf0\\xe8\\xe2\\xe5\\xf2";\n'
            "char yo = '\\xa8';\n"
        )
        transformed, count = clean_source.materialize_cp1251_literals(source)
        encoded = transformed.encode("cp1251")
        self.assertEqual(count, 7)
        self.assertIn('"Привет"'.encode("cp1251"), encoded)
        self.assertIn("'Ё'".encode("cp1251"), encoded)
        self.assertNotIn(b"\\xcf", encoded)

    def test_cp1251_literal_transform_preserves_escape_meaning(self):
        source = (
            'const char* escaped = "\\\\xcf";\n'
            'const char* long_hex = "\\xcf0";\n'
            'const char* ascii = "\\x41";\n'
            '// "\\xcf"\n'
        )
        transformed, count = clean_source.materialize_cp1251_literals(source)
        self.assertEqual((transformed, count), (source, 0))

    def test_cp1251_tree_verification_rejects_utf8(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            (output / "src").mkdir()
            source = output / "src/example.cpp"
            source.write_text('const char* text = "Привет";\n', encoding="utf-8")
            with self.assertRaisesRegex(SystemExit, "non-UTF8 CP1251"):
                clean_source.verify_cp1251_tree(output)
            source.write_bytes('const char* text = "Привет";\n'.encode("cp1251"))
            cyrillic, non_utf8_files = clean_source.verify_cp1251_tree(output)
            self.assertEqual((cyrillic, non_utf8_files), (6, 1))


class CleanSourceOutputSafetyTests(unittest.TestCase):
    def fixture_repo(self, root: Path) -> Path:
        repo = root / "repo"
        (repo / "include").mkdir(parents=True)
        (repo / "src").mkdir()
        (repo / "vendor/example").mkdir(parents=True)
        (repo / "scripts/homm2/clean/project").mkdir(parents=True)
        (repo / "include/example.h").write_text("#pragma once\n")
        (repo / "src/example.cpp").write_text("int example;\n")
        (repo / "vendor/example/license.txt").write_text("keep vendor license\n")
        (repo / "scripts/homm2/clean/project/flake.lock").write_text("{}\n")
        (repo / "scripts/homm2/clean/project/flake.nix").write_text(
            "{ outputs = _: {}; }\n"
        )
        (repo / "scripts/homm2/clean/project/.gitignore").write_text(
            "/build/\n/result\n"
        )
        (repo / "scripts/homm2/clean/project/README.md").write_text(
            "# Generated source\n"
        )
        (repo / "scripts/homm2/clean/project/run-game.sh").write_text("#!/bin/sh\n")
        return repo

    def test_generate_refuses_source_owned_output_without_deleting_it(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo = self.fixture_repo(root)
            output = repo / "src/generated"
            output.mkdir()
            keep = output / "keep.txt"
            keep.write_text("keep")
            with (
                mock.patch.object(clean_source, "REPO", repo),
                mock.patch.object(clean_source, "OVERRIDE_DIR", repo / "overrides"),
                self.assertRaisesRegex(SystemExit, "below build"),
            ):
                clean_source.generate(output)
            self.assertEqual(keep.read_text(), "keep")

    def test_generate_refuses_unmarked_external_directory_without_deleting_it(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo = self.fixture_repo(root)
            output = root / "existing"
            output.mkdir()
            keep = output / "keep.txt"
            keep.write_text("keep")
            with (
                mock.patch.object(clean_source, "REPO", repo),
                mock.patch.object(clean_source, "OVERRIDE_DIR", repo / "overrides"),
                self.assertRaisesRegex(SystemExit, "unmarked output"),
            ):
                clean_source.generate(output)
            self.assertEqual(keep.read_text(), "keep")

    def test_generate_refuses_unmarked_build_child_without_deleting_it(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo = self.fixture_repo(root)
            output = repo / "build/unrelated"
            output.mkdir(parents=True)
            keep = output / "keep.txt"
            keep.write_text("keep")
            with (
                mock.patch.object(clean_source, "REPO", repo),
                mock.patch.object(clean_source, "OVERRIDE_DIR", repo / "overrides"),
                self.assertRaisesRegex(SystemExit, "unmarked output"),
            ):
                clean_source.generate(output)
            self.assertEqual(keep.read_text(), "keep")

    def test_generate_replaces_its_own_marked_external_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo = self.fixture_repo(root)
            output = root / "existing"
            output.mkdir()
            (output / clean_source.GENERATED_MARKER).write_text(
                clean_source.GENERATED_MARKER_CONTENT
            )
            stale = output / "stale.txt"
            stale.write_text("stale")
            with (
                mock.patch.object(clean_source, "REPO", repo),
                mock.patch.object(clean_source, "OVERRIDE_DIR", repo / "overrides"),
                mock.patch.object(clean_source, "GENERATED_PATCHES", {}),
            ):
                sources, overrides, debris = clean_source.generate(output)
            self.assertEqual((sources, overrides, debris), (3, 0, []))
            self.assertFalse(stale.exists())
            self.assertTrue((output / "include/example.h").is_file())
            self.assertTrue((output / "src/example.cpp").is_file())
            self.assertEqual(
                (output / "vendor/example/license.txt").read_text(),
                "keep vendor license\n",
            )
            self.assertEqual((output / "flake.lock").read_text(), "{}\n")
            self.assertIn("outputs", (output / "flake.nix").read_text())
            self.assertIn("/result", (output / ".gitignore").read_text())
            self.assertEqual((output / "README.md").read_text(), "# Generated source\n")
            self.assertTrue((output / "run-game.sh").stat().st_mode & 0o111)
            ninja = (output / "build.ninja").read_text()
            self.assertIn("cxx = clang++", ninja)
            self.assertIn("--target=i686-w64-windows-gnu", ninja)
            self.assertIn("-Ivendor/audiere-1.9.2", ninja)
            self.assertIn("-O0", ninja)
            self.assertIn("-mno-sse -mno-sse2 -mfpmath=387", ninja)
            self.assertIn(
                "build $builddir/obj/example.o: cxx src/example.cpp",
                ninja,
            )
            self.assertIn("build $builddir/HMM2PL.exe: link", ninja)
            self.assertIn("default game", ninja)
            self.assertIn(
                "build $builddir/imports/MSS32.a: implib imports/MSS32.def",
                ninja,
            )
            self.assertIn(
                "build $builddir/imports/MSS32_aliases.o: cxx "
                "imports/MSS32_aliases.S",
                ninja,
            )
            self.assertIn(
                "build $builddir/imports/AUDIERE.a: implib imports/AUDIERE.def",
                ninja,
            )
            self.assertIn(
                "build $builddir/imports/AUDIERE_aliases.o: cxx "
                "imports/AUDIERE_aliases.S",
                ninja,
            )
            self.assertIn(
                "build $builddir/imports/WING32.a: implib imports/WING32.def",
                ninja,
            )
            self.assertIn("  dlltool_flags = -k", ninja)

            mss = (output / "imports/MSS32.def").read_text()
            self.assertIn("  _AIL_startup@0\n", mss)
            self.assertIn("  _AIL_digital_master_volume@4\n", mss)
            self.assertIn("  _AIL_set_digital_master_volume@8\n", mss)
            self.assertNotIn("AIL_startup@0 =", mss)

            mss_aliases = (output / "imports/MSS32_aliases.S").read_text()
            self.assertIn(
                '    .set "_AIL_startup@0", "__AIL_startup@0"\n',
                mss_aliases,
            )
            self.assertIn(
                '    .set "__imp__AIL_startup@0", "__imp___AIL_startup@0"\n',
                mss_aliases,
            )

            wing = (output / "imports/WING32.def").read_text()
            self.assertIn("  WinGCreateDC@0\n", wing)
            self.assertNotIn("WinGCreateDC@0 =", wing)

            audiere = (output / "imports/AUDIERE.def").read_text()
            self.assertIn("LIBRARY audiere.dll\n", audiere)
            self.assertIn("  _AdrOpenDevice@8\n", audiere)
            self.assertIn("  _AdrOpenSampleSource@4\n", audiere)

            audiere_aliases = (output / "imports/AUDIERE_aliases.S").read_text()
            self.assertIn(
                '    .set "_AdrOpenDevice@8", "__AdrOpenDevice@8"\n',
                audiere_aliases,
            )
            self.assertIn(
                '    .set "__imp__AdrOpenDevice@8", '
                '"__imp___AdrOpenDevice@8"\n',
                audiere_aliases,
            )

    def test_override_comments_are_removed(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo = self.fixture_repo(root)
            overrides = repo / "overrides/include"
            overrides.mkdir(parents=True)
            (overrides / "example.h").write_text("int value; // note\n")
            output = repo / "build/clean"
            with (
                mock.patch.object(clean_source, "REPO", repo),
                mock.patch.object(clean_source, "OVERRIDE_DIR", repo / "overrides"),
                mock.patch.object(clean_source, "GENERATED_PATCHES", {}),
            ):
                clean_source.generate(output)
            self.assertEqual(
                (output / "include/example.h").read_text(),
                "int value;\n",
            )


class CleanSourcePublishSafetyTests(unittest.TestCase):
    def fixture_repo(self, root: Path) -> tuple[Path, Path]:
        repo = root / "repo"
        repo.mkdir()
        git(repo, "init", "-b", "main")
        git(repo, "config", "user.name", "clean-source-test")
        git(repo, "config", "user.email", "clean-source@test.invalid")
        (repo / "include").mkdir()
        (repo / "src").mkdir()
        (repo / "include/example.h").write_text("#pragma once\n")
        (repo / "src/example.cpp").write_text("int source_value;\n")
        (repo / "decomp-only.txt").write_text("matching tools\n")
        git(repo, "add", "include", "src", "decomp-only.txt")
        git(repo, "commit", "-m", "source")

        output = root / "generated"
        output.mkdir()
        (output / clean_source.GENERATED_MARKER).write_text(
            clean_source.GENERATED_MARKER_CONTENT
        )
        (output / "include").mkdir()
        (output / "src").mkdir()
        (output / "imports").mkdir()
        (output / "include/example.h").write_text("#pragma once\n")
        (output / "src/example.cpp").write_text("int generated_value;\n")
        (output / "imports/example.def").write_text("EXPORTS\n")
        (output / "build.ninja").write_text("build objects: phony\n")
        (output / "README.md").write_text("# Generated source\n")
        return repo, output

    def test_publish_refuses_a_non_generated_branch(self):
        with tempfile.TemporaryDirectory() as directory:
            repo, output = self.fixture_repo(Path(directory))
            before = git(repo, "rev-parse", "main")
            with (
                mock.patch.object(clean_source, "REPO", repo),
                self.assertRaisesRegex(SystemExit, "non-generated branch"),
            ):
                clean_source.publish(output, "main")
            self.assertEqual(git(repo, "rev-parse", "main"), before)
            self.assertEqual(git(repo, "status", "--porcelain"), "")

    def test_new_generated_branch_descends_from_source(self):
        with tempfile.TemporaryDirectory() as directory:
            repo, output = self.fixture_repo(Path(directory))
            source = git(repo, "rev-parse", "main")

            with mock.patch.object(clean_source, "REPO", repo):
                clean_source.publish(output, "clean")

            self.assertEqual(git(repo, "rev-parse", "clean^"), source)
            self.assertNotIn(
                "decomp-only.txt",
                git(repo, "ls-tree", "--name-only", "clean"),
            )
            self.assertEqual(
                git(repo, "show", "clean:src/example.cpp"),
                "int generated_value;",
            )
            self.assertEqual(
                git(repo, "show", "clean:README.md"),
                "# Generated source",
            )

    def test_new_generated_branch_can_use_an_explicit_parent(self):
        with tempfile.TemporaryDirectory() as directory:
            repo, output = self.fixture_repo(Path(directory))
            parent = git(repo, "rev-parse", "HEAD")
            (repo / "decomp-only.txt").write_text("advanced matching tree\n")
            git(repo, "add", "decomp-only.txt")
            git(repo, "commit", "-m", "advance source")
            source = git(repo, "rev-parse", "HEAD")

            with mock.patch.object(clean_source, "REPO", repo):
                clean_source.publish(
                    output,
                    "classic",
                    full_tree=True,
                    publish_parent=parent,
                )

            self.assertEqual(git(repo, "rev-parse", "classic^"), parent)
            message = git(repo, "log", "-1", "--format=%B", "classic")
            self.assertIn(f"Source-Commit: {source}", message)
            self.assertIn(f"Publish-Parent: {parent}", message)

    def test_existing_generated_branch_absorbs_readme_metadata_commit(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo, output = self.fixture_repo(root)
            with mock.patch.object(clean_source, "REPO", repo):
                clean_source.publish(output, "clean")

            checkout = root / "clean-checkout"
            git(repo, "worktree", "add", str(checkout), "clean")
            (checkout / "README.md").write_text("# Historical status refresh\n")
            git(checkout, "add", "README.md")
            git(checkout, "commit", "-m", "docs: refresh generated status")

            with mock.patch.object(clean_source, "REPO", repo):
                clean_source.publish(output, "clean")

            self.assertEqual(
                git(repo, "show", "clean:README.md"),
                "# Generated source",
            )
            self.assertIn(
                "Generated by scripts/clean_source.py.",
                git(repo, "log", "-1", "--format=%B", "clean"),
            )

    def test_existing_generated_branch_merges_new_source_history(self):
        with tempfile.TemporaryDirectory() as directory:
            repo, output = self.fixture_repo(Path(directory))

            with mock.patch.object(clean_source, "REPO", repo):
                clean_source.publish(output, "clean")
            old_clean = git(repo, "rev-parse", "clean")

            (repo / "decomp-only.txt").write_text("new matching state\n")
            git(repo, "add", "decomp-only.txt")
            git(repo, "commit", "-m", "advance source")
            source = git(repo, "rev-parse", "main")

            with mock.patch.object(clean_source, "REPO", repo):
                clean_source.publish(output, "clean")

            parents = git(repo, "rev-list", "--parents", "-n", "1", "clean").split()
            self.assertEqual(parents[1:], [old_clean, source])
            git(repo, "merge-base", "--is-ancestor", "main", "clean")

    def test_publish_refuses_and_preserves_a_dirty_generated_worktree(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo, output = self.fixture_repo(root)
            git(repo, "branch", "clean")
            git(repo, "switch", "clean")
            git(
                repo,
                "commit",
                "--allow-empty",
                "-m",
                "clean: seed\n\nGenerated by scripts/clean_source.py.\n\n"
                "Source-Commit: seed",
            )
            git(repo, "switch", "main")
            checkout = root / "clean-checkout"
            git(repo, "worktree", "add", str(checkout), "clean")
            dirty = checkout / "src/example.cpp"
            dirty.write_text("int user_edit;\n")

            with (
                mock.patch.object(clean_source, "REPO", repo),
                self.assertRaisesRegex(SystemExit, "dirty generated worktree"),
            ):
                clean_source.publish(output, "clean")
            self.assertEqual(dirty.read_text(), "int user_edit;\n")
            self.assertIn("src/example.cpp", git(checkout, "status", "--porcelain"))

    def test_publish_records_unchanged_output_without_rewriting_history(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            repo, output = self.fixture_repo(root)
            git(repo, "branch", "clean")
            git(repo, "switch", "clean")
            (repo / "src/example.cpp").write_text("int generated_value;\n")
            (repo / "build.ninja").write_text("build objects: phony\n")
            git(repo, "add", "build.ninja", "src/example.cpp")
            git(
                repo,
                "commit",
                "-m",
                "clean: seed\n\nGenerated by scripts/clean_source.py.\n\n"
                "Source-Commit: older",
            )
            old_clean = git(repo, "rev-parse", "HEAD")
            git(repo, "switch", "main")

            with mock.patch.object(clean_source, "REPO", repo):
                clean_source.publish(output, "clean")

            new_clean = git(repo, "rev-parse", "clean")
            self.assertNotEqual(new_clean, old_clean)
            self.assertEqual(git(repo, "rev-parse", "clean^"), old_clean)


class CleanSourceVerifyTests(unittest.TestCase):
    def test_verify_runs_generated_ninja_build(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            built = subprocess.CompletedProcess([], 0)
            symbols = subprocess.CompletedProcess([], 0, stdout="")
            with mock.patch(
                "subprocess.run",
                side_effect=(built, symbols),
            ) as run:
                self.assertEqual(clean_source.verify(output), 0)
            self.assertEqual(
                run.call_args_list,
                [
                    mock.call(
                        ("ninja", "-C", str(output), "game"),
                        check=False,
                    ),
                    mock.call(
                        ("llvm-nm", "-C", str(output / "build/HMM2PL.exe")),
                        check=False,
                        capture_output=True,
                        text=True,
                    ),
                ],
            )


if __name__ == "__main__":
    unittest.main()
