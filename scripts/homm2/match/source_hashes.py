"""Normalized per-function source hashes used to scope retained match maxima."""
import csv, hashlib, os, re
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
RVA_BASE = 0x400000


def _rva_to_sym():
    """rva -> (unit, mangled_name) for .text functions, from the generated CSV."""
    out = {}
    csvp = REPO / "build/gen/symbol_names.csv"
    if not csvp.exists():
        return out
    with csvp.open() as stream:
        for row in csv.reader(stream):
            if len(row) < 5 or row[4] != "func":
                continue
            try:
                out[int(row[0], 16)] = (row[2], row[1])
            except ValueError:
                pass
    return out
def _class_members():
    """{class: (base|None, {member_name: 'off'})} parsed from include/ headers. Members carry a
    `// +0xNN` offset comment; the class line may say `: public Base`. Used to map member names to
    their OFFSET so a rename (field_0x4 -> width) is invisible to the hash (offset unchanged)."""
    cls = {}
    memre = re.compile(r"^\s*[A-Za-z_][\w\s\*]*?\b([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*;\s*//\s*\+0x([0-9a-fA-F]+)")
    clsre = re.compile(r"^class\s+(\w+)\b(?:\s*:\s*public\s+(\w+))?")
    for h in sorted((REPO / "include").rglob("*.h")):
        cur = None
        for ln in h.read_text(errors="replace").splitlines():
            m = clsre.match(ln)
            if m and "{" in ln:
                cur = m.group(1); cls.setdefault(cur, [m.group(2), {}])
                continue
            if cur:
                mm = memre.match(ln)
                if mm:
                    cls[cur][1][mm.group(1)] = mm.group(2).lower()
    return cls


def _members_of(clsname, cmap, _seen=None):
    """member_name -> off for a class and all its bases (flattened)."""
    _seen = _seen or set()
    if clsname in _seen or clsname not in cmap:
        return {}
    _seen.add(clsname)
    base, mem = cmap[clsname]
    out = dict(_members_of(base, cmap, _seen)) if base else {}
    out.update(mem)
    return out


def _normalize(block, cmap):
    """Make the hash independent of codegen-NEUTRAL renames: arg names -> a0,a1,... (position) and
    member names -> m<off> (offset). LOCAL names are kept — they drive the /Od stack slot, so a
    local rename that shifts a slot legitimately changes codegen (and must reset max%)."""
    # arg names from the `Class::method(args)` signature
    args = []
    sig = re.search(r"::[~\w]+\s*\(([^)]*)\)", block)
    if sig:
        for a in sig.group(1).split(","):
            nm = re.search(r"([A-Za-z_]\w*)\s*$", a.strip())
            if nm:
                args.append(nm.group(1))
    # owner class from `ret Class::method(`  (map its + base members)
    mem = {}
    cm = re.search(r"\b(\w+)::[~\w]+\s*\(", block)
    if cm:
        mem = _members_of(cm.group(1), cmap)
    n = re.sub(r"\bfield_0x([0-9a-fA-F]+)\b", lambda m: "m0x" + m.group(1).lower(), block)
    for name, off in mem.items():
        n = re.sub(r"\b" + re.escape(name) + r"\b", "m0x" + off, n)
    for i, a in enumerate(args):
        n = re.sub(r"\b" + re.escape(a) + r"\b", "a%d" % i, n)
    return n


_VA_MARKER_RE = re.compile(r"VA\(0x([0-9a-fA-F]+)\s*,")
_TOP_LEVEL_BOUNDARY_RE = re.compile(
    r"[ \t]*(?:DATA\(|VTBL\(|// ===|#endif\b)")


