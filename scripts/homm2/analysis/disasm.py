#!/usr/bin/env python3
"""homm2.analysis.disasm - per-function disassembly of a HEROES2W function.

Two sides, both via `llvm-objdump` on the per-symbol COFF objects the pipeline already
produces (so no raw-EXE byte carving, and both sides share ONE disassembler - the
cross-tool syntax normalization gruntz needs is unnecessary here):

  TARGET (default) : build/delink/<unit>.c.obj      - raw retail bytes, delinked
  BASE  (--base)   : build/objdiff/base/<unit>.obj  - raw compiled function
  DIFF             : build/objdiff/normalized/...   - disposable objdiff copies

Modes:
  (default)  TARGET disasm + relocs
  --base     BASE disasm + relocs
  --diff     unified diff of BASE vs TARGET (addresses/byte-columns masked; rc=1 if differ)
  --lite     asm only - no offset column, no byte columns, no reloc lines
  --blocks   basic-block/CFG view; combines with --base, --target, --diff, and --lite
  --dot      Graphviz CFG output (requires --blocks)
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
NORMAL_BASE = REPO / "build/objdiff/normalized/base"
NORMAL_TARGET = REPO / "build/objdiff/normalized/target"
LINES = REPO / "build/lines"


def die(msg: str):
    sys.exit(f"[disasm] {msg}")


def _resolve(arg):
    """(name, unit, rva, size, ordinal) for an RVA (0x..) or a mangled symbol name.

    The ordinal distinguishes duplicate public names within one object. Name lookup
    intentionally selects the first occurrence; an RVA selects the corresponding one.
    """
    if not SYMCSV.is_file():
        die(f"{SYMCSV} missing - run `homm2 init`")
    want_rva = None
    try:
        want_rva = int(arg, 16)
    except ValueError:
        pass
    rows = []
    for r in csv.DictReader(SYMCSV.open()):
        if (r.get("kind") or "func") != "func":
            continue
        try:
            rva = int(r["rva"], 16)
        except ValueError:
            continue
        try:
            size = int(r.get("size") or "0", 16)
        except ValueError:
            size = 0
        rows.append((r, rva, size))

    chosen = None
    for r, rva, size in rows:
        if (want_rva is not None and rva == want_rva) or \
           (want_rva is None and r["name"] == arg):
            chosen = (r, rva, size)
            break
    if chosen:
        r, rva, size = chosen
        unit = r.get("unit", "")
        same_name = sorted(
            (entry for entry in rows
             if entry[0].get("unit", "") == unit and entry[0]["name"] == r["name"]),
            key=lambda entry: entry[1])
        ordinal = next(i for i, entry in enumerate(same_name) if entry[0] is r)
        return r["name"], unit, rva, size, ordinal
    die(f"'{arg}' is not a known function RVA/name in symbol_names.csv")


def _public_text_symbols(obj: Path) -> set[str]:
    """Return external text symbol names from *obj*.

    Candidate library objects commonly place each function in a distinct COMDAT
    `.text` section whose address starts at zero.  Consequently, numeric address
    ranges cannot select one function from the whole object.
    """
    res = subprocess.run(
        ["llvm-nm", "-P", str(obj)],
        capture_output=True, text=True)
    if res.returncode != 0:
        die(f"llvm-nm failed on {obj.name}:\n{res.stderr.strip()}")

    names = set()
    for line in res.stdout.splitlines():
        fields = line.split()
        if len(fields) < 3 or fields[1] != "T":
            continue
        names.add(fields[0])
    return names


_SYMBOL_TITLE = re.compile(r"^\s*[0-9a-fA-F]+ <(.+)>:$")


def _slice_public_symbol(
        text: str, name: str, ordinal: int, public_names: set[str]) -> str | None:
    """Select one complete public function from a full object disassembly.

    Unlike `--disassemble-symbols`, this keeps compiler-generated private labels
    inside the function.  A new public label or section ends the selected span;
    private `$L...` labels do not.
    """
    selected = False
    occurrence = 0
    body = []
    for line in text.splitlines():
        title = _SYMBOL_TITLE.match(line)
        if not selected:
            if title and title.group(1) == name:
                if occurrence == ordinal:
                    selected = True
                    body.append(line)
                occurrence += 1
            continue

        if line.startswith("Disassembly of section "):
            break
        if title and title.group(1) in public_names:
            break
        body.append(line)

    if not selected:
        return None
    return "\n".join(body).rstrip() + "\n"


def _objdump(obj: Path, name: str, size_hint: int, ordinal: int) -> str:
    if not obj.is_file():
        die(f"{obj.relative_to(REPO)} missing - "
            + ("run `homm2 build` first" if BASE in obj.parents else "run `homm2 init` first"))
    if NORMAL_BASE in obj.parents or NORMAL_TARGET in obj.parents:
        from homm2.build.normalized_freshness import freshness_problems
        problems = freshness_problems(obj)
        if problems:
            die("stale normalized comparison object:\n  " + "\n  ".join(problems[:5]))
    public_names = _public_text_symbols(obj)
    res = subprocess.run(
        ["llvm-objdump", "-dr", "--x86-asm-syntax=intel", str(obj)],
        capture_output=True, text=True)
    if res.returncode != 0:
        die(f"llvm-objdump failed on {obj.name}:\n{res.stderr.strip()}")
    body = _slice_public_symbol(res.stdout, name, ordinal, public_names)
    if body is None:
        die(f"symbol {name} not found in {obj.relative_to(REPO)} "
            "(unit not implemented yet? base objs of un-bodied units are stubs)")
    return body


# llvm-objdump row = "<off>: <byte byte ...>\t<mnemonic>\t<operands>". The byte column is
# TAB-separated from the mnemonic, so split on tab (a byte-count regex mis-eats the last
# tab-adjacent byte). Title lines ("<name>:") carry no tab; relocs start with tabs.
_HEAD = re.compile(r"^\s*([0-9a-f]+):\s*(?:[0-9a-f]{2} ?)*\s*$")


def _parse_ins(ln: str):
    """(code_offset:int, 'mnemonic operands') for an instruction row, else None.

    Accept llvm-objdump's ``offset: bytes<TAB>op<TAB>args`` and GNU objdump's
    ``offset:<TAB>bytes<TAB>op args`` so retained raw-byte islands can be
    classified with the same CFG code.
    """
    if "\t" not in ln:
        return None
    parts = ln.split("\t")
    if not _HEAD.match(parts[0]):
        return None
    off = int(parts[0].split(":", 1)[0].strip(), 16)
    body = [part.strip() for part in parts[1:] if part.strip()]
    if body and re.fullmatch(r"(?:[0-9a-fA-F]{2}\s*)+", body[0]):
        body.pop(0)
    return (off, " ".join(body)) if body else None


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


_JCC = {
    "jmp", "je", "jne", "jz", "jnz", "ja", "jae", "jb", "jbe", "jc", "jnc",
    "jg", "jge", "jl", "jle", "js", "jns", "jo", "jno", "jp", "jnp",
    "jcxz", "jecxz",
}


def _branch_target(text: str, addresses: set[int]) -> int | None:
    fields = text.lower().split(None, 1)
    if len(fields) != 2:
        return None
    op, operand = fields
    if op not in _JCC and not op.startswith("loop"):
        return None
    match = re.match(r"0x([0-9a-f]+)\b", operand)
    if not match:
        return None
    target = int(match.group(1), 16)
    return target if target in addresses else None


def _mask_insn(text: str) -> str:
    """Normalize one instruction without hiding stack slots or small constants."""
    text = re.sub(r"\s+", " ", text.strip().lower())
    text = re.sub(r"\s*<[^>]*>", "", text)
    text = re.sub(r"\[\s*0x[0-9a-f]+\s*\]", "[<addr>]", text)
    text = re.sub(
        r"^((?:j[a-z]{1,4}|call|loop\w*)\s+)0x[0-9a-f]+\b.*$",
        r"\1<tgt>",
        text,
    )
    return text


def _cfg(text: str):
    """Return ordered basic blocks as ``(address, body, terminator)`` tuples.

    Terminators use block indices rather than code addresses, making normalized
    candidate and retail objects comparable. Emitted block count is deliberately
    diagnostic rather than authoritative: MSVC TU state can split or merge blocks
    without any source change.
    """
    insns = [parsed for line in text.splitlines()
             if (parsed := _parse_ins(line)) is not None]
    while insns and insns[-1][1].lower() == "nop":
        insns.pop()
    if not insns:
        return []

    addresses = {address for address, _ in insns}
    leaders = {insns[0][0]}
    for i, (address, instruction) in enumerate(insns):
        op = instruction.lower().split(None, 1)[0]
        target = _branch_target(instruction, addresses)
        following = insns[i + 1][0] if i + 1 < len(insns) else None
        if target is not None:
            leaders.add(target)
            # The byte after every branch starts a new block even when an
            # unconditional jump makes it unreachable. Otherwise alignment
            # bytes or a private-label body get folded into the jump block and
            # replace its terminator.
            if following is not None:
                leaders.add(following)
        elif (op == "jmp" or op.startswith("ret") or op in _JCC
              or op.startswith("loop")) and following is not None:
            leaders.add(following)

    import bisect
    order = sorted(leaders)
    index = {address: i for i, address in enumerate(order)}

    def block_of(address: int) -> int:
        return bisect.bisect_right(order, address) - 1

    result = [[address, [], None] for address in order]
    for i, (address, instruction) in enumerate(insns):
        block = result[block_of(address)]
        op = instruction.lower().split(None, 1)[0]
        target = _branch_target(instruction, addresses)
        following = insns[i + 1][0] if i + 1 < len(insns) else None
        block[1].append(_mask_insn(instruction))
        if following is not None and following not in index:
            continue

        if target is not None:
            destination = f"B{block_of(target)}" + ("^" if target <= address else "")
            if op == "jmp":
                block[2] = f"jmp {destination}"
            elif following is not None:
                block[2] = f"jcc {destination} | fall B{block_of(following)}"
            else:
                block[2] = f"jcc {destination}"
        elif op.startswith("ret"):
            block[2] = "ret"
        elif op == "jmp":
            block[2] = "jmp <ext>"
        elif following is not None:
            block[2] = f"fall B{block_of(following)}"
        else:
            block[2] = "end"
    return [(address, body, term) for address, body, term in result]


def _predecessor_counts(cfg) -> dict[int, int]:
    counts = {}
    for _, _, term in cfg:
        for match in re.finditer(r"B(\d+)", term or ""):
            target = int(match.group(1))
            counts[target] = counts.get(target, 0) + 1
    return counts


def _blocks(text: str, lite: bool = False) -> str:
    """Render one side's CFG, either as a skeleton or with instruction bodies."""
    cfg = _cfg(text)
    if not cfg:
        return "(no instruction rows found)\n"
    predecessors = _predecessor_counts(cfg)
    out = []
    for i, (address, body, term) in enumerate(cfg):
        tail = "  <== shared tail" if term == "ret" and predecessors.get(i, 0) > 2 else ""
        loop = "  LOOP" if "^" in (term or "") else ""
        if lite:
            first = body[0].split(None, 1)[0] if body else "?"
            out.append(
                f"  B{i:<3} @{address:<8x} {len(body):>3}i  "
                f"[{term}]{loop}{tail}  ({first}..)"
            )
            continue
        out.append("")
        out.append(
            f"block B{i} @{address:x}: {len(body)} instruction(s)  [{term}]"
            f"{loop}{tail}"
        )
        out.extend(f"    {instruction}" for instruction in body)
    return "\n".join(out).lstrip("\n") + "\n"


