"""HoMM2 matching-decompilation command line.

    homm2 [--image {game,editor}] <command> [args]

The retail executables and compiler media cannot be fetched by the repository;
`init` stages and verifies them (config/retail/targets.json).
"""
import os
import sys

from homm2.core.paths import REPO

COMMANDS = ("init inspect toolchain configure build link match play labels model delink "
            "compare audit sema permute lsp ghidra verify workflow clean localization tool")

#: Older spellings kept while other branches and notes use them.
ALIASES = {
    "redelink": ["delink"],
    "status": ["verify", "status"],
    "relocs": ["verify", "relocs"],
    "data-topology": ["verify", "data-topology"],
    "constants": ["verify", "constants"],
    "strict-allocations": ["verify", "strict-allocations"],
    "od-frames": ["verify", "od-frames"],
    "data-relocs": ["verify", "data-relocs"],
    "model-drift": ["model"],
    "clangd": ["lsp", "compdb"],
    "format": ["workflow", "format"],
}

#: Commands that read the selected image (`--image`); the rest refuse another
#: image instead of silently answering for the game.
IMAGE_AWARE = {"inspect", "help", "-h", "--help"}

TOOLS = ("wine", "cl", "ml", "link", "rc", "objdiff", "delinker")


def sh(*cmd):
    """Run a child with its output streamed through the usage log."""
    from homm2.core.usage import run_process
    return run_process([str(c) for c in cmd], cwd=REPO)


def py(module, *args):
    return sh(sys.executable, "-m", module, *args)


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


def _toolchain(argv):
    import argparse
    ap = argparse.ArgumentParser(prog="homm2 toolchain",
                                 description="the pinned VC6 SP5 release in build/toolchain")
    ap.add_argument("action", choices=("install", "check"))
    ap.add_argument("--force", action="store_true", help="refetch over an existing tree")
    a = ap.parse_args(argv)
    from homm2.init.toolchain import main as toolchain
    if a.action == "check":
        return toolchain(["--check"])
    return toolchain(["--force"] if a.force else [])


def _build(rest):
    if rest[:1] == ["verify"]:
        if _build(rest[1:]):
            return 1
        from homm2.verify import run_tier
        return run_tier()
    if '--ru' in rest and '--en' in rest:
        print('choose only one locale: --ru or --en', file=sys.stderr)
        return 1
    if '--no-match' in rest:
        return py('homm2.build.ordinary', *(arg for arg in rest if arg != '--no-match'))
    if '--en' in rest:
        print('English is not a matching target; use homm2 build --no-match --en',
              file=sys.stderr)
        return 1
    if '--help' in rest or '-h' in rest:
        print('homm2 build [--ru] [Ninja options/targets]   Russian matching build\n'
              'homm2 build verify                          build, then every gate\n'
              'homm2 build --no-match [--ru|--en] [-j JOBS] [-v]   compile + link')
        return 0
    rest = [arg for arg in rest if arg != '--ru']
    from homm2.core.retail import verify_retail
    try:
        verify_retail()
    except (OSError, ValueError) as error:
        print(f"[build] {error}", file=sys.stderr)
        return 1
    if py("homm2.build.localization"):
        return 1
    if sh(sys.executable, "configure.py"):
        return 1
    from homm2.core.paths import ninja_jobs
    jobs = [] if any(a.startswith("-j") for a in rest) else ninja_jobs()
    if sh("ninja", *jobs, *rest):
        return 1
    # The report is generated after Ninja has rebuilt every input, so a clean
    # build is self-contained.
    from homm2.match.status import load_report, main as status
    report = load_report()
    if report is None:
        return 1
    status(["--write-readme"], report)
    return status([], report)


