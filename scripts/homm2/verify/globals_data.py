#!/usr/bin/env python3
"""assert_globals_data.py — hard build gate for global DATA(VA) placement. DATA(0x<VA>) lives on
the global's DEFINITION in its owner .cpp (not on the header `extern`). Enforces:
  * every file-scope DEFINITION of an inventory data symbol carries DATA(0x<its exact VA>);
  * NO DATA() on a header `extern`;
  * every header global extern has an inventory symbol and an owner TU;
  * every DATA() VA is UNIQUE (one VA == one definition).
Run from repo root; exits 1 on any violation."""
from homm2.manifest import claim_files, image_lines
from homm2.core.paths import REPO, gen_dir, image_key
import csv, re, sys, glob

from homm2.core.usage import logged


@logged
def main(argv=None) -> int:
    IMG = 0x400000
    # External data is unique in the program, so its identifier alone names it. An
    # internal (file-static) definition is unique only within its unit — ten icon
    # decoders each define their own `s_clipB` at their own address — so its unit is
    # part of the key, and a .cpp definition is matched against its own unit first.
    rva_of = {}
    static_rva_of = {}
    for r in csv.DictReader(open(gen_dir() / "symbol_names.csv")):
        if r["kind"] != "data":
            continue
        m = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
        if not m:
            continue
        if r["name"].startswith("?"):
            rva_of.setdefault(m.group(1), int(r["rva"], 16))
        else:
            static_rva_of.setdefault((r["unit"], m.group(1)), int(r["rva"], 16))


    def want_rva(unit, name):
        """The VA this definition must claim, or None when nothing claims it."""
        if (unit, name) in static_rva_of:
            return static_rva_of[(unit, name)]
        return rva_of.get(name)

    DATA_RE = re.compile(r'^\s*DATA\(0x([0-9a-fA-F]+)\)\s+(.*)$')

    def def_name(code):
        """The global a file-scope definition line declares (type at col 0, no call/init), else None."""
        if not (code[:1].isalpha() or code[:1] == '_'):
            return None
        if '(' in code or '=' in code or ';' not in code:
            return None
        m = re.match(r'^[A-Za-z_][\w\s\*]*?[\s\*]([A-Za-z_]\w*)\s*(\[[^\]]*\])*\s*;', code)
        return m.group(1) if m else None

    bad = []; dup = []; seen = {}
    defined = set()                                     # globals this image's sources define
    # A `.bss` spelling alias (`#define gEditDialog gEditDlg // spelling fixes
    # .bss order`) declares the readable name; the inventory has the spelling.
    alias_re = re.compile(r'^\s*#\s*define\s+([A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*//\s*spelling fixes \.bss order')
    spelling = {}
    def note(va, loc):
        va = va.lower()
        if va in seen:
            dup.append((loc, va, seen[va]))
        else:
            seen[va] = loc

    # (1) .cpp DEFINITIONS: every inventory-global def carries DATA(exact VA). Unclaimed defs may carry
    #     DATA too (Phase-B module-private synthetic globals) — those just claim their VA for uniqueness.
    for c in [str(p.relative_to(REPO)) for p in claim_files()]:
        unit = c[len("src/"):-len(".cpp")]
        for i, line in enumerate(open(c), 1):
            loc = "%s:%d" % (c, i)
            dm = DATA_RE.match(line)
            rest = dm.group(2) if dm else line
            name = def_name(rest.split('//')[0])
            if not name:
                continue
            defined.add(name)
            claimed = want_rva(unit, name)
            if claimed is None:                           # unclaimed file-scope def (helper/static)
                if dm:
                    note(dm.group(1), loc)
                continue
            want = claimed + IMG
            if not dm:
                bad.append((loc, name, "no DATA() on definition", "%#010x" % want))
            elif int(dm.group(1), 16) != want:
                bad.append((loc, name, "DATA(%#010x)" % int(dm.group(1), 16), "%#010x" % want))
            else:
                note(dm.group(1), loc)

    # (2) HEADERS: declarations never claim storage, and every cross-TU global must have a retained
    #     inventory symbol. Anonymous/synthetic storage is module-private in its owning .cpp.
    # Only the headers this image's claim space includes: another image's
    # globals live in that image's inventory.
    include_re = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]')
    reachable, pending = set(), [p for p in claim_files()]
    while pending:
        for line in image_lines(pending.pop()):
            am = alias_re.match(line)
            if am:
                spelling[am.group(1)] = am.group(2)
            m = include_re.match(line)
            header = REPO / "include" / m.group(1) if m else None
            if header is not None and header.is_file() and header not in reachable:
                reachable.add(header)
                pending.append(header)
    for h in sorted(str(path.relative_to(REPO)) for path in reachable):
        for i, line in enumerate(open(h), 1):
            loc = "%s:%d" % (h, i)
            dm = DATA_RE.match(line)
            rest = (dm.group(2) if dm else line).strip()
            if not rest.startswith("extern"):
                if dm:
                    bad.append((loc, "?", "DATA() on a non-extern header line", "—"))
                continue
            nm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', rest.split('//')[0])
            name = nm.group(1) if nm else None
            if not name:
                continue                                  # `extern "C" T f(...);` — a function
            name = spelling.get(name, name)
            if image_key() != "game" and name not in defined:
                # Another image's header reaches the game's globals; only the
                # storage this image defines must carry its inventory symbol.
                continue
            if dm:
                bad.append((loc, name, "DATA() on header extern — move it to the .cpp definition", "—"))
            elif name not in rva_of:
                bad.append((loc, name,
                            "no inventory symbol -> make storage module-private or recover owner",
                            "—"))

    for loc, name, got, want in bad:
        print("  %s  %s  %s  (want DATA %s)" % (loc, name, got, want))
    for loc, va, first in dup:
        print("  %s  DATA(0x%s) repeats a VA first used at %s" % (loc, va, first))
    if bad or dup:
        print("\nGLOBALS-DATA FAIL: %d placement issue(s), %d duplicate VA(s)." % (len(bad), len(dup)))
        sys.exit(1)
    print("globals-data OK: every inventory global's DEFINITION carries DATA(its VA); no DATA on header "
          "externs; definition VAs unique.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
