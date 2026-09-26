"""Native final links must preserve raw object ownership and supported modes."""
import contextlib
import io
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build import native_link


CONFIGURED = [
    "WINMM.LIB", "build/link/wing32.lib",
    "build/link/generic-imports/audiere.lib",
    "build/objdiff/base/SOURCE/ADVMGR.obj",
    "build/objdiff/base/SOURCE/REQUEST.obj",
    "build/objdiff/base/SOURCE/X_GLOBAL.obj",
    "build/link/BASE-prefix.lib", "build/link/Midi.lib",
    "build/link/BASE-suffix.lib", "LIBCMT.LIB", "build/link/HMM2PL.res",
]


class NativeLinkTests(unittest.TestCase):
    def test_both_modes_preserve_all_raw_objects_and_archive_order(self):
        for resources in (False, True):
            with self.subTest(resources=resources):
                inputs = native_link.final_inputs(CONFIGURED, include_resources=resources)
                self.assertEqual(inputs[:3], ["/NODEFAULTLIB:LIBCMT",
                                             "/NODEFAULTLIB:LIBCPMT", "/NODEFAULTLIB:OLDNAMES"])
                self.assertEqual(inputs[3:6], CONFIGURED[3:6])
                self.assertEqual(inputs[6:10], ["OLDNAMES.LIB", *CONFIGURED[:3]])
                self.assertEqual(inputs[10:15], [*CONFIGURED[6:9], "MSVCPRT.LIB", "LIBCMT.LIB"])
                self.assertEqual(inputs[15:], [CONFIGURED[-1]] if resources else [])
                self.assertEqual(CONFIGURED[4], "build/objdiff/base/SOURCE/REQUEST.obj")

    def test_prepared_object_cannot_replace_raw_project_input(self):
        for replacement in ("build/link/bss-layout-all/SOURCE/REQUEST.obj",
                            "build/objdiff/normalized/base/SOURCE/REQUEST.obj"):
            for index in (3, 4):
                configured = list(CONFIGURED)
                configured[index] = replacement
                with self.subTest(replacement=replacement, index=index), self.assertRaisesRegex(
                        RuntimeError, "raw project objects"):
                    native_link.final_inputs(configured, include_resources=True)

    def test_prepared_archive_cannot_replace_raw_archive(self):
        configured = list(CONFIGURED)
        configured[8] = "build/link/plain-inputs/BASE-suffix.lib"
        with self.assertRaisesRegex(RuntimeError, "unexpected configured final-link tail"):
            native_link.final_inputs(configured, include_resources=True)

    def test_missing_project_objects_rejected(self):
        with self.assertRaisesRegex(RuntimeError, "no raw project objects"):
            native_link.final_inputs(CONFIGURED[:3] + CONFIGURED[6:], include_resources=False)

    def test_transform_flag_is_rejected_before_linking(self):
        with mock.patch.object(native_link.wine, "run") as run, contextlib.redirect_stderr(io.StringIO()):
            with self.assertRaises(SystemExit) as error:
                native_link.main(["--transform"])
        self.assertEqual(error.exception.code, 2)
        run.assert_not_called()

    def test_generic_driver_does_not_read_retail_and_retains_linker_output(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            tool = root / "LINK.EXE"
            tool.write_bytes(b"tool")
            output = root / "build/link/generic/HMM2PL.exe"

            def link(*args, **kwargs):
                output.write_bytes(b"untouched LINK output")
                output.with_suffix(".map").write_text("native map")

            with (mock.patch.object(native_link, "ROOT", root),
                  mock.patch.object(native_link, "LINK_ROOT", root / "build/link"),
                  mock.patch.object(native_link, "LINK_EXE", tool),
                  mock.patch.object(native_link, "LIBCMT", tool),
                  mock.patch.object(native_link, "MSVCPRT", tool),
                  mock.patch.object(native_link, "RETAIL") as retail,
                  mock.patch.object(native_link, "ninja_link_args", return_value=CONFIGURED),
                  mock.patch.object(native_link.wine, "run", side_effect=link) as run,
                  contextlib.redirect_stdout(io.StringIO())):
                self.assertEqual(native_link.main([]), 0)
                retail.read_bytes.assert_not_called()
                retail.exists.assert_not_called()
                run.assert_called_once()
            self.assertEqual(output.read_bytes(), b"untouched LINK output")
            response = output.with_suffix(".rsp").read_text()
            self.assertIn("build/objdiff/base/SOURCE/REQUEST.obj", response)
            self.assertNotIn("HMM2PL.res", response)

    def test_ninja_inputs_come_from_generic_edge_with_continuations(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            root.joinpath("build.ninja").write_text(
                "rule link_exe\n  command = native\n\n"
                "build build/link/generic/HMM2PL.exe build/link/generic/HMM2PL.map: link_exe\n"
                "  link_args = WINMM.LIB $\n    build/objdiff/base/SOURCE/REQUEST.obj\n"
                "  link_mode = \n\nbuild unrelated: phony\n")
            with mock.patch.object(native_link, "ROOT", root):
                self.assertEqual(native_link.ninja_link_args(),
                                 ["WINMM.LIB", "build/objdiff/base/SOURCE/REQUEST.obj"])
