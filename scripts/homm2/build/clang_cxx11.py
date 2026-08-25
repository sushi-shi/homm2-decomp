"""Advisory whole-tree Clang C++11 warning pass (no objects; diagnostics only).

Every TU is syntax-checked as C++20 with -Wall -Wextra (register mapped away; C++20 adds the
deprecated cross-enum/enum-float arithmetic diagnostics). Accepted repo-wide
classes are suppressed: writable-strings (retail-pinned char*
signatures), nonportable-include-path (retail-era include casing),
unused-parameter and missing-field-initializers (reconstruction style).
Run from the repo root inside `nix develop .#build`:
    python -m homm2.build.clang_cxx11 [--tu <substring>] [--errors-only]
"""

from __future__ import annotations

import json
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from homm2.clang_options import ClangMode

FLAGS = [
    "-fsyntax-only", ClangMode.STRICT.driver_flag, "--target=i386-pc-windows-msvc",
    "-fms-extensions", "-fms-compatibility", "-fms-compatibility-version=12.00",
    "-ferror-limit=5",
    "-Wall", "-Wextra", "-Dregister=",
    "-Wno-writable-strings", "-Wno-nonportable-include-path",
    "-Wno-unused-parameter", "-Wno-missing-field-initializers",
]


def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    only = argv[argv.index("--tu") + 1] if "--tu" in argv else None
    errors_only = "--errors-only" in argv
    root = Path.cwd()
    db = json.loads((root / "build/clangd/compile_commands.json").read_text())
    include = [
        "-I", "include",
        "-isystem", "build/clangd/inc-lower/msvc",
        "-isystem", "build/toolchain/msvc/include",
    ]

    def vendor(args):
        out = []
        for i, a in enumerate(args):
            if a in ("/I", "/imsvc") and i + 1 < len(args) and "vendor" in args[i + 1]:
                out += ["-isystem", args[i + 1]]
        return out

    jobs = []
    for e in db:
        f = e["file"]
        if only and only not in f:
            continue
        jobs.append((f, ["clang++", *FLAGS, *include, *vendor(e["arguments"]), f]))

    def run(job):
        f, cmd = job
        r = subprocess.run(cmd, capture_output=True, text=True)
        return f, r.returncode, r.stderr

    failed = 0
    warnings = 0
    for f, rc, err in ThreadPoolExecutor(12).map(run, jobs):
        warnings += err.count("warning:")
        if rc != 0:
            failed += 1
        if err.strip() and (not errors_only or rc != 0):
            sys.stdout.write(err)
    print("[clang-strict] units: %d  failed: %d  warnings: %d" % (len(jobs), failed, warnings))
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
