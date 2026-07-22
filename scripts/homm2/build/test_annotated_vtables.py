import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.annotated_vtables import source_vtables


class AnnotatedVtablesTest(unittest.TestCase):
    def test_recovers_primary_and_secondary_semantic_identities(self):
        with TemporaryDirectory() as directory:
            repo = Path(directory)
            source = repo / "src/SOURCE"
            source.mkdir(parents=True)
            (source / "Owner.cpp").write_text(
                "// VTBL(Ignored, 0x00400100);\n"
                "VTBL(Derived, 0x00400120);\n"
                "VTBL2(Derived, Base, 0x00400120);\n")
            rows = source_vtables(repo / "src", repo)

        self.assertEqual(
            [(row.unit, row.rva, row.mangled_name) for row in rows],
            [
                ("SOURCE/Owner", 0x120, "??_7Derived@@6B@"),
                ("SOURCE/Owner", 0x120, "??_7Derived@@6BBase@@@"),
            ])

    def test_rejects_duplicate_semantic_identity(self):
        with TemporaryDirectory() as directory:
            repo = Path(directory)
            source = repo / "src"
            source.mkdir()
            (source / "Owner.cpp").write_text(
                "VTBL(Derived, 0x00400120);\n"
                "VTBL(Derived, 0x00400124);\n")
            with self.assertRaisesRegex(ValueError, "duplicate source vtable"):
                source_vtables(source, repo)


if __name__ == "__main__":
    unittest.main()
