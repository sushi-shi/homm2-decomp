import subprocess
import tarfile
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.init import toolchain


def _tree(root: Path, cl=True):
    (root / "msvc" / "bin").mkdir(parents=True)
    if cl:
        (root / "msvc" / "bin" / "CL.EXE").write_bytes(b"MZ")
        (root / "msvc" / "bin" / "ML.EXE").write_bytes(b"MZ")
    return root


def _archive(path: Path, components=("msvc",)):
    """A tarball shaped like the VC6 release: components at the TOP level.

    No wrapping directory, so nothing is stripped on extract. An older release
    was the other shape; getting this wrong silently produces an empty tree.
    """
    with TemporaryDirectory() as staging:
        top = Path(staging)
        for component in components:
            (top / component / "bin").mkdir(parents=True)
            (top / component / "bin" / "CL.EXE").write_bytes(b"MZ")
        with tarfile.open(path, "w") as archive:
            for component in components:
                archive.add(top / component, arcname=component)
    return path


class ProvisionedTests(unittest.TestCase):
    def test_detects_either_case_of_the_compiler(self):
        with TemporaryDirectory() as directory:
            root = _tree(Path(directory) / "tc")
            self.assertTrue(toolchain.is_provisioned(root))

    def test_empty_tree_is_not_provisioned(self):
        with TemporaryDirectory() as directory:
            root = _tree(Path(directory) / "tc", cl=False)
            self.assertFalse(toolchain.is_provisioned(root))


class DownloadGateTests(unittest.TestCase):
    """The hash is the gate, not the release page."""

    def _download(self, payload):
        with TemporaryDirectory() as directory:
            destination = Path(directory)

            def fake_run(command, *args, **kwargs):
                (destination / toolchain.ASSET).write_bytes(payload)
                return subprocess.CompletedProcess(command, 0)

            with mock.patch.object(toolchain.shutil, "which", return_value="/usr/bin/gh"), \
                    mock.patch.object(toolchain.subprocess, "run", fake_run):
                return toolchain.download(destination), destination

    def test_wrong_payload_is_refused_and_not_left_behind(self):
        with self.assertRaises(SystemExit) as raised:
            self._download(b"not the toolchain")
        self.assertIn("REFUSING", str(raised.exception))

    def test_matching_payload_is_accepted(self):
        # Pin the expected digest to this payload so the test proves the comparison,
        # not the constant.
        payload = b"pretend tarball"
        import hashlib
        with mock.patch.object(toolchain, "ASSET_SHA256", hashlib.sha256(payload).hexdigest()):
            archive, _ = self._download(payload)
        self.assertEqual(archive.name, toolchain.ASSET)

    def test_missing_gh_explains_the_fallback(self):
        with mock.patch.object(toolchain.shutil, "which", return_value=None):
            with self.assertRaises(SystemExit) as raised:
                toolchain.download(Path("/nonexistent"))
        self.assertIn("create-toolchain-release.nix", str(raised.exception))


class InstallTests(unittest.TestCase):
    def test_unpacks_components_from_the_archive_top_level(self):
        with TemporaryDirectory() as directory:
            base = Path(directory)
            archive = _archive(base / "tc.tar")
            root = base / "toolchain"
            toolchain.install(archive, root)
            self.assertTrue((root / "msvc" / "bin" / "CL.EXE").exists())

    def test_replaces_an_existing_tree(self):
        with TemporaryDirectory() as directory:
            base = Path(directory)
            root = _tree(base / "toolchain")
            (root / "msvc" / "stale.txt").write_text("previous")
            toolchain.install(_archive(base / "tc.tar"), root)
            self.assertFalse((root / "msvc" / "stale.txt").exists())
            self.assertTrue((root / "msvc" / "bin" / "CL.EXE").exists())

    def test_archive_missing_a_component_leaves_the_old_tree_intact(self):
        with TemporaryDirectory() as directory:
            base = Path(directory)
            root = _tree(base / "toolchain")
            (root / "msvc" / "keep.txt").write_text("original")
            partial = _archive(base / "partial.tar", components=("bogus",))
            with self.assertRaises(SystemExit) as raised:
                toolchain.install(partial, root)
            self.assertIn("msvc", str(raised.exception))
            # The swap must not have happened.
            self.assertEqual((root / "msvc" / "keep.txt").read_text(), "original")


class MainTests(unittest.TestCase):
    def test_present_tree_is_not_refetched(self):
        with TemporaryDirectory() as directory:
            root = _tree(Path(directory) / "tc")
            with mock.patch.object(toolchain, "toolchain_dir", return_value=root), \
                    mock.patch.object(toolchain, "download") as download:
                self.assertEqual(toolchain.main([]), 0)
            download.assert_not_called()

    def test_force_refetches_a_present_tree(self):
        with TemporaryDirectory() as directory:
            base = Path(directory)
            root = _tree(base / "tc")
            with mock.patch.object(toolchain, "toolchain_dir", return_value=root), \
                    mock.patch.object(toolchain, "download",
                                      side_effect=lambda d: _archive(base / "tc.tar")), \
                    mock.patch.object(toolchain, "validate", return_value=True):
                self.assertEqual(toolchain.main(["--force"]), 0)

    def test_check_never_downloads(self):
        with TemporaryDirectory() as directory:
            root = _tree(Path(directory) / "tc", cl=False)
            with mock.patch.object(toolchain, "toolchain_dir", return_value=root), \
                    mock.patch.object(toolchain, "download") as download, \
                    mock.patch.object(toolchain, "validate", return_value=False):
                self.assertEqual(toolchain.main(["--check"]), 1)
            download.assert_not_called()

    def test_a_tree_failing_its_pinned_checks_fails_the_run(self):
        with TemporaryDirectory() as directory:
            base = Path(directory)
            root = base / "tc"
            with mock.patch.object(toolchain, "toolchain_dir", return_value=root), \
                    mock.patch.object(toolchain, "download",
                                      side_effect=lambda d: _archive(base / "tc.tar")), \
                    mock.patch.object(toolchain, "validate", return_value=False):
                self.assertEqual(toolchain.main([]), 1)


if __name__ == "__main__":
    unittest.main()
