#!/usr/bin/env python3
"""assert_relocs.py — OPT-IN reloc-target audit (`homm2 relocs`; NOT a hard build gate). For every
near-EXACT function, verify each relocation's TARGET address. objdiff MASKS all relocations when
scoring, so it never checks the target — a "100% match" can silently reference the WRONG
global/const/field/function, or even call a FABRICATED function that is declared, never defined, and
(because we compile-but-don't-link) never caught as an unresolved external. This closes that blind spot.

Order-INDEPENDENT: we resolve each side's relocs to a SET of final RVAs (symbol RVA + instruction
addend; REL32 -> the symbol's own RVA) and flag any address the BASE references that retail never
does. We also flag any base '?'-mangled symbol that resolves to NEITHER CodeView (symbol_names.csv)
NOR a DATA()-pinned definition — i.e. fabricated.

Data offsets come from symbol_names.csv (authoritative CodeView RVAs). Module-private synthetic
storage has no CodeView symbol, so its VA is read from the owning .cpp DATA(0x..) definition.

It is OPT-IN (not in `homm2 build`'s hard gates) because it also surfaces unreproducible link
artifacts — chiefly the delinker's `empty_stub`, the synthetic name for a COMDAT-folded empty (`ret`)
function that base still calls by its own CodeView name. That is a delinker-side concern, not a
source bug, so it must not break every build.

Review one function (order-independent; usable on <100% walls):
    homm2 relocs 0x<rva>
Review relocation-count deficits among incomplete functions:
    homm2 relocs --counts BASE
Run from repo root; exits 1 on any wrong/fabricated reloc target.
"""
import sys, os, re, csv, json, glob, struct, subprocess
from collections import Counter

IMAGE_BASE = 0x400000

def _norm(s):   # llvm-objdump renders each non-printable mangled byte as one '_'; symbol_names.csv
    s = s.replace('\xef\xbf\xbd', '_')          # stores it as the UTF-8 replacement char (3 bytes)
    return ''.join(c if 0x20 <= ord(c) < 0x7f else '_' for c in s)
def _key(s):    # encoding-stable key for ??_C@ strings: the ASCII "??_C@_<len>@<hash>@" prefix
    m = re.match(r'(\?\?_C@_\w+@\w+@)', s); return m.group(1) if m else _norm(s)

def load_symbols():
    sym, dups = {}, {}
    for r in csv.reader(open("build/gen/symbol_names.csv", encoding="latin-1")):
        if len(r) >= 2:
            try:
                v = int(r[0], 16)
            except ValueError:
                continue
            for k in (r[1], _norm(r[1]), _key(r[1])):
                # content-hashed string names (??_C@) can COLLIDE — two distinct addresses share one
                # name (e.g. two "!" literals). Record every address per name so an ambiguous target
                # reloc can match ANY of them (they're value-identical), not just the last one loaded.
                sym.setdefault(k, v); dups.setdefault(k, set()).add(v)
    data = {}                                    # DATA() lives on canonical .cpp definitions;
    for f in glob.glob("src/**/*.cpp", recursive=True) + glob.glob("include/**/*.h", recursive=True):
        for ln in open(f, encoding="latin-1"):   # headers are scanned to diagnose stale violations
            m = re.search(r'DATA\(0x([0-9a-fA-F]+)\).*?\b([A-Za-z_]\w*)\s*(?:\[|;|=)', ln)
            if m:
                data[m.group(2)] = int(m.group(1), 16) - IMAGE_BASE
    return sym, data, dups

def resolve(sym, data, typ, s, add):
    mc = re.match(r'const_([0-9a-fA-F]+)$', s)  # delinker names unlabeled data addrs const_<RVA>
    if mc:
        b = int(mc.group(1), 16)
    else:
        b = sym.get(s) or sym.get(_norm(s)) or sym.get(_key(s))
        if b is None:
            mm = re.match(r'\?(\w+)@@', s)      # ?<name>@@... -> a DATA()-pinned global
            if mm:
                b = data.get(mm.group(1))
    if b is None:
        return None
    return b if typ == 'REL32' else (b + add) & 0xffffffff

def is_fake(sym, data, s):
    if s.startswith('_') or s.startswith('??_C@') or s.startswith('$SG') or not s.startswith('?'):
        return False                             # CRT/import/string/local -> outside CodeView, fine
    return resolve(sym, data, 'DIR32', s, 0) is None

