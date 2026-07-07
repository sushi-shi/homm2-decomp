"""homm2 CLI - python -m homm2 {init|configure|build|status}."""
import os, subprocess, sys
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[2]))

def sh(*cmd):
    return subprocess.run([str(c) for c in cmd], cwd=REPO).returncode

def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    cmd = argv[0] if argv else "help"; rest = argv[1:]
    if cmd == "init":
        from homm2.init import main as m; return m(rest)
    if cmd == "configure":
        return sh("python3", "configure.py")
    if cmd == "clangd":
        from homm2.init.clangd import main as m; return m()
    if cmd == "build":
        if sh("python3", "configure.py"): return 1
        if sh("ninja", *rest): return 1
        # HARD gates: every declaration comes from a header (no drift), and every emitted
        # function symbol exists in CodeView (no invented/fake labels).
        if sh("python3", "-m", "homm2.build.assert_decls"): return 1
        if sh("python3", "-m", "homm2.build.assert_no_fake_labels"): return 1
        if sh("python3", "-m", "homm2.build.assert_globals_data"): return 1
        if sh("python3", "-m", "homm2.build.assert_defs_declared"): return 1
        if sh("python3", "-m", "homm2.build.assert_globals_defined"): return 1
        if sh("python3", "-m", "homm2.build.assert_vtables"): return 1
        from homm2.match.status import main as st
        st(["--write-readme"]); return st([])   # refresh README % block + print summary
    if cmd == "status":
        from homm2.match.status import main as st; return st(rest)
    if cmd == "sema":
        from homm2.analysis.sema import main as m; return m(rest)
    if cmd == "ghidra":
        from homm2.ghidra.driver import cli_main as m; return m(rest)
    print("usage: homm2 {init|configure|build|clangd|status|sema|ghidra}", file=sys.stderr)
    return 0 if cmd in ("help", "-h", "--help") else 1
