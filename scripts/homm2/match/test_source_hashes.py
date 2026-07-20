import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.match import source_hashes


class SourceHashBoundaryTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        (self.root / "src/SOURCE").mkdir(parents=True)
        (self.root / "include").mkdir()
        generated = self.root / "build/gen"
        generated.mkdir(parents=True)
        (generated / "symbol_names.csv").write_text(
            "0x1000,?First@@YAHXZ,SOURCE/UNIT,,func\n"
            "0x1100,?Second@@YAHXZ,SOURCE/UNIT,,func\n")
        self.source_path = self.root / "src/SOURCE/UNIT.cpp"
        self.repo_patch = mock.patch.object(source_hashes, "REPO", self.root)
        self.repo_patch.start()

    def tearDown(self):
        self.repo_patch.stop()
        self.temporary.cleanup()

    @staticmethod
    def source(first_return="localValue + 1", second_return="2", between=""):
        return f'''VA(0x00401000, 0x20)
i32 First(void)
{{
    DATA(0x00500000) static i32 localValue = 1;
    const char *markerText = "}} DATA(0x00500004)";
    /* VA(0x00401100, 0x1) {{ }} */
    if (localValue) {{
VA(0x0040ffff, 0x1)
        localValue++;
    }}
    return {first_return};
}}

{between}
VA(0x00401100, 0x10)
i32 Second(void)
{{
    return {second_return};
}}
'''

    def test_nested_markers_do_not_split_function_blocks(self):
        blocks = list(source_hashes._source_function_blocks(self.source()))
        self.assertEqual([va for va, _block in blocks], [0x00401000, 0x00401100])
        self.assertIn("DATA(0x00500000)", blocks[0][1])
        self.assertNotIn("i32 Second", blocks[0][1])

    def test_body_edit_changes_only_own_hash(self):
        first = ("SOURCE/UNIT", "?First@@YAHXZ")
        second = ("SOURCE/UNIT", "?Second@@YAHXZ")
        self.source_path.write_text(self.source(between="// durable evidence"))
        before = source_hashes.source_hashes()
        self.source_path.write_text(self.source(
            first_return="localValue + 2", between="// changed neighboring evidence"))
        after = source_hashes.source_hashes()
        self.assertNotEqual(before[first], after[first])
        self.assertEqual(before[second], after[second])


if __name__ == "__main__":
    unittest.main()
