"""homm2 status - run objdiff over the base<->target objs and report match %.

  homm2 status                 print per-unit + overall match %
  homm2 status update          refresh the baseline (per-function max% keyed by source hash)
  homm2 status check           gate: fail only if an EDITED function lost ground vs its own max
  homm2 status --write-readme  refresh the <!-- match-score --> block in README.md

Max-% model: each function's SOURCE block is hashed. The baseline stores, per function, the best
fuzzy% ever seen *for that source hash* (max%) plus the hash. When a sibling changes and perturbs a
function (tu-cumulative eval-order), its live fuzzy% (current) dips but its source hash is unchanged,
so max% is preserved — no need to chase it; a later pass recovers current. When the function's OWN
source changes, its hash changes and max% resets to the new current. **max% == 100% for every
function ⇒ the work is essentially done** (current recovers in a second pass).
"""
import json, os, re, subprocess, sys, hashlib, csv
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
RM_START, RM_END = "<!-- match-score:start -->", "<!-- match-score:end -->"
RVA_BASE = 0x400000


def _i(v): return int(v) if v not in (None, "") else 0


def load_report():
    od = REPO / "build/objdiff"; rep = od / "report.json"
    subprocess.run(["objdiff-cli", "report", "generate", "-p", str(od), "-o", str(rep)],
                   cwd=REPO, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return json.loads(rep.read_text()) if rep.exists() else None


def unit_pct(u):
    return float((u.get("measures", {}) or {}).get("matched_code_percent", 0) or 0)


# ---------------------------------------------------------------- source hashes ---
def _rva_to_sym():
    """rva -> (unit, mangled_name) for .text functions, from the generated CSV."""
    out = {}
    csvp = REPO / "build/gen/symbol_names.csv"
    if not csvp.exists():
        return out
    for row in csv.reader(csvp.open()):
        if len(row) < 5 or row[4] != "func":
            continue
        try:
            out[int(row[0], 16)] = (row[2], row[1])
        except ValueError:
            pass
    return out


def source_hashes():
    """{(unit, function): 12-hex sha1 of that function's SOURCE block}. A block runs from its
    VA(...) marker to the next VA/DATA/VTBL/section marker, so editing one function changes only
    its own hash — a sibling edit leaves it untouched (that is what keeps max% stable)."""
    sym = _rva_to_sym()
    out = {}
    for cpp in sorted((REPO / "src").rglob("*.cpp")):
        text = cpp.read_text(errors="replace")
        parts = re.split(r"(?m)^VA\(0x([0-9a-fA-F]+)\s*,", text)
        for i in range(1, len(parts), 2):
            try:
                rva = int(parts[i], 16) - RVA_BASE
            except ValueError:
                continue
            block = parts[i + 1] if i + 1 < len(parts) else ""
            block = re.split(r"(?m)^\s*(?:DATA\(|VTBL\(|// ===|#endif)", block)[0]
            key = sym.get(rva)
            if key:
                out[key] = hashlib.sha1(block.encode("utf-8", "replace")).hexdigest()[:12]
    return out


def _md_table(headers, aligns, rows):
    widths = [len(h) for h in headers]
    for r in rows:
        for i, c in enumerate(r):
            widths[i] = max(widths[i], len(c))

    def cell(text, i):
        return text.rjust(widths[i]) if aligns[i] == "r" else text.ljust(widths[i])

    def row(cells):
        return "| " + " | ".join(cell(c, i) for i, c in enumerate(cells)) + " |"

    sep = ["-" * (w - 1) + ":" if a == "r" else ":" + "-" * (w - 1)
           for w, a in zip(widths, aligns)]
    return [row(headers), "| " + " | ".join(sep) + " |", *(row(r) for r in rows)]


def readme_block(data, base):
    """per-tier match table. base = {(unit,fn): (max_pct, hash)}; shows current AND max fuzzy."""
    tiers = {}
    for u in data.get("units", []):
        tier = u.get("name", "?").split("/")[0]
        un = u.get("name", "?")
        m = u.get("measures", {}) or {}
        t = tiers.setdefault(tier, {"units": 0, "fe": 0, "ft": 0,
                                    "fz": 0.0, "fzt": 0, "mx": 0.0})
        t["units"] += 1
        t["fe"] += _i(m.get("matched_functions")); t["ft"] += _i(m.get("total_functions"))
        for f in u.get("functions", []) or []:
            sz = _i(f.get("size")); cur = f.get("fuzzy_match_percent") or 0.0
            mx = base.get((un, f.get("name", "?")), (cur, None))[0]
            mx = max(mx, cur)  # live current can only raise the displayed max
            t["fz"] += sz * cur / 100.0; t["fzt"] += sz; t["mx"] += sz * mx / 100.0
    rows = []
    TE = TT = 0; FZ = MX = 0.0; FZT = 0
    for tier in sorted(tiers, key=lambda k: -tiers[k]["ft"]):
        d = tiers[tier]
        if d["ft"] == 0:
            continue
        TE += d["fe"]; TT += d["ft"]; FZ += d["fz"]; FZT += d["fzt"]; MX += d["mx"]
        fp = 100 * d["fe"] / d["ft"]
        zp = 100 * d["fz"] / d["fzt"] if d["fzt"] else 0
        mp = 100 * d["mx"] / d["fzt"] if d["fzt"] else 0
        rows.append([f"`{tier}`", str(d["units"]),
                     f"{d['fe']} / {d['ft']} ({fp:.1f}%)", f"{zp:.1f}%", f"{mp:.1f}%"])
    overall_f = 100 * TE / TT if TT else 0
    overall_z = 100 * FZ / FZT if FZT else 0
    overall_m = 100 * MX / FZT if FZT else 0
    out = [RM_START, "## Match status", "",
           "_Auto-generated by `homm2 status --write-readme` (refreshed by `homm2 build`); do not hand-edit._", "",
           f"**Overall: {TE} / {TT} functions exact ({overall_f:.2f}%) &middot; "
           f"{overall_z:.2f}% fuzzy &middot; {overall_m:.2f}% fuzzy-max.**", "",
           "_**Functions exact** = byte-identical. **Fuzzy** = live size-weighted instruction match. "
           "**Fuzzy-max** = best fuzzy ever reached per function for its current source (tu-cumulative "
           "dips don't lower it); **fuzzy-max 100% ⇒ essentially done.**_", "",
           *_md_table(["Module", "Units", "Functions exact", "Fuzzy", "Fuzzy-max"], "lrrrr", rows),
           "", RM_END]
    return "\n".join(out)


BASELINE = REPO / "config/match_baseline.tsv"


def _fn_fuzzy(data):
    out = {}
    for u in data.get("units", []):
        un = u.get("name", "?")
        for f in u.get("functions", []) or []:
            out[(un, f.get("name", "?"))] = float(f.get("fuzzy_match_percent") or 0.0)
    return out


def load_baseline():
    """{(unit, function): (max_pct, src_hash|None)}. Tolerates the old 3-column format."""
    base = {}
    if BASELINE.exists():
        for line in BASELINE.read_text().splitlines():
            if not line or line.startswith("#"):
                continue
            p = line.split("\t")
            if len(p) >= 4:
                base[(p[0], p[1])] = (float(p[2]), p[3])
            elif len(p) == 3:
                base[(p[0], p[1])] = (float(p[2]), None)
    return base


def write_baseline(base):
    lines = ["# homm2 match baseline - per-function max fuzzy% keyed by source hash.",
             "# Generated by `homm2 status update`; do not hand-edit.  unit<TAB>fn<TAB>max_fuzzy<TAB>src_hash"]
    for (un, fn), (mx, h) in sorted(base.items()):
        lines.append(f"{un}\t{fn}\t{mx:.4f}\t{h or ''}")
    BASELINE.write_text("\n".join(lines) + "\n")


def cmd_update(data, _accept_ignored=False):
    """max% keyed by source hash: same hash -> max(old, current); changed hash -> reset to current.
    No blessing needed for tu-cumulative dips (same hash keeps its max)."""
    cur = _fn_fuzzy(data); base = load_baseline(); sh = source_hashes()
    out = {}
    for k, c in cur.items():
        h = sh.get(k)
        old_mx, old_h = base.get(k, (0.0, None))
        # reset only when a KNOWN hash actually changed (the function's own source was edited);
        # on first-time migration (old_h None) or an unchanged hash, keep the accumulated max.
        mx = c if (old_h and h != old_h) else max(old_mx, c)
        out[k] = (mx, h)
    write_baseline(out)
    at100 = sum(1 for (mx, _) in out.values() if mx >= 99.995)
    print(f"[status] baseline updated: {len(out)} functions -> {BASELINE.relative_to(REPO)} "
          f"({at100} at max 100%)")
    return 0


def cmd_check(data, eps=0.05):
    """Regression = a function whose SOURCE changed and whose current is now below its former max
    (an edit that lost ground). tu-cumulative dips (same hash, lower current) are NOT flagged —
    their max is preserved, so they recover in a later pass."""
    cur = _fn_fuzzy(data); base = load_baseline(); sh = source_hashes()
    if not base:
        print("[status] no baseline yet - seed it: homm2 status update"); return 0
    regr = []
    for k, (old_mx, old_h) in base.items():
        c = cur.get(k, 0.0); h = sh.get(k)
        if h is not None and h != old_h and c + eps < old_mx:
            regr.append((k, old_mx, c))
    if regr:
        print(f"[status] {len(regr)} REGRESSION(S) — edited function now below its former max%:")
        for (un, fn), b, c in sorted(regr)[:30]:
            print(f"   {c:6.2f}% < max {b:6.2f}%  {un}::{fn}")
        print("  intended? re-baseline (resets these to current): homm2 status update")
        return 1
    print("[status] no regressions (tu-cumulative dips ignored; max% preserved)."); return 0


def main(argv=None):
    argv = list(argv or [])
    data = load_report()
    if data is None:
        print("[status] no report (run 'homm2 build' first)"); return 1
    if argv and argv[0] == "update":
        return cmd_update(data, "--accept-regressions" in argv)
    if argv and argv[0] == "check":
        return cmd_check(data)
    if "--write-readme" in argv:
        block = readme_block(data, load_baseline()); rm = REPO / "README.md"
        text = rm.read_text() if rm.exists() else "# homm2-decomp\n\n" + RM_START + "\n" + RM_END + "\n"
        if RM_START in text and RM_END in text:
            pre = text[:text.index(RM_START)]; post = text[text.index(RM_END) + len(RM_END):]
            rm.write_text(pre + block + post)
        else:
            rm.write_text(text.rstrip() + "\n\n" + block + "\n")
        print("[status] refreshed README.md match block")
        return 0
    base = load_baseline(); sh = source_hashes(); cur = _fn_fuzzy(data)
    started = sorted((u for u in data["units"] if unit_pct(u) > 0 and
                      _i((u.get("measures", {}) or {}).get("total_functions"))), key=unit_pct, reverse=True)
    for u in started[:25]:
        print(f"  {unit_pct(u):6.2f}%  {u.get('name')}")
    atmax = sum(1 for k, (mx, _) in base.items() if mx >= 99.995)
    overall = float((data.get("measures", {}) or {}).get("matched_code_percent", 0) or 0)
    print(f"[status] units: {len(data['units'])}  with-progress: {len(started)}  "
          f"overall: {overall:.2f}%  functions-at-max-100%: {atmax}")
    return 0
