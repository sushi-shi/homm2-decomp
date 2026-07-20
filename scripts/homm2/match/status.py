"""Run objdiff and report the live and best-observed base-to-target comparison.

  homm2 status                 print per-unit and overall live metrics
  homm2 status update          record maxima for the current normalized source hashes
  homm2 status --force-refresh regenerate report.json even when its inputs are unchanged
  homm2 status --write-readme  refresh the generated match block in README.md

Builds and explicit updates record the current source-hash epoch and raise its
per-function maximum when appropriate. The maxima are never gates.
"""
import hashlib, json, os, shutil, struct, subprocess, sys, tempfile
from pathlib import Path
from homm2.match.source_hashes import source_hashes
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
RM_START, RM_END = "<!-- match-score:start -->", "<!-- match-score:end -->"
REPORT_CACHE_SCHEMA = 1
REPORT_STAMP = "report.stamp.json"
MAXIMA = REPO / "config/match_baseline.tsv"
EXACT_MATCH_PERCENT = 100.0


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


def _fn_fuzzy(data):
    return {
        (unit.get("name", "?"), function.get("name", "?")):
            float(function.get("fuzzy_match_percent") or 0.0)
        for unit in data.get("units", [])
        for function in (unit.get("functions", []) or [])
    }


def load_maxima():
    """Load ``(maximum, source hash)`` for source-backed functions."""
    maxima = {}
    if not MAXIMA.exists():
        return maxima
    for line in MAXIMA.read_text().splitlines():
        if not line or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) >= 4 and fields[3]:
            key = (fields[0], fields[1])
            maximum = float(fields[2])
            if key not in maxima or maximum > maxima[key][0]:
                maxima[key] = (maximum, fields[3])
    return maxima


def _updated_maxima(data, maxima, hashes):
    out = {}
    for key, current in _fn_fuzzy(data).items():
        source_hash = hashes.get(key)
        if not source_hash:
            continue
        old_maximum, old_hash = maxima.get(key, (0.0, None))
        maximum = max(old_maximum, current) if old_hash == source_hash else current
        out[key] = (maximum, source_hash)
    return out


def write_maxima(maxima):
    lines = [
        "# homm2 retained match maxima by normalized function source hash.",
        "# Observational only; never an enforcement baseline.",
        "# Updated by `homm2 status update` and `homm2 build`; do not hand-edit.",
        "# unit<TAB>fn<TAB>max_fuzzy<TAB>src_hash",
    ]
    lines.extend("%s\t%s\t%.4f\t%s" % (unit, function, maximum, source_hash)
                 for (unit, function), (maximum, source_hash) in sorted(maxima.items()))
    _atomic_write(MAXIMA, ("\n".join(lines) + "\n").encode("utf-8"))


def record_maxima(data):
    previous = load_maxima()
    maxima = _updated_maxima(data, previous, source_hashes())
    write_maxima(maxima)
    return maxima


def current_maxima():
    """Return only stored maxima whose hashes describe the current source."""
    hashes = source_hashes()
    return {key: value for key, value in load_maxima().items()
            if hashes.get(key) == value[1]}


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


def readme_block(data, maxima):
    """Render live and current-source-hash-best per-tier comparison metrics."""
    tiers = {}
    for u in data.get("units", []):
        tier = u.get("name", "?").split("/")[0]
        m = u.get("measures", {}) or {}
        unit_name = u.get("name", "?")
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
            maximum = max(cur, maxima.get((unit_name, f.get("name", "?")), (cur, None))[0])
            t["fem"] += maximum >= EXACT_MATCH_PERCENT
            t["fz"] += sz * cur / 100.0; t["fzt"] += sz
            t["mx"] += sz * maximum / 100.0
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
           "_**Functions exact** = byte-identical now. **Functions exact-max** = observed at "
           "100% at least once for the current source hash. **Fuzzy** is the live size-weighted "
           "instruction match; **fuzzy-max** retains each function's best observed score for its "
           "current source hash. "
           "Maxima are historical navigation data, not correctness proof or enforcement._", "",
           *_md_table(["Module", "Units", "Functions exact", "Functions exact-max", "Fuzzy",
                       "Fuzzy-max", "Data exact", "Data bytes"], "lrrrrrrr", rows),
           "", RM_END]
    return "\n".join(out)


