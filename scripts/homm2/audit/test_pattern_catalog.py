import re
import unittest
from collections import Counter
from pathlib import Path


REPO = Path(__file__).resolve().parents[3]
PATTERNS = REPO / "docs/patterns"
INDEX_ROW = re.compile(r"^\| \[([^]]+)\]\(([^)]+\.md)\) \|", re.MULTILINE)


class PatternCatalogTest(unittest.TestCase):
    def test_every_pattern_is_indexed_once_and_every_link_exists(self):
        rows = INDEX_ROW.findall((PATTERNS / "INDEX.md").read_text())
        names = Counter(name for name, _target in rows)
        targets = Counter(target for _name, target in rows)
        files = {path.name for path in PATTERNS.glob("*.md")}
        files.remove("INDEX.md")

        self.assertEqual([name for name, count in names.items() if count != 1], [])
        self.assertEqual([target for target, count in targets.items() if count != 1], [])
        self.assertEqual(set(targets), files)


if __name__ == "__main__":
    unittest.main()
