import os
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.audit.reloc_sweep import (
    SCRIPT,
    apply_reviewed_site_overrides,
    find_script,
    read_sites,
)

MANIFEST = (
    "# banner\n"
    "# more banner\n"
    "site_rva\tkind\n"
    "0x1016\tdir32\n"
    "0xd48fc\tdir32\n"
    "\n"
)


class ReadSitesTests(unittest.TestCase):
    def test_banner_header_and_blank_lines_are_not_rows(self):
        self.assertEqual(read_sites(MANIFEST), {0x1016, 0xD48FC})

    def test_empty_manifest_reads_as_no_sites(self):
        self.assertEqual(read_sites("site_rva\tkind\n"), set())

    def test_reviewed_site_overrides_are_applied(self):
        self.assertEqual(
            apply_reviewed_site_overrides(
                {0x10, 0x20}, {0x30}, {0x20, 0x40}),
            {0x10, 0x30},
        )

    def test_conflicting_reviewed_overrides_are_rejected(self):
        with self.assertRaises(ValueError):
            apply_reviewed_site_overrides(set(), {0x20}, {0x20})


class FindScriptTests(unittest.TestCase):
    def _checkout(self, directory):
        script = Path(directory) / SCRIPT
        script.parent.mkdir(parents=True, exist_ok=True)
        script.write_text("")
        return script

    def test_explicit_path_wins(self):
        with TemporaryDirectory() as directory:
            script = self._checkout(directory)
            self.assertEqual(find_script(str(script)), script)

    def test_explicit_missing_path_is_fatal(self):
        with self.assertRaises(LookupError):
            find_script("/nonexistent/find_relocs.py")

    def test_environment_checkout_is_searched(self):
        with TemporaryDirectory() as directory:
            script = self._checkout(directory)
            saved = os.environ.get("VOSTOK_DELINKER")
            os.environ["VOSTOK_DELINKER"] = directory
            try:
                self.assertEqual(find_script(None), script)
            finally:
                if saved is None:
                    del os.environ["VOSTOK_DELINKER"]
                else:
                    os.environ["VOSTOK_DELINKER"] = saved

    def test_absent_checkout_names_the_environment_variable(self):
        with TemporaryDirectory() as directory:
            saved = os.environ.get("VOSTOK_DELINKER")
            os.environ["VOSTOK_DELINKER"] = directory
            try:
                with self.assertRaises(LookupError) as raised:
                    find_script(None)
            finally:
                if saved is None:
                    del os.environ["VOSTOK_DELINKER"]
                else:
                    os.environ["VOSTOK_DELINKER"] = saved
        self.assertIn("VOSTOK_DELINKER", str(raised.exception))


if __name__ == "__main__":
    unittest.main()
