#!/usr/bin/env python3
"""homm2 sema <cmd> - semantic navigation for matchers (source & target).

ONE discoverable entrypoint for the read-only navigation tools a matcher reaches for:
the retail xref graph, the Ghidra decompiler, clangd LSP (symbol/def/refs/hover/rename),
target/base disasm, strings, per-fn match %, and an address dossier. Each subcommand is a
THIN delegation to a homm2.analysis / homm2.match module (all still runnable as
`python -m homm2.analysis.<...>`); nothing here re-implements analysis.

SEMANTIC questions go here; grep is lexical-only.

Every invocation is logged (all subcommands) to build/homm2_sema.log - a usage feed for
tool improvement.
"""
import argparse, json, os, subprocess, sys
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
GEN_NAMES = REPO / "build/gen/symbol_names.csv"
GHIDRA_FUNCS = REPO / "build/ghidra/exports/functions.csv"
REPORT = REPO / "build/objdiff/report.json"


def die(msg: str):
    sys.exit(f"[sema] {msg}")


def _pkg_env() -> dict:
    env = dict(os.environ)
    scripts = str(REPO / "scripts")
    env["PYTHONPATH"] = scripts + (os.pathsep + env["PYTHONPATH"] if env.get("PYTHONPATH") else "")
    return env


def _sema_tool(module: str, argv: list) -> int:
    """Stream a read-only navigation tool's output (package on PYTHONPATH)."""
    return subprocess.run([sys.executable, "-m", module, *map(str, argv)],
                          cwd=str(REPO), env=_pkg_env()).returncode


# --- usage logging ----------------------------------------------------------------
def _sema_log(rc: int) -> None:
    """One line per `homm2 sema` invocation (ALL subcommands); must NEVER break the
    tool. Metadata first, command after the `: ` (shell-quoted) so it copies straight out:
        [2026-07-07][19:55:01][0]: homm2 sema xref 0x00069120 --raw
    """
    try:
        import datetime, shlex
        now = datetime.datetime.now()
        cmd = shlex.join(["homm2", "sema", *_ARGV])
        path = REPO / "build" / "homm2_sema.log"
        path.parent.mkdir(parents=True, exist_ok=True)
        with open(path, "a") as f:
            f.write("[{}][{}][{}]: {}\n".format(
                now.date(), now.strftime("%H:%M:%S"), rc, cmd))
    except Exception:
        pass  # logging is best-effort by design


_ARGV: list = []  # the raw `sema` argv (set in main), for the log line


# --- helpers ----------------------------------------------------------------------
def _csv_find(path: Path, rva: int, key: str = "rva"):
    """First CSV row whose `key` column parses to `rva` (hex), or None."""
    import csv
    if not path.is_file():
        return None
    for r in csv.DictReader(path.open()):
        try:
            if int(r[key], 16) == rva:
                return r
        except (ValueError, KeyError):
            pass
    return None


def _name_of_rva(rva: int):
    """(name, unit) claimed at `rva` in symbol_names.csv, or None."""
    r = _csv_find(GEN_NAMES, rva)
    return (r["name"], r.get("unit", "")) if r else None


def _point_argv(args) -> list:
    """`<file> <line> [<col>]` -> the clangd_query positional list."""
    return [args.file, args.line] + ([args.col] if args.col is not None else [])


# --- subcommand handlers ----------------------------------------------------------
def cmd_xref(args) -> None:
    flags = (["--callees"] if args.callees else []) + (["--raw"] if args.raw else [])
    if args.tree:
        flags += ["--tree", "--depth", str(args.depth)]
    sys.exit(_sema_tool("homm2.analysis.xref", flags + args.target))


def cmd_symbol(args) -> None:
    sys.exit(_sema_tool("homm2.analysis.clangd_query", ["symbol", args.query]))


def cmd_point(args) -> None:                 # def / refs / hover share this
    sys.exit(_sema_tool("homm2.analysis.clangd_query", [args.sema, *_point_argv(args)]))


def cmd_rename(args) -> None:
    argv = ["rename", *_point_argv(args), args.new_name]
    if args.dry_run:
        argv.append("--dry-run")
    sys.exit(_sema_tool("homm2.analysis.clangd_query", argv))


def cmd_disasm(args) -> None:
    argv = [args.rva]
    if args.base:
        argv.append("--base")
    if args.target:
        argv.append("--target")
    if args.diff:
        argv.append("--diff")
    if args.rich:
        argv.append("--rich")
    if args.lite:
        argv.append("--lite")
    sys.exit(_sema_tool("homm2.analysis.disasm", argv))


