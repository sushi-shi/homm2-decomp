"""Run objdiff and report the live base-to-target comparison.

  homm2 status                 print per-unit and overall live metrics
  homm2 status --force-refresh regenerate report.json even when its inputs are unchanged
  homm2 status --write-readme  refresh the generated match block in README.md
"""
import hashlib, json, os, shutil, struct, subprocess, sys, tempfile
from pathlib import Path
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
RM_START, RM_END = "<!-- match-score:start -->", "<!-- match-score:end -->"
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


def readme_block(data):
    """Render the live per-tier comparison table."""
    tiers = {}
    for u in data.get("units", []):
        tier = u.get("name", "?").split("/")[0]
        m = u.get("measures", {}) or {}
        t = tiers.setdefault(tier, {"units": 0, "fe": 0, "ft": 0,
                                    "fz": 0.0, "fzt": 0,
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
            t["fz"] += sz * cur / 100.0; t["fzt"] += sz
    rows = []
    TE = TT = DM = DT = DE = DU = 0; FZ = 0.0; FZT = 0
    for tier in sorted(tiers, key=lambda k: -tiers[k]["ft"]):
        d = tiers[tier]
        if d["ft"] == 0:
            continue
        TE += d["fe"]; TT += d["ft"]
        DM += d["dm"]; DT += d["dt"]; DE += d["de"]; DU += d["du"]
        FZ += d["fz"]; FZT += d["fzt"]
        fp = 100 * d["fe"] / d["ft"]
        zp = 100 * d["fz"] / d["fzt"] if d["fzt"] else 0
        dp = 100 * d["dm"] / d["dt"] if d["dt"] else 0
        rows.append([f"`{tier}`", str(d["units"]),
                     f"{d['fe']} / {d['ft']} ({fp:.1f}%)",
                     f"{zp:.1f}%",
                     f"{d['de']} / {d['du']}",
                     f"{d['dm']:,} / {d['dt']:,} ({dp:.2f}%)"])
    overall_f = 100 * TE / TT if TT else 0
    overall_z = 100 * FZ / FZT if FZT else 0
    overall_d = 100 * DM / DT if DT else 0
    out = [RM_START, "## Match status", "",
           "_Auto-generated by `homm2 status --write-readme` (refreshed by `homm2 build`); do not hand-edit._", "",
           f"**Overall: {TE} / {TT} functions exact ({overall_f:.2f}%) &middot; "
           f"{overall_z:.2f}% fuzzy &middot; "
           f"{DM:,} / {DT:,} data bytes ({overall_d:.3f}%) &middot; "
           f"{DE} / {DU} data-bearing units exact.**", "",
           "_**Functions exact** = byte-identical now. **Fuzzy** = the live size-weighted "
           "instruction match; neither metric replaces raw-byte and relocation review._", "",
           *_md_table(["Module", "Units", "Functions exact", "Fuzzy", "Data exact", "Data bytes"],
                      "lrrrrr", rows),
           "", RM_END]
    return "\n".join(out)


def main(argv=None, data=None):
    argv = list(argv or [])
    force_refresh = "--force-refresh" in argv
    argv = [arg for arg in argv if arg != "--force-refresh"]
    if data is None:
        data = load_report(force_refresh=force_refresh)
    if data is None:
        print("[status] no report (run 'homm2 build' first)"); return 1
    if "--write-readme" in argv:
        block = readme_block(data); rm = REPO / "README.md"
        text = rm.read_text() if rm.exists() else "# homm2-decomp\n\n" + RM_START + "\n" + RM_END + "\n"
        if RM_START in text and RM_END in text:
            pre = text[:text.index(RM_START)]; post = text[text.index(RM_END) + len(RM_END):]
            rm.write_text(pre + block + post)
        else:
            rm.write_text(text.rstrip() + "\n\n" + block + "\n")
        print("[status] refreshed README.md match block")
        return 0
    if argv:
        print("usage: homm2 status [--force-refresh] [--write-readme]", file=sys.stderr)
        return 1
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
    print(f"[status] units: {len(data['units'])}  with-progress: {len(started)}  "
          f"matched-code-bytes: {matched_code_percent:.2f}%  "
          f"fuzzy: {fuzzy_match_percent:.2f}%  "
          f"functions-exact: {matched_functions}/{total_functions}  "
          f"data: {matched_data}/{total_data} ({data_percent:.3f}%)")
    return 0
