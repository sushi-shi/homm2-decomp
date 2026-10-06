#!/usr/bin/env python3
"""Emission-order probe campaigns over small synthetic or real translation units.

`batch_source_variants` scores one function's bytes. Layout walls instead depend
on the order in which VC6 emits functions into an object, on which COMDAT copy
LINK selects, and on whether one function inlines another. This runner builds a
Cartesian product of reviewed source/flag choices, compiles every variant with
the pinned compiler in parallel, and tabulates for each object the order of the
functions named in the manifest, their sizes, and their REL32 callees. With a
`link` block it also links each variant and reports the linked order and the
owner object of each tracked function from the native MAP.

Manifest (JSON, schema 1)::

    {
      "schema": 1,
      "files": {"a.cpp": "... @@dtor@@ ...", "b.cpp": "..."},
      "units": ["a.cpp", "b.cpp"],
      "flags": ["/nologo", "/c", "/Od", "/Gy"],
      "defaults": {"dtor": ""},
      "axes": [
        {"name": "dtor", "options": [
          {"name": "inclass", "slots": {"dtor": "~Node() {}"}},
          {"name": "ob2", "flags_add": ["/Ob2"], "flags_remove": ["/Ob1"]},
          {"name": "one_tu", "units": ["a.cpp"]}
        ]}
      ],
      "aliases": [["^\\\\?\\\\?1Node", "N"], ["Play", "P"]],
      "track": ["P"],
      "link": {"libs": ["LIBCMT.LIB"], "flags": ["/SUBSYSTEM:CONSOLE"]},
      "expect": {"a.cpp": "S A G R N", "link": "S A G R N"}
    }

`{work}` in a flag is replaced by the variant directory (for example
`/I{work}` lets a variant header shadow the repository copy).
`@@slot@@` markers in `files` take the selected option's text (or the default).
`files` may instead map a name to `{"path": "src/BASE/X.cpp"}` to read a real
source, with slots placed by exact `find` strings in the option
(`"edits": [{"file": ..., "find": ..., "replace": ...}]`).

Run inside ``nix develop .#build``::

    python3 -m homm2.permute.emission_order manifest.json --output build/probe/x

Compiles run at most ``nproc // 2`` at a time by default. Outputs are
`results.json` and `results.tsv` in the output directory; every variant keeps
its sources and objects there. Real sources are never modified in place.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import itertools
import json
import os
import re
import shutil
import sys
from pathlib import Path

from homm2.core.coff import REL32, CoffObject
from homm2.core.paths import REPO

CODE = 0x00000020
SLOT = re.compile(r"@@([A-Za-z_][A-Za-z0-9_]*)@@")


def load_manifest(path: Path) -> dict:
    manifest = json.loads(path.read_text())
    if manifest.get("schema") != 1:
        raise ValueError("emission-order manifest schema must be 1")
    files = {}
    for name, value in manifest["files"].items():
        if isinstance(value, dict):
            files[name] = (REPO / value["path"]).read_text(encoding="latin-1")
        else:
            files[name] = value
    manifest["files"] = files
    manifest.setdefault("defaults", {})
    manifest.setdefault("axes", [])
    manifest.setdefault("aliases", [])
    manifest.setdefault("track", [])
    manifest.setdefault("expect", {})
    if not manifest.get("units"):
        raise ValueError("manifest lists no units")
    for axis in manifest["axes"]:
        if not axis.get("options"):
            raise ValueError(f"axis {axis.get('name')} has no options")
    return manifest


def render(manifest: dict, choices: tuple[dict, ...]) -> tuple[dict, list[str], list[str]]:
    """Return (files, units, flags) for one combination of axis options."""
    slots = dict(manifest["defaults"])
    flags = list(manifest["flags"])
    units = list(manifest["units"])
    files = dict(manifest["files"])
    for option in choices:
        slots.update(option.get("slots", {}))
        for flag in option.get("flags_remove", []):
            if flag not in flags:
                raise ValueError(f"option {option['name']} removes absent flag {flag}")
            flags.remove(flag)
        flags.extend(option.get("flags_add", []))
        if "units" in option:
            units = list(option["units"])
        for edit in option.get("edits", []):
            text = files[edit["file"]]
            if text.count(edit["find"]) != 1:
                raise ValueError(
                    f"option {option['name']}: find text is not unique in {edit['file']}")
            files[edit["file"]] = text.replace(edit["find"], edit["replace"], 1)
    rendered = {}
    for name, text in files.items():
        def substitute(match):
            if match.group(1) not in slots:
                raise ValueError(f"slot {match.group(1)} has no value")
            return slots[match.group(1)]
        for _depth in range(8):  # slot values may themselves name slots
            text, count = SLOT.subn(substitute, text)
            if not count:
                break
        else:
            raise ValueError(f"slot expansion in {name} does not terminate")
        rendered[name] = text
    return rendered, units, flags


def alias_for(name: str, aliases: list[tuple[re.Pattern, str]]) -> str | None:
    for pattern, alias in aliases:
        if pattern.search(name):
            return alias
    return None


def object_functions(payload: bytes) -> list[dict]:
    """Functions in COFF section-table order with size and REL32 callees."""
    coff = CoffObject(payload)
    by_section: dict[int, list] = {}
    for symbol in coff.symbols.values():
        if symbol.section > 0 and symbol.typ & 0x20 and symbol.storage_class in (2, 3):
            by_section.setdefault(symbol.section, []).append(symbol)
    rows = []
    for section in coff.sections:
        if not section.characteristics & CODE:
            continue
        symbols = sorted(by_section.get(section.index, []), key=lambda s: s.value)
        for position, symbol in enumerate(symbols):
            end = (symbols[position + 1].value if position + 1 < len(symbols)
                   else section.raw_size)
            callees = [coff.symbols[r.symbol_index].name for r in coff.relocations
                       if r.section == section.index and r.typ == REL32
                       and symbol.value <= r.site < end]
            rows.append({"name": symbol.name, "section": section.index,
                         "section_name": section.name, "offset": symbol.value,
                         "size": end - symbol.value, "callees": callees})
    return rows


def parse_map(text: str) -> list[tuple[int, str, str]]:
    rows = []
    for line in text.splitlines():
        match = re.match(r"\s*[0-9a-f]{4}:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})"
                         r"(?:\s+f)?(?:\s+i)?\s+(\S+)\s*$", line)
        if match:
            rows.append((int(match.group(2), 16), match.group(1), match.group(3)))
    return sorted(rows)


def run_variant(manifest: dict, root: Path, index: int, choices, timeout: float) -> dict:
    from homm2.permute.tu_state_noise import compile_object

    name = "+".join(option["name"] for option in choices) or "baseline"
    files, units, flags = render(manifest, choices)
    work = root / f"{index:05d}"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    for file_name, text in files.items():
        (work / file_name).parent.mkdir(parents=True, exist_ok=True)
        (work / file_name).write_text(text, encoding="latin-1")
    from homm2.core.wine import winepath_w
    flags = [flag.replace("{work}", winepath_w(work)) for flag in flags]
    aliases = [(re.compile(p), a) for p, a in manifest["aliases"]]
    result = {"index": index, "variant": name,
              "choices": {axis["name"]: option["name"]
                          for axis, option in zip(manifest["axes"], choices)},
              "units": units, "flags": flags, "objects": {}, "ok": True}
    objects = []
    for unit in units:
        output = work / (Path(unit).stem + ".obj")
        ok, log, timed_out = compile_object(REPO, work / unit, output, flags, timeout)
        if not ok:
            result["ok"] = False
            result["error"] = f"{unit}: " + ("timeout" if timed_out else log.strip()[-600:])
            return result
        objects.append(output)
        functions = object_functions(output.read_bytes())
        for row in functions:
            row["alias"] = alias_for(row["name"], aliases)
            row["callees"] = [alias_for(c, aliases) or c for c in row["callees"]]
        result["objects"][unit] = {
            "order": " ".join(r["alias"] for r in functions if r["alias"]),
            "functions": functions,
        }
    if manifest.get("link"):
        result["link"] = link_variant(manifest["link"], work, objects, aliases)
    tracked = {}
    for unit, data in result["objects"].items():
        for row in data["functions"]:
            if row["alias"] in manifest["track"]:
                tracked[f"{row['alias']}@{unit}"] = {
                    "size": row["size"], "callees": row["callees"]}
    result["tracked"] = tracked
    hits = {}
    for key, wanted in manifest["expect"].items():
        got = (result.get("link", {}).get("order") if key == "link"
               else result["objects"].get(key, {}).get("order"))
        hits[key] = got == wanted
    result["expect"] = hits
    return result


def link_variant(spec: dict, work: Path, objects: list[Path], aliases) -> dict:
    from homm2.core import wine

    output, map_path = work / "probe.exe", work / "probe.map"
    lib = wine.msvc_dir() / "lib"
    args = ["/NOLOGO", "/INCREMENTAL:NO", f"/LIBPATH:{wine.winepath_w(lib)}",
            f"/MAP:{wine.winepath_w(map_path)}", f"/OUT:{wine.winepath_w(output)}",
            *spec.get("flags", []), *(wine.winepath_w(o) for o in objects),
            *spec.get("libs", [])]
    try:
        wine.run(wine.tool("LINK.EXE"), *args, cwd=work, log=work / "link.log")
    except RuntimeError as error:
        return {"ok": False, "error": str(error)[-600:]}
    rows = []
    for address, name, owner in parse_map(map_path.read_text(errors="replace")):
        alias = alias_for(name, aliases)
        if alias:
            rows.append({"alias": alias, "rva": address, "owner": owner, "name": name})
    return {"ok": True, "order": " ".join(r["alias"] for r in rows), "functions": rows}


def write_tsv(path: Path, manifest: dict, results: list[dict]) -> None:
    units = list(dict.fromkeys(unit for row in results for unit in row["units"]))
    columns = ([axis["name"] for axis in manifest["axes"]]
               + [f"order:{unit}" for unit in units]
               + ["link_order", "tracked", "expect_hits", "error"])
    lines = ["\t".join(columns)]
    for row in results:
        cells = [row["choices"].get(axis["name"], "") for axis in manifest["axes"]]
        cells += [row["objects"].get(unit, {}).get("order", "-") for unit in units]
        link = row.get("link") or {}
        cells.append(link.get("order", "-") if link.get("ok", True) else "link-failed")
        cells.append(";".join(f"{k}={v['size']:#x}:{','.join(v['callees'])}"
                              for k, v in sorted(row.get("tracked", {}).items())))
        cells.append(",".join(k for k, v in row.get("expect", {}).items() if v))
        cells.append(row.get("error", "").replace("\n", " ")[:200])
        lines.append("\t".join(cells))
    path.write_text("\n".join(lines) + "\n")


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    parser.add_argument("--compile-timeout", type=float, default=120.0)
    parser.add_argument("--limit", type=int, default=4096)
    args = parser.parse_args(argv)
    manifest = load_manifest(args.manifest)
    product = list(itertools.product(*(axis["options"] for axis in manifest["axes"])))
    if len(product) > args.limit:
        print(f"emission-order: {len(product)} variants exceed --limit {args.limit}",
              file=sys.stderr)
        return 2
    from homm2.core import wine
    wine.prepare_env()
    args.output.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(args.manifest, args.output / "manifest.json")
    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [pool.submit(run_variant, manifest, args.output, index, choices,
                               args.compile_timeout)
                   for index, choices in enumerate(product)]
        for future in concurrent.futures.as_completed(futures):
            results.append(future.result())
    results.sort(key=lambda row: row["index"])
    (args.output / "results.json").write_text(json.dumps(results, indent=1) + "\n")
    write_tsv(args.output / "results.tsv", manifest, results)
    failed = sum(1 for row in results if not row["ok"])
    hits = [row["variant"] for row in results
            if row.get("expect") and all(row["expect"].values())]
    print(f"emission-order: {len(results)} variants, {failed} failed, "
          f"{len(hits)} meet every expectation -> {args.output}")
    for name in hits[:20]:
        print(f"  hit: {name}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
