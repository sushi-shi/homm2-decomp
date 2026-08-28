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
            (commands / "wineserver").write_text(
                "#!/bin/sh\n"
                "{ printf 'wineserver'; for argument do printf '\\t%s' \"$argument\"; done; "
                "printf '\\n'; } >> \"$HOMM2_TEST_WINE_LOG\"\n"
            )
            for command in ("wine", "wineboot", "wineserver"):
                path = commands / command
                if command == "wineboot":
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

            commands_run = [line.split("\t") for line in log.read_text().splitlines()]
            launch = [command for command in commands_run if command[0] == "wine"][-1]
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
            self.assertEqual(commands_run[-1], ["wineserver", "-k"])

            (commands / "wine").write_text(
                "#!/bin/sh\n"
                "{ printf 'wine'; for argument do printf '\\t%s' \"$argument\"; done; "
                "printf '\\n'; } >> \"$HOMM2_TEST_WINE_LOG\"\n"
                "if [ \"${1:-}\" = explorer ]; then exit 37; fi\n"
            )
            failed = subprocess.run([runner], check=False, env=environment)
            self.assertEqual(failed.returncode, 37)
            self.assertEqual(
                log.read_text().splitlines()[-1].split("\t"),
                ["wineserver", "-k"],
            )

    def test_provisioner_installs_the_resource_link_output_without_transforming_it(self):
        provisioner = load_provisioner()
        provisioner_source = PROVISIONER.read_text()
        self.assertNotIn(".wine-compat", provisioner_source)
        self.assertNotIn("HOMM2_BUKA_WINE_TICK_PATCH", provisioner_source)
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
            executable = b"ordinary-resource-link-output"
            resource_link.write_bytes(executable)
            provisioner.install_rebuilt_exe(game)
            self.assertEqual((game / "HMM2PL.exe").read_bytes(), executable)
            self.assertFalse((game / ".wine-compat").exists())


if __name__ == "__main__":
    unittest.main()
