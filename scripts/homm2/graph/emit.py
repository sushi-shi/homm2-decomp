#!/usr/bin/env python3
"""homm2.graph.emit - generate build.ninja and objdiff.json.

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

from homm2.graph import ninja_syntax
from homm2.retail_labels.annotated_functions import source_function_spans
from homm2.manifest import load as load_manifest, units as image_units
from homm2.core.paths import DEFAULT_IMAGE, REPO, image_build, image_paths, retail_dir

from .compile_graph import emit_compile_graph
from .link_graph import LINK_DIFF_STAMP, emit_link_graph
from .rules import emit_rules


from homm2.core.usage import logged


@logged
def main() -> None:
    """Emit the selected image's graph: build.ninja at the root for the game,
    build/<image>/build.ninja for another image (`ninja -f`). Another image
    has no link graph yet; its rules select it through $HOMM2_IMAGE."""
    manifest = load_manifest()
    build = manifest.get("build", {})
    image = image_paths()
    game = image.key == DEFAULT_IMAGE
    units = image_units(manifest)
    root = image_build()

    od = root / "objdiff"
    od.mkdir(parents=True, exist_ok=True)
    # Minimal valid i386 COFF for units without a delinked target.
    dummy = (struct.pack("<HHIIIHH", 0x14C, 1, 0, 20 + 40, 0, 0, 0)
             + struct.pack("<8sIIIIIIHHI", b".text\0\0\0", 0, 0, 0, 0, 0, 0,
                           0, 0, 0x60000020)
             + struct.pack("<I", 4))
    dummy_path = od / "dummy.obj"
    if not dummy_path.exists() or dummy_path.read_bytes() != dummy:
        dummy_path.write_bytes(dummy)
    delink = root / "delink"
    reviewed_units = set()
    reviewed = retail_dir() / "data_initialized_storage.tsv"
    if reviewed.exists():
        with reviewed.open() as stream:
            for row in csv.DictReader(
                    (line for line in stream if not line.lstrip().startswith("#")),
                    delimiter="\t"):
                reviewed_units.add(row["unit"])
    first_function_rva = {}
    first_compgen_rva = {}
    symbols = root / "gen/symbol_names.csv"
    if symbols.exists():
        with symbols.open() as stream:
            for row in csv.DictReader(stream):
                if row["kind"] != "func":
                    continue
                rva = int(row["rva"], 0)
                rvas = (first_function_rva if row["provenance"] == "source-annotation"
                        else first_compgen_rva)
                rvas[row["unit"]] = min(rva, rvas.get(row["unit"], rva))

    # Source owns function boundaries; an owner split must configure before
    # redelink regenerates the previous symbol inventory.
    source_rvas = {}
    for span in (source_function_spans(REPO / "src", REPO) if game else ()):
        source_rvas[span.unit] = min(span.rva, source_rvas.get(span.unit, span.rva))
    first_function_rva.update(source_rvas)

    ninja_file = REPO / "build.ninja" if game else root / "build.ninja"
    ninja_file.parent.mkdir(parents=True, exist_ok=True)
    with open(ninja_file, "w") as f:
        w = ninja_syntax.Writer(f)
        if game:
            emit_rules(w)
        else:
            emit_rules(w, builddir=image.build, image=image)
        objs, base_symbol_sidecars, comparison_paths = emit_compile_graph(
            w, manifest, units, delink, reviewed_units, image)
        if game:
            emit_link_graph(w, units, objs, base_symbol_sidecars,
                            first_function_rva, first_compgen_rva)
            w.default(["all", LINK_DIFF_STAMP])
        else:
            w.default(["all"])

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
    print(f"configure: {len(units)} units -> {ninja_file.relative_to(REPO)} + "
          f"{(od / 'objdiff.json').relative_to(REPO)}")
