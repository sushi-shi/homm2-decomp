#!/usr/bin/env python3
"""Link the VC6 candidate with reviewed retail contribution topology."""

import argparse
from collections import Counter
import re
import struct
import subprocess
import sys
from pathlib import Path

from .crt_order import (
    archive_names,
    ninja_link_args,
    selected_members,
)
from .adapt_crt_data import adapt as adapt_crt_data_order
from .adapt_crt_rdata import adapt as adapt_crt_rdata_order


ROOT = next(path for path in Path(__file__).resolve().parents
            if (path / "flake.nix").exists())
ARCHIVE = ROOT / "build/toolchain/msvc/lib/LIBCMT.LIB"
OLDNAMES_ARCHIVE = ROOT / "build/toolchain/msvc/lib/OLDNAMES.LIB"
OUT_ROOT = ROOT / "build/link/split-crt-archives"
CTYPE_GUARD = "??_B?1???id@?$ctype@G@std@@$D@@9@51"
CRT_BSS_ORDER = (
    "fpinit.obj", "crt0dat.obj", "strerror.obj", "wincrt0.obj", "mlock.obj",
    "setlocal.obj", "nlsdata2.obj", "a_map.obj", "sbheap.obj", "stdargv.obj",
    "a_env.obj", "crt0msg.obj", "txtmode.obj", "_newmode.obj", "handler.obj",
    "_file.obj", "_sftbuf.obj", "unhandld.obj", "inittime.obj", "initnum.obj",
    "initmon.obj", "initctyp.obj", "nlsdata3.obj", "getqloc.obj", "a_str.obj",
    "mbctype.obj", "crtmbox.obj", "ncommode.obj", "winsig.obj", "inithelp.obj",
    "lconv.obj", "w_str.obj", "tzset.obj", "w_loc.obj", "a_loc.obj",
    "a_cmp.obj",
)


def undefine_common_symbol(payload, wanted):
    """Turn duplicate COMMON definitions into ordinary undefined references."""
    data = bytearray(payload)
    symbol_table = struct.unpack_from("<I", data, 8)[0]
    symbol_count = struct.unpack_from("<I", data, 12)[0]
    string_table = symbol_table + symbol_count * 18
    changed = 0
    index = 0
    while index < symbol_count:
        offset = symbol_table + index * 18
        raw_name = data[offset:offset + 8]
        if raw_name[:4] == b"\0\0\0\0":
            name_offset = struct.unpack_from("<I", raw_name, 4)[0]
            start = string_table + name_offset
            end = data.index(0, start)
            name = bytes(data[start:end]).decode("ascii")
        else:
            name = bytes(raw_name.rstrip(b"\0")).decode("ascii")
        value = struct.unpack_from("<I", data, offset + 8)[0]
        section = struct.unpack_from("<h", data, offset + 12)[0]
        storage = data[offset + 16]
        if name == wanted and section == 0 and value and storage == 2:
            struct.pack_into("<I", data, offset + 8, 0)
            changed += 1
        index += 1 + data[offset + 17]
    return bytes(data), changed


