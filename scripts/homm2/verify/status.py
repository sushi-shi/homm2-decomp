"""Run objdiff and report the live and best-observed base-to-target comparison.

  homm2 status                 print per-unit and overall live metrics
  homm2 status update          record maxima for the current normalized source hashes
  homm2 status --force-refresh regenerate report.json even when its inputs are unchanged
  homm2 status --write-readme  refresh the generated match block in README.md
                               (`homm2 verify readme`)
  homm2 status check           as plain status; exit 1 unless every function and
                               every data byte is exact (`homm2 verify check`)

Builds and explicit updates record the current source-hash epoch and raise its
per-function maximum when appropriate. The maxima are never gates.
"""
import hashlib, json, os, shutil, struct, subprocess, sys, tempfile
from pathlib import Path
from homm2.compare.normalized_freshness import freshness_problems
from homm2.verify.fingerprints import source_hashes
from homm2.core.paths import DEFAULT_IMAGE, delink_dir, gen_dir, image_build, image_key, images, objdiff_dir, retail_dir, retail_exe
REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
RM_START, RM_END = "<!-- match-score:start -->", "<!-- match-score:end -->"
REPORT_CACHE_SCHEMA = 1
REPORT_STAMP = "report.stamp.json"
# The game's ledger keeps its historical name; another image's is
# config/match_baseline.<image>.tsv.
MAXIMA = (REPO / "config/match_baseline.tsv" if image_key() == DEFAULT_IMAGE
          else REPO / f"config/match_baseline.{image_key()}.tsv")
MAXIMA_POLICY = "function-relocs-data-value-v1"
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
    stale = []
    for unit in config.get("units", []):
        unit_paths = {}
        for role in ("base", "target"):
            reference = unit.get(role + "_path")
            if not reference:
                raise RuntimeError("objdiff unit %s has no %s_path" %
                                   (unit.get("name", "?"), role))
            path = (objdiff_dir / reference).resolve()
            if not path.is_file():
                raise RuntimeError("objdiff %s object is missing: %s" % (role, path))
            unit_paths[role] = path
            key = str(path)
            if key not in digests:
                digests[key] = _sha256(path)
                # Raw candidate and delinker objects are provenance roots;
                # only derived comparison copies must carry a verified chain.
                if {"normalized", "paired"} & set(path.parts):
                    stale.extend(freshness_problems(path))
            objects.append({
                "unit": unit.get("name", "?"),
                "role": role,
                "reference": reference,
                "sha256": digests[key],
            })
        if unit_paths["base"].samefile(unit_paths["target"]):
            raise RuntimeError("objdiff unit %s compares an object to itself: %s" %
                               (unit.get("name", "?"), unit_paths["base"]))
    if stale:
        preview = "\n  ".join(stale[:10])
        if len(stale) > 10:
            preview += "\n  ... and %d more" % (len(stale) - 10)
        raise RuntimeError(
            "stale or unverifiable normalized comparison objects:\n  %s" % preview)

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
    from homm2.delink.reviewed_data import ensure_reviewed_targets
    reviewed_targets_refreshed = ensure_reviewed_targets()
    od = objdiff_dir()
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
    """Load ``(maximum, effective-source hash)`` for source-backed functions."""
    maxima = {}
    if not MAXIMA.exists():
        return maxima
    lines = MAXIMA.read_text().splitlines()
    policy = next(
        (line.split(":", 1)[1].strip()
         for line in lines if line.startswith("# scoring-policy:")),
        None,
    )
    if policy != MAXIMA_POLICY:
        return maxima
    for line in lines:
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
        same_source = old_hash == source_hash
        # One-time migration from the historical body-only hash to the
        # body.dependency composite. The body prefix proves that the annotated
        # function itself is unchanged; later dependency changes alter the
        # already-composite hash and reset the maximum normally.
        dependency_hash_upgrade = (
            old_hash is not None
            and "." not in old_hash
            and source_hash.startswith(old_hash + ".")
        )
        maximum = (
            max(old_maximum, current)
            if same_source or dependency_hash_upgrade else current)
        out[key] = (maximum, source_hash)
    return out


