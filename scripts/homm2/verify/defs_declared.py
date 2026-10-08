#!/usr/bin/env python3
"""assert_defs_declared.py — hard build gate: every free function DEFINED in a .cpp must be
DECLARED in that TU's owner header (include/<TIER>/<TU>.h, or the header OWNER_HEADERS names),
and the .cpp must #include it. With
local declarations already forbidden (assert_decls), this closes the loop: a definition's
prototype lives in a header, so callers share the one canonical declaration (no drift).
Member functions are exempt (declared in their class header). Run from repo root; exits 1."""
import re, os, glob, sys

from homm2.core.usage import logged

#: A unit whose free functions live in a camelCase header named for its class
#: or role (HoMM1's layout) rather than in include/<TIER>/<TU>.h.
OWNER_HEADERS = {
    "SOURCE/SMACKMGR": "SOURCE/smackManager.h",
}


@logged
def main(argv=None) -> int:
    sys.path.insert(0, "scripts")
    from homm2.workflow.gen_module_header import free_decls

    def fname(decl):
        # __declspec(naked) etc. is a definition-only attribute (not part of the header
        # declaration); strip it so the name regex doesn't grab "__declspec" as the fn name.
        decl = re.sub(r'__declspec\s*\([^)]*\)', ' ', decl)
        decl = re.sub(r'^H2_ENUM_RETURN\s*\([^)]*\)\s*', '', decl)
        return re.search(r'\b([A-Za-z_]\w*)\s*\(', decl).group(1)

    bad = []
    for cpp in sorted(glob.glob("src/**/*.cpp", recursive=True)):
        decls = free_decls(cpp)
        if not decls:
            continue
        tier = re.search(r'src/([A-Za-z]+)/', cpp).group(1)
        base = os.path.basename(cpp)[:-4]
        owner = OWNER_HEADERS.get("%s/%s" % (tier, base), "%s/%s.h" % (tier, base))
        hdr = "include/" + owner
        src = open(cpp).read()
        if "#include <%s>" % owner not in src:
            bad.append((cpp, "does not #include its owner header <%s>" % owner)); continue
        if not os.path.exists(hdr):
            bad.append((cpp, "owner header %s missing" % hdr)); continue
        htext = open(hdr).read()
        for d in decls:
            n = fname(d)
            if not re.search(r'\b%s\s*\(' % re.escape(n), htext):
                bad.append((cpp, "defines %s() but %s does not declare it" % (n, owner)))

    for cpp, msg in bad:
        print("  %s: %s" % (cpp, msg))
    if bad:
        print("\nDEFS-DECLARED FAIL: %d definition(s) without a header declaration." % len(bad))
        sys.exit(1)
    print("defs-declared OK: every free-fn definition is declared in its owner header.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
