"""Run finite-domain readability contracts with the real game headers and VC6.

Invoke inside the worktree's build shell with:
    python3 -m homm2.audit.readability_contracts
This is an executable integration check, separate from tool-only selftests.
"""

from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import unit_flags, units
from homm2.core.paths import REPO
from homm2.core.wine import msvc_dir, run, tool, winepath_w


def main() -> int:
    source = REPO / "scripts/homm2/audit/fixtures/readability_casefold.cpp"
    output = REPO / "build/readability/contracts"
    output.mkdir(parents=True, exist_ok=True)
    obj = output / "casefold.obj"
    exe = output / "casefold.exe"
    owner = next(row for row in units() if row["unit"] == "SOURCE/KB")
    rc, log, timed_out = run_compile(source, obj, unit_flags(owner), depfile=False)
    print(log, end="")
    if rc or timed_out:
        return rc or 1
    print(run(tool("link.exe"), "/nologo", "/subsystem:console",
              "/out:" + winepath_w(exe),
              "/libpath:" + winepath_w(msvc_dir() / "lib"),
              winepath_w(obj), "libcmt.lib", "kernel32.lib", cwd=output), end="")
    print(run(exe, cwd=output), end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
