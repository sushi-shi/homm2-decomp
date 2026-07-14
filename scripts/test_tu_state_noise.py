from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPT = Path(__file__).with_name("tu_state_noise.py")
SPEC = importlib.util.spec_from_file_location("tu_state_noise", SCRIPT)
noise = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = noise
SPEC.loader.exec_module(noise)


class TuStateNoiseTests(unittest.TestCase):
    def target(self, text: str, marker: str = "VA(0x00401234,"):
        marker_offset = text.index(marker)
        insertion = noise._leading_metadata_offset(text, marker_offset)
        return noise.Target(
            source=Path("unit.cpp"), unit="BASE/unit", rva=0x1234, va=0x401234,
            symbol="?Target@@YIXXZ", retail_size=1, marker_offset=marker_offset,
            insertion_offset=insertion, logical_line=noise.logical_line_at(text, insertion),
        )

    def test_variant_is_before_metadata_and_restores_line(self):
        original = "int predecessor;\n\n// @match-note\n// evidence\nVA(0x00401234, 0x1)\nvoid f() {}\n"
        target = self.target(original)
        variant = noise.Variant(1, "macro", "tag", "#define P 0\n#undef P\n")
        candidate = noise.insert_variant(original, target, variant)
        self.assertLess(candidate.index("h2-tu-state-probe"), candidate.index("// @match-note"))
        self.assertIn(f"#line {target.logical_line}\n\n// @match-note", candidate)
        self.assertEqual(noise.logical_line_at(original, target.insertion_offset), 2)

    def test_existing_line_directive_is_preserved_logically(self):
        original = "#line 80 \"retail.cpp\"\nint predecessor;\nVA(0x00401234, 0x1)\nvoid f() {}\n"
        target = self.target(original)
        self.assertEqual(target.logical_line, 81)
        candidate = noise.insert_variant(
            original, target, noise.Variant(1, "comment", "tag", "/* pad */\n")
        )
        self.assertIn("#line 81\nVA(0x00401234", candidate)

    def test_variants_are_deterministic_and_declaration_free(self):
        left = noise.make_variants(20, noise.DEFAULT_FAMILIES, 123)
        right = noise.make_variants(20, noise.DEFAULT_FAMILIES, 123)
        self.assertEqual(left, right)
        forbidden = __import__("re").compile(r"\b(class|struct|enum|extern|volatile)\b")
        for variant in left:
            self.assertIsNone(forbidden.search(variant.body))

    def test_temporary_source_restores_after_exception(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "unit.cpp"
            original = b"original\n"
            path.write_bytes(original)
            with self.assertRaises(RuntimeError):
                with noise.temporary_source(path, original, b"candidate\n"):
                    self.assertEqual(path.read_bytes(), b"candidate\n")
                    raise RuntimeError("stop")
            self.assertEqual(path.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