def cmd_strings(args) -> None:
    if args.find:
        sys.exit(_sema_tool("homm2.analysis.string_xref", ["--find", args.find]))
    if not args.rva:
        die("strings: give an <rva> or --find <text>")
    sys.exit(_sema_tool("homm2.analysis.string_xref", ["--rva", args.rva]))


def cmd_rva(args) -> None:
    sys.exit(_sema_tool("homm2.analysis.rva", [args.addr]))


def cmd_decomp(args) -> None:
    argv = [args.rva]
    if args.force:
        argv.append("--force")
    sys.exit(_sema_tool("homm2.analysis.decomp", argv))


def cmd_match(args) -> None:
    """Per-function/unit match % from build/objdiff/report.json. A unit name -> its
    per-fn table; an 0x RVA -> the single fn a src symbol claims. `fuzzy_match_percent`
    absent for a fn means it has not been diffed yet (0% - not started)."""
    if not REPORT.is_file():
        die(f"{REPORT.relative_to(REPO)} missing - run `homm2 build` first")
    rep = json.loads(REPORT.read_text())
    t = args.target
    units = {u.get("name"): u for u in rep.get("units", [])}

    def fn_line(fn):
        pct = fn.get("fuzzy_match_percent")
        nm = (fn.get("metadata") or {}).get("demangled_name") or fn.get("name")
        tag = f"{pct:6.2f}%" if pct is not None else "   -- "
        exact = "  EXACT" if (pct is not None and pct >= 100.0) else ""
        return f"  {tag}  {nm}{exact}"

    if t in units:
        u = units[t]
        m = u.get("measures", {})
        print(f"== {t} ==  fuzzy {m.get('fuzzy_match_percent') or 0.0:.2f}%"
              f"  functions {m.get('matched_functions') or 0}/{m.get('total_functions') or 0}")
        fns = u.get("functions") or []
        for fn in sorted(fns, key=lambda f: -(f.get("fuzzy_match_percent") or 0.0)):
            print(fn_line(fn))
        return

    # RVA -> which src symbol claims it -> find that fn in the report
    if t.lower().startswith("0x"):
        try:
            rva = int(t, 16)
        except ValueError:
            die(f"'{t}' is not a hex RVA")
        claim = _name_of_rva(rva)
        if not claim:
            die(f"no src symbol claims RVA {t} - try a unit name (e.g. SOURCE/KB)")
        name, unit = claim
        u = units.get(unit)
        if u:
            for fn in u.get("functions") or []:
                if fn.get("name") == name:
                    print(f"{name}  [{unit}]")
                    print(fn_line(fn))
                    return
        print(f"{name}  [{unit}]\n     --   (not yet in the diff report - 0%, not started)")
        return
    die(f"'{t}' is neither a known unit nor an 0x RVA")


