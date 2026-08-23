import unittest

from homm2.build.exact_link import plain


class PlainExactLinkTests(unittest.TestCase):
    def test_final_inputs_keep_only_the_reviewed_preparations(self):
        configured = [
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
        request = plain.ROOT / "build/link/bss-layout-all/SOURCE/REQUEST.obj"
        prefix = plain.ROOT / "build/link/plain-inputs/BASE-prefix.lib"
        result = plain.final_inputs(configured, request, prefix)

        self.assertEqual(
            result[:3],
            [
                "/NODEFAULTLIB:LIBCMT",
                "/NODEFAULTLIB:LIBCPMT",
                "/NODEFAULTLIB:OLDNAMES",
            ],
        )
        self.assertEqual(result[3], "build/objdiff/base/SOURCE/ADVMGR.obj")
        self.assertEqual(result[4], "build/link/bss-layout-all/SOURCE/REQUEST.obj")
        self.assertEqual(result[6:9], ["OLDNAMES.LIB", "WINMM.LIB", "build/link/wing32.lib"])
        self.assertEqual(
            result[-6:],
            [
                "build/link/plain-inputs/BASE-prefix.lib",
                "build/link/Midi.lib",
                "build/link/BASE-suffix.lib",
                "MSVCPRT.LIB",
                "LIBCMT.LIB",
                "build/link/HMM2PL.res",
            ],
        )

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
            plain.final_inputs(configured, path, path)


if __name__ == "__main__":
    unittest.main()
