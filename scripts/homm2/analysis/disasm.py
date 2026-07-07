#!/usr/bin/env python3
"""homm2.analysis.disasm - per-function disassembly of a HEROES2W function.

Two sides, both via `llvm-objdump` on the per-symbol COFF objects the pipeline already
produces (so no raw-EXE byte carving, and both sides share ONE disassembler - the
cross-tool syntax normalization gruntz needs is unnecessary here):

  TARGET (default) : build/delink/<unit>.c.obj      - the retail bytes, delinked
  BASE  (--base)   : build/objdiff/base/<unit>.obj  - YOUR compiled fn (what objdiff diffs)

Modes:
  (default)  TARGET disasm + relocs
  --base     BASE disasm + relocs
  --diff     unified diff of BASE vs TARGET (addresses/byte-columns masked; rc=1 if differ)
  --lite     asm only - no offset column, no byte columns, no reloc lines
  --rich     BASE disasm interleaved with the CodeView source line each instr came from
             (implies --base; needs build/lines/<unit>.json from `gen_lines.py`, build shell)

Usage:
    python3 -m homm2.analysis.disasm 0x000d4050              # target
    python3 -m homm2.analysis.disasm ?initialiseCRC@@YIXXZ --base
    python3 -m homm2.analysis.disasm 0x000d4050 --diff
"""
import csv, json, os, re, subprocess, sys
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
SYMCSV = REPO / "build/gen/symbol_names.csv"
DELINK = REPO / "build/delink"
BASE = REPO / "build/objdiff/base"
LINES = REPO / "build/lines"


def die(msg: str):
    sys.exit(f"[disasm] {msg}")


def _resolve(arg):
    """(name, unit, rva) for an RVA (0x..) or a (mangled) symbol name, from CodeView."""
    if not SYMCSV.is_file():
        die(f"{SYMCSV} missing - run `homm2 init`")
    want_rva = None
    try:
        want_rva = int(arg, 16)
    except ValueError:
        pass
    for r in csv.DictReader(SYMCSV.open()):
        if (r.get("kind") or "func") != "func":
            continue
        try:
            rva = int(r["rva"], 16)
        except ValueError:
            continue
        if (want_rva is not None and rva == want_rva) or \
           (want_rva is None and r["name"] == arg):
            return r["name"], r.get("unit", ""), rva
    die(f"'{arg}' is not a known function RVA/name in symbol_names.csv")


def _objdump(obj: Path, name: str) -> str:
    if not obj.is_file():
        die(f"{obj.relative_to(REPO)} missing - "
            + ("run `homm2 build` first" if BASE in obj.parents else "run `homm2 init` first"))
    res = subprocess.run(
        ["llvm-objdump", "-dr", "--x86-asm-syntax=intel",
         f"--disassemble-symbols={name}", str(obj)],
        capture_output=True, text=True)
    if res.returncode != 0:
        die(f"llvm-objdump failed on {obj.name}:\n{res.stderr.strip()}")
    body = res.stdout
    if f"<{name}>:" not in body:
        die(f"symbol {name} not found in {obj.relative_to(REPO)} "
            "(unit not implemented yet? base objs of un-bodied units are stubs)")
    return body


# llvm-objdump row = "<off>: <byte byte ...>\t<mnemonic>\t<operands>". The byte column is
# TAB-separated from the mnemonic, so split on tab (a byte-count regex mis-eats the last
# tab-adjacent byte). Title lines ("<name>:") carry no tab; relocs start with tabs.
_HEAD = re.compile(r"^\s*([0-9a-f]+):\s*(?:[0-9a-f]{2} ?)*\s*$")


def _parse_ins(ln: str):
    """(code_offset:int, 'mnemonic operands') for an instruction row, else None."""
    if "\t" not in ln:
        return None
    parts = ln.split("\t")
    if not _HEAD.match(parts[0]):
        return None
    off = int(parts[0].split(":", 1)[0].strip(), 16)
    return off, " ".join(p.strip() for p in parts[1:] if p.strip())


