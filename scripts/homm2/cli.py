"""homm2 CLI - python -m homm2 {init|configure|build|link|status}."""
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
    if cmd == "enum-types":
        from homm2.enum_types import main as m; return m(rest)
    if cmd == "strict-allocations":
        from homm2.build.strict_allocations import main as m; return m(rest)
    if cmd == "data-relocs":
        from homm2.build.coff_reloc_topology import main as m; return m(rest)
    if cmd == "data-topology":
        if rest and rest[0] == "audit":
            from homm2.build.data_topology_audit import main as m
            return m(rest[1:])
        if rest and rest[0] == "census":
            from homm2.build.data_topology_census import main as m
            return m(rest[1:])
        if rest and rest[0] == "assemble":
            from homm2.build.data_manifest_adapter import main as m
            return m(rest[1:])
        from homm2.build.reviewed_data import main as m
        if len(rest) != 1 or rest[0] not in (
                "propose", "promote", "finalize", "regenerate"):
            print("usage: homm2 data-topology "
                  "{audit|assemble|census|propose|promote|finalize|regenerate}",
                  file=sys.stderr)
            return 1
        return m(["--" + rest[0]])
    if cmd == "build":
        if sh("python3", "configure.py"): return 1
        if sh("ninja", *rest): return 1
        # HARD gates: every declaration comes from a header (no drift), and every emitted
        # function symbol exists in the retained-public/recovered-private inventory.
        if sh("python3", "-m", "homm2.build.assert_decls"): return 1
        if sh("python3", "-m", "homm2.build.assert_no_fake_labels"): return 1
        if sh("python3", "-m", "homm2.build.assert_globals_data"): return 1
        if sh("python3", "-m", "homm2.build.assert_defs_declared"): return 1
        if sh("python3", "-m", "homm2.build.assert_globals_defined"): return 1
        if sh("python3", "-m", "homm2.build.assert_vtables"): return 1
        if sh("python3", "-m", "homm2.build.assert_relocs", "--fields"): return 1
        if sh("python3", "-m", "homm2.build.assert_early_stop_bytes"): return 1
        if sh("python3", "-m", "homm2.build.assert_fixed_width_ints"): return 1
        from homm2.match.status import load_report, main as st
        report = load_report()
        if report is None:
            return 1
        st(["--write-readme"], report)
        return st([], report)   # refresh README % block + print summary
    if cmd == "link":
        if sh("python3", "configure.py"): return 1
        return sh("ninja", "link", *rest)
    if cmd == "link-layout":
        from homm2.build.link_layout_audit import main as m; return m(rest)
    if cmd == "relocs":
        # OPT-IN reloc-target audit (NOT a hard build gate): objdiff masks every relocation, so a
        # 100%-exact fn can silently read the wrong global/field or call a fabricated fn. This checks
        # each near-exact fn's reloc targets against retail. Off by default because it also surfaces
        # incomplete-function relocation shape. `homm2 relocs 0x<rva>` reviews one.
        return sh("python3", "-m", "homm2.build.assert_relocs", *rest)
    if cmd == "status":
        from homm2.match.status import main as st; return st(rest)
    if cmd == "breadth-audit":
        from homm2.match.breadth_audit import main as m; return m(rest)
    if cmd == "sema":
        from homm2.analysis.sema import main as m; return m(rest)
    if cmd == "ghidra":
        from homm2.ghidra.driver import cli_main as m; return m(rest)
    print("usage: homm2 {init|configure|build|link|link-layout|clangd|enum-types|strict-allocations|data-relocs|data-topology|status|breadth-audit|relocs|sema|ghidra}",
          file=sys.stderr)
    return 0 if cmd in ("help", "-h", "--help") else 1
