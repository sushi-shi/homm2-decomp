#!/usr/bin/env python3
"""The compile/comparison half of build.ninja: one edge chain per TU."""

from __future__ import annotations

from pathlib import Path

from homm2.graph.fixed_asm import unit as fixed_asm_unit
from homm2.manifest import unit_flags as manifest_unit_flags
from homm2.core.paths import REPO


def emit_compile_graph(w, manifest: dict, units: list[dict], delink: Path,
                       reviewed_units: set[str], image=None):
    """Emit the per-unit compile + normalization + pairing edges.

    Returns (objs, base_symbol_sidecars, comparison_paths) for the link graph
    and the objdiff configuration. `image` (homm2.core.paths.image_paths) roots
    every generated path in the image's build tree.
    """
    from homm2.core.paths import image_paths
    image = image or image_paths()
    B, R = image.build, image.retail
    weak_external_stamp = (
        f"{B}/objdiff/normalized/weak-external-link-set.stamp")
    normalizer = ["scripts/homm2/compare/canonicalize_data_symbols.py",
                  "scripts/homm2/compare/normalized_freshness.py",
                  f"{B}/gen/compiler_generated_functions.csv",
                  f"{B}/gen/delink_data_from_source.tsv",
                  weak_external_stamp]
    reloc_normalizer = "scripts/homm2/compare/canonicalize_relocs.py"
    normalized_dummy = f"{B}/objdiff/normalized/dummy.obj"
    normalized_dummy_sidecar = f"{B}/objdiff/normalized/dummy.symbols.tsv"
    w.build(normalized_dummy, "canonicalize_data_symbols",
            inputs=f"{B}/objdiff/dummy.obj", implicit=normalizer,
            implicit_outputs=normalized_dummy_sidecar,
            variables={"sidecar": normalized_dummy_sidecar, "unit": "dummy"})
    objs = []
    base_symbol_sidecars = []
    comparison_inputs = [normalized_dummy]
    comparison_paths = {}
    for u in units:
        obj = f"{B}/objdiff/base/{u['unit']}.obj"
        assembly = fixed_asm_unit(u["unit"], u["source"])
        if assembly is not None:
            w.build(obj, "ml_coff", inputs=u["source"],
                    implicit="scripts/homm2/graph/ml.py")
        else:
            # The one flag-assembly rule (profile + BASE tier /Gy) lives in
            # homm2.manifest.unit_flags; probes share it via resolve_target.
            unit_flags = manifest_unit_flags(u, manifest, image.key)
            w.build(obj, "cl", inputs=u["source"],
                    implicit=["scripts/homm2/graph/cc.py",
                              "scripts/homm2/graph/localization.py",
                              "scripts/homm2/graph/catalog.py",
                              "locales/messages.def", "locales/ru.po"],
                    variables={"flags": " ".join(unit_flags),
                               "unit": u["unit"]})
        objs.append(obj)
        normalized = f"{B}/objdiff/normalized/base/{u['unit']}.obj"
        sidecar = (
            f"{B}/objdiff/normalized/base/{u['unit']}.symbols.tsv")
        base_symbol_sidecars.append(sidecar)
        w.build(normalized, "canonicalize_data_symbols", inputs=obj,
                implicit=normalizer, implicit_outputs=sidecar,
                variables={"sidecar": sidecar, "unit": u["unit"]})
        comparison_inputs.append(normalized)

        target = delink / f"{u['unit']}.c.obj"
        if target.exists() or u["unit"] in reviewed_units:
            target_input = f"{B}/delink/{u['unit']}.c.obj"
            paired_target = (
                f"{B}/objdiff/paired/target/{u['unit']}.c.obj")
            w.build(paired_target, "canonicalize_relocs", inputs=target_input,
                    implicit=[obj, reloc_normalizer,
                              "scripts/homm2/compare/normalized_freshness.py",
                              "scripts/homm2/verify/assert_relocs.py",
                              "scripts/homm2/graph/vendor_imports.py",
                              f"{B}/gen/symbol_names.csv",
                              f"{R}/reloc_rel32_aliases.tsv",
                              image.exe],
                    variables={"base": obj, "unit": u["unit"]})
            target_normalized = (
                f"{B}/objdiff/normalized/target/{u['unit']}.c.obj")
            target_sidecar = (
                f"{B}/objdiff/normalized/target/{u['unit']}.symbols.tsv")
            w.build(target_normalized, "canonicalize_data_symbols",
                    inputs=paired_target, implicit=normalizer,
                    implicit_outputs=target_sidecar,
                    variables={"sidecar": target_sidecar, "unit": u["unit"]})
            comparison_inputs.append(target_normalized)
            comparison_paths[u["unit"]] = (
                f"./normalized/base/{u['unit']}.obj",
                f"./normalized/target/{u['unit']}.c.obj",
            )
        else:
            comparison_paths[u["unit"]] = (
                f"./normalized/base/{u['unit']}.obj",
                "./normalized/dummy.obj",
            )
    weak_link_inputs = objs + [
        str(path.relative_to(REPO))
        for path in sorted(delink.rglob("*.c.obj"))
    ]
    w.build(
        weak_external_stamp, "assert_weak_external_link_set",
        inputs=weak_link_inputs,
        implicit="scripts/homm2/compare/canonicalize_data_symbols.py")
    # Parenthesized root modules - "(unmatched)", "(libcmt)", "(imports)",
    # "(funclets)", "(compgen)" - are target-only: reviewed or residual
    # functions delinked without a base source to compile against. They
    # take data normalization only and list opposite the empty dummy.
    for target_obj in sorted(delink.glob("(*).c.obj")):
        module = target_obj.name[:-len(".c.obj")]
        normalized = f"{B}/objdiff/normalized/target/{module}.c.obj"
        sidecar = f"{B}/objdiff/normalized/target/{module}.symbols.tsv"
        # ninja shell-quotes $in/$out but not edge variables; the parens
        # are shell metacharacters, so quote these two here.
        w.build(normalized, "canonicalize_data_symbols",
                inputs=f"{B}/delink/{module}.c.obj", implicit=normalizer,
                implicit_outputs=sidecar,
                variables={"sidecar": f"'{sidecar}'",
                           "unit": f"'{module}'"})
        comparison_inputs.append(normalized)
        comparison_paths[module] = (
            "./normalized/dummy.obj",
            f"./normalized/target/{module}.c.obj",
        )
    w.build("all", "phony", inputs=comparison_inputs)
    w.build("base", "phony", inputs=objs)
    return objs, base_symbol_sidecars, comparison_paths
