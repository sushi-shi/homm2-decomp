"""homm2 status - run objdiff over the base<->target objs and report match %.

  homm2 status                 print per-unit + overall match %
  homm2 status update          refresh the baseline (per-function max% keyed by source hash)
  homm2 status check           gate: fail only if an EDITED function lost ground vs its own max
  homm2 status --force-refresh regenerate report.json even when its inputs are unchanged
  homm2 status --write-readme  refresh the <!-- match-score --> block in README.md

Max-% model: each function's SOURCE block is hashed. The baseline stores, per source-backed
function, the best fuzzy% ever seen *for that source hash* (max%) plus the hash. When a sibling
changes and perturbs a function (tu-cumulative eval-order), its live fuzzy% (current) dips but its
source hash is unchanged, so max% is preserved — no need to chase it; a later pass recovers current.
When the function's OWN source changes, its hash changes and max% resets to the new current.
Compiler-generated functions without their own source block remain in live objdiff/exact counts,
but have no meaningful retained source-hash maximum and are not written to the baseline.
"""
import csv, hashlib, json, os, re, shutil, struct, subprocess, sys, tempfile
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
RM_START, RM_END = "<!-- match-score:start -->", "<!-- match-score:end -->"
RVA_BASE = 0x400000
EXACT_MATCH_PERCENT = 100.0
REPORT_CACHE_SCHEMA = 1
REPORT_STAMP = "report.stamp.json"


def _i(v): return int(v) if v not in (None, "") else 0


def _sha256(path):
    digest = hashlib.sha256()
    with Path(path).open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _report_inputs_identity(objdiff_dir, executable):
    """Content identity for every input consumed by `objdiff-cli report generate`."""
    objdiff_dir = Path(objdiff_dir)
    config_path = objdiff_dir / "objdiff.json"
    config = json.loads(config_path.read_text())
    digests = {}
    objects = []
    for unit in config.get("units", []):
        for role in ("base", "target"):
            reference = unit.get(role + "_path")
            if not reference:
                raise RuntimeError("objdiff unit %s has no %s_path" %
                                   (unit.get("name", "?"), role))
            path = (objdiff_dir / reference).resolve()
            if not path.is_file():
                raise RuntimeError("objdiff %s object is missing: %s" % (role, path))
            key = str(path)
            if key not in digests:
                digests[key] = _sha256(path)
            objects.append({
                "unit": unit.get("name", "?"),
                "role": role,
                "reference": reference,
                "sha256": digests[key],
            })

    executable = Path(executable).resolve(strict=True)
    if not executable.is_file():
        raise RuntimeError("objdiff-cli is not a file: %s" % executable)
    return {
        "objdiff_config_sha256": _sha256(config_path),
        "objects": objects,
        "objdiff_cli": {"path": str(executable), "sha256": _sha256(executable)},
    }


def _read_json(path):
    try:
        return json.loads(Path(path).read_text())
    except (FileNotFoundError, json.JSONDecodeError, OSError):
        return None


def _valid_report(data):
    return (isinstance(data, dict) and isinstance(data.get("units"), list) and
            isinstance(data.get("measures", {}), dict))


def _load_cached_report(report_path, stamp_path, inputs, force_refresh=False,
                        reviewed_targets_refreshed=False):
    if force_refresh or reviewed_targets_refreshed:
        return None
    report = _read_json(report_path)
    stamp = _read_json(stamp_path)
    if not _valid_report(report) or not isinstance(stamp, dict):
        return None
    if stamp.get("schema") != REPORT_CACHE_SCHEMA or stamp.get("inputs") != inputs:
        return None
    try:
        if stamp.get("report_sha256") != _sha256(report_path):
            return None
    except OSError:
        return None
    return report


