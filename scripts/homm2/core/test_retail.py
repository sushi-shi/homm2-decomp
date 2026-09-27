"""Reject the wrong control before any build/redelink side effects."""
import contextlib
import hashlib
import io
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from homm2.core import retail
from homm2 import cli, redelink


class RetailIdentityTests(unittest.TestCase):
    def test_accepts_expected_digest(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "control.exe"
            path.write_bytes(b"test control")
            with patch.object(retail, "RETAIL_SHA256", hashlib.sha256(b"test control").hexdigest()):
                retail.verify_retail(path)

    def test_rejects_wrong_digest_with_both_hashes(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "reconstruction.exe"
            path.write_bytes(b"not retail")
            with self.assertRaises(ValueError) as caught:
                retail.verify_retail(path)
            self.assertIn(retail.RETAIL_SHA256, str(caught.exception))
            self.assertIn(hashlib.sha256(b"not retail").hexdigest(), str(caught.exception))

    def test_missing_control(self):
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaises(FileNotFoundError):
                retail.verify_retail(Path(directory) / "missing.exe")

    def test_build_rejects_before_configure(self):
        with patch.object(retail, "verify_retail", side_effect=ValueError("wrong control")), \
                patch.object(cli, "sh") as run, contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(cli.main(["build"]), 1)
            run.assert_not_called()

    def test_redelink_rejects_before_configure(self):
        with patch.object(retail, "verify_retail", side_effect=ValueError("wrong control")), \
                patch.object(redelink, "run") as run, contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(redelink.main([]), 1)
            run.assert_not_called()