def _branch_kind(term: str | None, at: int) -> str:
    parts = []
    for match in re.finditer(r"(jcc|jmp|ret|fall|end)(?: B(\d+)(\^?))?", term or ""):
        op, target, back = match.groups()
        if target is None:
            parts.append(op)
        else:
            direction = "^" if back else ">" if int(target) > at else "<"
            parts.append(op + direction)
    return " ".join(parts)


def _skeleton_diff(base_cfg, target_cfg) -> tuple[str, bool]:
    """Side-by-side block sizes/terminators with separate flow/size signals."""
    out = [f"       {'BASE':48} FLOW SIZE TARGET"]
    same = len(base_cfg) == len(target_cfg)
    counts = {"exact": 0, "size": 0, "shift": 0, "flow": 0, "missing": 0}
    first_flow = None

    def summary(cfg, i):
        if i >= len(cfg):
            return "-"
        _, body, term = cfg[i]
        first = body[0].split(None, 1)[0] if body else "?"
        last = body[-1].split(None, 1)[0] if body else "?"
        return f"{len(body):>3}i {first}..{last} [{term}]"

    for i in range(max(len(base_cfg), len(target_cfg))):
        base, target = summary(base_cfg, i), summary(target_cfg, i)
        if i >= len(base_cfg) or i >= len(target_cfg):
            flow_mark, size_mark = "--", "--"
            counts["missing"] += 1
            same = False
        else:
            base_term, target_term = base_cfg[i][2], target_cfg[i][2]
            base_kind = _branch_kind(base_term, i)
            target_kind = _branch_kind(target_term, i)
            if base_term == target_term:
                flow_mark = "=="
            elif base_kind == target_kind:
                flow_mark = "~="
                counts["shift"] += 1
            else:
                flow_mark = "!!"
                counts["flow"] += 1
                if first_flow is None:
                    first_flow = (i, base_kind, target_kind)
            size_mark = "==" if len(base_cfg[i][1]) == len(target_cfg[i][1]) else "##"
            if flow_mark == "==" and size_mark == "==":
                counts["exact"] += 1
            elif flow_mark == "==" and size_mark == "##":
                counts["size"] += 1
            same &= flow_mark == "==" and size_mark == "=="
        out.append(
            f"  B{i:<3} {base:48}  {flow_mark:^4} {size_mark:^4} {target}"
        )

    out.insert(
        0,
        f"[skeleton diff: base {len(base_cfg)} vs target {len(target_cfg)} blocks; "
        f"{counts['exact']} exact, {counts['size']} size-only, "
        f"{counts['shift']} target-shift, {counts['flow']} flow-kind, "
        f"{counts['missing']} missing]",
    )
    out.append("[legend: FLOW == exact, ~= same branch kind/direction with shifted "
               "block target, !! branch-kind mismatch; SIZE ## differs]")
    if first_flow is not None:
        i, base_kind, target_kind = first_flow
        out.append(
            f"[first branch-kind divergence B{i}: base [{base_kind}] vs "
            f"target [{target_kind}]]"
        )
    return "\n".join(out) + "\n", bool(same)


