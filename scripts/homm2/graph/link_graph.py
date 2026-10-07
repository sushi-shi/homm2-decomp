#!/usr/bin/env python3
"""The link half of build.ninja: import libraries, resources, archives, LINK."""

from __future__ import annotations

import sys

from homm2.graph.fixed_asm import UNITS as FIXED_ASM_UNITS

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


LINK_DIFF_STAMP = "build/link/historical/HMM2PL.link-diff.tsv"


def emit_link_graph(w, units: list[dict], objs: list[str],
                    base_symbol_sidecars: list[str],
                    first_function_rva: dict[str, int],
                    first_compgen_rva: dict[str, int]) -> None:
    generic_import_outputs = []
    for name in ("audiere", "mss32"):
        output = f"build/link/generic-imports/{name}.lib"
        w.build(output, "native_implib" if name == "audiere" else "definition_implib",
                inputs=f"imports/{name}.def",
                implicit=["scripts/homm2/graph/regular_import_lib.py",
                          "scripts/homm2/graph/import_lib.py"],
                variables={"dll": f"{name}.dll"})
        generic_import_outputs.append(output)
    for name, dll, options in (
            ("smackw32", "smackw32.DLL", ""),
            ("netapi32", "NETAPI32.dll", "--symbol _Netbios@4 --lookup Netbios --hint 180")):
        output = f"build/link/generic-imports/{name}.lib"
        w.build(output, "definition_vendor_implib", inputs=f"imports/{name}.def",
                implicit=["scripts/homm2/graph/regular_vendor_import_lib.py",
                          "scripts/homm2/graph/regular_import_lib.py"],
                variables={"dll": dll, "options": options})
        generic_import_outputs.append(output)
    output = "build/link/wing32.lib"
    w.build(output, "legacy_implib", inputs="imports/wing32.def",
            implicit="scripts/homm2/graph/legacy_import_lib.py")
    generic_import_outputs.append(output)
    resource_output = "build/link/HMM2PL.res"
    w.build([resource_output, "build/link/HMM2PL.resources.json"], "link_resources",
            inputs=["res/HMM2PL.rc", "build/orig/HMM2PL.exe"],
            implicit=["scripts/homm2/graph/rc.py",
                      "scripts/homm2/graph/extract_resources.py",
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
                implicit="scripts/homm2/graph/ml.py")
        omf_link_objects[f"build/objdiff/base/{unit}.obj"] = output
    base_objects = [omf_link_objects.get(obj, obj) for obj in base_objects]
    # Every archive member is an untouched compiler or assembler output.
    midi_index = base_objects.index("build/objdiff/base/BASE/Midi.obj")
    prefix_libraries = [("build/link/BASE-prefix.lib", base_objects[:midi_index])]
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
                    "scripts/homm2/graph/link.py",
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
                "scripts/homm2/graph/link_exe.py",
                "build/gen/symbol_names.csv",
                "config/retail/data_initialized_storage.tsv",
                "config/retail/absolute_relocations.tsv",
                "build/orig/HMM2PL.exe",
            ] + base_symbol_sidecars)
    # The regression ceiling runs in the default graph so an ordinary build
    # cannot move the historical image further from retail unnoticed.
    w.build(LINK_DIFF_STAMP, "link_diff",
            inputs="build/link/historical/HMM2PL.exe",
            implicit=["scripts/homm2/verify/link_diff.py",
                      "config/link_diff.tsv", "build/orig/HMM2PL.exe"])
    w.build("link-diff", "phony", inputs=LINK_DIFF_STAMP)
    w.build("link", "phony", inputs="build/link/generic/HMM2PL.exe")
    w.build("link-rsrc", "phony", inputs="build/link/rsrc/HMM2PL.exe")
    w.build("link-historical", "phony", inputs="build/link/historical/HMM2PL.exe")
    w.build("link-audit", "phony", inputs="build/link/historical/HMM2PL.link.json")
    for alias in ("link-imports", "link-generic-imports"):
        w.build(alias, "phony", inputs=generic_import_outputs)
    w.build("link-resources", "phony", inputs=resource_output)
    w.build("link-map", "phony", inputs="build/link/generic/HMM2PL.map")


