#!/usr/bin/env python3
"""Build the selected VC6 CRT members in reviewed retail link order."""

import csv
import re
import shlex
import subprocess
from pathlib import Path


ROOT = next(path for path in Path(__file__).resolve().parents
            if (path / "flake.nix").exists())
ARCHIVE = ROOT / "build/toolchain/msvc/lib/LIBCMT.LIB"
OUT_ROOT = ROOT / "build/link/ordered-crt"


def ninja_link_args():
    text = (ROOT / "build.ninja").read_text()
    block = text[text.index("build build/link/HMM2PL.exe "):]
    block = block[:block.index("\nbuild ", 1)]
    value = block[block.index("  link_args = ") + len("  link_args = "):]
    value = value.replace("$\n", " ")
    return shlex.split(value)


def selected_members():
    members = [
        line.strip()
        for line in (ROOT / "config/retail_crt_order.txt").read_text().splitlines()
        if line.strip() and not line.startswith("#")
    ]
    if len(members) != 177 or len({member.lower() for member in members}) != 177:
        raise RuntimeError("expected 177 unique selected LIBCMT members")
    return members


def retail_ranks():
    with (ROOT / "config/crt_functions.csv").open(newline="") as stream:
        rows = csv.DictReader(line for line in stream if not line.startswith("#"))
        rank = {}
        for row in rows:
            member = row["member"].lower()
            if member == "(unresolved)":
                continue
            rank.setdefault(member, int(row["entry_rva"], 16))
    # The linked Windows startup member is wincrt0.obj; the census identifies
    # its retail body through the byte-identical crt0.obj candidate.
    rank["wincrt0.obj"] = rank["crt0.obj"]
    return rank


def candidate_code_ranks():
    text = (ROOT / "build/link/HMM2PL.map").read_text(errors="replace")
    ranks = {}
    pattern = re.compile(
        r"^ 0001:[0-9a-f]{8}\s+\S+\s+00([0-9a-f]{6}) f\s+"
        r"LIBCMT:([^\s]+\.obj)\s*$",
        re.MULTILINE | re.IGNORECASE,
    )
    for rva, member in pattern.findall(text):
        ranks.setdefault(member.lower(), int(rva, 16))
    return ranks


def archive_names():
    output = subprocess.check_output(["llvm-ar", "t", ARCHIVE], text=True)
    by_base = {}
    for name in output.splitlines():
        base = re.split(r"[\\/]", name)[-1].lower()
        by_base.setdefault(base, []).append(name)
    return by_base


def extract(member, archive_member):
    destination = OUT_ROOT / "obj" / member.lower()
    destination.parent.mkdir(parents=True, exist_ok=True)
    payload = subprocess.check_output(["llvm-ar", "p", ARCHIVE, archive_member])
    destination.write_bytes(payload)
    return destination.relative_to(ROOT).as_posix()


def main():
    current = selected_members()
    retail = retail_ranks()
    candidate = candidate_code_ranks()
    archive = archive_names()

    # Retail evidence orders every known code-bearing member.  For census gaps,
    # retain the current candidate code position; data-only support members stay
    # in their current relative order after the code members.
    current_index = {member.lower(): index for index, member in enumerate(current)}
    ordered = sorted(
        current,
        key=lambda member: (
            retail.get(
                member.lower(),
                candidate.get(member.lower(), 0x1000000 + current_index[member.lower()]),
            ),
            current_index[member.lower()],
        ),
    )
    delete_matches = archive["delete.obj"]
    if len(delete_matches) != 1:
        raise RuntimeError("ambiguous selected archive basename: delete.obj")
    objects = [extract("delete.obj", delete_matches[0])]
    for member in ordered:
        matches = archive[member.lower()]
        if len(matches) != 1:
            raise RuntimeError("ambiguous selected archive basename: " + member)
        objects.append(extract(member, matches[0]))

    archive_path = OUT_ROOT / "ordered-crt.lib"
    archive_rsp = OUT_ROOT / "ordered-crt.lib.rsp"
    archive_rsp.write_text(
        "/NOLOGO /MACHINE:IX86 /OUT:" + archive_path.relative_to(ROOT).as_posix()
        + " " + " ".join(reversed(objects)) + "\n"
    )
    subprocess.run(
        ["wine", str(ROOT / "build/toolchain/msvc/bin/LIB.EXE"), "@" + str(archive_rsp)],
        cwd=ROOT,
        check=True,
    )

    args = ninja_link_args()
    delete_archive = "LIBCMT.LIB"
    resource = "build/link/HMM2PL.res"
    if delete_archive not in args or args[-1] != resource:
        raise RuntimeError("unexpected normal-link argument tail")
    insert_at = args.index(delete_archive)
    args[insert_at:insert_at + 1] = [archive_path.relative_to(ROOT).as_posix()]
    args.insert(0, "/NODEFAULTLIB:LIBCPMT")
    args.insert(0, "/NODEFAULTLIB:LIBCMT")

    output = "build/link/ordered-crt/HMM2PL.exe"
    map_path = "build/link/ordered-crt/HMM2PL.map"
    rsp_path = OUT_ROOT / "HMM2PL.rsp"
    prefix = [
        "/NOLOGO", "/MACHINE:IX86", "/BASE:0x400000",
        "/SUBSYSTEM:WINDOWS,4.0", "/STACK:66112,4096",
        "/HEAP:1048576,4096", "/INCREMENTAL:NO", "/OPT:NOREF",
        "/DEBUG", "/PDB:build/link/ordered-crt/HMM2PL.pdb",
        "/LIBPATH:build/toolchain/msvc/lib", "/MAP:" + map_path,
        "/OUT:" + output,
    ]
    rsp_path.write_text(" ".join(prefix + args) + "\n")
    print("ordered members:", len(ordered))
    print("retail-ranked:", sum(member.lower() in retail for member in ordered))
    print("candidate-ranked gaps:", sum(
        member.lower() not in retail and member.lower() in candidate for member in ordered
    ))
    subprocess.run(
        ["wine", str(ROOT / "build/toolchain/msvc/bin/LINK.EXE"), "@" + str(rsp_path)],
        cwd=ROOT,
        check=True,
    )


if __name__ == "__main__":
    main()
