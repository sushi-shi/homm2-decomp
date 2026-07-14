import unittest

from .strict import _diagnostics


class StrictDiagnosticsTest(unittest.TestCase):
    def test_diagnostics_are_normalized_and_grouped(self):
        results = [{
            "source": "src/SOURCE/example.cpp",
            "returncode": 1,
            "output": "src/SOURCE/example.cpp:9:4: error: implicit conversion from FileMode [-Wenum-conversion]\n",
        }]
        diagnostics = _diagnostics(results, {"FileMode"})
        self.assertEqual(diagnostics[0]["domain"], "FileMode")
        self.assertEqual(diagnostics[0]["warning"], "-Wenum-conversion")
        self.assertEqual(diagnostics[0]["line"], 9)

    def test_clang_cl_diagnostic_format_is_supported(self):
        results = [{
            "source": "src/SOURCE/example.cpp",
            "returncode": 1,
            "output": "src/SOURCE/example.cpp(12,7) : error: mixed FileMode [-Werror,-Wenum-compare]\n",
        }]
        diagnostic = _diagnostics(results, {"FileMode"})[0]
        self.assertEqual(diagnostic["severity"], "error")
        self.assertEqual(diagnostic["warning"], "-Wenum-compare")
        self.assertEqual(diagnostic["column"], 7)
        self.assertTrue(diagnostic["promoted"])

    def test_unnamed_enum_location_is_grouped(self):
        results = [{
            "source": "src/SOURCE/example.cpp",
            "returncode": 1,
            "output": "src/SOURCE/example.cpp(4,2) : error: bad (unnamed enum at include/x.h:8:1)\n",
        }]
        diagnostic = _diagnostics(
            results, {}, {("include/x.h", 8): "AnonymousEnum@VALUE"}
        )[0]
        self.assertEqual(diagnostic["domain"], "AnonymousEnum@VALUE")


if __name__ == "__main__":
    unittest.main()
