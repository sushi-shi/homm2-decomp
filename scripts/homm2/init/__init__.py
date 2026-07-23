"""First-time setup: redelink the retail image, then configure editor tooling.

The delinking pipeline itself lives under ``homm2 redelink`` and is safe to rerun.
"""
import os, subprocess
from pathlib import Path
from homm2.redelink import main as redelink
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))

def run(*cmd):
    print("[init]", " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run([str(c) for c in cmd], cwd=REPO).returncode

def main(argv=None):
    if argv:
        print("usage: homm2 init")
        return 1
    if redelink([]):
        return 1
    if run("python3", "-m", "homm2.init.clangd"):
        print("[init] WARN: clangd DB step failed (editor-only; build is unaffected)")
    print("[init] done. Next: `homm2 build`")
    return 0
