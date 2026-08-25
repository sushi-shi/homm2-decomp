#!/usr/bin/env python3
"""The link half of build.ninja: import libraries, resources, archives, LINK."""

from __future__ import annotations

import sys

from homm2.core.paths import REPO

# WINMM and ADVAPI have no pending roots at their first scan, so this
# preserves the retail descriptor order while matching the retail
# import-thunk family order when later CRT/vendor members pull them.
LINK_LIBRARIES = [
    "WINMM.LIB", "KERNEL32.LIB", "USER32.LIB", "GDI32.LIB", "WSOCK32.LIB",
    "build/link/wing32.lib", "build/link/netapi32.lib",
    "ADVAPI32.LIB", "build/link/mss32.lib",
    "build/link/smackw32.lib",
    "build/link/audiere.lib",
]


def emit_link_graph(w, objs: list[str], base_symbol_sidecars: list[str],
                    first_function_rva: dict[str, int],
                    first_compgen_rva: dict[str, int]) -> None:
    retail_import_outputs = []
    generic_import_outputs = []
    for name in ("audiere",):
        output = f"build/link/{name}.lib"
        w.build(
            output,
            "implib_stub",
            inputs="build/orig/HMM2PL.exe",
            implicit=[
                "scripts/homm2/build/import_lib.py",
                "scripts/homm2/build/link_exe.py",
                "scripts/homm2/build/cc_wrap.py",
            ],
            variables={
                "dll": f"{name}.dll",
            },
        )
        retail_import_outputs.append(output)
        generic_output = f"build/link/generic-imports/{name}.lib"
        w.build(
            generic_output,
            "definition_implib",
            inputs=f"imports/{name}.def",
            implicit="scripts/homm2/build/regular_import_lib.py",
            variables={"dll": f"{name}.dll"},
        )
        generic_import_outputs.append(generic_output)
    output = "build/link/mss32.lib"
    w.build(
        output,
        "regular_implib",
        inputs="build/orig/HMM2PL.exe",
        implicit=[
            "scripts/homm2/build/regular_import_lib.py",
            "scripts/homm2/build/legacy_import_lib.py",
            "scripts/homm2/build/import_lib.py",
            "scripts/homm2/build/link_exe.py",
        ],
        variables={
            "dll": "mss32.dll",
        },
    )
    retail_import_outputs.append(output)
    generic_output = "build/link/generic-imports/mss32.lib"
    w.build(
        generic_output,
        "definition_implib",
        inputs="imports/mss32.def",
        implicit="scripts/homm2/build/regular_import_lib.py",
        variables={"dll": "mss32.dll"},
    )
    generic_import_outputs.append(generic_output)
    output = "build/link/smackw32.lib"
    w.build(
        output,
        "regular_vendor_implib",
        inputs="build/orig/HMM2PL.exe",
        implicit=[
            "scripts/homm2/build/regular_vendor_import_lib.py",
            "scripts/homm2/build/regular_import_lib.py",
            "scripts/homm2/build/legacy_import_lib.py",
            "scripts/homm2/build/link_exe.py",
            "imports/smackw32.def",
        ],
        variables={
            "dll": "smackw32.DLL",
            "definition": "imports/smackw32.def",
        },
    )
    retail_import_outputs.append(output)
    generic_output = "build/link/generic-imports/smackw32.lib"
    w.build(
        generic_output,
        "definition_vendor_implib",
        inputs="imports/smackw32.def",
        implicit=["scripts/homm2/build/regular_vendor_import_lib.py",
                  "scripts/homm2/build/regular_import_lib.py"],
        variables={"dll": "smackw32.DLL", "options": ""},
    )
    generic_import_outputs.append(generic_output)
    output = "build/link/netapi32.lib"
    w.build(
        output,
        "regular_vendor_implib",
        inputs="build/orig/HMM2PL.exe",
        implicit=[
            "scripts/homm2/build/regular_vendor_import_lib.py",
            "scripts/homm2/build/regular_import_lib.py",
            "scripts/homm2/build/legacy_import_lib.py",
            "scripts/homm2/build/link_exe.py",
            "imports/netapi32.def",
        ],
        variables={
            "dll": "NETAPI32.dll",
            "definition": "imports/netapi32.def",
            "options": "--symbol _Netbios@4 --lookup Netbios --hint 180",
        },
    )
    retail_import_outputs.append(output)
    generic_output = "build/link/generic-imports/netapi32.lib"
    w.build(
        generic_output,
        "definition_vendor_implib",
        inputs="imports/netapi32.def",
        implicit=["scripts/homm2/build/regular_vendor_import_lib.py",
                  "scripts/homm2/build/regular_import_lib.py"],
        variables={
            "dll": "NETAPI32.dll",
            "options": "--symbol _Netbios@4 --lookup Netbios --hint 180",
        },
    )
    generic_import_outputs.append(generic_output)
    output = "build/link/wing32.lib"
    w.build(
        output,
        "legacy_implib",
        inputs="imports/wing32.def",
        implicit="scripts/homm2/build/legacy_import_lib.py",
    )
    retail_import_outputs.append(output)
    generic_import_outputs.append(output)
    resource_output = "build/link/HMM2PL.res"
    w.build([resource_output, "build/link/HMM2PL.resources.json"], "link_resources",
            inputs=["res/HMM2PL.rc", "build/orig/HMM2PL.exe"],
            implicit=["scripts/homm2/build/rc_res.py",
                      "scripts/homm2/build/extract_resources.py",
                      "build/toolchain/msvc/bin/RC.EXE"])
    link_objects = sorted(
        objs,
        key=lambda obj: first_function_rva.get(
            obj.removeprefix("build/objdiff/base/").removesuffix(".obj"),
            first_compgen_rva.get(
                obj.removeprefix("build/objdiff/base/").removesuffix(".obj"),
                sys.maxsize)))
    link_outputs = ["build/link/HMM2PL.exe", "build/link/HMM2PL.map"]
    source_end = link_objects.index(
        "build/objdiff/base/SOURCE/X_GLOBAL.obj") + 1
    source_objects = link_objects[:source_end]
    base_objects = link_objects[source_end:]
    omf_link_objects = {}
    for unit in ("BASE/BITS", "BASE/TILE"):
        source = f"src/{unit}.asm"
        output = f"build/link/omf/{unit}.obj"
        w.build(output, "ml_omf", inputs=source,
                implicit="scripts/homm2/build/ml_wrap.py")
        omf_link_objects[f"build/objdiff/base/{unit}.obj"] = output
    base_objects = [omf_link_objects.get(obj, obj) for obj in base_objects]
    # The archives hold raw compiled objects only. `homm2 link --transform`
    # applies the reviewed COFF transforms (exact_link/transforms.py) when
    # rebuilding its own prefix/suffix variants; generic and --rsrc links
    # consume these archives untouched.
    midi_index = base_objects.index("build/objdiff/base/BASE/Midi.obj")
    base_libraries = []
    for library, members in (
            ("build/link/BASE-prefix.lib", base_objects[:midi_index]),
            ("build/link/Midi.lib", base_objects[midi_index:midi_index + 1]),
            ("build/link/BASE-suffix.lib", base_objects[midi_index + 1:])):
        # VC6 LIB prepends each input member.  Feed the reviewed retail
        # order backwards so each archive scans forwards.
        w.build(library, "archive", inputs=list(reversed(members)))
        base_libraries.append(library)
    # The plain-link driver places the SP5 MSVCPRT scan before this ordinary
    # LIBCMT scan so the retail operator-delete owner resolves first.
    runtime_delete_scan = "LIBCMT.LIB"
    link_args = (LINK_LIBRARIES + source_objects + base_libraries
                 + [runtime_delete_scan, resource_output])
    exact_link_helpers = sorted(
        str(path.relative_to(REPO))
        for path in (REPO / "scripts/homm2/build/exact_link").glob("*.py"))
    w.build(link_outputs, "link_exe",
            inputs=(source_objects + base_libraries + [resource_output]),
            implicit=(retail_import_outputs + exact_link_helpers + [
                "scripts/homm2/build/adapt_comdat_link_order.py",
                "config/retail_crt_order.txt",
                "build/gen/delink_data_from_source.tsv",
                "build/toolchain/msvc/lib/LIBCMT.LIB",
                "build/toolchain/msvc/lib/MSVCPRT.LIB",
            ] + base_symbol_sidecars),
            variables={"link_args": " ".join(link_args)})
    # `homm2 link` (generic) and `homm2 link --rsrc` run the plain driver
    # directly.  Generic inputs are built only from source and reviewed ABI
    # manifests; the resource-only mode adds the separate retail resource edge.
    w.build("link-inputs", "phony",
            inputs=(source_objects + base_libraries + retail_import_outputs))
    w.build("link-generic-inputs", "phony",
            inputs=(source_objects + base_libraries + generic_import_outputs))
    link_audit_outputs = [
        "build/link/HMM2PL.link.json",
        "build/link/HMM2PL.missing-data.tsv",
    ]
    w.build(link_audit_outputs, "link_audit", inputs=link_outputs,
            implicit=[
                "scripts/homm2/build/link_exe.py",
                "build/gen/symbol_names.csv",
                "config/required_initialized_storage.tsv",
                "config/delink_relocs.tsv",
                "build/orig/HMM2PL.exe",
            ] + base_symbol_sidecars)
    w.build("link", "phony", inputs="build/link/HMM2PL.link.json")
    w.build("link-imports", "phony", inputs=retail_import_outputs)
    w.build("link-generic-imports", "phony", inputs=generic_import_outputs)
    w.build("link-resources", "phony", inputs=resource_output)
    w.build("link-map", "phony", inputs="build/link/HMM2PL.map")
