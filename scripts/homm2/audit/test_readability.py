"""Reading credit and inventory rejection controls; no retail assets required."""
import os
from pathlib import Path
import shutil
import tempfile
import unittest

from homm2.audit.readability import (
    assembly_rows, attach_addresses, ctags_rows, digest, reviewed, tag_rows, tsv,
)


class ReadabilityTests(unittest.TestCase):
    def test_new_file_is_unread(self):
        self.assertEqual(reviewed("a.cpp", "new", {}), ("unread", ""))

    def test_only_review_of_current_content_earns_credit(self):
        reviews = {"a.cpp": {"sha256": "old", "note": "B01: all bodies reviewed"}}
        self.assertEqual(reviewed("a.cpp", "old", reviews)[0], "read")
        self.assertEqual(reviewed("a.cpp", "new", reviews)[0], "unread")
        self.assertTrue(reviewed("a.cpp", "new", reviews)[1].startswith("STALE:"))

    def test_empty_review_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "substantive note"):
            reviewed("a.cpp", "hash", {"a.cpp": {"sha256": "hash"}})

    def test_overloads_and_destructor_are_separate_definitions(self):
        blob = b"C::C() {}\nC::C(int x) {}\nC::~C() {}\n"
        tags = [dict(path="a.cpp", line=i, end=i, kind="function", scope="C",
                     name=name, signature=signature)
                for i, name, signature in ((1, "C", "()"), (2, "C", "(int x)"),
                                           (3, "~C", "()"))]
        rows, _ = tag_rows(tags + tags[:1], {"a.cpp": blob})
        self.assertEqual(len(rows), 3)
        self.assertEqual(rows[-1]["name"], "C::~C")

    def test_missing_end_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "closing extent"):
            tag_rows([dict(path="a.cpp", line=1, kind="function", name="f")],
                     {"a.cpp": b"void f() {}\n"})

    def test_defaulted_definition_but_not_plain_declaration_is_indexed(self):
        rows, _ = tag_rows([
            dict(path="a.h", line=1, name="C", kind="prototype"),
            dict(path="a.h", line=2, name="f", kind="prototype"),
        ], {"a.h": b"C() = default;\nvoid f();\n"})
        self.assertEqual([row["name"] for row in rows], ["C"])

    def test_macro_does_not_count_as_a_function(self):
        rows, macros = tag_rows([
            dict(path="a.h", line=1, end=2, name="WRAP", kind="macro"),
        ], {"a.h": b"#define WRAP(x) " + bytes([92, 10]) + b"    (x)\n"})
        self.assertEqual(rows, [])
        self.assertEqual(macros[0]["end"], 2)

    def test_unexpanded_project_macro_fails(self):
        with self.assertRaisesRegex(ValueError, "Unexpanded"):
            tag_rows([dict(path="a.cpp", line=1, end=1, name="H2_ENUM_RETURN",
                           kind="function")], {"a.cpp": b"void f() {}\n"})

    def test_type_mistaken_for_hidden_name_fails(self):
        with self.assertRaisesRegex(ValueError, "Type mistaken"):
            tag_rows([dict(path="a.cpp", line=1, end=1, name="i16", kind="function")],
                     {"a.cpp": b"i16 H2_UNUSED(Width)() {}\n"})

    def test_address_matches_body_not_unannotated_following_helper(self):
        blob = b"VA(0x00400001, 1)\nvoid f() {}\nvoid helper() {}\n"
        rows = [dict(path="a.cpp", line=2, va=""), dict(path="a.cpp", line=3, va="")]
        self.assertEqual(attach_addresses(rows, {"a.cpp": blob}), 1)
        self.assertEqual(rows[0]["va"], "0x00400001")
        self.assertEqual(rows[1]["va"], "")

    def test_unindexed_address_fails(self):
        with self.assertRaisesRegex(ValueError, "no definition"):
            attach_addresses([], {"a.cpp": b"VA(0x00400001, 1)\nvoid f() {}\n"})

    def test_skipped_body_cannot_steal_next_functions_address(self):
        blob = b"VA(0x00400001, 1)\nvoid missed() {}\nvoid f() {}\n"
        with self.assertRaisesRegex(ValueError, "missed a body"):
            attach_addresses([dict(path="a.cpp", line=3, va="")], {"a.cpp": blob})

    def test_assembly_functions_have_closed_extents(self):
        rows = assembly_rows("a.asm", b"f PROC C\n ret\nf ENDP\n")
        self.assertEqual((rows[0]["line"], rows[0]["end"]), (1, 3))
        with self.assertRaisesRegex(ValueError, "Unterminated"):
            assembly_rows("a.asm", b"f PROC C\n ret\n")

    def test_hash_and_tsv_are_deterministic(self):
        self.assertNotEqual(digest(b"old"), digest(b"new"))
        self.assertEqual(tsv([dict(a="hello", b="line\tbreak")], ["a", "b"]),
                         'a\tb\nhello\t"line\tbreak"\n')


CTAGS = os.environ.get("HOMM2_READABILITY_CTAGS") or shutil.which("ctags")


@unittest.skipUnless(CTAGS, "Universal Ctags is not available")
class CtagsIntegrationTests(unittest.TestCase):
    def index(self, text):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "fixture.cpp").write_text(text)
            return tag_rows(ctags_rows(root, ["fixture.cpp"], CTAGS),
                            {"fixture.cpp": text.encode()})[0]

    def test_project_macros_do_not_hide_constructor_or_method(self):
        rows = self.index("""H2_ENUM_BEGIN(Kind)
    A = 0
H2_ENUM_END(Kind)
VA(0x00400001, 1)
Thing::Thing() {}
H2_ENUM_RETURN(Result, i16) Thing::Main() { return 0; }
""")
        self.assertEqual([r["name"] for r in rows], ["Thing::Thing", "Thing::Main"])

    def test_unused_wrapper_preserves_all_three_helpers(self):
        rows = self.index("""static inline i16 H2_UNUSED(Width)(bitmap* p) { return p->w; }
static inline u8* H2_UNUSED(Pixels)(bitmap* p) { return p->pixels; }
static inline u8* H2_UNUSED(Pixels)(bitmap* p, i32 n) { return p->pixels + n; }
""")
        self.assertEqual([r["name"] for r in rows], ["Width", "Pixels", "Pixels"])

    def test_constraints_defaulted_bodies_and_final_owner(self):
        rows = self.index("""class Thing H2_FINAL {
public:
    Thing() = default;
    template <typename T> requires(__is_integral(T))
    explicit operator T() const { return 0; }
    void PlainDeclaration();
};
template <typename T> requires __is_enum(T)
inline void Log(T value) {}
""")
        self.assertEqual([r["name"] for r in rows],
                         ["Thing::Thing", "Thing::operator T", "Log"])


if __name__ == "__main__":
    unittest.main()