def _blocks_diff(base_text: str, target_text: str) -> tuple[str, bool]:
    """Block-aligned body diff; exact only when all normalized blocks align."""
    import difflib
    base_cfg, target_cfg = _cfg(base_text), _cfg(target_text)
    base_rows = ["\n".join(body + [term or ""]) for _, body, term in base_cfg]
    target_rows = ["\n".join(body + [term or ""]) for _, body, term in target_cfg]
    base_flow = [term or "" for _, _, term in base_cfg]
    target_flow = [term or "" for _, _, term in target_cfg]
    out = [
        f"[block diff: base {len(base_cfg)} blocks vs target {len(target_cfg)} blocks; "
        f"flow {'SAME' if base_flow == target_flow else 'DIFFERS'}]"
    ]

    if base_flow != target_flow:
        base_kinds = [_branch_kind(term, i) for i, term in enumerate(base_flow)]
        target_kinds = [_branch_kind(term, i) for i, term in enumerate(target_flow)]
        first = next(
            (i for i, pair in enumerate(zip(base_kinds, target_kinds))
             if pair[0] != pair[1]),
            None,
        )
        if first is not None:
            out.append(
                f"[skeleton diverges at B{first}: base [{base_flow[first]}] vs "
                f"target [{target_flow[first]}]]"
            )
        elif len(base_kinds) != len(target_kinds):
            out.append(
                f"[same branch-kind skeleton for {min(len(base_kinds), len(target_kinds))} "
                f"shared blocks; {abs(len(base_kinds) - len(target_kinds))} extra block(s)]"
            )
        else:
            out.append("[same branch-kind skeleton; block-index targets differ]")

    matcher = difflib.SequenceMatcher(a=base_rows, b=target_rows, autojunk=False)
    differences = 0
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            for offset in range(i2 - i1):
                bi, ti = i1 + offset, j1 + offset
                out.append(
                    f"  B{ti} @{base_cfg[bi][0]:x}/@{target_cfg[ti][0]:x}  == "
                    f"({len(target_cfg[ti][1])} insns)  [{target_cfg[ti][2]}]"
                )
            continue
        for offset in range(max(i2 - i1, j2 - j1)):
            bi = i1 + offset if i1 + offset < i2 else None
            ti = j1 + offset if j1 + offset < j2 else None
            differences += 1
            if bi is not None and ti is not None:
                out.append(
                    f"  B{ti} @{base_cfg[bi][0]:x}/@{target_cfg[ti][0]:x}  DIFFERS:"
                )
                diff = difflib.unified_diff(
                    base_cfg[bi][1] + [base_cfg[bi][2] or ""],
                    target_cfg[ti][1] + [target_cfg[ti][2] or ""],
                    lineterm="",
                    n=2,
                )
                out.extend("      " + line for line in diff
                           if not line.startswith(("---", "+++", "@@")))
            elif bi is not None:
                out.append(
                    f"  -- @{base_cfg[bi][0]:x}  BASE-ONLY "
                    f"({len(base_cfg[bi][1])} insns) [{base_cfg[bi][2]}]"
                )
            else:
                out.append(
                    f"  B{ti} @{target_cfg[ti][0]:x}  TARGET-ONLY "
                    f"({len(target_cfg[ti][1])} insns) [{target_cfg[ti][2]}]"
                )
    out.append(f"[{differences} block(s) differ]" if differences
               else "[all aligned blocks identical]")
    return "\n".join(out) + "\n", differences == 0 and base_flow == target_flow


