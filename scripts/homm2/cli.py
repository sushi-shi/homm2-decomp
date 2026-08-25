"""homm2 reconstruction CLI."""
import os, subprocess, sys
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[2]))

# Every audit is off while the reconstruction is unmarked. They were written for a
# COMPLETE inventory, and here the inventory starts empty and grows one proven
# address at a time, so the same checks report the whole image as broken and say
# nothing: audit_text_coverage calls all 951,827 bytes of .text unexplained, and
# every assert_* keyed on a symbol model has no model to read.
#
# They come back on as the campaign earns them, and several could return early: the
# source-only ones (assert_decls, assert_defs_declared, assert_globals_defined,
# assert_no_fake_labels, assert_fixed_width_ints) check the tree against itself and
# do not depend on the target at all.
AUDITS = False

def sh(*cmd):
    return subprocess.run([str(c) for c in cmd], cwd=REPO).returncode

def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    cmd = argv[0] if argv else "help"; rest = argv[1:]
    if cmd == "init":
        from homm2.init import main as m; return m(rest)
    if cmd == "redelink":
        from homm2.redelink import main as m; return m(rest)
    if cmd == "model-drift":
        from homm2.build.symbol_model_drift import main as m; return m(rest)
    if cmd == "configure":
        return sh("python3", "configure.py")
    if cmd == "clangd":
        from homm2.init.clangd import main as m; return m()
    if cmd == "format":
        if any(argument != "--check" for argument in rest) or len(rest) > 1:
            print("usage: homm2 format [--check]", file=sys.stderr)
            return 1
        headers = sorted(REPO.glob("include/**/*.h"))
        sources = sorted(REPO.glob("src/**/*.cpp"))
        header_status = sh(
            "python3", "-m", "homm2.format.headers", *rest, *headers)
        enum_status = sh(
            "python3", "-m", "homm2.format.enums", *rest, *headers, *sources)
        return int(bool(header_status or enum_status))
    if cmd == "constants":
        from homm2.constants_audit import main as m; return m(rest)
    if cmd == "strict-allocations":
        from homm2.build.strict_allocations import main as m; return m(rest)
    if cmd == "od-frames":
        from homm2.build.od_frame_audit import main as m; return m(rest)
    if cmd == "data-relocs":
        from homm2.build.coff_reloc_topology import main as m; return m(rest)
    if cmd == "data-topology":
        # Target regeneration lives in `homm2 redelink`; the census is the
        # candidate-COFF inspection tool that stays meaningful without it.
        if rest and rest[0] == "census":
            from homm2.build.data_topology_census import main as m
            return m(rest[1:])
        print("usage: homm2 data-topology census", file=sys.stderr)
        return 1
    if cmd == "build":
        if AUDITS:
            if sh("python3", "-m", "homm2.build.annotated_functions", "--check"): return 1
        if sh("python3", "configure.py"): return 1
        if sh("ninja", *rest): return 1
        # Relocation field validation consumes the objdiff report. Generate it
        # after Ninja has rebuilt every input so a clean build is self-contained.
        from homm2.match.status import load_report, main as st
        report = load_report()
        if report is None:
            return 1
        if AUDITS:
            # Fast and warning-only: half-built TUs may intentionally need a later redelink.
            sh("python3", "-m", "homm2.build.symbol_model_drift")
            if sh("python3", "-m", "homm2.build.annotated_functions", "--check",
                  "--objects", "build/objdiff/base"): return 1
            # HARD gates: every declaration comes from a header (no drift), and every emitted
            # function symbol exists in the retained-public/recovered-private inventory.
            if sh("python3", "-m", "homm2.build.assert_decls"): return 1
            if sh("python3", "-m", "homm2.build.assert_no_fake_labels"): return 1
            if sh("python3", "-m", "homm2.build.assert_globals_data"): return 1
            if sh("python3", "-m", "homm2.build.assert_defs_declared"): return 1
            if sh("python3", "-m", "homm2.build.assert_globals_defined"): return 1
            if sh("python3", "-m", "homm2.build.assert_vtables"): return 1
            if sh("python3", "-m", "homm2.build.assert_relocs", "--fields"): return 1
            if sh("python3", "-m", "homm2.build.assert_fixed_width_ints"): return 1
        st(["--write-readme"], report)
        return st([], report)   # refresh README % block + print summary
    if cmd == "link":
        # Three modes: `link` (generic: raw objects, no retail resources),
        # `link --rsrc` (adds .rsrc extracted from build/orig/HMM2PL.exe),
        # `link --transform` (adds the reviewed COFF transforms, four-pass
        # historical PDB, retail SHA assertion, and the strict image audit).
        if sh("python3", "configure.py"): return 1
        if "--transform" in rest:
            return sh("ninja", "link")
        targets = ["link-inputs"]
        if "--rsrc" in rest:
            targets.append("link-resources")
        if sh("ninja", *targets): return 1
        return sh("python3", "-m", "homm2.build.exact_link.plain", *rest)
    if cmd == "relocs":
        # OPT-IN reloc-target audit (NOT a hard build gate): objdiff masks every relocation, so a
        # 100%-exact fn can silently read the wrong global/field or call a fabricated fn. This checks
        # each near-exact fn's reloc targets against retail. Off by default because it also surfaces
        # incomplete-function relocation shape. `homm2 relocs 0x<rva>` reviews one.
        return sh("python3", "-m", "homm2.build.assert_relocs", *rest)
    if cmd == "status":
        from homm2.match.status import main as st; return st(rest)
    if cmd == "sema":
        from homm2.analysis.sema import main as m; return m(rest)
    if cmd == "selftest":
        from homm2.selftest import main as m; return m(rest)
    if cmd == "clean":
        # Derive the shipped tree from the matching tree; see homm2/clean/__init__.py.
        from homm2.clean.clean_source import main as m; return m(rest)
    if cmd == "audit":
        # On-demand campaign diagnostics. NOT build gates - those are the assert_*
        # modules run inside `homm2 build`. No argument lists the tools.
        from homm2.audit import main as m; return m(rest)
    if cmd == "permute":
        # The measured source-variant search; see homm2/permute/__init__.py for the
        # layering. This is the frontend, never a lower stage.
        from homm2.permute.match_variants import main as m; return m(rest)
    if cmd == "ghidra":
        from homm2.ghidra.driver import cli_main as m; return m(rest)
    print("usage: homm2 {init|redelink|model-drift|configure|build|link|clangd|format|constants|strict-allocations|od-frames|data-relocs|data-topology|status|relocs|sema|permute|audit|clean|selftest|ghidra}",
          file=sys.stderr)
    return 0 if cmd in ("help", "-h", "--help") else 1
