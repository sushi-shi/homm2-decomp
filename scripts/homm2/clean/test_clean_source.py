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

    def test_current_strict_enum_conversions(self):
        cases = (
            ("src/SOURCE/ARMY.cpp", "targetSide_8 >= 0", "H2EnumIndex(targetSide_8) >= 0"),
            ("src/SOURCE/PHILAI.cpp", "best.type >= 0", "H2EnumIndex(best.type) >= 0"),
        )
        for relative, source, expected in cases:
            with self.subTest(relative=relative):
                self.assertEqual(
                    clean_source.apply_patches(relative, source),
                    expected,
                )

    def test_menu_handles_use_the_windows_type(self):
        source = "#include <Ints.h>\n" + "\n".join(
            f"extern void* {name};"
            for name in ("hmnuAdv", "hmnuCmbt", "hmnuDflt", "hmnuTown")
        )
        result = clean_source.apply_patches("include/SOURCE/KB.h", source)
        self.assertNotIn("void*", result)
        self.assertEqual(result.count("extern HMENU"), 4)

    def test_makefileid_does_not_modify_literals(self):
        declaration = clean_source.apply_patches(
            "include/BASE/Misc.h",
            "u32l MAKEFILEID(char* text);",
        )
        self.assertEqual(declaration, "u32l MAKEFILEID(const char* text);")

        source = """u32l MAKEFILEID(char* text) {
    u32 hash = 0;
    i32 sum = 0;
    for (i32 i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] &= ~('a' - 'A');
        }
        u32 shiftedHash = hash << HASH_LEFT_SHIFT;
        hash >>= HASH_RIGHT_SHIFT;
        hash += shiftedHash;
        sum += text[i];
        hash += text[i] + sum;
    }
    return hash;
}"""
        result = clean_source.apply_patches("src/BASE/Misc.cpp", source)
        self.assertIn("u32l MAKEFILEID(const char* text)", result)
        self.assertIn("char value = text[i];", result)
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


class CleanSourceOutputSafetyTests(unittest.TestCase):
    def fixture_repo(self, root: Path) -> Path:
        repo = root / "repo"
        (repo / "include").mkdir(parents=True)
        (repo / "src").mkdir()
        (repo / "scripts/homm2/clean/project").mkdir(parents=True)
        (repo / "include/example.h").write_text("#pragma once\n")
        (repo / "src/example.cpp").write_text("int example;\n")
        (repo / "scripts/homm2/clean/project/flake.lock").write_text("{}\n")
        (repo / "scripts/homm2/clean/project/flake.nix").write_text(
            "{ outputs = _: {}; }\n"
        )
        (repo / "scripts/homm2/clean/project/.gitignore").write_text(
            "/build/\n/result\n"
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
            self.assertEqual((sources, overrides, debris), (2, 0, []))
            self.assertFalse(stale.exists())
            self.assertTrue((output / "include/example.h").is_file())
            self.assertTrue((output / "src/example.cpp").is_file())
            self.assertEqual((output / "flake.lock").read_text(), "{}\n")
            self.assertIn("outputs", (output / "flake.nix").read_text())
            self.assertIn("/result", (output / ".gitignore").read_text())
            self.assertTrue((output / "run-game.sh").stat().st_mode & 0o111)
            ninja = (output / "build.ninja").read_text()
            self.assertIn("cxx = clang++", ninja)
            self.assertIn("--target=i686-w64-windows-gnu", ninja)
            self.assertIn("-O0", ninja)
            self.assertIn("-mno-sse -mno-sse2 -mfpmath=387", ninja)
            self.assertIn(
                "build $builddir/obj/example.o: cxx src/example.cpp",
                ninja,
            )
            self.assertIn("build $builddir/HEROES2W.EXE: link", ninja)
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
                "build $builddir/imports/WING32.a: implib imports/WING32.def",
                ninja,
            )
            self.assertIn("  dlltool_flags = -k", ninja)

            mss = (output / "imports/MSS32.def").read_text()
            self.assertIn("  _AIL_startup@0\n", mss)
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
                        ("llvm-nm", "-C", str(output / "build/HEROES2W.EXE")),
                        check=False,
                        capture_output=True,
                        text=True,
                    ),
                ],
            )


if __name__ == "__main__":
    unittest.main()