#: Another image's import libraries: (import library, rule, definition, DLL).
#: The DLL spelling is the one its retail import descriptor names.
IMAGE_IMPORT_LIBRARIES = {
    "editor": (
        ("wing32.lib", "legacy_implib", "imports/wing32.def", "WING32.dll"),
        ("generic-imports/mss32.lib", "definition_implib", "imports/mss32.def", "mss32.dll"),
        ("generic-imports/audiere.lib", "native_implib", "imports/audiere.def", "audiere.dll"),
    ),
}
#: Another image's library line, in LINK's search order. Generated import
#: libraries are named by their path under the image's build/link. As in the
#: game, WINMM leads: only BASE (soundmgr) references it, so its descriptor
#: still follows the first scan's DLLs while its thunks lead the second block.
IMAGE_LINK_LIBRARIES = {
    "editor": ("WINMM.LIB", "KERNEL32.LIB", "USER32.LIB", "GDI32.LIB", "wing32.lib",
               "ADVAPI32.LIB", "generic-imports/mss32.lib",
               "generic-imports/audiere.lib"),
}


#: Another image's BASE archive cuts: the unit that opens each further
#: archive. LINK resolves one library at a time, pulling members in the order
#: of the undefined-symbol list (first reference, appended as members are
#: pulled), so a member's archive decides whether an earlier-referenced member
#: of a later archive can precede it. The editor's BASE library is one archive.
IMAGE_ARCHIVE_CUTS: dict[str, tuple[str, ...]] = {
    "editor": (),
}


def image_link_order(image, objs: list[str]) -> list[str]:
    """Another image's objects in retail link order: each unit's first
    function in the image's symbol inventory. A unit the inventory has not
    placed yet sorts last, so a fresh checkout still configures."""
    from homm2.core.paths import REPO
    first = {}
    symbols = REPO / image.build / "gen/symbol_names.csv"
    if symbols.exists():
        import csv
        with symbols.open() as stream:
            for row in csv.DictReader(stream):
                if row["kind"] == "func" and not row["unit"].startswith("("):
                    rva = int(row["rva"], 0)
                    first[row["unit"]] = min(rva, first.get(row["unit"], rva))
    prefix = f"{image.build}/objdiff/base/"
    return sorted(objs, key=lambda obj: first.get(
        obj.removeprefix(prefix).removesuffix(".obj"), sys.maxsize))