# --- argparse group ---------------------------------------------------------------
def _build_parser() -> argparse.ArgumentParser:
    ap = argparse.ArgumentParser(
        prog="homm2 sema", formatter_class=argparse.RawDescriptionHelpFormatter,
        description="homm2 sema <cmd> - source & target navigation for matchers.\n"
                    "Thin wrappers over the analysis tools (each also runnable as "
                    "`python -m homm2.analysis.<...>`).\nSEMANTIC questions go here; grep "
                    "is lexical-only.",
        epilog="examples (use when ...):\n"
        "  homm2 sema xref 0x0004a3c0            who calls this fn (attribution)\n"
        "  homm2 sema xref --callees 0x0000126d  its own call targets\n"
        "  homm2 sema xref --tree 0x0004a3c0     caller ancestry tree\n"
        "  homm2 sema decomp 0x0004a3c0          Ghidra decompiler C for a fn\n"
        "  homm2 sema disasm 0x0004a3c0          retail disasm (--base/--diff/--rich/--lite)\n"
        "  homm2 sema strings 0x0000126d         the string set of a fn\n"
        "  homm2 sema strings --find Smack       reverse literal lookup\n"
        "  homm2 sema match SOURCE/SMACKMGR      per-fn % of a unit (or an RVA)\n"
        "  homm2 sema rva 0x0004a3c0             address dossier (claim / src loc / %)\n"
        "  homm2 sema symbol combatManager       fuzzy workspace-symbol search (clangd)\n"
        "  homm2 sema def   src/SOURCE/KB.cpp 42 jump to the definition\n"
        "  homm2 sema refs  include/... 30       every ref (USR-exact; no grep collisions)\n"
        "  homm2 sema hover src/SOURCE/KB.cpp 42 type/decl at point\n"
        "  homm2 sema rename include/... 40 m_new --dry-run   tree-wide rename preview\n")
    ss = ap.add_subparsers(dest="sema", required=True)

    sx = ss.add_parser("xref", help="retail caller/callee graph (attribution)")
    sx.add_argument("target", nargs="+", help="RVA(s) (0x..) or (mangled) symbol name(s)")
    sx.add_argument("--callees", action="store_true", help="forward: its call targets")
    sx.add_argument("--raw", action="store_true", help="every call site (no dedup)")
    sx.add_argument("--tree", action="store_true", help="caller ancestry tree")
    sx.add_argument("--depth", type=int, default=4, metavar="N",
                    help="--tree expansion cap (default 4; 0 = unlimited)")
    sx.set_defaults(func=cmd_xref)

    sdc = ss.add_parser("decomp", help="Ghidra decompiler C for a fn (needs `homm2 ghidra`)")
    sdc.add_argument("rva", help="RVA (0x..) of the function to decompile")
    sdc.add_argument("--force", action="store_true",
                     help="create a function at the RVA first if Ghidra found none")
    sdc.set_defaults(func=cmd_decomp)

    sy = ss.add_parser("symbol", help="fuzzy workspace-symbol search (clangd)")
    sy.add_argument("query")
    sy.set_defaults(func=cmd_symbol)

    for nm, hlp in (("def", "definition at point"),
                    ("refs", "references at point (USR-exact)"),
                    ("hover", "type/decl at point")):
        p = ss.add_parser(nm, help=hlp + " (clangd)")
        p.add_argument("file")
        p.add_argument("line", type=int)
        p.add_argument("col", type=int, nargs="?")
        p.set_defaults(func=cmd_point)

    sr = ss.add_parser("rename", help="tree-wide symbol rename (clangd; USR-exact)")
    sr.add_argument("file")
    sr.add_argument("line", type=int)
    sr.add_argument("col", type=int, nargs="?")
    sr.add_argument("new_name")
    sr.add_argument("--dry-run", action="store_true",
                    help="preview the edit set (file:line: old -> new); write nothing")
    sr.set_defaults(func=cmd_rename)

    sd = ss.add_parser("disasm",
                       help="disasm: TARGET (retail delink, default) / --base (compiled) / "
                            "--rich (base + CodeView source lines) / --diff / --lite")
    sd.add_argument("rva", help="RVA (0x..) or (mangled) symbol name")
    sdside = sd.add_mutually_exclusive_group()
    sdside.add_argument("--target", action="store_true", help="retail delink side (default)")
    sdside.add_argument("--base", action="store_true", help="your compiled obj instead")
    sd.add_argument("--rich", action="store_true",
                    help="BASE disasm interleaved with the CodeView source lines (implies --base)")
    sd.add_argument("--lite", action="store_true", help="asm only - no addresses/bytes")
    sd.add_argument("--diff", action="store_true",
                    help="unified diff of base-vs-target asm (addresses masked; rc=1 if differs)")
    sd.set_defaults(func=cmd_disasm)

    st = ss.add_parser("strings", help="per-fn string set / --find reverse lookup")
    st.add_argument("rva", nargs="?", help="RVA (0x..) whose string set to print")
    st.add_argument("--find", metavar="TEXT", help="reverse: fns referencing TEXT")
    st.set_defaults(func=cmd_strings)

    sm = ss.add_parser("match", help="per-function/unit match summary (report.json)")
    sm.add_argument("target", help="a unit name (e.g. SOURCE/KB), or an 0x RVA a src fn claims")
    sm.set_defaults(func=cmd_match)

    srv = ss.add_parser("rva", help="address dossier (src claim + file:line / ghidra / match)")
    srv.add_argument("addr", help="hex RVA, e.g. 0x0004a3c0")
    srv.set_defaults(func=cmd_rva)

    return ap


def main(argv=None) -> int:
    global _ARGV
    argv = list(sys.argv[1:] if argv is None else argv)
    _ARGV = argv
    ap = _build_parser()
    args = ap.parse_args(argv)
    rc = 0
    try:
        args.func(args)
    except SystemExit as e:
        rc = e.code if isinstance(e.code, int) else (0 if e.code is None else 1)
        _sema_log(rc)
        raise
    _sema_log(rc)
    return rc


if __name__ == "__main__":
    sys.exit(main())
