import importlib.util
import os
import subprocess
import tempfile
import unittest
from contextlib import redirect_stderr
from io import StringIO
from pathlib import Path

from homm2.core.paths import REPO


RUNNER = REPO / "scripts/homm2/clean/project/run-game.sh"
PROVISIONER = REPO / "scripts/toolchain/create-wine-prefix.py"
DELAY_TIL_MILLI_OFFSET = 498493
DELAY_TIL_MILLI = bytes.fromhex(
    "558bec51894dfce8af84ffff3945fc7e0ce8a67dffffe898c0feffebea8be55dc3"
)
PATCH_OFFSET = 498508


def load_provisioner():
    spec = importlib.util.spec_from_file_location("create_wine_prefix", PROVISIONER)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class WineLaunchContractTest(unittest.TestCase):
    def test_generic_runner_skips_intro_without_rewriting_the_executable(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            game = root / "game"
            game.mkdir()
            runner = game / "run-game.sh"
            runner.write_bytes(RUNNER.read_bytes())
            runner.chmod(0o755)

            original = b"portable-generated-executable"
            (game / "HMM2PL.exe").write_bytes(original)
            for middleware in ("audiere.dll", "MSS32.DLL", "SMACKW32.DLL", "WING32.DLL"):
                (game / middleware).touch()

            prefix = root / "prefix"
            (prefix / "drive_c").mkdir(parents=True)
            (prefix / "dosdevices").mkdir()
            cd = root / "cd"
            cd.mkdir()

            commands = root / "commands"
            commands.mkdir()
            log = root / "wine.log"
            (commands / "wine").write_text(
                "#!/bin/sh\n"
                "{ printf 'wine'; for argument do printf '\\t%s' \"$argument\"; done; "
                "printf '\\n'; } >> \"$HOMM2_TEST_WINE_LOG\"\n"
            )
            for command in ("wine", "wineboot", "wineserver"):
                path = commands / command
                if command != "wine":
                    path.write_text("#!/bin/sh\nexit 0\n")
                path.chmod(0o755)

            environment = dict(
                os.environ,
                PATH=f"{commands}:{os.environ['PATH']}",
                HOMM2_WINEPREFIX=str(prefix),
                HOMM2_CD_DIR=str(cd),
                HOMM2_EXE="HMM2PL.exe",
                HOMM2_SETUP_ONLY="0",
                HOMM2_TEST_WINE_LOG=str(log),
                NIRI_SOCKET="",
            )
            subprocess.run([runner, "/I1"], check=True, env=environment)

            self.assertEqual((game / "HMM2PL.exe").read_bytes(), original)
            self.assertFalse((game / ".wine-compat").exists())

            launch = log.read_text().splitlines()[-1].split("\t")
            self.assertEqual(
                launch,
                [
                    "wine",
                    "explorer",
                    "/desktop=HOMM2,640x480",
                    "HMM2PL.exe",
                    "/I0",
                    "/I1",
                ],
            )

    def test_provisioner_requires_the_resource_link_output(self):
        provisioner = load_provisioner()
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            game = root / "game"
            game.mkdir()
            generic = root / "build/link/HMM2PL.exe"
            generic.parent.mkdir(parents=True)
            generic.write_bytes(b"generic-or-transform-output")

            provisioner.REPO = root
            with redirect_stderr(StringIO()):
                with self.assertRaises(SystemExit):
                    provisioner.install_rebuilt_exe(game)
            self.assertFalse((game / "HMM2PL.exe").exists())

            resource_link = root / "build/link/rsrc/HMM2PL.exe"
            resource_link.parent.mkdir()
            executable = bytearray(DELAY_TIL_MILLI_OFFSET + len(DELAY_TIL_MILLI))
            executable[DELAY_TIL_MILLI_OFFSET:] = DELAY_TIL_MILLI
            resource_link.write_bytes(executable)
            provisioner.install_rebuilt_exe(game)
            provisioner.create_wine_compatibility_exe(game)

            original = (game / "HMM2PL.exe").read_bytes()
            compatible = (game / ".wine-compat/HMM2PL-WINE.exe").read_bytes()
            self.assertEqual(original, bytes(executable))
            differences = [
                offset
                for offset, (before, after) in enumerate(zip(original, compatible))
                if before != after
            ]
            self.assertEqual(differences, [PATCH_OFFSET])
            self.assertEqual((original[PATCH_OFFSET], compatible[PATCH_OFFSET]), (0x7E, 0x76))


if __name__ == "__main__":
    unittest.main()
