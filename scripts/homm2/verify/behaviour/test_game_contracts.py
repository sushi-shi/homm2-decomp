"""Run game_contracts.cpp: the reconstructed game's predicates and formulas."""
import unittest
from pathlib import Path

from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import unit_flags, units
from homm2.core.paths import REPO
from homm2.core.wine import msvc_dir, run, tool, winepath_w

FIXTURE = Path(__file__).resolve().with_name("game_contracts.cpp")
EXPECTED = (
    "Enum contracts: notifications, dialog slots, payloads and gameplay flags pass",
    "CP1251: all 256 uppercase and 256 lowercase inputs pass",
    "Classification: all 256 signed-byte inputs for six creature sets and selection pass",
    "Sprite predicate: all 32768 tileset/flag/sentinel combinations pass",
    "Domain formulas: 2048 tent masks, 2001 hex/level inputs and 1089 delta pairs pass",
    "File values: scalar/record sizes, single operand evaluation, short read and EOF pass",
)


class GameContractsTest(unittest.TestCase):
    def test_game_contracts(self):
        output = REPO / "build/behaviour/game_contracts"
        output.mkdir(parents=True, exist_ok=True)
        obj, exe = output / "game_contracts.obj", output / "game_contracts.exe"
        exe.unlink(missing_ok=True)
        owner = next(row for row in units() if row["unit"] == "SOURCE/KB")
        rc, log, timed_out = run_compile(FIXTURE, obj, unit_flags(owner), depfile=False)
        self.assertFalse(rc or timed_out, log)
        run(tool("link.exe"), "/nologo", "/subsystem:console",
            "/out:" + winepath_w(exe), "/libpath:" + winepath_w(msvc_dir() / "lib"),
            winepath_w(obj), "libcmt.lib", "kernel32.lib", cwd=output, quiet=True)
        # wine.run raises on a non-zero exit: each failing check returns its own code.
        lines = run(exe, cwd=output, quiet=True).splitlines()
        self.assertEqual([line for line in lines if line.rstrip() in EXPECTED],
                         [line for line in lines if line.strip()], lines)
        self.assertEqual(len([line for line in lines if line.rstrip() in EXPECTED]),
                         len(EXPECTED))


if __name__ == "__main__":
    unittest.main()
