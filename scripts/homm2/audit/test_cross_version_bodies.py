import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.audit.cross_version_bodies import (
    classify,
    compare,
    rename_map,
    spans_by_rva,
    tokenize,
    unit_sources,
)

SOURCE = """\
#include <SOURCE/GAME.h>

VA(0x00401000, 0x40)
i32 game::GetLuck(hero* who) {
    i32 luck = who->m_luck;  // a comment
    return luck;
}

VA(0x00401040, 0x10)
void game::Reset(void) {
    m_turn = 0;
}
"""


def tree(directory, source=SOURCE, unit="SOURCE/GAME", path="src/SOURCE/GAME.cpp"):
    root = Path(directory)
    (root / "config").mkdir(parents=True, exist_ok=True)
    (root / path).parent.mkdir(parents=True, exist_ok=True)
    (root / path).write_text(source)
    (root / "config/units.toml").write_text(
        f'[[unit]]\nunit = "{unit}"\nsource = "{path}"\nflags = "base"\n')
    return root


class UnitSourcesTests(unittest.TestCase):
    def test_the_manifest_supplies_the_unit_mapping(self):
        with TemporaryDirectory() as directory:
            root = tree(directory)
            self.assertEqual(unit_sources(root),
                             {"SOURCE/GAME": root / "src/SOURCE/GAME.cpp"})


class SpansTests(unittest.TestCase):
    def test_a_span_runs_to_the_next_marker(self):
        with TemporaryDirectory() as directory:
            spans = spans_by_rva(tree(directory))
        self.assertEqual(sorted(spans), [("SOURCE/GAME", 0x1000),
                                         ("SOURCE/GAME", 0x1040)])
        self.assertIn("GetLuck", spans[("SOURCE/GAME", 0x1000)])
        self.assertNotIn("Reset", spans[("SOURCE/GAME", 0x1000)])

    def test_the_marker_line_is_excluded_from_the_body(self):
        # Address and size differ between the images by definition. Keeping the
        # marker would classify every single body as divergent.
        with TemporaryDirectory() as directory:
            spans = spans_by_rva(tree(directory))
        self.assertNotIn("VA(0x00401000", spans[("SOURCE/GAME", 0x1000)])

    def test_the_last_span_runs_to_end_of_file(self):
        with TemporaryDirectory() as directory:
            spans = spans_by_rva(tree(directory))
        self.assertIn("m_turn", spans[("SOURCE/GAME", 0x1040)])


class TokenizeTests(unittest.TestCase):
    def test_comments_and_whitespace_carry_no_codegen_meaning(self):
        self.assertEqual(tokenize("a  /* x */ +\n b // y"), tokenize("a+b"))

    def test_a_string_holding_a_comment_marker_stays_one_token(self):
        self.assertEqual(tokenize('"a // b"'), [("string", '"a // b"')])

    def test_multi_character_operators_are_single_tokens(self):
        self.assertIn(("punct", "->"), tokenize("p->m"))
        self.assertIn(("punct", "<<="), tokenize("a <<= 1"))


class RenameMapTests(unittest.TestCase):
    def test_a_consistent_one_to_one_map_is_a_rename(self):
        ours, theirs = tokenize("i32 luck = x;"), tokenize("i32 bonus = x;")
        self.assertEqual(rename_map(ours, theirs), {"bonus": "luck"})

    def test_two_locals_folding_onto_one_name_is_not_a_rename(self):
        # A one-way map would call these equivalent. They are not: the second body
        # uses one variable where the first uses two.
        ours = tokenize("a = b;")
        theirs = tokenize("c = c;")
        self.assertIsNone(rename_map(ours, theirs))

    def test_a_changed_literal_is_not_a_rename(self):
        self.assertIsNone(rename_map(tokenize("x = 1;"), tokenize("x = 2;")))

    def test_differing_token_counts_cannot_be_a_rename(self):
        self.assertIsNone(rename_map(tokenize("a;"), tokenize("a; b;")))


class ClassifyTests(unittest.TestCase):
    def test_equal_text_is_identical(self):
        self.assertEqual(classify("a = 1;", "a = 1;")[0], "identical")

    def test_layout_and_comments_only_is_formatting(self):
        body, similarity, _ = classify("a = 1;", "a  =  1; // note")
        self.assertEqual((body, similarity), ("formatting", 1.0))

    def test_a_consistent_local_rename_is_its_own_class(self):
        # The expected outcome for much of the gap: local names are per-compiler
        # slot pins, so VC6 forces different ones than 4.2 did for the same code.
        body, _, renames = classify("i32 luck = 0;", "i32 bonus = 0;")
        self.assertEqual(body, "renames")
        self.assertEqual(renames, {"bonus": "luck"})

    def test_a_changed_constant_is_divergent_not_a_rename(self):
        body, similarity, _ = classify("if (h <= 125) f();", "if (h < 117) f();")
        self.assertEqual(body, "divergent")
        self.assertLess(similarity, 1.0)

    def test_similarity_ranks_a_small_edit_above_a_rewrite(self):
        small = classify("a; b; c; d; e;", "a; b; c; d; z;")[1]
        large = classify("a; b; c; d; e;", "q(w, r);")[1]
        self.assertGreater(small, large)

    def test_a_function_missing_on_one_side_is_not_a_comparison(self):
        self.assertEqual(classify("a;", None)[0], "missing")


class CompareTests(unittest.TestCase):
    ROW = {"unit": "SOURCE/GAME", "symbol": "?GetLuck@game@@QAEHPAVhero@@@Z",
           "lead": "parity-gap", "transition": "od-od", "unmatched_bytes": 12}

    def test_rows_are_joined_through_each_tree_s_own_address(self):
        # The whole point: the same function sits at different addresses in the two
        # images, so each side must be looked up with its own inventory.
        buka_syms = {("SOURCE/GAME", self.ROW["symbol"]): (0x1000, 0x40)}
        pol_syms = {("SOURCE/GAME", self.ROW["symbol"]): (0x9000, 0x40)}
        results = compare([dict(self.ROW)],
                          {("SOURCE/GAME", 0x1000): "a = 1;"},
                          {("SOURCE/GAME", 0x9000): "a = 1;"},
                          buka_syms, pol_syms)
        self.assertEqual(results[0]["body"], "identical")

    def test_a_function_absent_from_the_reference_reports_missing(self):
        buka_syms = {("SOURCE/GAME", self.ROW["symbol"]): (0x1000, 0x40)}
        results = compare([dict(self.ROW)], {("SOURCE/GAME", 0x1000): "a;"}, {},
                          buka_syms, {})
        self.assertEqual(results[0]["body"], "missing")


if __name__ == "__main__":
    unittest.main()
