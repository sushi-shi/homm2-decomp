#!/usr/bin/env python3
"""homm2.ghidra.driver - PyGhidra driver + the `homm2 ghidra` command.

homm2's Ghidra use is MINIMAL, read-only, one-time and cached - CodeView is authoritative
so Ghidra never *discovers* names. It exists only to:
  1. give xref the WHOLE-.text function-boundary map (incl. the library/runtime funcs
     CodeView omits), and
  2. back `homm2 sema decomp` (the decompiler), with OUR names applied so the C reads well.

`homm2 ghidra` boots PyGhidra in-process (CPython3 + JPype), imports HEROES2W.EXE into a
cached project (build/ghidra/homm2.{gpr,rep}), auto-analyzes it once (SEVERAL MINUTES;
skipped on re-runs), then runs two GhidraScripts:
  - apply_names.py      : create a function at every symbol_names.csv RVA Ghidra missed and
                          apply our (demangled) CodeView name - "set the symbols we know".
  - export_functions.py : dump build/ghidra/exports/functions.csv (entry_rva,byte_size,name).

Re-run `homm2 ghidra --no-analyze` to re-apply/re-export instantly (no re-analysis).
`sema decomp` reopens the same project with --no-analyze and runs decomp_export.py.

Needs the dev shell's Ghidra env (GHIDRA_INSTALL_DIR / JAVA_HOME) and the pyghidra-carrying
python - both provided by `nix develop` (see flake.nix).
"""
import os
import sys
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
EXE = Path(os.environ.get("HOMM2_EXE") or REPO / "build/orig/HEROES2W.EXE")
PROJ_DIR = REPO / "build/ghidra"
PROJ_NAME = "homm2"
SCRIPTS_DIR = Path(__file__).resolve().parent / "scripts"
APPLY_NAMES = SCRIPTS_DIR / "apply_names.py"
EXPORT_FUNCS = SCRIPTS_DIR / "export_functions.py"
DECOMP_EXPORT = SCRIPTS_DIR / "decomp_export.py"
RECOVER_STRUCTS = SCRIPTS_DIR / "recover_structs.py"
FUNCTIONS_CSV = PROJ_DIR / "exports" / "functions.csv"
STRUCT_SUMMARY_CSV = PROJ_DIR / "exports" / "struct_summary.csv"


def _project_exists() -> bool:
    return (PROJ_DIR / f"{PROJ_NAME}.rep").is_dir()


def _preflight() -> None:
    if not os.environ.get("GHIDRA_INSTALL_DIR"):
        sys.exit("[homm2 ghidra] GHIDRA_INSTALL_DIR unset - enter the dev shell (`nix develop`)")
    if not EXE.is_file():
        sys.exit(f"[homm2 ghidra] target EXE not found: {EXE} (copy HEROES2W.EXE into build/orig/)")
    try:
        import pyghidra  # noqa: F401
    except Exception as e:
        sys.exit(f"[homm2 ghidra] pyghidra not importable ({e}) - enter the dev shell (`nix develop`)")


def run_scripts(scripts, analyze: bool) -> int:
    """Boot PyGhidra, open (import/reuse) the project, optionally auto-analyze, then run
    each GhidraScript in order with currentProgram bound. Persists the project on exit."""
    _preflight()
    PROJ_DIR.mkdir(parents=True, exist_ok=True)
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
            # unreferenced gaps, recovering the extra .text function boundaries CodeView
            # doesn't list. Costs ~3-4x the analysis phase, paid once on this path only.
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


def cli_main(argv) -> int:
    """`homm2 ghidra [--analyze|--no-analyze]` - build/refresh the cached project + export
    functions.csv. First run imports + auto-analyzes (minutes); afterwards it reuses the DB."""
    argv = list(argv)
    if "-h" in argv or "--help" in argv:
        print("usage: homm2 ghidra [--analyze | --no-analyze] [--structs]\n" + __doc__)
        return 0

    # --structs: decompiler-backed class LAYOUT recovery (FillOutStructureHelper). Reuses the
    # analyzed DB (analyze only if never built), re-applies our CodeView names (idempotent) so
    # `this`/__thiscall are set, then runs recover_structs.py -> struct_{layouts,summary}.csv.
    # Read-only w.r.t. the DB (every probe transaction is rolled back).
    if "--structs" in argv:
        analyze = ("--analyze" in argv) or not _project_exists()
        if analyze:
            print("[homm2 ghidra] importing + auto-analyzing HEROES2W.EXE (SEVERAL MINUTES, "
                  "one-time) ...", flush=True)
        else:
            print("[homm2 ghidra --structs] reusing analyzed project; recovering class "
                  "layouts ...", flush=True)
        rc = run_scripts([APPLY_NAMES, RECOVER_STRUCTS], analyze)
        if rc == 0 and STRUCT_SUMMARY_CSV.is_file():
            n = sum(1 for _ in STRUCT_SUMMARY_CSV.open()) - 1
            print("[homm2 ghidra --structs] done - %d classes -> %s (+ struct_layouts.csv)"
                  % (n, STRUCT_SUMMARY_CSV.relative_to(REPO)))
        return rc

    if "--no-analyze" in argv:
        analyze = False
    elif "--analyze" in argv:
        analyze = True
    else:
        analyze = not _project_exists()  # analyze once on first build
    if analyze:
        print("[homm2 ghidra] importing + auto-analyzing HEROES2W.EXE (SEVERAL MINUTES, "
              "one-time) ...", flush=True)
    else:
        print("[homm2 ghidra] reusing analyzed project (--no-analyze) ...", flush=True)
    rc = run_scripts([APPLY_NAMES, EXPORT_FUNCS], analyze)
    if rc == 0 and FUNCTIONS_CSV.is_file():
        n = sum(1 for _ in FUNCTIONS_CSV.open()) - 1
        print(f"[homm2 ghidra] done - {n} function boundaries -> "
              f"{FUNCTIONS_CSV.relative_to(REPO)}")
        print("[homm2 ghidra] xref now attributes library callers; `homm2 sema decomp <rva>` ready.")
    return rc


if __name__ == "__main__":
    sys.exit(cli_main(sys.argv[1:]))