def write_maxima(maxima):
    lines = [
        "# homm2 retained match maxima by normalized effective-source hash.",
        f"# scoring-policy: {MAXIMA_POLICY}",
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


# Identification modules: functions carved out of the reconstruction-target
# universe because they are generated or library code, not independent
# matching targets. "(unmatched)" is NOT here - unclaimed reconstruction
# targets stay in the denominator.
CARVE_OUTS = {
    "(libcmt)": "FID-identified static runtime (config/retail/functions_static_libs.csv)",
    "(imports)": "import thunks (config/retail/functions_imports.csv)",
    "(funclets)": "compiler /GX EH; match with their parent function",
    "(compgen)": "compiler-generated bodies awaiting an owner unit",
}


def split_carve_outs(data):
    """(target_units, carved_units) - identification modules leave the score."""
    targets, carved = [], []
    for unit in data.get("units", []):
        (carved if unit.get("name") in CARVE_OUTS else targets).append(unit)
    return targets, carved


def readme_block(data, maxima, image_section=False):
    """The README score block: every score at MAX (the best observed for each
    function's current effective-source hash), plus one CUR / MAX line.

    The game's block carries the markers and appends a section per other
    image with a report (rendered by that image's own process). Data bytes
    are checked by `homm2 build verify` and reported by `homm2 verify status`;
    the README shows functions only."""
    target_units, _carved_units = split_carve_outs(data)
    tiers = {}
    cur_exact = 0
    cur_weight = max_weight = 0.0
    for u in target_units:
        unit_name = u.get("name", "?")
        t = tiers.setdefault(unit_name.split("/")[0],
                             {"units": 0, "exact": 0, "functions": 0, "size": 0, "weight": 0.0})
        t["units"] += 1
        for f in u.get("functions", []) or []:
            size = _i(f.get("size")); cur = f.get("fuzzy_match_percent") or 0.0
            maximum = max(cur, maxima.get((unit_name, f.get("name", "?")), (cur, None))[0])
            t["functions"] += 1
            t["exact"] += maximum >= EXACT_MATCH_PERCENT
            t["size"] += size
            t["weight"] += size * maximum
            cur_exact += cur >= EXACT_MATCH_PERCENT
            cur_weight += size * cur
            max_weight += size * maximum
    tiers = {name: t for name, t in tiers.items() if t["functions"]}
    rows = []
    for tier in sorted(tiers, key=lambda k: -tiers[k]["functions"]):
        t = tiers[tier]
        rows.append([f"`{tier}`", f"{t['units']}",
                     f"{t['exact']:,} / {t['functions']:,} "
                     f"({_pct(t['exact'], t['functions']):.1f}%)",
                     f"{t['weight'] / t['size'] if t['size'] else 0.0:.1f}%"])
    functions = sum(t["functions"] for t in tiers.values())
    exact = sum(t["exact"] for t in tiers.values())
    size = sum(t["size"] for t in tiers.values())
    fuzzy_cur = cur_weight / size if size else 0.0
    fuzzy_max = max_weight / size if size else 0.0
    headline = (f"**{exact:,} / {functions:,} functions exact "
                f"({_pct(exact, functions):.2f}%) &middot; {fuzzy_max:.2f}% fuzzy.**")
    table = _md_table(["Module", "Units", "Functions exact", "Fuzzy"], "lrrr", rows)
    exe = retail_exe().name
    if image_section:
        return "\n".join([f"### {exe}", "", headline + " A separate image with its own "
                          "link graph and scores; shared units compile once per image.",
                          "", *table])
    label = f" ({exe})" if len(images()) > 1 else ""
    out = [RM_START, "## Match status", "",
           "_Auto-generated by `homm2 verify readme`; do not hand-edit. Scores are "
           "MAX (the best of each function's current source)._", "",
           headline + label, "",
           "_Comparison mode: strict relocations and data values._", "",
           *table, "",
           f"_CUR / MAX: {cur_exact:,} / {exact:,} exact &middot; "
           f"{fuzzy_cur:.2f}% / {fuzzy_max:.2f}% fuzzy (defined in "
           "[docs/match-status.md](docs/match-status.md)). Totals cover every in-`.text` "
           "reconstruction target; generated and library code is excluded._"]
    out += image_sections()
    out += [RM_END]
    return "\n".join(out)


def _pct(num, den):
    return 100.0 * num / den if den else 0.0


def image_sections() -> list[str]:
    """README sections of every other image with a report, each rendered by a
    child process that selects that image (homm2.core.paths)."""
    import os
    from homm2.core.paths import IMAGE_ENV
    out: list[str] = []
    for image in images():
        if image == DEFAULT_IMAGE or not (image_build(image) / "objdiff/report.json").is_file():
            continue
        env = dict(os.environ, **{IMAGE_ENV: image})
        result = subprocess.run([sys.executable, "-m", "homm2.verify.status", "--image-section"],
                                env=env, capture_output=True, text=True, cwd=REPO)
        if result.returncode != 0:
            # A stale or missing report (the image is rebuilt after the game)
            # keeps the section the README already has.
            previous = _readme_section(image)
            if previous:
                out += ["", *previous]
            continue
        out += ["", *result.stdout.rstrip("\n").splitlines()]
    return out


def _readme_section(image: str) -> list[str]:
    """The README's current section for `image`, or []."""
    readme = REPO / "README.md"
    if not readme.is_file():
        return []
    lines = readme.read_text().splitlines()
    title = f"### {retail_exe(image).name}"
    if title not in lines:
        return []
    start = lines.index(title)
    end = start + 1
    while end < len(lines) and not lines[end].startswith("### ") and lines[end] != RM_END:
        end += 1
    while end > start and not lines[end - 1].strip():
        end -= 1
    return lines[start:end]


from homm2.core.usage import logged


@logged
def main(argv=None, data=None):
    argv = list(argv or [])
    force_refresh = "--force-refresh" in argv
    argv = [arg for arg in argv if arg != "--force-refresh"]
    check = argv == ["check"]
    if check:
        argv = []
    if argv == ["--image-section"]:
        data = data if data is not None else load_report(force_refresh=force_refresh)
        if data is None:
            return 1
        print(readme_block(data, current_maxima(), image_section=True))
        return 0
    if argv not in ([], ["update"], ["--write-readme"]):
        print("usage: homm2 status [update|check] [--force-refresh] [--write-readme]",
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
        if image_key() != DEFAULT_IMAGE:
            record_maxima(data)
            # The README block is the game's; it renders this image's section.
            from homm2.core.paths import IMAGE_ENV
            env = dict(os.environ, **{IMAGE_ENV: DEFAULT_IMAGE})
            return subprocess.run([sys.executable, "-m", "homm2.verify.status",
                                   "--write-readme"], env=env, cwd=REPO).returncode
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
    target_units, carved_units = split_carve_outs(data)
    started = sorted((u for u in target_units if unit_pct(u) > 0 and
                      _i((u.get("measures", {}) or {}).get("total_functions"))), key=unit_pct, reverse=True)
    if started:
        print("[status] highest objdiff matched-code byte percentages by unit:")
    for u in started[:25]:
        print(f"  {unit_pct(u):6.2f}%  {u.get('name')}")

    def _sum(units, key):
        return sum(_i((u.get("measures", {}) or {}).get(key)) for u in units)

    matched_code = _sum(target_units, "matched_code")
    total_code = _sum(target_units, "total_code")
    matched_code_percent = 100 * matched_code / total_code if total_code else 0
    fuzzy_match_percent = (sum(
        float((u.get("measures", {}) or {}).get("fuzzy_match_percent") or 0)
        * _i((u.get("measures", {}) or {}).get("total_code"))
        for u in target_units) / total_code if total_code else 0)
    matched_data = _sum(target_units, "matched_data")
    total_data = _sum(target_units, "total_data")
    data_percent = 100 * matched_data / total_data if total_data else 100.0
    matched_functions = _sum(target_units, "matched_functions")
    total_functions = _sum(target_units, "total_functions")
    exact_max = sum(
        maxima.get((unit.get("name", "?"), function.get("name", "?")),
                   (float(function.get("fuzzy_match_percent") or 0.0), None))[0]
        >= EXACT_MATCH_PERCENT
        for unit in target_units
        for function in (unit.get("functions", []) or []))
    fuzzy_max_numerator = sum(
        _i(function.get("size")) * maxima.get(
            (unit.get("name", "?"), function.get("name", "?")),
            (float(function.get("fuzzy_match_percent") or 0.0), None))[0]
        for unit in target_units
        for function in (unit.get("functions", []) or []))
    fuzzy_max_denominator = sum(
        _i(function.get("size"))
        for unit in target_units
        for function in (unit.get("functions", []) or []))
    fuzzy_max = (fuzzy_max_numerator / fuzzy_max_denominator
                 if fuzzy_max_denominator else 0.0)
    carved_functions = _sum(carved_units, "total_functions")
    print(f"[status] units: {len(target_units)}  with-progress: {len(started)}  "
          f"matched-code-bytes: {matched_code_percent:.2f}%  "
          f"fuzzy: {fuzzy_match_percent:.2f}%  "
          f"functions-exact: {matched_functions}/{total_functions}  "
          f"functions-exact-max: {exact_max}/{total_functions}  "
          f"fuzzy-max: {fuzzy_max:.2f}%  "
          f"data: {matched_data}/{total_data} ({data_percent:.3f}%)")
    print("[status] normalized compiled-object scores only; unbuilt source edits are not "
          "measured. Raw-object gates and whole-image equality: `homm2 build` "
          "(`link-diff`).")
    if carved_units:
        print(f"[status] identified carve-outs: {carved_functions} functions in "
              f"{len(carved_units)} modules "
              f"({', '.join(sorted(u.get('name', '?') for u in carved_units))})")
    if check and image_key() != DEFAULT_IMAGE:
        # An image still under reconstruction is held to its own record: no
        # function may fall below the maximum banked for its current source.
        live = _fn_fuzzy(data)
        regressed = sorted(key for key, (maximum, _hash) in maxima.items()
                           if round(live.get(key, 0.0), 4) + 1e-4 < maximum)
        for unit, name in regressed[:20]:
            print(f"[status] REGRESSION {unit} {name}: {live.get((unit, name), 0.0):.2f}% "
                  f"< banked {maxima[(unit, name)][0]:.2f}%")
        if regressed:
            print(f"[status] FAIL: {len(regressed)} function(s) below their banked maximum")
            return 1
        return 0
    if check and (matched_functions < total_functions or matched_data < total_data):
        print(f"[status] FAIL: {total_functions - matched_functions} function(s) and "
              f"{total_data - matched_data} data byte(s) are not exact")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