def _addend(insn):
    insn = insn.split('#')[0]                    # drop llvm-objdump's "# imm = 0xNN" annotation
    m = re.search(r'\[([^\]]+)\]', insn)         # Intel memory operand: use its displacement,
    if m:                                        # never a following store immediate
        disps = re.findall(r'(?<![\w\]])(-?0x[0-9a-f]+)', m.group(1))
        if disps:
            return int(disps[-1], 16) & 0xffffffff
        return 0
    # llvm-objdump -dr defaults to AT&T syntax. An indexed memory displacement precedes `(`;
    # it is the relocation addend even when the instruction also stores an immediate afterward.
    m = re.search(r'(?:(?<![\w$])(-?0x[0-9a-f]+))?\([^)]*\)', insn)
    if m:
        return int(m.group(1), 16) & 0xffffffff if m.group(1) else 0
    # Absolute AT&T memory operands have no parentheses and no `$`. Prefer one over an immediate.
    disps = re.findall(r'(?<![\w$])(-?0x[0-9a-f]+)', insn)
    if disps:
        return int(disps[-1], 16) & 0xffffffff
    imms = re.findall(r'\$(-?0x[0-9a-f]+)', insn)
    return int(imms[-1], 16) & 0xffffffff if imms else 0     # -0x4c disp -> 0xffffffb4 addend

def _text_bytes(obj):
    """Read the COFF .text payload so each relocation uses its own encoded addend.

    An x86 instruction may contain multiple relocation fields (for example a
    memory displacement and an immediate function pointer). Deriving both from
    the rendered instruction text assigns the first operand's displacement to
    every relocation. The four bytes at the relocation site are the actual COFF
    addend and are unambiguous.
    """
    blob = open(obj, "rb").read()
    if len(blob) < 20:
        return b""
    section_count = struct.unpack_from("<H", blob, 2)[0]
    optional_size = struct.unpack_from("<H", blob, 16)[0]
    first = 20 + optional_size
    for index in range(section_count):
        off = first + index * 40
        if blob[off:off + 8].rstrip(b"\0") != b".text":
            continue
        size, raw = struct.unpack_from("<II", blob, off + 16)
        return blob[raw:raw + size]
    return b""

def parse_obj(obj, with_sites=False):
    """llvm-objdump -dr -> ordered relocations per function; __imp__ skipped.

    ``with_sites`` prefixes each relocation with its function-relative COFF offset.  Those offsets
    let the empty-COMDAT audit recover the original retail REL32 destination from the PE without
    weakening ordinary target checking.
    """
    out = subprocess.run(["llvm-objdump", "-dr", obj], capture_output=True, text=True).stdout
    text_bytes = _text_bytes(obj)
    funcs, cur, cur_start, prev = {}, None, 0, ""
    for ln in out.splitlines():
        m = re.match(r'^([0-9a-f]+) <(.+?)>:', ln)
        if m:
            name = m.group(2)
            if name.startswith('$') and cur is not None:
                # MSVC emits block labels as COFF symbols inside a function COMDAT. llvm-objdump
                # prints each as a new title, but its following relocations still belong to the
                # containing function; do not truncate the function at the first local label.
                prev = ""
                continue
            cur = name
            cur_start = int(m.group(1), 16)
            funcs.setdefault(cur, [])
            prev = ""
            continue
        if cur is None:
            continue
        mr = re.search(r'IMAGE_REL_I386_(\w+)\s+(\S+)', ln)
        if mr:
            s = mr.group(2)
            if not s.startswith('__imp__'):
                mo = re.match(r'^\s*([0-9a-f]+):', ln)
                site = int(mo.group(1), 16) if mo else -1
                if 0 <= site <= len(text_bytes) - 4:
                    addend = struct.unpack_from("<I", text_bytes, site)[0]
                else:
                    addend = _addend(prev)
                reloc = (mr.group(1), s, addend)
                if with_sites:
                    funcs[cur].append((site - cur_start,) + reloc)
                else:
                    funcs[cur].append(reloc)
            continue
        mi = re.match(r'^\s*[0-9a-f]+:\s+(?:[0-9a-f]{2}\s+)+(.+)$', ln)
        if mi:
            prev = mi.group(1).replace('\t', ' ').strip()
    return funcs