def _dot(cfg, differing: set[int] | None = None, title: str = "") -> str:
    """Render a CFG as Graphviz DOT."""
    def quote(value: str) -> str:
        return value.replace("\\", "\\\\").replace('"', '\\"')

    predecessors = _predecessor_counts(cfg)
    out = [
        "digraph cfg {",
        '  graph [rankdir=TB, fontname="monospace"'
        + (f', label="{quote(title)}", labelloc=t' if title else "") + "];",
        '  node [shape=box, fontname="monospace", fontsize=10];',
    ]
    for i, (address, body, term) in enumerate(cfg):
        first = quote(body[0] if body else "")
        last = quote(body[-1] if body else "")
        label = f"B{i} @{address:x} ({len(body)}i)\\l{first}\\l"
        if len(body) > 1:
            label += ("...\\l" if len(body) > 2 else "") + f"{last}\\l"
        attributes = [f'label="{label}"']
        if term == "ret" and predecessors.get(i, 0) > 2:
            attributes.append("peripheries=2")
        if differing is not None:
            attributes.append(
                'style=filled, fillcolor="#ffcccc"'
                if i in differing else 'style=filled, fillcolor="#ccffcc"'
            )
        out.append(f"  B{i} [{', '.join(attributes)}];")
        for match in re.finditer(r"(jcc|jmp|fall) B(\d+)(\^?)", term or ""):
            kind, target, back = match.groups()
            edge = []
            if kind == "fall":
                edge.append("style=dashed")
            if back:
                edge.append("color=red")
                edge.append("constraint=false")
            suffix = f" [{', '.join(edge)}]" if edge else ""
            out.append(f"  B{i} -> B{target}{suffix};")
    out.append("}")
    return "\n".join(out) + "\n"