def bss_layout_base_libraries(undefine_guard=False, misc_data_layout=False):
    output_root = ROOT / "build/link/bss-layout-all/lib"
    output_root.mkdir(parents=True, exist_ok=True)
    results = []
    for library_name in ("BASE-prefix.lib", "Midi.lib", "BASE-suffix.lib"):
        original_library = ROOT / "build/link" / library_name
        members = subprocess.check_output(
            ["llvm-ar", "t", original_library], text=True
        ).splitlines()
        inputs = []
        for member in members:
            selected = ROOT / member
            marker = "BASE/"
            if marker in member:
                unit_relative = member[member.index(marker):]
                adapted = ROOT / "build/link/bss-layout-all" / unit_relative
                if adapted.exists() and unit_relative not in {
                    "BASE/BITS.obj", "BASE/TILE.obj"
                }:
                    selected = adapted
                    if unit_relative == "BASE/AudiereEffects.obj":
                        combined = (
                            ROOT / "build/link/bss-layout-all-comdat"
                            / unit_relative
                        )
                        combined.parent.mkdir(parents=True, exist_ok=True)
                        subprocess.run(
                            [
                                sys.executable, "-m",
                                "homm2.build.adapt_comdat_link_order",
                                "--input", selected.relative_to(ROOT).as_posix(),
                                "--output", combined.relative_to(ROOT).as_posix(),
                                "--unit", "BASE/AudiereEffects",
                            ],
                            cwd=ROOT,
                            check=True,
                        )
                        selected = combined
                if unit_relative == "BASE/Misc.obj" and misc_data_layout:
                    combined = ROOT / "build/link/misc-data-layout/BASE/Misc.obj"
                    subprocess.run(
                        [
                            sys.executable, "-m",
                            "homm2.build.exact_link.adapt_misc_data",
                            selected.relative_to(ROOT).as_posix(),
                            combined.relative_to(ROOT).as_posix(),
                        ],
                        cwd=ROOT,
                        check=True,
                    )
                    selected = combined
            if undefine_guard and unit_relative not in {
                "BASE/BITS.obj", "BASE/TILE.obj"
            }:
                payload, changed = undefine_common_symbol(
                    selected.read_bytes(), CTYPE_GUARD
                )
                if changed:
                    common_undef = (
                        ROOT / "build/link/common-undef/BASE"
                        / selected.name
                    )
                    common_undef.parent.mkdir(parents=True, exist_ok=True)
                    common_undef.write_bytes(payload)
                    selected = common_undef
            inputs.append(selected.relative_to(ROOT).as_posix())

        output = output_root / library_name
        output.unlink(missing_ok=True)
        subprocess.run(
            [
                "wine", str(ROOT / "build/toolchain/msvc/bin/LIB.EXE"),
                "/NOLOGO", "/OUT:" + output.relative_to(ROOT).as_posix(),
                *reversed(inputs),
            ],
            cwd=ROOT,
            check=True,
        )
        results.append(output.relative_to(ROOT).as_posix())
    return results


def sort_text_after_import_thunks(payload):
    """Rename a single ordinary .text contribution to the .text$Z tail."""
    data = bytearray(payload)
    section_count = struct.unpack_from("<H", data, 2)[0]
    optional_size = struct.unpack_from("<H", data, 16)[0]
    section_table = 20 + optional_size
    renamed = 0
    for index in range(section_count):
        offset = section_table + index * 40
        if data[offset:offset + 8].rstrip(b"\0") != b".text":
            continue
        data[offset:offset + 8] = b".text$Z\0"
        renamed += 1
    if renamed != 1:
        raise RuntimeError(f"expected one .text section, found {renamed}")
    return bytes(data)


def adapt_crt_bss_order(member, payload):
    """Give each retail-placed CRT BSS contribution an independent sort key."""
    if member not in CRT_BSS_ORDER:
        return payload
    data = bytearray(payload)
    section_count = struct.unpack_from("<H", data, 2)[0]
    optional_size = struct.unpack_from("<H", data, 16)[0]
    section_table = 20 + optional_size
    renamed = 0
    suffix = CRT_BSS_ORDER.index(member)
    for index in range(section_count):
        offset = section_table + index * 40
        if data[offset:offset + 8].rstrip(b"\0") != b".bss":
            continue
        data[offset:offset + 8] = f".bss${suffix:02x}".encode("ascii").ljust(8, b"\0")
        renamed += 1
    if renamed != 1:
        raise RuntimeError(f"{member}: expected one .bss section, found {renamed}")
    return bytes(data)


