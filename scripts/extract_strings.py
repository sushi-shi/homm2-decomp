#!/usr/bin/env python3
"""Extract & categorize interesting strings from a PE binary -> markdown.

The high-value categories for a decomp are SOURCE PATHS (original .c/.cpp/.h file
names) and ASSERT/EXPRESSION strings: MSVC `assert(expr)` embeds the *expression
text*, which often names real arguments / locals / members — a hint for naming that
CodeView (publics only, no locals) doesn't give us. These are HINTS, not ground
truth; always cross-check against the disassembly.

Usage:  extract_strings.py <exe> <out.md> [title]
"""
import re, sys


def scan(data, n=5):
    out, cur = [], bytearray()
    for b in data:
        if 0x20 <= b < 0x7F:
            cur.append(b)
        else:
            if len(cur) >= n:
                out.append(cur.decode("latin1"))
            cur = bytearray()
    if len(cur) >= n:
        out.append(cur.decode("latin1"))
    return out


def main():
    exe, out = sys.argv[1], sys.argv[2]
    title = sys.argv[3] if len(sys.argv) > 3 else exe
    uniq = sorted(set(scan(open(exe, "rb").read())))
    # drop font/charset ramp tables and sub-printable-sequence garbage
    ramp = re.compile(r"0123456789|abcdefghij|ABCDEFGHIJ|!\"#\$%&|\)\*\+,-\./")
    clean = [s for s in uniq if not ramp.search(s) and re.search(r"[A-Za-z]{3}", s)]

    paths = sorted({s for s in clean
                    if re.search(r"[A-Za-z]:\\", s)
                    or re.search(r"\.(c|cpp|cxx|h|hpp|asm)$", s, re.I)
                    or ("\\" in s and re.search(r"\.(c|cpp|h)", s, re.I))})
    exprs = sorted({s for s in clean if s not in paths and not s.lstrip().startswith("%")
                    and re.search(r"[A-Za-z_]\w*\s*(==|!=|<=|>=|->|\[|::)|"
                                  r"\b(assert|sizeof|NULL|nullptr)\b", s)})
    fmts = sorted({s for s in clean if re.search(r"%[-+ #0-9.*]*[diouxXeEfgGcspn]", s)})
    used = set(paths) | set(exprs) | set(fmts)
    other = [s for s in clean if s not in used and len(s) >= 6]

    def sec(name, items):
        return [f"## {name} ({len(items)})", "", *("    " + s for s in items), ""]

    md = ["\n".join([
        f"# Strings — {title}", "",
        "Extracted by `scripts/extract_strings.py` (printable runs >= 5 bytes, deduped).",
        "Source paths recover original file names; assert/expression strings can recover",
        "argument / local / member identifiers (MSVC `assert()` embeds the expression).",
        "**Hints, not authoritative** — cross-check against the disassembly.", "",
        *sec("Source paths / file names", paths),
        *sec("Assert / expression strings (identifier hints)", exprs),
        *sec("Format strings", fmts),
        *sec("Other strings", other),
    ])]
    open(out, "w").write(md[0])
    print(f"{out}: {len(uniq)} unique  ({len(paths)} paths, {len(exprs)} exprs, "
          f"{len(fmts)} fmts, {len(other)} other)")


if __name__ == "__main__":
    main()
