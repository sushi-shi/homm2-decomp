"""Generate delinker owner intervals from retail NB09 sstModule contributions."""
import argparse
import os
import tomllib
from pathlib import Path

from homm2.build.link_exe import RETAIL_EXE, read_nb09_module_contributions


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
UNITS = REPO / "config/units.toml"
OUTPUT = REPO / "build/gen/delink_contributions.tsv"
HEADER = "object\tstorage\trva\tsize\tsegment\tsection\tprovenance"
SUPPORTED_SECTIONS = {".text": "text", ".rdata": "rdata", ".data": "data"}


def split_contribution(contribution):
    """Split a writable PE contribution at the initialized/loader-zero boundary."""
    section = contribution["section_name"]
    storage = SUPPORTED_SECTIONS.get(section)
    if storage is None:
        return []
    start = contribution["rva"]
    end = start + contribution["size"]
    if end > contribution["section_rva"] + contribution["section_virtual_size"]:
        raise ValueError("NB09 contribution exceeds PE virtual section: 0x%x..0x%x" %
                         (start, end))
    if section != ".data":
        return [(storage, start, end - start)]

    initialized_end = contribution["section_rva"] + contribution["section_raw_size"]
    rows = []
    if start < initialized_end:
        split = min(end, initialized_end)
        rows.append(("data", start, split - start))
    if end > initialized_end:
        split = max(start, initialized_end)
        rows.append(("bss", split, end - split))
    return rows


def contribution_rows(exe=RETAIL_EXE, units_path=UNITS):
    modules = read_nb09_module_contributions(exe, executable_only=False)
    manifest = tomllib.loads(Path(units_path).read_text())
    rows = []
    for unit in manifest.get("unit", []):
        unit_name = unit["unit"]
        stem = Path(unit["source"]).stem.lower()
        records = modules.get(stem, [])
        if len(records) != 1:
            raise ValueError("expected one NB09 module named %s for %s, found %d" %
                             (stem, unit_name, len(records)))
        object_name = unit_name.replace("/", "\\") + ".c"
        for contribution in records[0]["contributions"]:
            for storage, rva, size in split_contribution(contribution):
                rows.append({
                    "object": object_name,
                    "storage": storage,
                    "rva": rva,
                    "size": size,
                    "segment": contribution["section"],
                    "section": contribution["section_name"],
                    "provenance": "retail-nb09-sstModule",
                })
    rows.sort(key=lambda row: (row["rva"], row["object"], row["storage"]))
    for previous, current in zip(rows, rows[1:]):
        if previous["rva"] + previous["size"] > current["rva"]:
            raise ValueError("overlapping NB09 contribution intervals: %s and %s" %
                             (previous["object"], current["object"]))
    return rows


def manifest_bytes(exe=RETAIL_EXE, units_path=UNITS):
    lines = [
        "# Whole-compiland owner ranges from retail NB09 sstModule; sizes are not symbols.",
        HEADER,
    ]
    for row in contribution_rows(exe, units_path):
        lines.append("{object}\t{storage}\t0x{rva:x}\t0x{size:x}\t{segment}\t{section}\t{provenance}".
                     format(**row))
    return ("\n".join(lines) + "\n").encode("utf-8")


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", type=Path, default=RETAIL_EXE)
    parser.add_argument("--units", type=Path, default=UNITS)
    parser.add_argument("--output", type=Path, default=OUTPUT)
    args = parser.parse_args(argv)
    payload = manifest_bytes(args.exe, args.units)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(payload)
    print("[contribution_manifest] %d ranges -> %s" %
          (len(payload.splitlines()) - 2, args.output))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
