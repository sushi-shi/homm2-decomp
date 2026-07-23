"""homm2 init - the one-time setup that splits HEROES2W.EXE into target objects.

This does the heavy one-time work. It runs ONCE: minimal CodeView public anchors plus
validated reconstructed private ranges -> manifest/coverage audit -> ??_C@ string names -> synth
PDB -> vostok-delinker -> per-unit target objs -> configure. Needs `nix develop .#build`
(wine + MSVC for the ??_C@ string oracle). Idempotent: safe to re-run.
"""
import os, subprocess, sys
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))

def run(*cmd):
    print("[init]", " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run([str(c) for c in cmd], cwd=REPO).returncode

def main(argv=None):
    # 1. linker publics plus source-annotated private functions -> symbol manifest
    if run("python3", "scripts/gen_manifest.py", "build/orig/HEROES2W.EXE", "."): return 1
    if run("python3", "-m", "homm2.build.audit_text_coverage"): return 1
    # 2. give string-literal constants their real ??_C@ names (cl oracle)
    if run("python3", "scripts/name_strings.py"): return 1
    # 3. synthesize the PDB the delinker needs
    if run("python3", "-m", "homm2.build.synth_pdb"): return 1
    # 4. Candidate COFF topology must exist before source data claims can be bound.
    if run("python3", "configure.py"): return 1
    if run("ninja", "base"): return 1
    # 5. Source annotations plus candidate COFF topology deterministically regenerate
    # every Vostok data/section/contribution input.
    if run("python3", "-m", "homm2.build.reviewed_data", "--regenerate"): return 1
    # 6. Reconfigure now that canonical delinked targets exist.
    if run("python3", "configure.py"): return 1
    # 7. generate the clangd compile DB (editor tooling: resolves <va.h> + MSVC headers)
    if run("python3", "-m", "homm2.init.clangd"):
        print("[init] WARN: clangd DB step failed (editor-only; build is unaffected)")
    print("[init] done. Next: `homm2 build`")
    return 0