_PE_SECTIONS = None

def _pe_sections():
    """Return (image bytes, [(rva, span, raw offset), ...]) for the authoritative retail PE."""
    global _PE_SECTIONS
    if _PE_SECTIONS is not None:
        return _PE_SECTIONS
    data = open("build/orig/HEROES2W.EXE", "rb").read()
    pe = struct.unpack_from("<L", data, 0x3c)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    optsz = struct.unpack_from("<H", data, pe + 20)[0]
    sections = []
    off = pe + 24 + optsz
    for i in range(nsec):
        sh = off + i * 40
        vsize, rva, raw_size, raw = struct.unpack_from("<LLLL", data, sh + 8)
        sections.append((rva, max(vsize, raw_size), raw))
    _PE_SECTIONS = data, sections
    return _PE_SECTIONS

def _pe_read(rva, size):
    data, sections = _pe_sections()
    for start, span, raw in sections:
        if start <= rva and rva + size <= start + span:
            off = raw + rva - start
            return data[off:off + size]
    return None

def _normalize_empty_stub_relocs(function_rva, target_relocs, base_sites, target_sites, sym, data):
    """Recover delinked ``empty_stub`` identities from the original retail REL32 bytes.

    Acceptance is site-specific: the base and target relocations must occupy the same relative
    field, and the retail PE displacement at that field must resolve to the base CodeView callee.
    A wrong named empty function therefore remains a wrong target instead of being generically
    allowlisted merely because its body also returns.
    """
    base_by_site = {r[0]: r[1:] for r in base_sites}
    normalized = []
    for reloc, site_reloc in zip(target_relocs, target_sites):
        site, typ, target_name, _add = site_reloc
        if typ != 'REL32' or target_name != 'empty_stub':
            normalized.append(reloc)
            continue
        base = base_by_site.get(site)
        if base is None or base[0] != 'REL32':
            normalized.append(reloc)
            continue
        base_rva = resolve(sym, data, *base)
        disp_bytes = _pe_read(function_rva + site, 4)
        if base_rva is None or disp_bytes is None:
            normalized.append(reloc)
            continue
        disp = struct.unpack("<l", disp_bytes)[0]
        retail_rva = (function_rva + site + 4 + disp) & 0xffffffff
        normalized.append(base if retail_rva == base_rva else reloc)
    return normalized

# Pre-existing reloc-target discrepancies surfaced when this gate was introduced (objdiff masked
# them, so they scored ~100%). Two kinds: (A) SUSPECTED wrong VA — a base symbol that disagrees with
# where retail reads (likely a wrong synthetic-global DATA(0x..) guess, or a wrong array base — the
# gDwelling*/gHierarchyMask/gMonsterInfo cases are the same shape: base is 0x4c off retail). (B)
# factoring/codegen differences the multiset can't distinguish (base uses a static field+offset or a
# direct call where retail computes the offset at runtime / dispatches virtually — same effect).
# TODO: confirm each and fix (correct the VA / restructure the source), then delete the entry.
# Keyed (unit, function, base-symbol).
ALLOWLIST = set()

def _cands(dups, s, add):
    """Every address a target reloc name could denote, +addend. Content-hashed string names (??_C@)
    COLLIDE — one name, several value-identical addresses; the reloc may mean ANY, so accept all."""
    c = dups.get(s) or dups.get(_norm(s)) or dups.get(_key(s)) or set()
    return {(a + add) & 0xffffffff for a in c}

def _tvas(sym, data, dups, tgt_relocs):
    """Multiset of every RVA retail references. Order-independent (robust to near-100%-wall reorder).
    An ambiguous (dup) name contributes ALL its candidate addrs per occurrence, so a base ref to any
    one of them clears; a unique name contributes its single resolved addr."""
    tvas = Counter()
    for r in tgt_relocs:
        v = resolve(sym, data, *r)
        if v is None:
            continue
        cs = _cands(dups, r[1], r[2]) if r[0] != 'REL32' else set()
        if len(cs) > 1:
            for c in cs:
                tvas[c] += 1
        else:
            tvas[v] += 1
    return tvas

