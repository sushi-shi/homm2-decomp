from __future__ import annotations

import importlib.util
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


class CleanSourceCurrentEnumTests(unittest.TestCase):
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


class CleanSourceOutputSafetyTests(unittest.TestCase):
    def fixture_repo(self, root: Path) -> Path:
        repo = root / "repo"
        (repo / "include").mkdir(parents=True)
        (repo / "src").mkdir()
        (repo / "include/example.h").write_text("#pragma once\n")
        (repo / "src/example.cpp").write_text("int example;\n")
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
            ninja = (output / "build.ninja").read_text()
            self.assertIn("cxx = clang++", ninja)
            self.assertIn("--target=i686-w64-windows-gnu", ninja)
            self.assertIn(
                "build $builddir/obj/example.o: cxx src/example.cpp",
                ninja,
            )
            self.assertIn("build $builddir/HEROES2W.EXE: link", ninja)
            self.assertIn("default game", ninja)
            self.assertTrue((output / "imports/MSS32.def").is_file())

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
        git(repo, "add", "include", "src")
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
            with mock.patch(
                "subprocess.run",
                return_value=subprocess.CompletedProcess([], 0),
            ) as run:
                self.assertEqual(clean_source.verify(output), 0)
            run.assert_called_once_with(
                ("ninja", "-C", str(output), "game"),
                check=False,
            )


if __name__ == "__main__":
    unittest.main()
