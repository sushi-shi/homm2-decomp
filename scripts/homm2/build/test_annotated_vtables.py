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
                "class Base { public: virtual ~Base(); };\n"
                "class Derived : public Base {};\n"
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
                "class Derived { public: virtual ~Derived(); };\n"
                "typedef Derived First; typedef Derived Second;\n"
                "VTBL(First, 0x00400120);\n"
                "VTBL(Second, 0x00400124);\n")
            with self.assertRaisesRegex(ValueError, "duplicate source vtable"):
                source_vtables(source, repo)

    def test_resolves_nested_template_typedef_in_marker_scope(self):
        with TemporaryDirectory() as directory:
            repo = Path(directory)
            source = repo / "src"
            source.mkdir()
            (source / "Owner.cpp").write_text(
                "class widget { public: virtual ~widget(); };\n"
                "class heroWindow { public:\n"
                "  template<class BaseWidget> class DimmerWidget : public BaseWidget {};\n"
                "};\n"
                "typedef heroWindow::DimmerWidget<widget> dimmerWidget;\n"
                "VTBL(dimmerWidget, 0x004eaa04);\n"
                "namespace other {\n"
                "  class Owner { public: virtual ~Owner(); };\n"
                "  typedef Owner dimmerWidget;\n"
                "  VTBL(dimmerWidget, 0x004eaa20);\n"
                "}\n")
            rows = source_vtables(source, repo)
        self.assertEqual([row.mangled_name for row in rows], [
            "??_7?$DimmerWidget@Vwidget@@@heroWindow@@6B@",
            "??_7Owner@other@@6B@",
        ])

    def test_rejects_unresolved_type_instead_of_fabricating_a_symbol(self):
        with TemporaryDirectory() as directory:
            repo = Path(directory)
            source = repo / "src"
            source.mkdir()
            (source / "Owner.cpp").write_text("VTBL(Missing, 0x00400120);\n")
            with self.assertRaisesRegex(ValueError, "vtable owner"):
                source_vtables(source, repo)


if __name__ == "__main__":
    unittest.main()
