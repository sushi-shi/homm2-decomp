#!/usr/bin/env python3
"""Run a native engine regression against an existing CMake/Ninja build.

Reuse the game's compile/link commands, replacing only its POSIX main. Each
test gets its own executable, game data, and user files. No installed game
assets are required. Assertions remain enabled in release builds.
"""

import json
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile


def main():
    build = Path(sys.argv[1]).resolve()
    source = Path(sys.argv[2]).resolve()
    commands = json.loads((build / "compile_commands.json").read_text())
    entry = next(c for c in commands if c["file"].endswith("/EXECUTABLE/POSIX/Main.cpp"))
    with tempfile.TemporaryDirectory(prefix="homm2-engine-test-") as directory:
        temp = Path(directory)
        obj = temp / "test.o"
        executable = temp / "test"
        args = shlex.split(entry["command"])
        main_object = args[args.index("-o") + 1]
        args[args.index("-o") + 1] = str(obj)
        args[args.index(entry["file"])] = str(source)
        args += ["-UNDEBUG", "-fno-access-control"]
        subprocess.run(args, cwd=entry["directory"], check=True)
        link = subprocess.check_output(
            ["ninja", "-C", str(build), "-t", "commands", "homm2"], text=True
        ).splitlines()[-1]
        if link.count(main_object) != 1 or " -o homm2 " not in link:
            raise RuntimeError("Unrecognized native game link command")
        link = link.replace(main_object, shlex.quote(str(obj)))
        link = link.replace(" -o homm2 ", " -o " + shlex.quote(str(executable)) + " ")
        subprocess.run(link, cwd=build, shell=True, check=True)
        data = temp / "data"
        modules = data / "SCRIPTS" / "MODULES"
        modules.mkdir(parents=True)
        binding = Path(__file__).resolve().parent.parent / "scripts" / "ironfist" / "binding.lua"
        (modules / "binding.lua").write_text(binding.read_text())
        env = dict(os.environ, HOMM2_DATA=str(data), XDG_DATA_HOME=str(temp / "user"),
                   SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
        subprocess.run([str(executable)], cwd=temp, env=env, check=True, timeout=30)


if __name__ == "__main__":
    main()