def _atomic_write(path, data):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    handle, temporary = tempfile.mkstemp(prefix=".%s." % path.name, dir=path.parent)
    try:
        with os.fdopen(handle, "wb") as stream:
            stream.write(data)
        os.replace(temporary, path)
    except BaseException:
        try:
            os.unlink(temporary)
        except FileNotFoundError:
            pass
        raise


def _store_report_stamp(report_path, stamp_path, inputs, reviewed_targets_refreshed):
    stamp = {
        "schema": REPORT_CACHE_SCHEMA,
        "inputs": inputs,
        "report_sha256": _sha256(report_path),
        "reviewed_targets_refreshed_before_generation": bool(reviewed_targets_refreshed),
    }
    _atomic_write(stamp_path, (json.dumps(stamp, indent=2) + "\n").encode("utf-8"))


def _generate_report(objdiff_dir, report_path, executable):
    handle, temporary = tempfile.mkstemp(prefix=".report.", suffix=".json",
                                         dir=objdiff_dir)
    os.close(handle)
    os.unlink(temporary)
    try:
        subprocess.run([str(executable), "report", "generate", "-p", str(objdiff_dir),
                        "-o", temporary], cwd=REPO, check=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        report = _read_json(temporary)
        if not _valid_report(report):
            raise RuntimeError("objdiff-cli generated an invalid report")
        os.replace(temporary, report_path)
        return report
    finally:
        try:
            os.unlink(temporary)
        except FileNotFoundError:
            pass


def _trusted_incremental_base_units(report_path, stamp_path, inputs):
    """Return the prior report and base-only changed units, or fail closed."""
    report = _read_json(report_path)
    stamp = _read_json(stamp_path)
    if not _valid_report(report) or not isinstance(stamp, dict):
        return None, None
    if stamp.get("schema") != REPORT_CACHE_SCHEMA:
        return None, None
    try:
        if stamp.get("report_sha256") != _sha256(report_path):
            return None, None
    except OSError:
        return None, None

    previous = stamp.get("inputs")
    if not isinstance(previous, dict):
        return None, None
    if (previous.get("objdiff_config_sha256") != inputs.get("objdiff_config_sha256") or
            previous.get("objdiff_cli") != inputs.get("objdiff_cli")):
        return None, None

    old_objects = previous.get("objects")
    new_objects = inputs.get("objects")
    if not isinstance(old_objects, list) or not isinstance(new_objects, list):
        return None, None
    if len(old_objects) != len(new_objects):
        return None, None

    changed = []
    for old, new in zip(old_objects, new_objects):
        identity = ("unit", "role", "reference")
        if any(old.get(key) != new.get(key) for key in identity):
            return None, None
        if old.get("sha256") == new.get("sha256"):
            continue
        if new.get("role") != "base":
            return None, None
        changed.append(new.get("unit"))
    if not changed:
        return None, None
    return report, sorted(set(changed))


def _generate_partial_report(objdiff_dir, executable, units):
    objdiff_dir = Path(objdiff_dir).resolve()
    config = json.loads((objdiff_dir / "objdiff.json").read_text())
    selected = set(units)
    partial_units = []
    for unit in config.get("units", []):
        if unit.get("name") not in selected:
            continue
        unit = dict(unit)
        for role in ("base", "target"):
            key = role + "_path"
            unit[key] = str((objdiff_dir / unit[key]).resolve())
        partial_units.append(unit)
    if {unit.get("name") for unit in partial_units} != selected:
        raise RuntimeError("incremental objdiff units are absent from objdiff.json")
    config["units"] = partial_units

    with tempfile.TemporaryDirectory(prefix=".partial-report.", dir=objdiff_dir) as directory:
        project = Path(directory)
        _atomic_write(project / "objdiff.json",
                      (json.dumps(config, indent=2) + "\n").encode("utf-8"))
        return _generate_report(project, project / "report.json", executable)


def _float32(value):
    return struct.unpack("f", struct.pack("f", float(value)))[0]


def _aggregate_measures(units):
    def integer(measures, key):
        return int(measures.get(key, 0) or 0)

    total_code = sum(integer(unit.get("measures", {}), "total_code") for unit in units)
    matched_code = sum(integer(unit.get("measures", {}), "matched_code") for unit in units)
    total_data = sum(integer(unit.get("measures", {}), "total_data") for unit in units)
    matched_data = sum(integer(unit.get("measures", {}), "matched_data") for unit in units)
    total_functions = sum(integer(unit.get("measures", {}), "total_functions")
                          for unit in units)
    matched_functions = sum(integer(unit.get("measures", {}), "matched_functions")
                            for unit in units)
    fuzzy_numerator = sum(
        float(unit.get("measures", {}).get("fuzzy_match_percent", 0) or 0) *
        integer(unit.get("measures", {}), "total_code")
        for unit in units)

    def percent(matched, total, empty=0.0):
        return _float32(100.0 * matched / total) if total else empty

    return {
        "fuzzy_match_percent": _float32(fuzzy_numerator / total_code) if total_code else 0.0,
        "total_code": str(total_code),
        "matched_code": str(matched_code),
        "matched_code_percent": percent(matched_code, total_code),
        "total_data": str(total_data),
        "matched_data": str(matched_data),
        "matched_data_percent": percent(matched_data, total_data, 100.0),
        "total_functions": total_functions,
        "matched_functions": matched_functions,
        "matched_functions_percent": percent(matched_functions, total_functions),
        "total_units": len(units),
    }


def _merge_partial_report(previous, partial, unit_order, changed_units):
    changed = set(changed_units)
    replacements = {unit.get("name"): unit for unit in partial.get("units", [])}
    if set(replacements) != changed:
        raise RuntimeError("incremental objdiff report returned unexpected units")
    by_name = {unit.get("name"): unit for unit in previous.get("units", [])}
    by_name.update(replacements)
    if set(by_name) != set(unit_order):
        raise RuntimeError("incremental objdiff report does not cover objdiff.json")
    merged_units = [by_name[name] for name in unit_order]
    return {
        "version": partial.get("version", previous.get("version")),
        "units": merged_units,
        "measures": _aggregate_measures(merged_units),
    }


def load_report(force_refresh=False):
    from homm2.build.reviewed_data import ensure_reviewed_targets
    reviewed_targets_refreshed = ensure_reviewed_targets()
    od = REPO / "build/objdiff"
    rep = od / "report.json"
    stamp = od / REPORT_STAMP
    executable_name = shutil.which("objdiff-cli")
    if not executable_name:
        raise RuntimeError("objdiff-cli is required to generate the match report")
    executable = Path(executable_name).resolve(strict=True)
    inputs = _report_inputs_identity(od, executable)
    cached = _load_cached_report(rep, stamp, inputs, force_refresh,
                                 reviewed_targets_refreshed)
    if cached is not None:
        return cached

    report = None
    if not force_refresh and not reviewed_targets_refreshed:
        previous, changed_units = _trusted_incremental_base_units(rep, stamp, inputs)
        if previous is not None:
            partial = _generate_partial_report(od, executable, changed_units)
            config = json.loads((od / "objdiff.json").read_text())
            unit_order = [unit.get("name") for unit in config.get("units", [])]
            report = _merge_partial_report(previous, partial, unit_order, changed_units)
            _atomic_write(rep, json.dumps(report, separators=(",", ":")).encode("utf-8"))
    if report is None:
        report = _generate_report(od, rep, executable)
    final_inputs = _report_inputs_identity(od, executable)
    if final_inputs != inputs:
        raise RuntimeError("objdiff report inputs changed during generation")
    _store_report_stamp(rep, stamp, final_inputs, reviewed_targets_refreshed)
    return report


def unit_pct(u):
    return float((u.get("measures", {}) or {}).get("matched_code_percent", 0) or 0)


# ---------------------------------------------------------------- source hashes ---
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
    comma in its column-zero ``VA(...)`` marker and ends at the next top-level
    VA/DATA/VTBL/section/#endif marker. Unlike the old regular-expression
    split, markers inside a function body are not boundaries. This matters for
    function-local ``DATA(...)`` definitions and remains robust in the presence
    of nested blocks or marker-like text in comments and literals.
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
        block_end = markers[position + 1][0] if position + 1 < len(markers) else length
        yield absolute_va, text[block_start:block_end]


def source_hashes():
    """{(unit, function): 12-hex sha1 of that function's NORMALIZED source block}. A block runs from
    its VA(...) marker to the next top-level VA/DATA/VTBL/section marker; names are normalized
    (see _normalize) so editing one function changes only its own hash, and codegen-neutral
    arg/member renames don't. Function-local audit markers remain part of the function block."""
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
        t = tiers.setdefault(tier, {"units": 0, "fe": 0, "fem": 0, "ft": 0,
                                    "fz": 0.0, "fzt": 0, "mx": 0.0,
                                    "dm": 0, "dt": 0, "de": 0, "du": 0})
        t["units"] += 1
        t["fe"] += _i(m.get("matched_functions")); t["ft"] += _i(m.get("total_functions"))
        matched_data = _i(m.get("matched_data")); total_data = _i(m.get("total_data"))
        t["dm"] += matched_data; t["dt"] += total_data
        if total_data:
            t["du"] += 1
            t["de"] += matched_data == total_data
        for f in u.get("functions", []) or []:
            sz = _i(f.get("size")); cur = f.get("fuzzy_match_percent") or 0.0
            mx = base.get((un, f.get("name", "?")), (cur, None))[0]
            mx = max(mx, cur)  # live current can only raise the displayed max
            if mx >= EXACT_MATCH_PERCENT:
                t["fem"] += 1
            t["fz"] += sz * cur / 100.0; t["fzt"] += sz; t["mx"] += sz * mx / 100.0
    rows = []
    TE = TEM = TT = DM = DT = DE = DU = 0; FZ = MX = 0.0; FZT = 0
    for tier in sorted(tiers, key=lambda k: -tiers[k]["ft"]):
        d = tiers[tier]
        if d["ft"] == 0:
            continue
        TE += d["fe"]; TEM += d["fem"]; TT += d["ft"]
        DM += d["dm"]; DT += d["dt"]; DE += d["de"]; DU += d["du"]
        FZ += d["fz"]; FZT += d["fzt"]; MX += d["mx"]
        fp = 100 * d["fe"] / d["ft"]
        fmp = 100 * d["fem"] / d["ft"]
        zp = 100 * d["fz"] / d["fzt"] if d["fzt"] else 0
        mp = 100 * d["mx"] / d["fzt"] if d["fzt"] else 0
        dp = 100 * d["dm"] / d["dt"] if d["dt"] else 0
        rows.append([f"`{tier}`", str(d["units"]),
                     f"{d['fe']} / {d['ft']} ({fp:.1f}%)",
                     f"{d['fem']} / {d['ft']} ({fmp:.1f}%)",
                     f"{zp:.1f}%", f"{mp:.1f}%",
                     f"{d['de']} / {d['du']}",
                     f"{d['dm']:,} / {d['dt']:,} ({dp:.2f}%)"])
    overall_f = 100 * TE / TT if TT else 0
    overall_fm = 100 * TEM / TT if TT else 0
    overall_z = 100 * FZ / FZT if FZT else 0
    overall_m = 100 * MX / FZT if FZT else 0
    overall_d = 100 * DM / DT if DT else 0
    out = [RM_START, "## Match status", "",
           "_Auto-generated by `homm2 status --write-readme` (refreshed by `homm2 build`); do not hand-edit._", "",
           f"**Overall: {TE} / {TT} functions exact ({overall_f:.2f}%) &middot; "
           f"{TEM} / {TT} functions exact-max ({overall_fm:.2f}%) &middot; "
           f"{overall_z:.2f}% fuzzy &middot; {overall_m:.2f}% fuzzy-max &middot; "
           f"{DM:,} / {DT:,} data bytes ({overall_d:.3f}%) &middot; "
           f"{DE} / {DU} data-bearing units exact.**", "",
           "_**Functions exact** = byte-identical now. **Functions exact-max** = byte-identical "
           "at least once for the current source hash. **Fuzzy** = live size-weighted instruction match. "
           "**Fuzzy-max** = best fuzzy ever reached per function for its current source (tu-cumulative "
           "dips don't lower it); **fuzzy-max 100% ⇒ essentially done.**_", "",
           *_md_table(["Module", "Units", "Functions exact", "Functions exact-max", "Fuzzy",
                       "Fuzzy-max", "Data exact", "Data bytes"], "lrrrrrrr", rows),
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
            if len(p) >= 4 and p[3]:
                base[(p[0], p[1])] = (float(p[2]), p[3])
            elif len(p) == 3:
                base[(p[0], p[1])] = (float(p[2]), None)
    return base


def write_baseline(base):
    lines = ["# homm2 match baseline - per-function max fuzzy% keyed by source hash.",
             "# Generated by `homm2 status update`; do not hand-edit.  unit<TAB>fn<TAB>max_fuzzy<TAB>src_hash"]
    for (un, fn), (mx, h) in sorted(base.items()):
        if h:
            lines.append(f"{un}\t{fn}\t{mx:.4f}\t{h}")
    BASELINE.write_text("\n".join(lines) + "\n")


def _updated_baseline(cur, base, sh, accept_regressions=False):
    """Merge live scores into the retained ledger, omitting functions without a source hash."""
    out = {}
    for k, c in cur.items():
        h = sh.get(k)
        if not h:
            continue
        old_mx, old_h = base.get(k, (0.0, None))
        # reset only when a KNOWN hash actually changed (the function's own source was edited);
        # on first-time migration (old_h None) or an unchanged hash, keep the accumulated max.
        if old_h and h != old_h:
            mx = max(old_mx, c) if accept_regressions else c
        else:
            mx = max(old_mx, c)
        out[k] = (mx, h)
    return out


def cmd_update(data, accept_regressions=False):
    """max% keyed by source hash: same hash -> max(old, current); changed hash -> reset to current.
    With --accept-regressions, intentional shared-layout edits adopt the new hash without discarding
    the retained maximum. No blessing is needed for tu-cumulative dips (same hash keeps its max)."""
    cur = _fn_fuzzy(data); base = load_baseline(); sh = source_hashes()
    out = _updated_baseline(cur, base, sh, accept_regressions)
    write_baseline(out)
    at100 = sum(1 for (mx, _) in out.values() if mx >= EXACT_MATCH_PERCENT)
    print(f"[status] baseline updated: {len(out)} source-backed functions -> "
          f"{BASELINE.relative_to(REPO)} "
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


def main(argv=None, data=None):
    argv = list(argv or [])
    force_refresh = "--force-refresh" in argv
    argv = [arg for arg in argv if arg != "--force-refresh"]
    if data is None:
        data = load_report(force_refresh=force_refresh)
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
    atmax = sum(1 for k, (mx, _) in base.items() if mx >= EXACT_MATCH_PERCENT)
    overall = float((data.get("measures", {}) or {}).get("matched_code_percent", 0) or 0)
    measures = data.get("measures", {}) or {}
    matched_data = _i(measures.get("matched_data"))
    total_data = _i(measures.get("total_data"))
    data_percent = float(measures.get("matched_data_percent", 0) or 0)
    print(f"[status] units: {len(data['units'])}  with-progress: {len(started)}  "
          f"overall: {overall:.2f}%  functions-at-max-100%: {atmax}  "
          f"data: {matched_data}/{total_data} ({data_percent:.3f}%)")
    return 0