def _source_function_blocks(text):
    """Yield ``(absolute_va, block)`` using lexical top-level boundaries.

    Keep the historical hash surface: each block starts immediately after the
    comma in its column-zero ``VA(...)`` marker. Its semantic extent ends at the
    matched closing brace of the function body; only the contiguous legacy
    whitespace after that brace is retained. Comments, directives, and audit
    markers between neighboring functions therefore belong to neither hash.
    The top-level boundary scan remains as the historical whitespace cap, while
    a second lexical pass keeps function-local ``DATA(...)`` definitions,
    preprocessor lines, nested blocks, and marker-like text inside literals or
    comments in the owning function.
    """
    markers = []
    depth = 0
    state = "code"
    at_line_start = True
    index = 0
    length = len(text)

    while index < length:
        if at_line_start and state == "code" and depth == 0:
            va = _VA_MARKER_RE.match(text, index)
            if va is not None:
                markers.append((index, va.end(), int(va.group(1), 16)))
            elif _TOP_LEVEL_BOUNDARY_RE.match(text, index) is not None:
                # The historical ``^\s*BOUNDARY`` regex consumed blank lines
                # before a file-scope boundary. Preserve that hash surface.
                boundary_start = index
                while boundary_start > 0:
                    previous_end = boundary_start - 1
                    previous_start = text.rfind("\n", 0, previous_end) + 1
                    if text[previous_start:previous_end].strip():
                        break
                    boundary_start = previous_start
                markers.append((boundary_start, None, None))

        char = text[index]
        following = text[index + 1] if index + 1 < length else ""

        if state == "line-comment":
            if char == "\n":
                state = "code"
                at_line_start = True
            else:
                at_line_start = False
            index += 1
            continue
        if state == "block-comment":
            if char == "*" and following == "/":
                state = "code"
                index += 2
                at_line_start = False
            else:
                at_line_start = char == "\n"
                index += 1
            continue
        if state in ("string", "character"):
            delimiter = '"' if state == "string" else "'"
            if char == "\\" and following:
                at_line_start = following == "\n"
                index += 2
            else:
                if char == delimiter:
                    state = "code"
                at_line_start = char == "\n"
                index += 1
            continue
        if state == "preprocessor":
            if char == "\\" and following == "\n":
                index += 2
                at_line_start = True
            elif char == "\n":
                state = "code"
                at_line_start = True
                index += 1
            else:
                at_line_start = False
                index += 1
            continue

        if at_line_start:
            line_end = text.find("\n", index)
            if line_end < 0:
                line_end = length
            first = index
            while first < line_end and text[first] in " \t":
                first += 1
            if first < line_end and text[first] == "#":
                state = "preprocessor"
                index = first + 1
                at_line_start = False
                continue
        if char == "/" and following == "/":
            state = "line-comment"
            index += 2
            at_line_start = False
            continue
        if char == "/" and following == "*":
            state = "block-comment"
            index += 2
            at_line_start = False
            continue
        if char == '"':
            state = "string"
        elif char == "'":
            state = "character"
        elif char == "{":
            depth += 1
        elif char == "}" and depth:
            depth -= 1
        at_line_start = char == "\n"
        index += 1

    for position, marker in enumerate(markers):
        marker_start, block_start, absolute_va = marker
        if block_start is None:
            continue
        legacy_end = markers[position + 1][0] if position + 1 < len(markers) else length
        block_end = _matched_function_end(text, block_start, legacy_end)
        if block_end is None:
            raise ValueError("VA(0x%x) has no lexically matched function body" % absolute_va)
        yield absolute_va, text[block_start:block_end]


def _matched_function_end(text, block_start, legacy_end):
    """Return the matched body end plus contiguous legacy trailing whitespace."""
    depth = 0
    body_started = False
    state = "code"
    at_line_start = False
    index = block_start

    while index < legacy_end:
        char = text[index]
        following = text[index + 1] if index + 1 < legacy_end else ""

        if state == "line-comment":
            if char == "\n":
                state = "code"
                at_line_start = True
            index += 1
            continue
        if state == "block-comment":
            if char == "*" and following == "/":
                state = "code"
                index += 2
                at_line_start = False
            else:
                at_line_start = char == "\n"
                index += 1
            continue
        if state in ("string", "character"):
            delimiter = '"' if state == "string" else "'"
            if char == "\\" and following:
                at_line_start = following == "\n"
                index += 2
            else:
                if char == delimiter:
                    state = "code"
                at_line_start = char == "\n"
                index += 1
            continue
        if state == "preprocessor":
            if char == "\\" and following == "\n":
                index += 2
                at_line_start = True
            elif char == "\n":
                state = "code"
                at_line_start = True
                index += 1
            else:
                index += 1
            continue

        if at_line_start:
            line_end = text.find("\n", index, legacy_end)
            if line_end < 0:
                line_end = legacy_end
            first = index
            while first < line_end and text[first] in " \t":
                first += 1
            if first < line_end and text[first] == "#":
                state = "preprocessor"
                index = first + 1
                at_line_start = False
                continue
        if char == "/" and following == "/":
            state = "line-comment"
            index += 2
            at_line_start = False
            continue
        if char == "/" and following == "*":
            state = "block-comment"
            index += 2
            at_line_start = False
            continue
        if char == '"':
            state = "string"
        elif char == "'":
            state = "character"
        elif char == "{":
            body_started = True
            depth += 1
        elif char == "}" and body_started:
            depth -= 1
            if depth == 0:
                end = index + 1
                while end < legacy_end and text[end].isspace():
                    end += 1
                return end
        at_line_start = char == "\n"
        index += 1
    return None


def source_hashes():
    """{(unit, function): 12-hex sha1 of its normalized, brace-bounded source body}.

    Blocks begin inside ``VA(...)`` for historical compatibility and end after
    the matched function brace plus legacy trailing whitespace. Names are
    normalized by :func:`_normalize`, so codegen-neutral argument/member renames
    do not perturb the hash.
    """
    sym = _rva_to_sym(); cmap = _class_members()
    out = {}
    for cpp in sorted((REPO / "src").rglob("*.cpp")):
        text = cpp.read_text(errors="replace")
        for absolute_va, block in _source_function_blocks(text):
            rva = absolute_va - RVA_BASE
            key = sym.get(rva)
            if key:
                norm = _normalize(block, cmap)
                out[key] = hashlib.sha1(norm.encode("utf-8", "replace")).hexdigest()[:12]
    return out
