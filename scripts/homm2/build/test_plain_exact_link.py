import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build.exact_link import plain, transforms


CONFIGURED = [
    "WINMM.LIB",
    "build/link/wing32.lib",
    "build/objdiff/base/SOURCE/ADVMGR.obj",
    "build/objdiff/base/SOURCE/REQUEST.obj",
    "build/objdiff/base/SOURCE/X_GLOBAL.obj",
    "build/link/BASE-prefix.lib",
    "build/link/Midi.lib",
    "build/link/BASE-suffix.lib",
    "LIBCMT.LIB",
    "build/link/HMM2PL.res",
]
REQUEST = plain.ROOT / "build/objdiff/base/SOURCE/REQUEST.obj"


class PlainExactLinkTests(unittest.TestCase):
    def test_request_layout_transform_is_confined_to_transform_mode(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            raw = root / "build/objdiff/base/SOURCE/REQUEST.obj"
            transformed = root / "build/link/bss-layout-all/SOURCE/REQUEST.obj"
            raw.parent.mkdir(parents=True)
            transformed.parent.mkdir(parents=True)
            raw.write_bytes(b"raw")
            transformed.write_bytes(b"transformed")
            with (
                mock.patch.object(plain, "ROOT", root),
                mock.patch.object(plain, "LINK_ROOT", root / "build/link"),
                mock.patch("subprocess.run") as run,
            ):
                self.assertEqual(plain.prepare_request(False), raw)
                run.assert_not_called()
                self.assertEqual(plain.prepare_request(True), transformed)
                run.assert_called_once()

    def test_transform_inputs_swap_the_prepared_archives_and_keep_resources(self):
        result = plain.final_inputs(
            list(CONFIGURED), REQUEST,
            "build/link/plain-inputs/BASE-prefix.lib",
            "build/link/plain-inputs/BASE-suffix.lib",
            include_resources=True,
        )
        self.assertEqual(
            result[:3],
            ["/NODEFAULTLIB:LIBCMT", "/NODEFAULTLIB:LIBCPMT", "/NODEFAULTLIB:OLDNAMES"],
        )
        self.assertEqual(result[3], "build/objdiff/base/SOURCE/ADVMGR.obj")
        self.assertEqual(result[4], "build/objdiff/base/SOURCE/REQUEST.obj")
        self.assertEqual(result[6:9], ["OLDNAMES.LIB", "WINMM.LIB", "build/link/wing32.lib"])
        self.assertEqual(
            result[-6:],
            [
                "build/link/plain-inputs/BASE-prefix.lib",
                "build/link/Midi.lib",
                "build/link/plain-inputs/BASE-suffix.lib",
                "MSVCPRT.LIB",
                "LIBCMT.LIB",
                "build/link/HMM2PL.res",
            ],
        )

    def test_generic_inputs_use_raw_archives_and_omit_resources(self):
        result = plain.final_inputs(
            list(CONFIGURED), REQUEST,
            "build/link/BASE-prefix.lib",
            "build/link/BASE-suffix.lib",
            include_resources=False,
        )
        self.assertNotIn("build/link/HMM2PL.res", result)
        self.assertEqual(
            result[-5:],
            [
                "build/link/BASE-prefix.lib",
                "build/link/Midi.lib",
                "build/link/BASE-suffix.lib",
                "MSVCPRT.LIB",
                "LIBCMT.LIB",
            ],
        )

    def test_rsrc_inputs_keep_raw_archives_with_resources(self):
        result = plain.final_inputs(
            list(CONFIGURED), REQUEST,
            "build/link/BASE-prefix.lib",
            "build/link/BASE-suffix.lib",
            include_resources=True,
        )
        self.assertEqual(result[-1], "build/link/HMM2PL.res")
        self.assertIn("build/link/BASE-suffix.lib", result)
        self.assertNotIn("build/link/plain-inputs/BASE-suffix.lib", result)

    def test_final_inputs_reject_unreviewed_tail_drift(self):
        configured = [
            "WINMM.LIB",
            "build/objdiff/base/SOURCE/ADVMGR.obj",
            "build/objdiff/base/SOURCE/REQUEST.obj",
            "build/link/BASE-prefix.lib",
            "build/link/Midi.lib",
            "build/link/BASE-suffix.lib",
            "OTHER.LIB",
            "build/link/HMM2PL.res",
        ]
        path = plain.ROOT / "build/link/probe.obj"
        with self.assertRaisesRegex(RuntimeError, "unexpected configured final-link tail"):
            plain.final_inputs(configured, path, "a.lib", "b.lib", include_resources=True)

    def test_transform_registry_names_exactly_the_three_reviewed_units(self):
        self.assertEqual(
            sorted(transforms.TRANSFORMS),
            ["BASE/AudiereEffects", "BASE/DIMMER", "BASE/Misc"],
        )
