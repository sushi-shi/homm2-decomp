import json
import tempfile
import unittest
from collections import Counter
from pathlib import Path

import clang.cindex as ci

from homm2.audit.gotos import DEFAULT_REVIEW, _finalize, _text, analyze_translation_unit
from homm2.build.annotated_data import configure_libclang


SOURCE = r"""
void cleanup(int value) {
    if (value) goto done;
    ++value;
done:
    return;
}

void retry(int &value) {
again:
    --value;
    if (value) goto again;
}

void shared(int value) {
    if (value == 1) goto join;
    if (value == 2) goto join;
    ++value;
join:
    ++value;
}

void single(int value) {
    if (value) goto next;
    ++value;
next:
    ++value;
}
"""


def parse(repo: Path):
    source = repo / "src/SOURCE/TEST.cpp"
    source.parent.mkdir(parents=True)
    source.write_text(SOURCE)
    configure_libclang()
    translation = ci.Index.create().parse(
        str(source), args=["-x", "c++", "-std=c++20"]
    )
    errors = [str(item) for item in translation.diagnostics
              if item.severity >= ci.Diagnostic.Error]
    if errors:
        raise AssertionError(errors)
    return translation


class GotoAuditTests(unittest.TestCase):
    def test_resolves_and_classifies_edges(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            rows = _finalize(analyze_translation_unit(parse(repo), repo))

        counts = Counter(item.category for item in rows)
        self.assertEqual(counts, {
            "backward-loop": 1,
            "cleanup-tail": 1,
            "forward-single": 1,
            "shared-tail": 2,
        })
        shared = [item for item in rows if item.category == "shared-tail"]
        self.assertTrue(all(item.source_count == 2 for item in shared))
        self.assertTrue(all(item.direction == "forward" for item in shared))
        backward = next(item for item in rows if item.category == "backward-loop")
        self.assertGreater(backward.distance_lines, 0)
        self.assertEqual(backward.label, "again")

    def test_default_text_focuses_single_forward_edges(self):
        with tempfile.TemporaryDirectory() as directory:
            repo = Path(directory)
            rows = _finalize(analyze_translation_unit(parse(repo), repo))
        counts = Counter(item.category for item in rows)
        report = {
            "whole_program": True,
            "translation_units": 1,
            "gotos": [item.__dict__ for item in rows],
            "counts": dict(counts),
            "lexical_gotos": len(rows),
            "unmapped_lexical_gotos": 0,
        }
        output = _text(report, include_all=False, categories=frozenset())
        self.assertIn("FORWARD-SINGLE", output)
        self.assertNotIn("BACKWARD-LOOP", output)
        self.assertEqual(DEFAULT_REVIEW, {"forward-single"})
        json.dumps(report)


if __name__ == "__main__":
    unittest.main()
