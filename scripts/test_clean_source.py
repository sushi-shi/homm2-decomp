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
        ("git",) + args,
        cwd=repo,
        check=True,
        capture_output=True,
        text=True,
    ).stdout.strip()


class CleanSourcePatchTests(unittest.TestCase):
    def test_hero_integer_materialization_returns_to_typed_flag_subtraction(self):
        text = (
            "m_eventFlags = static_cast<i32>(m_eventFlags) - HERO_EVENT_GRAVEYARD;\n"
            "m_eventFlags = static_cast<i32>(m_eventFlags) - HERO_EVENT_SHIPWRECK;\n"
        )
        result = clean_source.apply_patches("src/SOURCE/HERO.cpp", text)
        self.assertEqual(
            result,
            "m_eventFlags = m_eventFlags - HERO_EVENT_GRAVEYARD;\n"
            "m_eventFlags = m_eventFlags - HERO_EVENT_SHIPWRECK;\n",
        )

    def test_overview_frame_arithmetic_uses_typed_index(self):
        text = (
            "static_cast<i16>(giOverviewType + ROW_BACKGROUND_FRAME_BASE)\n"
            "i32 ssLevel = selectedHero13->GetNthSS(widgetId - HERO_SKILL_FIRST);\n"
            "DoSSLevelDialog(ssLevel, quickView)\n"
        )
        result = clean_source.apply_patches("src/SOURCE/Overview.cpp", text)
        self.assertIn(
            "static_cast<i16>(H2EnumIndex(giOverviewType) "
            "+ ROW_BACKGROUND_FRAME_BASE)",
            result,
        )


class CleanSourceCallingConventionTests(unittest.TestCase):
    def test_direct_draw_proc_keeps_stdcall_function_pointer_abi(self):
        text = (
            "typedef HRESULT(__stdcall* DirectDrawCreateProc)(\n"
            "    GUID*, IDirectDraw**, IUnknown*\n"
            ");\n"
        )
        self.assertEqual(
            clean_source.clean(text),
            text.replace("__stdcall", "H2_STDCALL"),
        )

    def test_netbios_callback_declaration_and_type_keep_stdcall(self):
        text = (
            "typedef void(__stdcall* NetbiosPostRoutine)(NetbiosControlBlock*);\n"
            "static void __stdcall nb_call_done(NetbiosControlBlock*);\n"
        )
        self.assertEqual(
            clean_source.clean(text),
            text.replace("__stdcall", "H2_STDCALL"),
        )

    def test_winmain_keeps_stdcall_entry_abi(self):
        text = (
            'extern "C" i32 __stdcall WinMain('
            "HINSTANCE, HINSTANCE, char*, i32);\n"
        )
        self.assertEqual(
            clean_source.clean(text),
            text.replace("__stdcall", "H2_STDCALL"),
        )

    def test_naked_asm_keeps_cdecl_stack_contract(self):
        text = (
            'extern "C" __declspec(naked) void __cdecl\n'
            "TileToBitmap(tileset*, u32, bitmap*, i32, i32);\n"
        )
        self.assertEqual(
            clean_source.clean(text),
            text.replace("__cdecl", "H2_CDECL"),
        )

    def test_explicit_fastcall_declaration_keeps_register_contract(self):
        text = (
            'extern "C" void __fastcall '
            "BlitBitmapToScreenVesa(bitmap*, i32);\n"
        )
        self.assertEqual(
            clean_source.clean(text),
            text.replace("__fastcall", "H2_FASTCALL"),
        )

    def test_clean_ints_defines_portable_calling_convention_contract(self):
        ints = (
            clean_source.OVERRIDE_DIR / "include/Ints.h"
        ).read_text()
        for convention in ("CDECL", "STDCALL", "FASTCALL", "PASCAL"):
            self.assertIn(f"#define H2_{convention}", ints)


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
                mock.patch.object(clean_source, "PORT_PATCHES", {}),
                mock.patch.object(clean_source, "BEHAVIOUR_PATCHES", {}),
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