def check_fn(sym, data, dups, unit, name, base_relocs, tgt_relocs):
    """Order-INDEPENDENT MULTISET reloc-target check. We flag `only_base`: an address the BASE
    references MORE times than retail does — a wrong global/const/field/fn (multiset, not set, so an
    injected wrong ref is caught even when that address is ALSO referenced elsewhere in the same fn).
    Plus a FAKE check for unresolvable base '?'-symbols. Factoring/near-wall reorder no longer false-
    positive here: they're fixed at the resolve layer (signed addend, const_<rva>, dup names)."""
    probs = []
    for r in base_relocs:                        # (1) fabricated function/data symbol
        if is_fake(sym, data, r[1]) and (unit, name, r[1]) not in ALLOWLIST:
            probs.append("FAKE call/ref '%s' — exists in neither CodeView nor a DATA() global" % r[1])
    tvas = _tvas(sym, data, dups, tgt_relocs)
    bvas, va_sym = Counter(), {}
    for r in base_relocs:
        v = resolve(sym, data, *r)
        if v is not None:
            bvas[v] += 1; va_sym.setdefault(v, r[1])
    for v, n in (bvas - tvas).items():            # (2) base references an addr retail never does (or fewer)
        bs = va_sym[v]
        if '_00A@' in bs or (unit, name, bs) in ALLOWLIST:
            continue
        probs.append("WRONG: base references 0x%x (%s) x%d — retail never references it (or fewer)" % (v, bs, n))
    return probs

def _function_for_arg(arg):
    """Resolve an RVA or decorated name to the generated (unit, function) identity."""
    try:
        wanted_rva = int(arg, 0)
    except ValueError:
        wanted_rva = None
    with open("build/gen/symbol_names.csv", encoding="latin-1") as f:
        for row in csv.DictReader(f):
            if row.get("kind") != "func":
                continue
            try:
                row_rva = int(row["rva"], 0)
            except (KeyError, ValueError):
                continue
            if row["name"] == arg or (wanted_rva is not None and row_rva == wanted_rva):
                return row["unit"], row["name"], row_rva
    raise SystemExit("function not found in build/gen/symbol_names.csv: %s" % arg)

def review(rva):
    """Single-function multiset review (order-independent; usable on <100% walls)."""
    sym, data, dups = load_symbols()
    unit, name, function_rva = _function_for_arg(rva)
    base_obj = "build/objdiff/base/%s.obj" % unit
    target_obj = "build/delink/%s.c.obj" % unit
    B = parse_obj(base_obj).get(name, [])
    T = parse_obj(target_obj).get(name, [])
    BS = parse_obj(base_obj, with_sites=True).get(name, [])
    TS = parse_obj(target_obj, with_sites=True).get(name, [])
    T = _normalize_empty_stub_relocs(function_rva, T, BS, TS, sym, data)
    for s in sorted({r[1] for r in B if is_fake(sym, data, r[1])}):
        print("  !! FAKE base references '%s'" % s)
    def bvas(rs):
        c, sy = Counter(), {}
        for r in rs:
            if '_00A@' in r[1]:
                continue
            v = resolve(sym, data, *r)
            if v is not None:
                c[v] += 1; sy.setdefault(v, r[1])
        return c, sy
    bc, va_sym = bvas(B)
    tvas = _tvas(sym, data, dups, [r for r in T if '_00A@' not in r[1]])
    diff = bc - tvas
    for v, n in sorted(diff.items()):
        print("  base references 0x%x (%s) x%d that retail never does (wrong/extra global/const/fn)"
              % (v, va_sym[v], n))
    print("base relocs=%d target relocs=%d  only-base=%d" % (len(B), len(T), sum(diff.values())))

