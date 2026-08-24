#!/usr/bin/env python3
"""homm2.build.configure - generate build.ninja and objdiff.json.

The repo-root ``configure.py`` is a shim onto this package. The generator is
split by graph: ``rules`` declares every ninja rule, ``compile_graph`` emits
the per-TU compile/normalize/pairing edges, ``link_graph`` emits the import
libraries, resources, archives, and LINK edges. Emission order is stable;
a refactor here must leave build.ninja byte-identical.
"""

from __future__ import annotations

import csv
import json
import struct

from homm2.build import ninja_syntax
from homm2.core.manifest import load as load_manifest
from homm2.core.paths import REPO

from .compile_graph import emit_compile_graph
from .link_graph import emit_link_graph
from .rules import emit_rules


def main() -> None:
    manifest = load_manifest()
    build = manifest.get("build", {})
    units = manifest.get("unit", [])

    od = REPO / "build/objdiff"
    od.mkdir(parents=True, exist_ok=True)
    # Minimal valid i386 COFF for units without a delinked target.
    dummy = (struct.pack("<HHIIIHH", 0x14C, 1, 0, 20 + 40, 0, 0, 0)
             + struct.pack("<8sIIIIIIHHI", b".text\0\0\0", 0, 0, 0, 0, 0, 0,
                           0, 0, 0x60000020)
             + struct.pack("<I", 4))
    dummy_path = od / "dummy.obj"
    if not dummy_path.exists() or dummy_path.read_bytes() != dummy:
        dummy_path.write_bytes(dummy)
    delink = REPO / "build/delink"
    reviewed_units = set()
    reviewed = REPO / "config/required_initialized_storage.tsv"
    if reviewed.exists():
        with reviewed.open() as stream:
            for row in csv.DictReader(
                    (line for line in stream if not line.lstrip().startswith("#")),
                    delimiter="\t"):
                reviewed_units.add(row["unit"])
    first_function_rva = {}
    first_compgen_rva = {}
    symbols = REPO / "build/gen/symbol_names.csv"
    if symbols.exists():
        with symbols.open() as stream:
            for row in csv.DictReader(stream):
                if row["kind"] != "func":
                    continue
                rva = int(row["rva"], 0)
                rvas = (first_function_rva if row["provenance"] == "source-annotation"
                        else first_compgen_rva)
                rvas[row["unit"]] = min(rva, rvas.get(row["unit"], rva))

    with open(REPO / "build.ninja", "w") as f:
        w = ninja_syntax.Writer(f)
        emit_rules(w)
        objs, base_symbol_sidecars, comparison_paths = emit_compile_graph(
            w, manifest, units, delink, reviewed_units)
        emit_link_graph(w, objs, base_symbol_sidecars,
                        first_function_rva, first_compgen_rva)
        w.default("all")

    units_j = []
    for u in units:
        base_path, target_path = comparison_paths[u["unit"]]
        units_j.append({
            "name": u["unit"],
            "base_path": base_path,
            "target_path": target_path,
            "scratch": {"platform": build.get("platform", "win32"),
                        "compiler": build.get("compiler", "msvc4.2")},
        })
    for module in sorted(comparison_paths):
        if not module.startswith("("):
            continue
        base_path, target_path = comparison_paths[module]
        units_j.append({
            "name": module,
            "base_path": base_path,
            "target_path": target_path,
            "scratch": {"platform": build.get("platform", "win32"),
                        "compiler": build.get("compiler", "msvc4.2")},
        })
    (od / "objdiff.json").write_text(json.dumps({
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "build_base": False, "build_target": False,
        # Relocation-masked instruction bytes are not sufficient proof: two globals
        # can generate identical opcodes while naming different storage.  Keep the
        # strictest objdiff relocation comparison visible in the normal report;
        # the owner/addend gates remain independent project-specific proof.
        "options": {"functionRelocDiffs": "all"},
        "watch_patterns": ["*.obj"], "units": units_j,
    }, indent=2) + "\n")
    print(f"configure: {len(units)} units -> build.ninja + build/objdiff/objdiff.json")
