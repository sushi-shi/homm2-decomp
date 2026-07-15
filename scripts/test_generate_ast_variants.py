import unittest

from generate_ast_variants import AstEdit, AstMutation, candidate_payloads


class AstVariantGenerationTests(unittest.TestCase):
    def test_disjoint_mutations_are_combined(self):
        blob = b"abcdefghij"
        mutations = [
            AstMutation("first", "a", (AstEdit(0, 1, b"A"),)),
            AstMutation("second", "c", (AstEdit(2, 3, b"C"),)),
        ]
        candidates, truncated = candidate_payloads(blob, mutations, 2, 20)
        self.assertFalse(truncated)
        self.assertEqual(len(candidates), 3)
        self.assertEqual(len(candidates[-1]["edits"]), 2)

    def test_helpers_at_the_same_insertion_point_are_merged(self):
        blob = b"abcdefghij"
        mutations = [
            AstMutation("inline", "one", (
                AstEdit(0, 0, b"helper1 "), AstEdit(2, 3, b"C"),
            )),
            AstMutation("inline", "two", (
                AstEdit(0, 0, b"helper2 "), AstEdit(5, 6, b"F"),
            )),
        ]
        candidates, truncated = candidate_payloads(blob, mutations, 2, 20)
        self.assertFalse(truncated)
        self.assertEqual(len(candidates), 3)
        combined = candidates[-1]
        self.assertEqual(len(combined["edits"]), 3)
        self.assertEqual(combined["edits"][0]["replace"], "helper1 helper2 ")


if __name__ == "__main__":
    unittest.main()
