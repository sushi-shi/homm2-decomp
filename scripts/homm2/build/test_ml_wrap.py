import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.ml_wrap import assemble


class AssembleTests(unittest.TestCase):
    def _assemble(self, coff):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "unit.asm"
            output = root / "unit.obj"
            source.write_text("END\n")

            def produce(*_args, **_kwargs):
                output.write_bytes(b"object")

            with mock.patch("homm2.build.ml_wrap.wine.prepare_env"), \
                    mock.patch("homm2.build.ml_wrap.wine.tool",
                            return_value=Path("ML.EXE")), \
                    mock.patch("homm2.build.ml_wrap.wine.winepath_w",
                               side_effect=lambda path: str(path)), \
                    mock.patch("homm2.build.ml_wrap.wine.run",
                               side_effect=produce) as run:
                assemble(source, output, coff=coff)
            return run.call_args.args

    def test_default_output_is_retail_omf(self):
        self.assertNotIn("/coff", self._assemble(False))

    def test_comparison_output_requests_coff(self):
        self.assertIn("/coff", self._assemble(True))


if __name__ == "__main__":
    unittest.main()
