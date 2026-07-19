import unittest

from .strict import WARNING_FLAGS, _command, _diagnostics, _domain_summary


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
        self.assertEqual(diagnostics[0]["kind"], "promoted-enum-policy")
        self.assertTrue(diagnostics[0]["promoted"])

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
        self.assertEqual(diagnostic["kind"], "promoted-enum-policy")

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

    def test_driver_context_and_summary_lines_are_ignored(self):
        results = [{
            "source": "src/SOURCE/example.cpp",
            "returncode": 1,
            "output": (
                "In file included from include/x.h:3:\n"
                "src/SOURCE/example.cpp:4:2: error: incompatible enum FileMode\n"
                "1 error generated.\n"
            ),
        }]
        diagnostics = _diagnostics(results, {"FileMode"})
        self.assertEqual(len(diagnostics), 1)
        self.assertEqual(diagnostics[0]["kind"], "hard-enum-conversion")

    def test_clang_cl_warning_flags_are_forwarded(self):
        entry = {
            "directory": "/repo",
            "file": "src/example.cpp",
            "arguments": ["clang-cl", "/c", "src/example.cpp", "/I", "/repo/include"],
        }
        command = _command(entry, "/tool/clang-cl")
        self.assertNotIn("-Wall", command)
        self.assertIn("/clang:-Wall", command)
        self.assertIn("/clang:-fno-caret-diagnostics", command)
        self.assertEqual(command.count("/DHOMM2_STRICT_ENUM_TYPES"), 1)
        for flag in WARNING_FLAGS:
            self.assertIn(f"/clang:{flag}", command)

    def test_domain_summary_is_deterministic(self):
        diagnostics = [
            {"domain": "Second", "severity": "warning", "kind": "enum-warning",
             "warning": "-Wswitch-enum"},
            {"domain": None, "severity": "error", "kind": "compatibility-error",
             "warning": None},
            {"domain": "Second", "severity": "error", "kind": "promoted-enum-policy",
             "warning": "-Wenum-conversion"},
        ]
        summary = _domain_summary(diagnostics)
        self.assertEqual(list(summary), ["<unclassified>", "Second"])
        self.assertEqual(summary["Second"]["diagnostics"], 2)
        self.assertEqual(summary["Second"]["errors"], 1)


if __name__ == "__main__":
    unittest.main()
