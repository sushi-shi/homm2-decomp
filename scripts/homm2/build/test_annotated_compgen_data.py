import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.annotated_compgen_data import source_compgen_data


REPO = next(path for path in Path(__file__).resolve().parents
            if (path / "flake.nix").exists())


class AnnotatedCompgenDataTest(unittest.TestCase):
    def test_clang_derives_literal_kind_and_logical_size(self):
        with TemporaryDirectory(dir=REPO / "build") as directory:
            source = Path(directory) / "SOURCE"
            source.mkdir()
            (source / "Literal.cpp").write_text(
                '#include <va.h>\n'
                'double number = DATA_COMPGEN(0x00401000, speedScale, 0.08);\n'
                'const char *text = DATA_COMPGEN(0x00401008, defaultName, "Unknown");\n')
            rows = source_compgen_data(Path(directory), REPO)

        self.assertEqual(
            [(row.rva, row.semantic_name, row.kind, row.size) for row in rows],
            [
                (0x1000, "speedScale", "FLOAT64_LITERAL", 8),
                (0x1008, "defaultName", "STRING_LITERAL", 8),
            ])

    def test_rejects_duplicate_semantic_identity(self):
        with TemporaryDirectory(dir=REPO / "build") as directory:
            source = Path(directory) / "SOURCE"
            source.mkdir()
            (source / "Literal.cpp").write_text(
                '#include <va.h>\n'
                'double first = DATA_COMPGEN(0x00401000, scale, 0.08);\n'
                'double second = DATA_COMPGEN(0x00401008, scale, 0.12);\n')
            with self.assertRaisesRegex(ValueError, "duplicate DATA_COMPGEN semantic"):
                source_compgen_data(Path(directory), REPO)

    def test_collects_claim_from_constant_macro_definition_once(self):
        with TemporaryDirectory(dir=REPO / "build") as directory:
            source = Path(directory) / "SOURCE"
            source.mkdir()
            (source / "Literal.cpp").write_text(
                '#include <va.h>\n'
                '#define SPEED_SCALE '
                'DATA_COMPGEN(0x00401000, speedScale, 0.08)\n'
                'double first() { return SPEED_SCALE; }\n'
                'double second() { return SPEED_SCALE; }\n')
            rows = source_compgen_data(Path(directory), REPO)

        self.assertEqual(
            [(row.rva, row.semantic_name, row.kind, row.size) for row in rows],
            [(0x1000, "speedScale", "FLOAT64_LITERAL", 8)])

    def test_preserves_array_initialization_and_resolves_nested_string_macro(self):
        with TemporaryDirectory(dir=REPO / "build") as directory:
            source = Path(directory) / "SOURCE"
            source.mkdir()
            (source / "Literal.cpp").write_text(
                '#include <va.h>\n'
                '#define RETAIL_FILE "file.cpp"\n'
                '#define PASS(value) value\n'
                'struct Record { char name[5]; };\n'
                'Record record = { DATA_COMPGEN(0x00401000, wolfName, "wolf") };\n'
                'const char *file() { return PASS(DATA_COMPGEN('
                '0x00401008, sourceFile, RETAIL_FILE)); }\n')
            rows = source_compgen_data(Path(directory), REPO)

        self.assertEqual(
            [(row.rva, row.semantic_name, row.kind, row.size) for row in rows],
            [
                (0x1000, "wolfName", "STRING_LITERAL", 5),
                (0x1008, "sourceFile", "STRING_LITERAL", 9),
            ])

    def test_collects_static_initialization_guard_claim(self):
        with TemporaryDirectory(dir=REPO / "build") as directory:
            source = Path(directory) / "SOURCE"
            source.mkdir()
            (source / "Guard.cpp").write_text(
                '#include <va.h>\n'
                'struct Object { Object(); };\n'
                'DATA_COMPGEN_GUARD(0x00401000, objectInitializationGuard, object)\n'
                'Object object;\n')
            rows = source_compgen_data(Path(directory), REPO)

        self.assertEqual(
            [(row.rva, row.semantic_name, row.kind, row.size, row.owners)
             for row in rows],
            [(0x1000, "objectInitializationGuard", "STATIC_INIT_GUARD", 4,
              ("object",))])

    def test_clang_infers_compiler_float_from_integer_expression_context(self):
        with TemporaryDirectory(dir=REPO / "build") as directory:
            source = Path(directory) / "SOURCE"
            source.mkdir()
            (source / "Fold.cpp").write_text(
                '#include <va.h>\n'
                'float value() { return DATA_COMPGEN('
                '0x00401000, integerConvertedToFloat, 16); }\n')
            rows = source_compgen_data(Path(directory), REPO)

        self.assertEqual(
            [(row.rva, row.semantic_name, row.kind, row.size) for row in rows],
            [(0x1000, "integerConvertedToFloat", "FLOAT32_LITERAL", 4)])


if __name__ == "__main__":
    unittest.main()