def _link(rest):
    if rest in (["--help"], ["-h"]):
        print("usage: homm2 link [--rsrc | --historical] (native raw-object link)")
        return 0
    if rest not in ([], ["--rsrc"], ["--historical"]):
        print("usage: homm2 link [--rsrc | --historical]; layout corrections are not supported",
              file=sys.stderr)
        return 1
    if sh(sys.executable, "configure.py"):
        return 1
    from homm2.core.paths import ninja_jobs
    target = {"--rsrc": "link-rsrc", "--historical": "link-historical"}
    return sh("ninja", *ninja_jobs(), target[rest[0]] if rest else "link")


def _match(rest):
    """Compile the selected units, refresh their comparison and print them."""
    import argparse
    from pathlib import Path
    from homm2.core.manifest import units
    ap = argparse.ArgumentParser(prog="homm2 match",
                                 description="the selected-unit compile and compare loop")
    ap.add_argument("unit", nargs="+", help="a unit (SOURCE/KB) or its source path")
    a = ap.parse_args(rest)
    known = {u["unit"] for u in units()}
    by_source = {str(Path(u["source"])): u["unit"] for u in units()}
    selected = []
    for spec in a.unit:
        name = spec if spec in known else None
        if name is None and Path(spec).exists():
            name = by_source.get(str(Path(spec).resolve().relative_to(REPO)))
        if name is None:
            print(f"homm2 match: {spec!r} is not a unit or unit source", file=sys.stderr)
            return 2
        selected.append(name)
    if sh(sys.executable, "configure.py"):
        return 1
    from homm2.core.paths import ninja_jobs
    targets = []
    for name in selected:
        targets.append(f"build/objdiff/normalized/base/{name}.obj")
        if (REPO / "build/delink" / f"{name}.c.obj").exists():
            targets.append(f"build/objdiff/normalized/target/{name}.c.obj")
    if sh("ninja", *ninja_jobs(), *targets):
        return 1
    from homm2.match.status import load_report
    if load_report() is None:
        return 1
    rc = 0
    for name in selected:
        rc |= py("homm2.analysis.sema", "match", name)
    return rc


def _play(rest):
    import argparse
    ap = argparse.ArgumentParser(prog="homm2 play",
                                 description="build, link with resources and run the game under Wine")
    ap.add_argument("--game", help="a legally obtained Buka installation ($HOMM2_DATA)")
    ap.add_argument("--prepare-only", action="store_true")
    a, game_arguments = ap.parse_known_args(rest)
    if a.game:
        os.environ["HOMM2_DATA"] = a.game
    return sh(sys.executable, "scripts/toolchain/run-rebuilt-game.py",
              *(["--prepare-only"] if a.prepare_only else []), *game_arguments)


def _permute(rest):
    verbs = {
        "variants": "homm2.permute.match_variants",
        "state": "homm2.permute.tu_state_noise",
        "emission-order": "homm2.permute.emission_order",
        "recover-residual": "homm2.permute.recover_residual_functions",
        "recover-historical": "homm2.permute.recover_historical_exact",
    }
    if not rest or rest[0] in ("-h", "--help"):
        print("homm2 permute variants <tu.cpp> <rva> [options]   reviewed axes x AST x TU state\n"
              "homm2 permute state --source <tu.cpp> --rva <rva> [options]   TU-state census\n"
              "homm2 permute emission-order [options]   emission-order probe campaigns\n"
              "homm2 permute recover-residual | recover-historical   queue drivers")
        return 0 if rest else 2
    if rest[0] in verbs:
        return py(verbs[rest[0]], *rest[1:])
    # The original spelling: `homm2 permute <tu.cpp> <rva> ...` is `variants`.
    return py(verbs["variants"], *rest)


def _lsp(rest):
    if not rest or rest[0] in ("-h", "--help"):
        print("homm2 lsp compdb                     write build/clangd/compile_commands.json\n"
              "homm2 lsp index | symbol QUERY | def|refs|hover FILE LINE [COL] | rename ...")
        return 0 if rest else 2
    if rest[0] == "compdb":
        return py("homm2.init.clangd", *rest[1:])
    return py("homm2.analysis.clangd_query", *rest)