def main(argv=None, data=None):
    argv = list(argv or [])
    force_refresh = "--force-refresh" in argv
    argv = [arg for arg in argv if arg != "--force-refresh"]
    if argv not in ([], ["update"], ["--write-readme"]):
        print("usage: homm2 status [update] [--force-refresh] [--write-readme]",
              file=sys.stderr)
        return 1
    if data is None:
        data = load_report(force_refresh=force_refresh)
    if data is None:
        print("[status] no report (run 'homm2 build' first)"); return 1
    if argv == ["update"]:
        maxima = record_maxima(data)
        print("[status] retained maxima updated: %d source-backed functions" % len(maxima))
        return 0
    if "--write-readme" in argv:
        maxima = record_maxima(data)
        block = readme_block(data, maxima); rm = REPO / "README.md"
        text = rm.read_text() if rm.exists() else "# homm2-decomp\n\n" + RM_START + "\n" + RM_END + "\n"
        if RM_START in text and RM_END in text:
            pre = text[:text.index(RM_START)]; post = text[text.index(RM_END) + len(RM_END):]
            rm.write_text(pre + block + post)
        else:
            rm.write_text(text.rstrip() + "\n\n" + block + "\n")
        print("[status] refreshed README.md match block")
        return 0
    maxima = current_maxima()
    started = sorted((u for u in data["units"] if unit_pct(u) > 0 and
                      _i((u.get("measures", {}) or {}).get("total_functions"))), key=unit_pct, reverse=True)
    if started:
        print("[status] highest objdiff matched-code byte percentages by unit:")
    for u in started[:25]:
        print(f"  {unit_pct(u):6.2f}%  {u.get('name')}")
    measures = data.get("measures", {}) or {}
    matched_code_percent = float(measures.get("matched_code_percent", 0) or 0)
    fuzzy_match_percent = float(measures.get("fuzzy_match_percent", 0) or 0)
    matched_data = _i(measures.get("matched_data"))
    total_data = _i(measures.get("total_data"))
    data_percent = float(measures.get("matched_data_percent", 0) or 0)
    matched_functions = _i(measures.get("matched_functions"))
    total_functions = _i(measures.get("total_functions"))
    exact_max = sum(
        maxima.get((unit.get("name", "?"), function.get("name", "?")),
                   (float(function.get("fuzzy_match_percent") or 0.0), None))[0]
        >= EXACT_MATCH_PERCENT
        for unit in data.get("units", [])
        for function in (unit.get("functions", []) or []))
    fuzzy_max_numerator = sum(
        _i(function.get("size")) * maxima.get(
            (unit.get("name", "?"), function.get("name", "?")),
            (float(function.get("fuzzy_match_percent") or 0.0), None))[0]
        for unit in data.get("units", [])
        for function in (unit.get("functions", []) or []))
    fuzzy_max_denominator = sum(
        _i(function.get("size"))
        for unit in data.get("units", [])
        for function in (unit.get("functions", []) or []))
    fuzzy_max = (fuzzy_max_numerator / fuzzy_max_denominator
                 if fuzzy_max_denominator else 0.0)
    print(f"[status] units: {len(data['units'])}  with-progress: {len(started)}  "
          f"matched-code-bytes: {matched_code_percent:.2f}%  "
          f"fuzzy: {fuzzy_match_percent:.2f}%  "
          f"functions-exact: {matched_functions}/{total_functions}  "
          f"functions-exact-max: {exact_max}/{total_functions}  "
          f"fuzzy-max: {fuzzy_max:.2f}%  "
          f"data: {matched_data}/{total_data} ({data_percent:.3f}%)")
    return 0