def review_counts(scope="BASE"):
    """List incomplete functions whose relocation occurrence counts still differ.

    This is a structural-work queue, not a wrong-target report: missing/excess counts usually mean
    the source has not yet recovered retail's control flow or publication lifetimes. Retained fuzzy
    maxima select incomplete functions so transient TU-state dips do not reopen finished work.
    """
    prefix = scope.rstrip("/") + "/"
    maxima = {}
    with open("config/match_baseline.tsv", encoding="utf-8") as f:
        for row in csv.reader(f, delimiter="\t"):
            if len(row) < 3 or row[0].startswith("#"):
                continue
            try:
                maxima[(row[0], row[1])] = float(row[2])
            except ValueError:
                continue

    report = json.load(open("build/objdiff/report.json"))
    rows = []
    for unit in report["units"]:
        unit_name = unit["name"]
        if not unit_name.startswith(prefix):
            continue
        base_obj = "build/objdiff/base/%s.obj" % unit_name
        target_obj = "build/delink/%s.c.obj" % unit_name
        if not (os.path.exists(base_obj) and os.path.exists(target_obj)):
            continue
        base_functions, target_functions = parse_obj(base_obj), parse_obj(target_obj)
        for fn in unit.get("functions", []):
            name = fn["name"]
            retained = maxima.get((unit_name, name), fn.get("fuzzy_match_percent", 0.0))
            if retained >= 100.0 or name not in base_functions or name not in target_functions:
                continue
            base_count = len(base_functions[name])
            target_count = len(target_functions[name])
            if base_count == target_count:
                continue
            missing = max(target_count - base_count, 0)
            excess = max(base_count - target_count, 0)
            size = int(fn.get("size", 0))
            unmatched = size * (100.0 - retained) / 100.0
            demangled = fn.get("metadata", {}).get("demangled_name", name)
            rows.append((missing + excess, unmatched, unit_name, retained, size,
                         base_count, target_count, missing, excess, demangled))

    # This is an input to the normal hardest-first campaign, so rank recovered byte potential
    # before raw relocation-count delta.
    rows.sort(key=lambda row: (-row[1], -row[0], row[2], row[9]))
    print("unit\tretained_pct\tsize\tunmatched_bytes\tbase_relocs\tretail_relocs\tmissing\texcess\tfunction")
    for _, unmatched, unit, retained, size, base_count, target_count, missing, excess, name in rows:
        print("%s\t%.4f\t%d\t%.2f\t%d\t%d\t%d\t%d\t%s" %
              (unit, retained, size, unmatched, base_count, target_count, missing, excess, name))
    print("summary\tfunctions=%d\tmissing=%d\texcess=%d" %
          (len(rows), sum(row[7] for row in rows), sum(row[8] for row in rows)))
    return 0

def main():
    if len(sys.argv) > 1 and sys.argv[1] == "--counts":
        return review_counts(sys.argv[2] if len(sys.argv) > 2 else "BASE")
    if len(sys.argv) > 1:                        # single-function review mode
        review(sys.argv[1]); return 0
    sym, data, dups = load_symbols()
    function_rvas = {}
    with open("build/gen/symbol_names.csv", encoding="latin-1") as f:
        for row in csv.DictReader(f):
            if row.get("kind") == "func":
                function_rvas[(row["unit"], row["name"])] = int(row["rva"], 0)
    report = json.load(open("build/objdiff/report.json"))
    bad = []
    # A WRONG reloc doesn't drop objdiff to a low %, it costs a TINY fraction (~0.005%/reloc) that
    # rounds to "100.00%" in the display — so we must audit near-exact fns, not just == 100. The
    # multiset check (check_fn) is order-independent, so 99.5% (with its minor reordering) is safe.
    THRESHOLD = 99.5
    for u in report["units"]:
        unit = u["name"]
        exact = {f["name"] for f in u.get("functions", []) if f.get("fuzzy_match_percent", 0) >= THRESHOLD}
        if not exact:
            continue
        base_obj = "build/objdiff/base/%s.obj" % unit
        tgt_obj = "build/delink/%s.c.obj" % unit
        if not (os.path.exists(base_obj) and os.path.exists(tgt_obj)):
            continue
        bf, tf = parse_obj(base_obj), parse_obj(tgt_obj)
        bfs = parse_obj(base_obj, with_sites=True)
        tfs = parse_obj(tgt_obj, with_sites=True)
        for name in sorted(exact):
            if name not in bf or name not in tf:
                continue
            target_relocs = _normalize_empty_stub_relocs(
                function_rvas[(unit, name)], tf[name], bfs[name], tfs[name], sym, data)
            for p in check_fn(sym, data, dups, unit, name, bf[name], target_relocs):
                bad.append((unit, name, p))
    for unit, name, p in bad:
        print("  %s  %s: %s" % (unit, name, p))
    if bad:
        print("\nRELOCS FAIL: %d exact function(s) reference a wrong/fabricated target "
              "(objdiff masks relocs, so it can't see this)." % len(bad))
        return 1
    print("relocs OK: every 100%-exact function's reloc targets resolve to the retail address.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