def _workflow(rest):
    if rest[:1] != ["format"] or rest[1:] not in ([], ["--check"]):
        print("usage: homm2 workflow format [--check]", file=sys.stderr)
        return 2
    check = rest[1:]
    headers = sorted(REPO.glob("include/**/*.h"))
    sources = sorted(REPO.glob("src/**/*.cpp"))
    header_status = py("homm2.format.headers", *check, *headers)
    enum_status = py("homm2.format.enums", *check, *headers, *sources)
    return int(bool(header_status or enum_status))


def _tool(rest):
    if not rest or rest[0] not in TOOLS:
        print(f"homm2 tool: pick one of {', '.join(TOOLS)}", file=sys.stderr)
        return 2
    name, args = rest[0], rest[1:]
    if name == "objdiff":
        return sh("objdiff-cli", *args)
    if name == "delinker":
        return sh("vostok-delinker", *args)
    import subprocess
    from homm2.core.wine import child_env, prepare_env, tool
    prepare_env()
    program = ["wine", *args] if name == "wine" else ["wine", str(tool(f"{name}.exe")), *args]
    return subprocess.run(program, env=child_env()).returncode


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
    if not argv or argv[0] in ("help", "-h", "--help"):
        print(__doc__.strip())
        print(f"\ncommands: {COMMANDS}")
        print("aliases: " + ", ".join(f"{k} = {' '.join(v)}" for k, v in ALIASES.items()))
        return 0 if argv else 2
    cmd, rest = argv[0], argv[1:]
    if cmd == "data-topology" and rest[:1] == ["census"]:
        rest = rest[1:]
    if cmd in ALIASES:
        cmd, rest = ALIASES[cmd][0], [*ALIASES[cmd][1:], *rest]
    from homm2.core.paths import DEFAULT_IMAGE, image_key
    if image_key() != DEFAULT_IMAGE and cmd not in IMAGE_AWARE:
        print(f"homm2 {cmd}: not yet keyed by image; it reads the game only "
              f"(image-aware: {', '.join(sorted(IMAGE_AWARE))})", file=sys.stderr)
        return 2
    if cmd == "inspect":
        return _inspect(rest)
    if cmd == "init":
        from homm2.init import main as m
        return m(rest)
    if cmd == "toolchain":
        return _toolchain(rest)
    if cmd == "configure":
        return sh(sys.executable, "configure.py", *rest)
    if cmd == "build":
        return _build(rest)
    if cmd == "link":
        return _link(rest)
    if cmd == "match":
        return _match(rest)
    if cmd == "play":
        return _play(rest)
    if cmd == "labels":
        return py("homm2.build.source_symbols", *rest)
    if cmd == "model":
        return py("homm2.build.symbol_model_drift", *rest)
    if cmd == "delink":
        from homm2.redelink import main as m
        return m(rest)
    if cmd == "compare":
        from homm2.match.status import main as m
        return m(["--force-refresh", *rest])
    if cmd == "audit":
        from homm2.audit import main as m
        return m(rest)
    if cmd == "sema":
        from homm2.analysis.sema import main as m
        return m(rest)
    if cmd == "permute":
        return _permute(rest)
    if cmd == "lsp":
        return _lsp(rest)
    if cmd == "ghidra":
        from homm2.ghidra.driver import cli_main as m
        return m(rest)
    if cmd == "verify":
        from homm2.verify import main as m
        return m(rest)
    if cmd == "workflow":
        return _workflow(rest)
    if cmd == "clean":
        # Derive the shipped tree from the matching tree; see homm2/clean/__init__.py.
        from homm2.clean.clean_source import main as m
        return m(rest)
    if cmd == "localization":
        return py("homm2.build.localization", *rest)
    if cmd == "tool":
        return _tool(rest)
    print(f"homm2: unknown command {cmd!r}\ncommands: {COMMANDS}", file=sys.stderr)
    return 2


if __name__ == "__main__":
    sys.exit(main())