def _lite(text: str) -> str:
    """Only the asm: drop offsets, byte columns, reloc blocks; keep the title line."""
    keep = []
    for ln in text.splitlines():
        p = _parse_ins(ln)
        if p:
            keep.append("    " + p[1])
        elif ln.rstrip().endswith(">:"):
            keep.append(ln)
    return "\n".join(keep) + "\n"


def _norm(text: str) -> list:
    """Instruction stream for --diff: mnemonic+operands only, addresses masked. Both
    sides come from the same llvm-objdump so only real byte-level diffs survive."""
    out = []
    for ln in text.splitlines():
        p = _parse_ins(ln)
        if p:
            ins = p[1].lower()
            ins = re.sub(r"\s*<[^>]*>", "", ins)          # drop <sym+0xN> annotations
            ins = re.sub(r"0x[0-9a-f]+", "<addr>", ins)   # mask absolute addrs/disps
            out.append(ins)
        elif "IMAGE_REL_I386_" in ln:
            # reloc lines match on both sides (same target symbol) - keep the target
            out.append("reloc " + ln.split("IMAGE_REL_I386_")[1].strip())
    while out and out[-1] == "nop":
        out.pop()  # trailing COMDAT alignment padding (base only)
    return out


def _rich(name: str, unit: str) -> str:
    """BASE disasm interleaved with the CodeView source line each code offset came from
    (build/lines/<unit>.json: {mangled: [[code_off, src_line, text], ...]}). Degrades to
    plain base asm if the line map is absent."""
    obj = BASE / f"{unit}.obj"
    body = _objdump(obj, name)
    lm = {}
    lj = LINES / f"{unit}.json"
    if lj.is_file():
        try:
            info = json.loads(lj.read_text()).get(name)
            if info:  # [[code_off, src_line, "text"], ...]
                lm = {int(off): (int(sl), txt) for off, sl, txt in info}
        except Exception:
            lm = {}
    out = [f"{name}  [{unit}]",
           "('NNNNN| text' = source line; indented = asm)" if lm
           else f"(no build/lines/{unit}.json - run `gen_lines.py {unit}` in the build "
                "shell for source interleaving; bare asm below)"]
    cur = None
    for ln in body.splitlines():
        p = _parse_ins(ln)
        if not p:
            if ln.rstrip().endswith(">:") or "IMAGE_REL" in ln:
                out.append(ln)
            continue
        off, text = p
        if off in lm and lm[off][0] != cur:
            cur = lm[off][0]
            out.append(f"{cur:5d}| {lm[off][1]}")
        out.append("      " + text)
    return "\n".join(out) + "\n"


def main():
    args = sys.argv[1:]
    flags = {a for a in args if a.startswith("--")}
    pos = [a for a in args if not a.startswith("--")]
    if not pos:
        sys.exit(__doc__)
    name, unit, rva = _resolve(pos[0])
    if not unit:
        die(f"{name} has no unit in symbol_names.csv")

    if "--rich" in flags:
        print(_rich(name, unit), end="")
        sys.exit(0)

    if "--diff" in flags:
        import difflib
        base = _norm(_objdump(BASE / f"{unit}.obj", name))
        tgt = _norm(_objdump(DELINK / f"{unit}.c.obj", name))
        if base == tgt:
            print(f"identical asm ({len(tgt)} instruction(s); addresses/relocs masked)")
            sys.exit(0)
        print(f"[diff: BASE (compiled) vs TARGET (retail) @ 0x{rva:08x} {name}; "
              "addresses masked as <addr>]")
        for ln in difflib.unified_diff(base, tgt, "base", "target", lineterm=""):
            print(ln)
        sys.exit(1)

    if "--base" in flags:
        print(f"[disasm BASE (compiled): {name}  build/objdiff/base/{unit}.obj]")
        text = _objdump(BASE / f"{unit}.obj", name)
    else:
        print(f"[disasm TARGET (retail): {name}  build/delink/{unit}.c.obj]")
        text = _objdump(DELINK / f"{unit}.c.obj", name)
    print(_lite(text) if "--lite" in flags else text, end="")
    sys.exit(0)


if __name__ == "__main__":
    main()
