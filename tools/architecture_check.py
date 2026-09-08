#!/usr/bin/env python3
"""Compile core game units for another architecture and run the save codecs.

This deliberately stops short of claiming that the SDL game runs on that host.
The compiler, emulator, and libbz2 headers come from the invoking toolchain.
"""

from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import shlex
import subprocess


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--runner", required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--jobs", type=int, default=1)
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    compiler = shlex.split(args.cxx)
    flags = ["-std=c++20", "-O2", "-Wall", "-Werror", "-Wno-invalid-offsetof",
             "-Wno-class-memaccess",  # Matches the game target's GCC policy.
             "-D__cdecl=", "-D__stdcall=", "-D__fastcall=",
             "-I" + str(root / "include"), "-I" + str(root)]
    sources = sorted(path for directory in ("BASE", "SOURCE", "EDITOR")
                     for path in (root / "src" / directory).glob("*.cpp"))
    if not sources:
        raise RuntimeError("no game translation units found")

    def compile_source(source: Path) -> None:
        relative = source.relative_to(root)
        object_path = output / relative.with_suffix(".o")
        object_path.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(compiler + flags + ["-c", str(source), "-o", str(object_path)], check=True)

    with ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        list(pool.map(compile_source, sources))
    print(f"Compiled {len(sources)} core game translation units", flush=True)
    executable = output / "save-records-test"
    subprocess.run(compiler + flags + [str(root / "tools/save_records_test.cpp"),
        str(output / "src/SOURCE/SaveRecords.o"), "-o", str(executable)], check=True)
    subprocess.run(shlex.split(args.runner) + [str(executable)], check=True)
    print("Save record checks passed under the target emulator", flush=True)
    message_test = output / "message-result-test"
    subprocess.run(compiler + flags + [str(root / "tools/message_result_test.cpp"),
        "-o", str(message_test)], check=True)
    subprocess.run(shlex.split(args.runner) + [str(message_test)], check=True)
    print("Dialog result checks passed under the target emulator", flush=True)


if __name__ == "__main__":
    main()
