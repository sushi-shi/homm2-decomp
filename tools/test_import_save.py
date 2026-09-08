import bz2
import pathlib
import subprocess
import sys
import tempfile
import unittest


IMPORTER = pathlib.Path(sys.argv.pop(1)).resolve()
FIXTURES = pathlib.Path(__file__).parent / "fixtures" / "legacy-save"


class ImportSaveTest(unittest.TestCase):
    def test_import_all_kinds_and_preserve_originals(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            for fixture in FIXTURES.glob("*.bz2"):
                with self.subTest(fixture=fixture.name):
                    data = bz2.decompress(fixture.read_bytes())
                    source = root / ("старое сохранение " + fixture.stem)
                    output = root / ("новое сохранение " + fixture.stem + ".h2s")
                    source.write_bytes(data)
                    command = [str(IMPORTER), "--encoding", "windows-1251", str(source), str(output)]
                    result = subprocess.run(command, capture_output=True, text=True)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertEqual(source.read_bytes(), data)
                    native = output.read_bytes()
                    self.assertTrue(native.startswith(b"H2SAVE\r\n"))
                    self.assertIn("Александр".encode(), native)
                    self.assertIn("Белокаменный".encode(), native)
                    self.assertNotEqual(subprocess.run(command, capture_output=True).returncode, 0)
                    self.assertEqual(output.read_bytes(), native)

    def test_failed_import_creates_no_output(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            source = root / "broken.gx2"
            output = root / "broken.h2s"
            source.write_bytes(b"broken")
            result = subprocess.run([str(IMPORTER), "--encoding", "windows-1251", str(source), str(output)],
                                    capture_output=True)
            self.assertNotEqual(result.returncode, 0)
            self.assertFalse(output.exists())
            self.assertEqual(list(root.iterdir()), [source])

    def test_encoding_is_explicit(self):
        result = subprocess.run([str(IMPORTER), "old.gx2", "new.h2s"], capture_output=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(b"--encoding", result.stderr)


if __name__ == "__main__":
    unittest.main()
