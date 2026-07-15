import csv
import tempfile
import unittest
from pathlib import Path

from permute_ast import resolve_rva


class PermuteAstCompatibilityTests(unittest.TestCase):
    def test_resolve_rva_requires_matching_unit_and_symbol(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        root = Path(temporary.name)
        generated = root / "build/gen"
        generated.mkdir(parents=True)
        with (generated / "symbol_names.csv").open("w", newline="") as handle:
            writer = csv.DictWriter(handle, fieldnames=["rva", "name", "unit", "kind"])
            writer.writeheader()
            writer.writerow({
                "rva": "0x1234", "name": "?Target@@YAXXZ", "unit": "BASE/unit", "kind": "func",
            })
        self.assertEqual(resolve_rva(root, "BASE/unit", "?Target@@YAXXZ"), 0x1234)
        with self.assertRaisesRegex(ValueError, "found 0"):
            resolve_rva(root, "BASE/other", "?Target@@YAXXZ")


if __name__ == "__main__":
    unittest.main()
