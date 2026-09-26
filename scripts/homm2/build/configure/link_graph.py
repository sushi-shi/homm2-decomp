#!/usr/bin/env python3
"""The link half of build.ninja: import libraries, resources, archives, LINK."""

from __future__ import annotations

import sys

from homm2.build.fixed_asm import UNITS as FIXED_ASM_UNITS

# WINMM and ADVAPI have no pending roots at their first scan, so this
# preserves the retail descriptor order while matching the retail
# import-thunk family order when later CRT/vendor members pull them.
LINK_LIBRARIES = [
    "WINMM.LIB", "KERNEL32.LIB", "USER32.LIB", "GDI32.LIB", "WSOCK32.LIB",
    "build/link/wing32.lib", "build/link/generic-imports/netapi32.lib",
    "ADVAPI32.LIB", "build/link/generic-imports/mss32.lib",
    "build/link/generic-imports/smackw32.lib",
    "build/link/generic-imports/audiere.lib",
]


def emit_link_graph(w, units: list[dict], objs: list[str],
                    base_symbol_sidecars: list[str],
                    first_function_rva: dict[str, int],
                    first_compgen_rva: dict[str, int]) -> None:
    generic_import_outputs = []
    for name in ("audiere", "mss32"):
        output = f"build/link/generic-imports/{name}.lib"
        w.build(output, "native_implib" if name == "audiere" else "definition_implib",
                inputs=f"imports/{name}.def",
                implicit=["scripts/homm2/build/regular_import_lib.py",
                          "scripts/homm2/build/import_lib.py"],
                variables={"dll": f"{name}.dll"})
        generic_import_outputs.append(output)
    for name, dll, options in (
            ("smackw32", "smackw32.DLL", ""),
            ("netapi32", "NETAPI32.dll", "--symbol _Netbios@4 --lookup Netbios --hint 180")):
        output = f"build/link/generic-imports/{name}.lib"
        w.build(output, "definition_vendor_implib", inputs=f"imports/{name}.def",
                implicit=["scripts/homm2/build/regular_vendor_import_lib.py",
                          "scripts/homm2/build/regular_import_lib.py"],
                variables={"dll": dll, "options": options})
        generic_import_outputs.append(output)
    output = "build/link/wing32.lib"
    w.build(output, "legacy_implib", inputs="imports/wing32.def",
            implicit="scripts/homm2/build/legacy_import_lib.py")
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
    # Archive ownership comes from the unit tier, not an address-list cut.
    # X_GLOBAL has data but no function anchor, and a fresh checkout has no
    # symbol inventory at all. In either case it can sort after BASE/Midi;
    # slicing there incorrectly consumes BASE objects as direct-link inputs.
    base_prefix = "build/objdiff/base/BASE/"
    source_objects = [obj for obj in link_objects if not obj.startswith(base_prefix)]
    base_objects = [obj for obj in link_objects if obj.startswith(base_prefix)]
    omf_link_objects = {}
    configured = {entry["unit"]: entry["source"] for entry in units}
    for unit, assembly in FIXED_ASM_UNITS.items():
        if configured.get(unit) != assembly.source:
            raise ValueError(
                f"{unit} must use fixed MASM source {assembly.source}"
            )
        output = f"build/link/omf/{unit}.obj"
        w.build(output, "ml_omf", inputs=assembly.source,
                implicit="scripts/homm2/build/ml_wrap.py")
        omf_link_objects[f"build/objdiff/base/{unit}.obj"] = output
    base_objects = [omf_link_objects.get(obj, obj) for obj in base_objects]
    # Every archive member is an untouched compiler or assembler output.
    midi_index = base_objects.index("build/objdiff/base/BASE/Midi.obj")
    prefix_libraries = [("build/link/BASE-prefix.lib", base_objects[:midi_index])]
    if any(obj in base_objects for obj in (
            "build/objdiff/base/BASE/Misc.obj", "build/objdiff/base/BASE/MiscRuntime.obj")):
        misc_index = base_objects.index("build/objdiff/base/BASE/Misc.obj")
        misc_runtime_index = base_objects.index("build/objdiff/base/BASE/MiscRuntime.obj")
        if misc_runtime_index != misc_index + 1 or misc_runtime_index >= midi_index:
            raise ValueError("Misc and MiscRuntime must be adjacent before Midi")
        prefix_libraries = [
            ("build/link/BASE-prefix.lib", base_objects[:misc_index]),
            ("build/link/Misc.lib", base_objects[misc_index:misc_index + 1]),
            ("build/link/MiscRuntime.lib", base_objects[misc_runtime_index:misc_runtime_index + 1]),
            ("build/link/BASE-middle.lib", base_objects[misc_runtime_index + 1:midi_index]),
        ]
    base_libraries = []
    for library, members in (
            *prefix_libraries,
            ("build/link/Midi.lib", base_objects[midi_index:midi_index + 1]),
            ("build/link/BASE-suffix.lib", base_objects[midi_index + 1:])):
        # VC6 LIB prepends each input member.  Feed the reviewed retail
        # order backwards so each archive scans forwards.
        w.build(library, "archive", inputs=list(reversed(members)))
        base_libraries.append(library)
    # The native-link driver places the SP5 MSVCPRT scan before this ordinary
    # LIBCMT scan so the retail operator-delete owner resolves first.
    runtime_delete_scan = "LIBCMT.LIB"
    link_args = (LINK_LIBRARIES + source_objects + base_libraries
                 + [runtime_delete_scan, resource_output])
    for mode in ("generic", "rsrc", "historical"):
        outputs = [f"build/link/{mode}/HMM2PL.exe", f"build/link/{mode}/HMM2PL.map"]
        w.build(outputs, "link_exe",
                inputs=(source_objects + base_libraries
                        + ([resource_output] if mode != "generic" else [])),
                implicit=(generic_import_outputs + [
                    "build.ninja",  # The driver reads link_args from this graph.
                    "scripts/homm2/build/native_link.py",
                    "build/toolchain/msvc/bin/LINK.EXE",
                    "build/toolchain/msvc/lib/LIBCMT.LIB",
                    "build/toolchain/msvc/lib/MSVCPRT.LIB",
                ]),
                variables={"link_args": " ".join(link_args),
                           "link_mode": "--" + mode if mode != "generic" else ""})
    for alias in ("link-inputs", "link-generic-inputs"):
        w.build(alias, "phony",
                inputs=source_objects + base_libraries + generic_import_outputs)
    link_audit_outputs = [
        "build/link/historical/HMM2PL.link.json",
        "build/link/historical/HMM2PL.missing-data.tsv",
    ]
    w.build(link_audit_outputs, "link_audit",
            inputs=["build/link/historical/HMM2PL.exe", "build/link/historical/HMM2PL.map"],
            implicit=[
                "scripts/homm2/build/link_exe.py",
                "build/gen/symbol_names.csv",
                "config/required_initialized_storage.tsv",
                "config/delink_relocs.tsv",
                "build/orig/HMM2PL.exe",
            ] + base_symbol_sidecars)
    w.build("link", "phony", inputs="build/link/generic/HMM2PL.exe")
    w.build("link-rsrc", "phony", inputs="build/link/rsrc/HMM2PL.exe")
    w.build("link-historical", "phony", inputs="build/link/historical/HMM2PL.exe")
    w.build("link-audit", "phony", inputs="build/link/historical/HMM2PL.link.json")
    for alias in ("link-imports", "link-generic-imports"):
        w.build(alias, "phony", inputs=generic_import_outputs)
    w.build("link-resources", "phony", inputs=resource_output)
    w.build("link-map", "phony", inputs="build/link/generic/HMM2PL.map")