def unique_force_symbols(objects):
    """Choose one archive-indexed, non-COMMON definition per selected member."""
    definitions = {}
    counts = Counter()
    pattern = re.compile(r"\s*(?:[0-9a-fA-F]+)?\s*([A-Z])\s+(\S+)$")
    for path in objects:
        output = subprocess.check_output(
            ["llvm-nm", path], text=True, stderr=subprocess.DEVNULL
        )
        rows = []
        for line in output.splitlines():
            match = pattern.match(line)
            if not match or match.group(1) in {"C", "U"}:
                continue
            rows.append((match.group(1), match.group(2)))
            counts[match.group(2)] += 1
        definitions[path] = rows
    result = []
    for path in objects:
        rows = [row for row in definitions[path] if counts[row[1]] == 1]
        if not rows:
            raise RuntimeError("no unique force symbol for " + str(path))
        rows.sort(key=lambda row: ({"T": 0, "D": 1, "R": 2, "B": 3}.get(row[0], 4), row[1]))
        result.append(rows[0][1])
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--imports-after-crt", action="store_true")
    parser.add_argument("--duplicate-imports-after-crt", action="store_true")
    parser.add_argument("--tail-after-imports", action="store_true")
    parser.add_argument("--icf", action="store_true")
    parser.add_argument("--include-fptrap", action="store_true")
    parser.add_argument("--split-oldnames-tail", action="store_true")
    parser.add_argument("--tail-section-z", action="store_true")
    parser.add_argument("--advmgr-bss-layout", action="store_true")
    parser.add_argument("--source-bss-layout", action="store_true")
    parser.add_argument("--all-bss-layout", action="store_true")
    parser.add_argument("--common-order", action="store_true")
    parser.add_argument("--misc-data-layout", action="store_true")
    parser.add_argument("--crt-data-order", action="store_true")
    parser.add_argument("--crt-rdata-order", action="store_true")
    parser.add_argument("--crt-bss-order", action="store_true")
    parser.add_argument(
        "--retail-exact", action="store_true",
        help="run the complete reviewed retail-exact link and normalization pipeline",
    )
    options = parser.parse_args()
    if options.retail_exact:
        options.include_fptrap = True
        options.tail_section_z = True
        options.all_bss_layout = True
        options.common_order = True
        options.misc_data_layout = True
        options.crt_data_order = True
        options.crt_rdata_order = True
        options.crt_bss_order = True
    if sum((options.imports_after_crt, options.duplicate_imports_after_crt,
            options.tail_after_imports)) > 1:
        parser.error("choose only one import-order experiment")
    if sum((options.advmgr_bss_layout, options.source_bss_layout,
            options.all_bss_layout)) > 1:
        parser.error("choose only one BSS-layout experiment")
    ordered = selected_members()
    archive = archive_names()

    if options.retail_exact:
        for command in (
            [sys.executable, "-m", "homm2.build.exact_link.batch_bss",
             "--owner", "SOURCE"],
            [sys.executable, "-m", "homm2.build.exact_link.batch_bss",
             "--owner", "BASE", "--previous-end", "0x1348a1"],
        ):
            subprocess.run(command, cwd=ROOT, check=True)

    archive_root = OUT_ROOT / "lib"
    object_root = OUT_ROOT / "obj"
    archive_root.mkdir(parents=True, exist_ok=True)
    object_root.mkdir(parents=True, exist_ok=True)
    libraries = []
    selected_objects = []
    delete_matches = archive["delete.obj"]
    if len(delete_matches) != 1:
        raise RuntimeError("ambiguous selected archive basename: delete.obj")
    delete_object = object_root / "delete.obj"
    delete_object.write_bytes(subprocess.check_output(
        ["llvm-ar", "p", ARCHIVE, delete_matches[0]]
    ))
    delete_library = archive_root / "delete.lib"
    delete_library.unlink(missing_ok=True)
    subprocess.run(
        [
            "llvm-ar", "rcs", delete_library,
            delete_object.relative_to(ROOT).as_posix(),
        ],
        check=True,
        cwd=ROOT,
    )
    for index, member in enumerate(ordered):
        matches = archive[member.lower()]
        if len(matches) != 1:
            raise RuntimeError("ambiguous selected archive basename: " + member)
        payload = subprocess.check_output(["llvm-ar", "p", ARCHIVE, matches[0]])
        if options.tail_section_z and member.lower() in {
            "tell.obj", "access.obj", "strrev.obj"
        }:
            payload = sort_text_after_import_thunks(payload)
        if options.crt_data_order:
            payload = adapt_crt_data_order(
                re.split(r"[\\/]", member)[-1].lower(), payload
            )
        if options.crt_rdata_order:
            payload = adapt_crt_rdata_order(index, len(ordered), payload)
        if options.crt_bss_order:
            payload = adapt_crt_bss_order(
                re.split(r"[\\/]", member)[-1].lower(), payload
            )
        object_path = object_root / re.split(r"[\\/]", member)[-1].lower()
        object_path.write_bytes(payload)
        selected_objects.append(object_path)
        library_path = archive_root / f"{index:03d}-{object_path.stem}.lib"
        library_path.unlink(missing_ok=True)
        subprocess.run(
            ["llvm-ar", "rcs", library_path, object_path], check=True, cwd=ROOT
        )
        libraries.append(library_path.relative_to(ROOT).as_posix())

    oldnames_output = subprocess.check_output(
        ["llvm-ar", "t", OLDNAMES_ARCHIVE], text=True
    )
    oldnames_by_base = {
        re.split(r"[\\/]", name)[-1].lower(): name
        for name in oldnames_output.splitlines()
    }
    early_alias_names = [
        "strcmpi.obj", "write.obj", "read.obj", "close.obj",
        "open.obj", "lseek.obj", "strnicmp.obj", "chdir.obj",
    ]
    tail_alias_names = ["tell.obj", "access.obj", "strrev.obj"]
    oldnames_root = OUT_ROOT / "oldnames"
    oldnames_root.mkdir(parents=True, exist_ok=True)
    alias_libraries = {}
    for index, member in enumerate(early_alias_names + tail_alias_names):
        payload = subprocess.check_output(
            ["llvm-ar", "p", OLDNAMES_ARCHIVE, oldnames_by_base[member]]
        )
        object_path = oldnames_root / member
        object_path.write_bytes(payload)
        library_path = oldnames_root / f"{index:02d}-{Path(member).stem}.lib"
        library_path.unlink(missing_ok=True)
        subprocess.run(
            [
                "wine", str(ROOT / "build/toolchain/msvc/bin/LIB.EXE"),
                "/NOLOGO", "/OUT:" + library_path.relative_to(ROOT).as_posix(),
                object_path.relative_to(ROOT).as_posix(),
            ],
            check=True,
            cwd=ROOT,
        )
        alias_libraries[member] = library_path.relative_to(ROOT).as_posix()

    args = ninja_link_args()
    delete_scan = args.index("LIBCMT.LIB")
    args[delete_scan] = delete_library.relative_to(ROOT).as_posix()
    common_order_object = None
    if options.common_order:
        command = [sys.executable, "-m", "homm2.build.exact_link.common_order"]
        if options.crt_bss_order:
            command.append("--defined-section")
        subprocess.run(command, cwd=ROOT, check=True)
        common_order_object = "build/link/common-order.obj"
    if options.advmgr_bss_layout:
        original = "build/objdiff/base/SOURCE/ADVMGR.obj"
        adapted = "build/link/bss-layout/SOURCE/ADVMGR.obj"
        args[args.index(original)] = adapted
    elif options.source_bss_layout or options.all_bss_layout:
        for index, path in enumerate(args):
            prefix = "build/objdiff/base/SOURCE/"
            if not path.startswith(prefix) or not path.endswith(".obj"):
                continue
            adapted = "build/link/bss-layout-all/SOURCE/" + path[len(prefix):]
            if (ROOT / adapted).exists():
                args[index] = adapted
    if options.all_bss_layout:
        adapted_libraries = bss_layout_base_libraries(
            undefine_guard=options.common_order,
            misc_data_layout=options.misc_data_layout,
        )
        for original, adapted in zip(
            ("build/link/BASE-prefix.lib", "build/link/Midi.lib",
             "build/link/BASE-suffix.lib"),
            adapted_libraries,
        ):
            args[args.index(original)] = adapted
    resource = "build/link/HMM2PL.res"
    args.remove(resource)
    first_source_path = (
        "build/link/bss-layout/SOURCE/ADVMGR.obj"
        if options.advmgr_bss_layout
        else "build/objdiff/base/SOURCE/ADVMGR.obj"
    )
    if options.source_bss_layout or options.all_bss_layout:
        first_source_path = "build/link/bss-layout-all/SOURCE/ADVMGR.obj"
    first_source = args.index(first_source_path)
    import_libraries = args[:first_source]
    if options.common_order:
        for index in range(first_source, len(args)):
            path = args[index]
            marker = next(
                (value for value in ("SOURCE/", "EDITOR/") if value in path),
                None,
            )
            if marker is None or not path.endswith(".obj"):
                continue
            source = ROOT / path
            payload, changed = undefine_common_symbol(
                source.read_bytes(), CTYPE_GUARD
            )
            if not changed:
                continue
            relative = Path(path[path.index(marker):])
            output_path = ROOT / "build/link/common-undef" / relative
            output_path.parent.mkdir(parents=True, exist_ok=True)
            output_path.write_bytes(payload)
            args[index] = output_path.relative_to(ROOT).as_posix()
    if options.imports_after_crt:
        args = args[first_source:]
    nodefault = [
        "/NODEFAULTLIB:LIBCMT",
        "/NODEFAULTLIB:LIBCPMT",
        "/NODEFAULTLIB:OLDNAMES",
    ]
    if options.imports_after_crt:
        args = nodefault + args + libraries + import_libraries + ["OLDNAMES.LIB"]
    elif options.duplicate_imports_after_crt:
        args = nodefault + args + libraries + import_libraries + ["OLDNAMES.LIB"]
    elif options.tail_after_imports:
        tail_stems = ("-tell.lib", "-access.lib", "-strrev.lib")
        tail_libraries = [
            library for library in libraries if library.endswith(tail_stems)
        ]
        early_libraries = [
            library for library in libraries if library not in tail_libraries
        ]
        if len(tail_libraries) != 3:
            raise RuntimeError("expected tell/access/strrev CRT tail")
        args = (nodefault + args + ["OLDNAMES.LIB"] + early_libraries
                + import_libraries + tail_libraries)
    elif options.split_oldnames_tail:
        tail_stems = ("-tell.lib", "-access.lib", "-strrev.lib")
        tail_libraries = [
            library for library in libraries if library.endswith(tail_stems)
        ]
        early_libraries = [
            library for library in libraries if library not in tail_libraries
        ]
        args = (
            nodefault + args
            + [alias_libraries[name] for name in early_alias_names]
            + early_libraries
            + [alias_libraries[name] for name in tail_alias_names]
            + import_libraries + tail_libraries
        )
    else:
        args = nodefault + args + ["OLDNAMES.LIB"] + libraries
    if common_order_object:
        args.append(common_order_object)
    args += [resource]

    link_root = OUT_ROOT
    if options.imports_after_crt:
        link_root = ROOT / "build/link/split-crt-imports-after"
    elif options.duplicate_imports_after_crt:
        link_root = ROOT / "build/link/split-crt-double-imports"
    elif options.tail_after_imports:
        link_root = ROOT / "build/link/split-crt-tail-after-imports"
    if options.icf:
        link_root = link_root.with_name(link_root.name + "-icf")
    if options.include_fptrap:
        link_root = link_root.with_name(link_root.name + "-fptrap")
    if options.split_oldnames_tail:
        link_root = link_root.with_name(link_root.name + "-split-oldnames")
    if options.tail_section_z:
        link_root = link_root.with_name(link_root.name + "-tail-z")
    if options.advmgr_bss_layout:
        link_root = link_root.with_name(link_root.name + "-advmgr-bss")
    if options.source_bss_layout:
        link_root = link_root.with_name(link_root.name + "-source-bss")
    if options.all_bss_layout:
        link_root = link_root.with_name(link_root.name + "-all-bss")
    if options.common_order:
        link_root = link_root.with_name(link_root.name + "-common-order")
    if options.misc_data_layout:
        link_root = link_root.with_name(link_root.name + "-misc-data")
    if options.crt_data_order:
        link_root = link_root.with_name(link_root.name + "-crt-data")
    if options.crt_rdata_order:
        link_root = link_root.with_name(link_root.name + "-crt-rdata")
    if options.crt_bss_order:
        link_root = link_root.with_name(link_root.name + "-crt-bss")
    if options.retail_exact:
        link_root = ROOT / "build/link"
    link_root.mkdir(parents=True, exist_ok=True)
    linked_name = "HMM2PL.raw.exe" if options.retail_exact else "HMM2PL.exe"
    output = (link_root / linked_name).relative_to(ROOT).as_posix()
    map_path = (link_root / "HMM2PL.map").relative_to(ROOT).as_posix()
    rsp = link_root / "HMM2PL.rsp"
    prefix = [
        "/NOLOGO", "/MACHINE:IX86", "/BASE:0x400000",
        "/SUBSYSTEM:WINDOWS,4.0", "/STACK:66112,4096",
        "/HEAP:1048576,4096", "/INCREMENTAL:NO", "/OPT:NOREF",
        "/DEBUG", "/PDB:" + (link_root / "HMM2PL.pdb").relative_to(ROOT).as_posix(),
        "/LIBPATH:build/toolchain/msvc/lib", "/MAP:" + map_path,
        "/OUT:" + output,
    ]
    if options.icf:
        prefix.append("/OPT:ICF")
    if options.include_fptrap:
        prefix.append("/INCLUDE:__fptrap")
    if options.common_order:
        prefix.extend(
            "/INCLUDE:" + symbol
            for symbol in unique_force_symbols(selected_objects)
        )
    rsp.write_text(" ".join(prefix + args) + "\n")
    subprocess.run(
        ["wine", str(ROOT / "build/toolchain/msvc/bin/LINK.EXE"), "@" + str(rsp)],
        cwd=ROOT,
        check=True,
    )
    if options.retail_exact:
        imported = link_root / "HMM2PL.imports.exe"
        text = link_root / "HMM2PL.text.exe"
        final = link_root / "HMM2PL.exe"
        for command in (
            [sys.executable, "-m", "homm2.build.exact_link.normalize_imports",
             str(ROOT / output), str(imported)],
            [sys.executable, "-m", "homm2.build.exact_link.normalize_text",
             str(imported), str(ROOT / map_path), str(text)],
            [sys.executable, "-m", "homm2.build.exact_link.normalize_pe",
             str(text), str(final)],
        ):
            subprocess.run(command, cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
