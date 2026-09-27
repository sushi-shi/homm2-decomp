"""Review credit must be explicit and bound to the physical function body."""
from pathlib import Path
import os
import shutil
import tempfile
import unittest

from homm2.audit.reconstruction import (
    PATTERNS, assess, attach_addresses, ctags_rows, tag_rows,
)


class ReviewEvidenceTests(unittest.TestCase):
    def setUp(self):
        self.row = dict(id="a.cpp:f()#1", body_sha256="current")
        self.review = dict(body_sha256="current", note="Reads a typed owner field.",
                           patterns={p: "absent" for p in PATTERNS})

    def test_inventory_or_other_function_review_gives_no_credit(self):
        result = assess(self.row, {"a.cpp:g()#1": self.review})
        self.assertEqual(result["status"], "pending")
        self.assertEqual(result["note"], "Not yet reviewed.")
        self.assertTrue(all(result[p] == "pending" for p in PATTERNS))

    def test_changed_body_invalidates_all_verdicts(self):
        self.review["body_sha256"] = "old"
        result = assess(self.row, {self.row["id"]: self.review})
        self.assertEqual(result["status"], "pending")
        self.assertTrue(all(result[p] == "pending" for p in PATTERNS))

    def test_all_ten_verdicts_and_note_required(self):
        for pattern in PATTERNS:
            with self.subTest(pattern=pattern):
                verdicts = {p: "absent" for p in PATTERNS if p != pattern}
                with self.assertRaisesRegex(ValueError, "Incomplete"):
                    assess(self.row, {self.row["id"]: dict(self.review, patterns=verdicts)})
        with self.assertRaisesRegex(ValueError, "substantive"):
            assess(self.row, {self.row["id"]: dict(self.review, note="")})

    def test_open_is_reviewed_but_remains_open(self):
        self.review["patterns"]["casts"] = "open"
        result = assess(self.row, {self.row["id"]: self.review})
        self.assertEqual(result["status"], "reviewed")
        self.assertEqual(result["casts"], "open")

    def test_missing_body_cannot_donate_its_va_to_next_function(self):
        blob = b"VA(0x00400100, 1)\nvoid missed() {}\nvoid next() {}\n"
        with self.assertRaisesRegex(ValueError, "missed a body"):
            attach_addresses([dict(path="a.cpp", line=3, va="")], {"a.cpp": blob})


CTAGS = os.environ.get("HOMM2_RECONSTRUCTION_CTAGS") or shutil.which("ctags")


@unittest.skipUnless(CTAGS, "Universal Ctags 6 not available")
class PhysicalDefinitionTests(unittest.TestCase):
    def test_overloads_macro_wrapped_names_and_conditional_bodies(self):
        source = """class C {
public:
    C() = default;
    void declaration();
};
H2_ENUM_BEGIN(Kind)
    A = 0
H2_ENUM_END(Kind)
VA(0x00400001, 1)
H2_ENUM_RETURN(Kind, i16) C::f(i16 n) { return A; }
void C::f(char* n) {}
static inline i16 H2_UNUSED(Width)(C* p) { return 0; }
#ifdef STRICT
inline Kind Next(Kind value) { return A; }
#else
#define Next(value) ((value) + 1)
#endif
"""
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "a.cpp").write_text(source)
            functions, macros = tag_rows(ctags_rows(root, ["a.cpp"], CTAGS),
                                         {"a.cpp": source.encode()})
        self.assertEqual([r["name"] for r in functions],
                         ["C::C", "C::f", "C::f", "Width", "Next"])
        self.assertEqual([r["name"] for r in macros], ["Next"])
        self.assertEqual(attach_addresses(functions, {"a.cpp": source.encode()}), 1)
        self.assertEqual(functions[1]["va"], "0x00400001")


if __name__ == "__main__":
    unittest.main()