def emit_image_link_graph(w, image, units: list[dict], objs: list[str]) -> str:
    """Link another image the way the game links: its own objects explicitly
    in retail order, the BASE library, the C runtime, import libraries built
    from the reviewed ABI manifests, and resources compiled from source.
    Returns the link-diff stamp (the image's default target)."""
    from homm2.graph.link import PROFILES
    B = image.build
    profile = PROFILES[image.key]
    stem = profile.stem
    link_root = f"{B}/link"
    import_outputs = []
    for library, rule, definition, dll in IMAGE_IMPORT_LIBRARIES[image.key]:
        output = f"{link_root}/{library}"
        implicit = {"legacy_implib": ["scripts/homm2/graph/legacy_import_lib.py"],
                    "native_implib": ["scripts/homm2/graph/regular_import_lib.py",
                                      "scripts/homm2/graph/import_lib.py"],
                    "definition_implib": ["scripts/homm2/graph/regular_import_lib.py",
                                          "scripts/homm2/graph/import_lib.py"]}[rule]
        w.build(output, rule, inputs=definition, implicit=implicit,
                variables={"dll": dll})
        import_outputs.append(output)
    resource_output = f"{link_root}/{stem}.res"
    w.build([resource_output, f"{link_root}/{stem}.resources.json"], "image_link_resources",
            inputs=[f"res/{stem}.rc", image.exe],
            implicit=["scripts/homm2/graph/rc.py",
                      "scripts/homm2/graph/extract_resources.py",
                      "build/toolchain/msvc/bin/RC.EXE"])
    ordered = image_link_order(image, objs)
    base_prefix = f"{B}/objdiff/base/BASE/"
    source_objects = [obj for obj in ordered if not obj.startswith(base_prefix)]
    base_objects = [obj for obj in ordered if obj.startswith(base_prefix)]
    configured = {entry["unit"]: entry["source"] for entry in units}
    for unit, assembly in FIXED_ASM_UNITS.items():
        if unit not in configured:
            continue
        if configured[unit] != assembly.source:
            raise ValueError(f"{unit} must use fixed MASM source {assembly.source}")
        output = f"{link_root}/omf/{unit}.obj"
        w.build(output, "ml_omf", inputs=assembly.source,
                implicit="scripts/homm2/graph/ml.py")
        base_objects = [output if obj == f"{B}/objdiff/base/{unit}.obj" else obj
                        for obj in base_objects]
    # The BASE archives, their members in the image's retail order. VC6 LIB
    # prepends each input member, so feed them backwards.
    cuts = [index for index, obj in enumerate(base_objects)
            if obj.removeprefix(f"{B}/objdiff/base/").removesuffix(".obj")
            in IMAGE_ARCHIVE_CUTS[image.key]]
    bounds = [0, *cuts, len(base_objects)]
    names = (["BASE"] if len(bounds) == 2 else
             ["BASE-prefix", *(f"BASE-{index}" for index in range(2, len(bounds) - 2)),
              "BASE-suffix"])
    base_libraries = []
    for name, start, end in zip(names, bounds, bounds[1:]):
        library = f"{link_root}/{name}.lib"
        w.build(library, "archive", inputs=list(reversed(base_objects[start:end])))
        base_libraries.append(library)
    generated = {library for library, *_ in IMAGE_IMPORT_LIBRARIES[image.key]}
    libraries = [f"{link_root}/{name}" if name in generated else name
                 for name in IMAGE_LINK_LIBRARIES[image.key]]
    # As in the game's link (homm2.graph.link.final_inputs): the runtime's
    # default libraries are named explicitly, OLDNAMES searched first. Its
    # members' empty .text sections (default 16-byte alignment) are the fill
    # before the first import thunk; MSVCPRT precedes LIBCMT.
    link_args = (["/NODEFAULTLIB:LIBCMT", "/NODEFAULTLIB:LIBCPMT", "/NODEFAULTLIB:OLDNAMES",
                  *source_objects, "OLDNAMES.LIB", *libraries, *base_libraries,
                  "MSVCPRT.LIB", "LIBCMT.LIB", resource_output])
    for mode in ("generic", "rsrc", "historical"):
        outputs = [f"{link_root}/{mode}/{profile.exe}", f"{link_root}/{mode}/{stem}.map"]
        w.build(outputs, "link_exe",
                inputs=(source_objects + base_libraries
                        + ([resource_output] if mode != "generic" else [])),
                implicit=(import_outputs + [
                    f"{B}/build.ninja",  # The driver reads link_args from this graph.
                    "scripts/homm2/graph/link.py",
                    "build/toolchain/msvc/bin/LINK.EXE",
                    "build/toolchain/msvc/lib/LIBCMT.LIB",
                    "build/toolchain/msvc/lib/MSVCPRT.LIB",
                ]),
                variables={"link_args": " ".join(link_args),
                           "link_mode": "--" + mode if mode != "generic" else ""})
    stamp = f"{link_root}/historical/{stem}.link-diff.tsv"
    from homm2.core.paths import REPO
    ceiling = f"{image.retail}/link_diff.tsv"
    w.build(stamp, "link_diff", inputs=f"{link_root}/historical/{profile.exe}",
            implicit=["scripts/homm2/verify/link_diff.py", image.exe]
            + ([ceiling] if (REPO / ceiling).exists() else []))
    w.build("link-diff", "phony", inputs=stamp)
    w.build("link", "phony", inputs=f"{link_root}/generic/{profile.exe}")
    w.build("link-rsrc", "phony", inputs=f"{link_root}/rsrc/{profile.exe}")
    w.build("link-historical", "phony", inputs=f"{link_root}/historical/{profile.exe}")
    w.build("link-imports", "phony", inputs=import_outputs)
    w.build("link-resources", "phony", inputs=resource_output)
    return stamp
