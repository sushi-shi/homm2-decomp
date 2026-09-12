"""homm2 reconstruction CLI."""
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
        if sh("python3", "-m", "homm2.build.annotated_functions", "--check"): return 1
        if sh("python3", "configure.py"): return 1
        if sh("ninja", *rest): return 1
        # Relocation field validation consumes the objdiff report. Generate it
        # after Ninja has rebuilt every input so a clean build is self-contained.
        from homm2.match.status import load_report, main as st
        report = load_report()
        if report is None:
            return 1
        # Fast and warning-only: half-built TUs may intentionally need a later redelink.
        sh("python3", "-m", "homm2.build.symbol_model_drift")
        if sh("python3", "-m", "homm2.build.annotated_functions", "--check",
              "--objects", "build/objdiff/base"): return 1
        # These are unconditional gates, not an optional campaign switch. A score
        # is not evidence that declarations, data owners, or relocations are sound.
        for audit in ("assert_decls", "assert_no_fake_labels", "assert_globals_data",
                      "assert_defs_declared", "assert_globals_defined", "assert_vtables"):
            if sh("python3", "-m", "homm2.build." + audit): return 1
        # Unordered target identities plus ordered resolved sites/owner offsets.
        # Ordinary source functions use raw objects; compiler-generated identities
        # retain the audit's explicit normalized-name fallback.
        if sh("python3", "-m", "homm2.build.assert_relocs"): return 1
        if sh("python3", "-m", "homm2.build.assert_relocs", "--resolved"): return 1
        if sh("python3", "-m", "homm2.build.assert_fixed_width_ints"): return 1
        st(["--write-readme"], report)
        return st([], report)   # refresh README % block + print summary
    if cmd == "link":
        # Three modes: `link` (generic: source + reviewed ABI manifests only),
        # `link --rsrc` (the only ordinary mode that opens the retail exe: it
        # extracts the icon and verifies the reconstructed .rsrc payloads),
        # `link --transform` (adds the reviewed COFF transforms, four-pass
        # historical PDB, retail SHA assertion, and the strict image audit).
        if sh("python3", "configure.py"): return 1
        if "--transform" in rest:
            return sh("ninja", "link")
        targets = ["link-generic-inputs"]
        if "--rsrc" in rest:
            targets.append("link-resources")
        if sh("ninja", *targets): return 1
        return sh("python3", "-m", "homm2.build.exact_link.plain", *rest)
    if cmd == "relocs":
        # Also run by `build`; this entry point permits focused diagnosis.
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
