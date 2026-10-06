#!/usr/bin/env python3
"""homm2.ghidra.driver - PyGhidra driver + the `homm2 ghidra` command.

This target is stripped, so Ghidra's analysis IS the candidate function inventory:
config/retail/functions.csv started life as an export from this project, and
export_functions.py is the refresh path when analysis improves. Boundaries stay
analysis opinion - source VA() markers are what turn them into claims. The project
also:
  1. gives xref the WHOLE-.text function-boundary map, and
  2. backs `python3 -m homm2.sema.decomp` with our names applied so the C reads well.

`homm2 ghidra` boots PyGhidra in-process (CPython3 + JPype), imports the selected
image's retail executable into a cached project (build/ghidra/homm2.{gpr,rep}; another
image under build/<image>/ghidra), auto-analyzes it once (SEVERAL MINUTES; skipped on
re-runs), then runs two GhidraScripts:
  - apply_names.py      : create a function at every symbol_names.csv RVA Ghidra missed and
                          apply the source-claimed name - "set the symbols we know".
  - export_functions.py : dump <ghidra>/exports/functions.csv (entry_rva,byte_size,name).

`homm2 --image editor ghidra` and `homm2 --image editor sema decomp` work on EDT2PL.exe.

Re-run `homm2 ghidra --no-analyze` to re-apply/re-export instantly (no re-analysis).
The decomp module reopens the same project without analysis and runs decomp_export.py.

Needs the dev shell's Ghidra env (GHIDRA_INSTALL_DIR / JAVA_HOME) and the pyghidra-carrying
python - both provided by `nix develop` (see flake.nix).
"""
import os
import sys
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
from homm2.core.paths import gen_dir, image_build, image_key, retail_exe

IMAGE = image_key()
EXE = retail_exe(IMAGE)  # honours the pin's own environment variable
PROJ_DIR = image_build(IMAGE) / "ghidra"
PROJ_NAME = "homm2" if IMAGE == "game" else f"homm2-{IMAGE}"
SYMBOLS_CSV = gen_dir(IMAGE) / "symbol_names.csv"
SCRIPTS_DIR = Path(__file__).resolve().parent / "scripts"
APPLY_NAMES = SCRIPTS_DIR / "apply_names.py"
EXPORT_FUNCS = SCRIPTS_DIR / "export_functions.py"
DECOMP_EXPORT = SCRIPTS_DIR / "decomp_export.py"
FUNCTIONS_CSV = PROJ_DIR / "exports" / "functions.csv"


def _project_exists() -> bool:
    return (PROJ_DIR / f"{PROJ_NAME}.rep").is_dir()


def _preflight() -> None:
    if not os.environ.get("GHIDRA_INSTALL_DIR"):
        sys.exit("[homm2 ghidra] GHIDRA_INSTALL_DIR unset - enter the dev shell (`nix develop`)")
    if not EXE.is_file():
        sys.exit(f"[homm2 ghidra] target EXE not found: {EXE} (run `homm2 inputs`)")
    try:
        import pyghidra  # noqa: F401
    except Exception as e:
        sys.exit(f"[homm2 ghidra] pyghidra not importable ({e}) - enter the dev shell (`nix develop`)")


def run_scripts(scripts, analyze: bool) -> int:
    """Boot PyGhidra, open (import/reuse) the project, optionally auto-analyze, then run
    each GhidraScript in order with currentProgram bound. Persists the project on exit."""
    _preflight()
    PROJ_DIR.mkdir(parents=True, exist_ok=True)
    # The GhidraScripts read the image's names and write its exports here.
    os.environ["HOMM2_GHIDRA_SYMBOLS"] = str(SYMBOLS_CSV)
    os.environ["HOMM2_GHIDRA_EXPORTS"] = str(PROJ_DIR / "exports")
    import pyghidra
    pyghidra.start()
    # _setup_project is what open_program calls internally; it imports the EXE (or reuses an
    # existing program of the same name) and hands back the GhidraProject + Program, and
    # GhidraProject.getProject() is the framework Project that ghidra_script wants.
    from pyghidra.core import _setup_project, _analyze_program
    from ghidra.app.script import GhidraScriptUtil
    from ghidra.program.flatapi import FlatProgramAPI

    gproject, program = _setup_project(
        binary_path=str(EXE), project_location=str(PROJ_DIR),
        project_name=PROJ_NAME, nested_project_location=False)
    project = gproject.getProject()

    GhidraScriptUtil.acquireBundleHostReference()
    try:
        if analyze:
            # Aggressive Instruction Finder (OFF by default) disassembles code in
            # unreferenced gaps, recovering .text function boundaries plain
            # reference-following misses. Costs ~3-4x the analysis phase, once.
            from ghidra.program.model.listing import Program
            opts = program.getOptions(Program.ANALYSIS_PROPERTIES)
            tx = program.startTransaction("enable-aggressive-instruction-finder")
            try:
                opts.setBoolean("Aggressive Instruction Finder", True)
            finally:
                program.endTransaction(tx, True)
            _analyze_program(FlatProgramAPI(program), program)  # only if not yet analyzed
        for script in scripts:
            print(f"[homm2 ghidra] running {Path(script).name} ...", flush=True)
            pyghidra.ghidra_script(str(script), project, program=program)
    finally:
        GhidraScriptUtil.releaseBundleHostReference()
        gproject.save(program)
        gproject.close()
    return 0


from homm2.core.usage import logged


@logged
def cli_main(argv) -> int:
    """`homm2 ghidra [--analyze|--no-analyze]` - build/refresh the cached project + export
    functions.csv. First run imports + auto-analyzes (minutes); afterwards it reuses the DB."""
    argv = list(argv)
    if "-h" in argv or "--help" in argv:
        print("usage: homm2 ghidra [--analyze | --no-analyze]\n" + __doc__)
        return 0

    if "--no-analyze" in argv:
        analyze = False
    elif "--analyze" in argv:
        analyze = True
    else:
        analyze = not _project_exists()  # analyze once on first build
    if analyze:
        print(f"[homm2 ghidra] importing + auto-analyzing {EXE.name} (SEVERAL MINUTES, "
              "one-time) ...", flush=True)
    else:
        print("[homm2 ghidra] reusing analyzed project (--no-analyze) ...", flush=True)
    rc = run_scripts([APPLY_NAMES, EXPORT_FUNCS], analyze)
    if rc == 0 and FUNCTIONS_CSV.is_file():
        n = sum(1 for _ in FUNCTIONS_CSV.open()) - 1
        print(f"[homm2 ghidra] done - {n} function boundaries -> "
              f"{FUNCTIONS_CSV.relative_to(REPO)}")
        print("[homm2 ghidra] xref now attributes library callers; "
              "`python3 -m homm2.sema.decomp <rva>` ready.")
    return rc


if __name__ == "__main__":
    sys.exit(cli_main(sys.argv[1:]))