def _rich(name: str, unit: str, size: int, ordinal: int) -> str:
    """BASE disasm interleaved with the CodeView source line each code offset came from
    (build/lines/<unit>.json: {mangled: [[code_off, src_line, text], ...]}). Degrades to
    plain base asm if the line map is absent."""
    obj = BASE / f"{unit}.obj"
    body = _objdump(obj, name, size, ordinal)
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
    name, unit, rva, size, ordinal = _resolve(pos[0])
    if not unit:
        die(f"{name} has no unit in symbol_names.csv")

    if "--rich" in flags:
        if "--blocks" in flags:
            die("--rich does not combine with --blocks")
        print(_rich(name, unit, size, ordinal), end="")
        sys.exit(0)

    if "--dot" in flags and "--blocks" not in flags:
        die("--dot requires --blocks")

    if "--blocks" in flags:
        if "--diff" in flags:
            base_text = _objdump(
                NORMAL_BASE / f"{unit}.obj", name, size, ordinal)
            target_text = _objdump(
                NORMAL_TARGET / f"{unit}.c.obj", name, size, ordinal)
            base_cfg, target_cfg = _cfg(base_text), _cfg(target_text)
            if "--dot" in flags:
                import difflib
                base_rows = ["\n".join(body + [term or ""])
                             for _, body, term in base_cfg]
                target_rows = ["\n".join(body + [term or ""])
                               for _, body, term in target_cfg]
                differing = set(range(len(target_cfg)))
                matcher = difflib.SequenceMatcher(
                    a=base_rows, b=target_rows, autojunk=False)
                for tag, _, _, j1, j2 in matcher.get_opcodes():
                    if tag == "equal":
                        differing.difference_update(range(j1, j2))
                print(_dot(
                    target_cfg,
                    differing,
                    f"0x{rva:08x} TARGET (red = differs from base)",
                ), end="")
                sys.exit(0)
            if "--lite" in flags:
                output, exact = _skeleton_diff(base_cfg, target_cfg)
            else:
                output, exact = _blocks_diff(base_text, target_text)
            print(output, end="")
            sys.exit(0 if exact else 1)

        use_base = "--base" in flags
        side = "BASE (compiled)" if use_base else "TARGET (retail)"
        obj = BASE / f"{unit}.obj" if use_base else DELINK / f"{unit}.c.obj"
        text = _objdump(obj, name, size, ordinal)
        cfg = _cfg(text)
        if "--dot" in flags:
            print(_dot(cfg, title=f"0x{rva:08x} {side}"), end="")
        else:
            print(f"[basic blocks: {side} @ 0x{rva:08x} {name}]")
            print(_blocks(text, lite="--lite" in flags), end="")
        sys.exit(0)

    if "--diff" in flags:
        import difflib
        base = _norm(_objdump(NORMAL_BASE / f"{unit}.obj", name, size, ordinal))
        tgt = _norm(_objdump(NORMAL_TARGET / f"{unit}.c.obj", name, size, ordinal))
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
        text = _objdump(BASE / f"{unit}.obj", name, size, ordinal)
    else:
        print(f"[disasm TARGET (retail): {name}  build/delink/{unit}.c.obj]")
        text = _objdump(DELINK / f"{unit}.c.obj", name, size, ordinal)
    print(_lite(text) if "--lite" in flags else text, end="")
    sys.exit(0)


if __name__ == "__main__":
    main()
