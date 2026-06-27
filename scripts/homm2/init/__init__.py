"""homm2 init - the one-time setup that splits HEROES2W.EXE into target objects.

This does the heavy one-time work. Since our CodeView symbols are
complete + frozen, it runs ONCE: CodeView -> manifest -> ??_C@ string names -> synth
PDB -> vostok-delinker -> per-unit target objs -> configure. Needs `nix develop .#build`
(wine + MSVC for the ??_C@ string oracle). Idempotent: safe to re-run.
"""
import os, shutil, subprocess, sys
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))

def run(*cmd):
    print("[init]", " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run([str(c) for c in cmd], cwd=REPO).returncode

def main(argv=None):
    # 1. manifest straight from CodeView (units.toml + symbol_names.csv) - cl-free
    if run("python3", "scripts/gen_manifest.py", "build/orig/HEROES2W.EXE", "."): return 1
    # 2. give string-literal constants their real ??_C@ names (cl oracle)
    if run("python3", "scripts/name_strings.py"): return 1
    # 3. synthesize the PDB the delinker needs
    if run("python3", "-m", "homm2.build.synth_pdb"): return 1
    # 4. delink HEROES2W.EXE -> per-unit COFF target objects (folder-structured)
    shutil.rmtree(REPO / "build/delink", ignore_errors=True); (REPO / "build/delink").mkdir(parents=True)
    if run("vostok-delinker", "--pdb-path", "build/pdb/HEROES2W.pdb", "--exe-path", "build/orig/HEROES2W.EXE",
           "--output-path", "build/delink", "--engine-path", "c:\\proj\\"): return 1
    # 5. configure the base build + objdiff project
    if run("python3", "configure.py"): return 1
    # 6. generate the clangd compile DB (editor tooling: resolves <va.h> + MSVC headers)
    if run("python3", "-m", "homm2.init.clangd"):
        print("[init] WARN: clangd DB step failed (editor-only; build is unaffected)")
    print("[init] done. Next: `homm2 build`")
    return 0
