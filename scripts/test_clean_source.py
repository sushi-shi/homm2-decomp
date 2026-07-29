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


class CleanSourceCurrentEnumTests(unittest.TestCase):
    def test_output_has_one_final_newline(self):
        self.assertEqual(clean_source.clean("int value;\n\n"), "int value;\n")

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
                mock.patch.object(clean_source, "STRICT_ENUM_PATCHES", {}),
            ):
                sources, overrides, debris = clean_source.generate(output)
            self.assertEqual((sources, overrides, debris), (2, 0, []))
            self.assertFalse(stale.exists())
            self.assertTrue((output / "include/example.h").is_file())
            self.assertTrue((output / "src/example.cpp").is_file())


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
        (output / "include/example.h").write_text("#pragma once\n")
        (output / "src/example.cpp").write_text("int generated_value;\n")
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
            git(repo, "add", "src/example.cpp")
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
    def test_verify_requires_compatibility_headers(self):
        with self.assertRaises(SystemExit):
            clean_source.main(["--verify"])


if __name__ == "__main__":
    unittest.main()
