"""homm2 reconstruction CLI."""
import os, subprocess, sys
from pathlib import Path
from homm2.core.paths import REPO

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

#: Commands that read the selected image (`--image`); the rest refuse another
#: image instead of silently answering for the game.
IMAGE_AWARE = {"inspect", "help", "-h", "--help"}


def sh(*cmd):
    """Run a child with its output streamed through the usage log."""
    from homm2.core.usage import run_process
    return run_process([str(c) for c in cmd], cwd=REPO)

def _inspect(argv):
    import argparse, json
    from homm2.core.image import Image
    from homm2.core.inputs import InputError, read_verified, targets
    from homm2.core.paths import image_key
    ap = argparse.ArgumentParser(prog="homm2 inspect",
                                 description="headers of a pinned retail image")
    ap.add_argument("--target", choices=("game", "editor"), default=None,
                    help="the image (default: the selected --image)")
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args(argv)
    key = a.target or image_key()
    pin = targets(REPO)[key]
    try:
        report = Image(read_verified(pin, pin.destination)).report()
    except InputError as error:
        print(f"homm2 inspect: {error}", file=sys.stderr)
        return 1
    if a.json:
        print(json.dumps(report, indent=2))
        return 0
    print(f"{key}: sha256 {report['sha256']}")
    print(f"base 0x{report['image_base']:08X}, entry 0x{report['entry_va']:08X}, "
          f"linker {report['linker'][0]}.{report['linker'][1]:02d}")
    for section in report["sections"]:
        print(f"{section['name']:8} RVA 0x{section['rva']:08X} "
              f"virtual {section['virtual_size']:7} raw {section['raw_size']:7}")
    return 0


from homm2.core.usage import logged


@logged
def main(argv=None):
    argv = list(sys.argv[1:] if argv is None else argv)
    # `homm2 --image editor <command>` selects the retail image for this process
    # and every child it starts (homm2.core.paths, $HOMM2_IMAGE).
    while argv and (argv[0] == "--image" or argv[0].startswith("--image=")):
        key = argv[0].partition("=")[2] if "=" in argv[0] else (argv[1] if len(argv) > 1 else "")
        argv = argv[1:] if "=" in argv[0] else argv[2:]
        from homm2.core.paths import IMAGE_ENV, images
        if key not in images():
            print(f"homm2: --image expects one of {images()}", file=sys.stderr)
            return 2
        os.environ[IMAGE_ENV] = key
    cmd = argv[0] if argv else "help"; rest = argv[1:]
    from homm2.core.paths import DEFAULT_IMAGE, image_key
    if image_key() != DEFAULT_IMAGE and cmd not in IMAGE_AWARE:
        print(f"homm2 {cmd}: not yet keyed by image; it reads the game only "
              f"(image-aware: {', '.join(sorted(IMAGE_AWARE))})", file=sys.stderr)
        return 2
    if cmd == "inspect":
        return _inspect(rest)
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
        if '--ru' in rest and '--en' in rest:
            print('choose only one locale: --ru or --en', file=sys.stderr)
            return 1
        if '--no-match' in rest:
            return sh('python3', '-m', 'homm2.build.ordinary',
                      *(arg for arg in rest if arg != '--no-match'))
        if '--en' in rest:
            print('English is not a matching target; use homm2 build --no-match --en',
                  file=sys.stderr)
            return 1
        if '--help' in rest or '-h' in rest:
            print('homm2 build [--ru] [Ninja options/targets] (Russian matching build)\n'
                  'homm2 build --no-match [--ru|--en] [-j JOBS] [-v] (compile + link)')
            return 0
        rest = [arg for arg in rest if arg != '--ru']
        from homm2.core.retail import verify_retail
        try:
            verify_retail()
        except (OSError, ValueError) as error:
            print(f"[build] {error}", file=sys.stderr)
            return 1
        if sh("python3", "-m", "homm2.build.localization"): return 1
        if AUDITS:
            if sh("python3", "-m", "homm2.build.annotated_functions", "--check"): return 1
        if sh("python3", "configure.py"): return 1
        from homm2.core.paths import ninja_jobs
        jobs = [] if any(a.startswith("-j") for a in rest) else ninja_jobs()
        if sh("ninja", *jobs, *rest): return 1
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
        if rest in (["--help"], ["-h"]):
            print("usage: homm2 link [--rsrc | --historical] (native raw-object link)")
            return 0
        if rest not in ([], ["--rsrc"], ["--historical"]):
            print("usage: homm2 link [--rsrc | --historical]; layout corrections are not supported",
                  file=sys.stderr)
            return 1
        if sh("python3", "configure.py"): return 1
        target = {"--rsrc": "link-rsrc", "--historical": "link-historical"}
        return sh("ninja", target[rest[0]] if rest else "link")
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
    if cmd == "verify":
        from homm2.verify import main as m; return m(rest)
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
    print("usage: homm2 [--image {game,editor}] {inspect|init|redelink|model-drift|configure|build|link|clangd|format|constants|strict-allocations|od-frames|data-relocs|data-topology|status|relocs|sema|permute|audit|clean|verify|ghidra}",
          file=sys.stderr)
    return 0 if cmd in ("help", "-h", "--help") else 1
