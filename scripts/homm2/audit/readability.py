"""Source-reading inventory, separate from matching scores and retail boundaries.

Run with Universal Ctags 6 on PATH:
  python3 -m homm2.audit.readability --write
  python3 -m homm2.audit.readability --check

Ctags indexes physical definitions, including header bodies and inactive branches.
Project enum macros are expanded only for indexing; game files are never rewritten.
Every VA marker must resolve to exactly one definition. Unmarked/header bodies are
also indexed, but the file-by-file human pass remains the completeness backstop.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import io
import json
from pathlib import Path
import re
import subprocess

REPO = Path(__file__).resolve().parents[3]
REPORT = Path("docs/readability")
VA = re.compile(r"^\s*VA\(\s*(0x[0-9a-fA-F]+)\s*,[^\n]*\)\s*$", re.M)
PROC = re.compile(r"^(\w+)\s+PROC\b", re.M | re.I)
IGNORES = (
    "VA+,DATA+,VTBL+,VTBL2+,SIZE+,VA_COMPGEN+,DATA_COMPGEN_GUARD+,"
    "NEW_VA+,NEW_SIZE+,H2_RETAIL_INLINE,OVERRIDE,H2_CONST,H2_FINAL,"
    "requires+,__is_enum+,__is_integral+"
)
DEFINES = [
    "H2_UNUSED(n)=n",
    "H2_ENUM_BEGIN(n)=enum n {", "H2_ENUM_END(n)=};",
    "H2_ENUM_CLASS_BEGIN(n)=enum n {", "H2_ENUM_CLASS_END(n)=};",
    "H2_ENUM_CLASS_BEGIN_T(n,s)=enum n {", "H2_ENUM_CLASS_END_T(n,s)=};",
    "H2_ENUM_CLASS_BEGIN_SPLIT(n,s)=enum n {", "H2_ENUM_CLASS_END_SPLIT(n,s)=};",
    "H2_ENUM_CLASS_FORWARD(n)=enum n", "H2_ENUM_CLASS_FORWARD_SPLIT(n,s)=enum n",
    "H2_ENUM_FLAGS(n)=", "H2_ENUM_STEPPED(n)=", "H2_ENUM_INDEX_OFFSET(n)=",
]
DEFINES += [f"{name}(n,s)=n" for name in (
    "H2_ENUM_PARAM", "H2_ENUM_RETURN", "H2_ENUM_STORAGE", "H2_ENUM_STORAGE_STEPPED",
    "H2_OPEN_CODE_PARAM", "H2_OPEN_CODE_STORAGE", "H2_ENUM_BITFIELD",
)]


def digest(blob: bytes) -> str:
    return hashlib.sha256(blob).hexdigest()


def tracked_files(root: Path) -> list[str]:
    result = subprocess.run(
        ["git", "ls-files", "-z", "src", "include"], cwd=root,
        check=True, capture_output=True,
    )
    return sorted(p for p in result.stdout.decode().split("\0") if p)


def ctags_rows(root: Path, paths: list[str], executable: str) -> list[dict]:
    command = [executable, "--options=NONE", "--output-format=json", "--sort=no",
               "--fields=+neKSt", "--fields-C++=+{properties}",
               "--kinds-C++=fpd", "--language-force=C++", "--if0=yes", "-I", IGNORES]
    for definition in DEFINES:
        command.extend(("-D", definition))
    command += ["-o", "-"] + paths
    result = subprocess.run(command, cwd=root, check=True, capture_output=True, text=True)
    return [row for line in result.stdout.splitlines()
            if (row := json.loads(line)).get("_type") == "tag"]


def tag_rows(tags: list[dict], blobs: dict[str, bytes]) -> tuple[list[dict], list[dict]]:
    functions, macros = [], []
    seen = set()
    for tag in tags:
        path, start = tag["path"], tag["line"]
        if path not in blobs:
            raise ValueError(f"Ctags returned an out-of-scope path: {path}")
        lines = blobs[path].splitlines(keepends=True)
        end = tag.get("end", start)
        if not 1 <= start <= end <= len(lines):
            raise ValueError(f"Invalid definition extent: {path}:{start}-{end}")
        scope = re.sub(r"\b__anon[0-9a-f]+\b", "(anonymous)", tag.get("scope", ""))
        name = "::".join(filter(None, (scope, tag["name"])))
        kind = tag["kind"]
        key = (path, start, end, kind, name)
        if key in seen:
            continue
        seen.add(key)
        defaulted = (kind == "prototype" and
                     re.search(rb"=\s*(?:default|delete)\s*;", b"".join(lines[start - 1:end])))
        if kind == "function" or defaulted:
            if tag["name"] in {"i8", "i16", "i32", "u8", "u16", "u32", "void", "bool"}:
                raise ValueError(f"Type mistaken for function name: {key}")
            if tag["name"].startswith("H2_") and "H2_" in tag["name"] and tag["name"].isupper():
                raise ValueError(f"Unexpanded project macro mistaken for function: {key}")
            if kind == "function" and "end" not in tag:
                raise ValueError(f"Function has no closing extent: {key}")
            row = dict(path=path, line=start, end=end, name=name,
                       signature=tag.get("signature", ""),
                       kind="cpp-default-or-delete" if defaulted else "cpp",
                       properties=tag.get("properties", ""), va="",
                       body_sha256=digest(b"".join(lines[start - 1:end])))
            functions.append(row)
        elif kind == "macro":
            # Include object-like macros as well: some hide executable expressions.
            macros.append(dict(path=path, line=start, end=end, name=name,
                               body_sha256=digest(b"".join(lines[start - 1:end]))))
    return functions, macros


def assembly_rows(path: str, blob: bytes) -> list[dict]:
    text = blob.decode("utf-8")
    rows = []
    for match in PROC.finditer(text):
        name = match[1]
        tail = re.search(rf"^{re.escape(name)}\s+ENDP\b", text[match.end():], re.M | re.I)
        if tail is None:
            raise ValueError(f"Unterminated assembly function: {path}:{name}")
        start = text.count("\n", 0, match.start()) + 1
        end = text.count("\n", 0, match.end() + tail.end()) + 1
        rows.append(dict(path=path, line=start, end=end, name=name, signature="",
                         kind="asm", properties="", va="",
                         body_sha256=digest(b"".join(blob.splitlines(keepends=True)[start - 1:end]))))
    return rows


def attach_addresses(functions: list[dict], blobs: dict[str, bytes]) -> int:
    count = 0
    for path, blob in blobs.items():
        text = blob.decode("utf-8")
        markers = list(VA.finditer(text))
        for index, marker in enumerate(markers):
            first = text.count("\n", 0, marker.end()) + 1
            last = (text.count("\n", 0, markers[index + 1].start()) + 1
                    if index + 1 < len(markers) else len(text.splitlines()) + 1)
            candidates = [row for row in functions if row["path"] == path
                          and first <= row["line"] < last]
            # Unannotated helpers after an annotated body are legal; the immediate
            # next definition owns the marker, whose intervening text is checked.
            candidates.sort(key=lambda row: row["line"])
            if not candidates:
                raise ValueError(f"VA marker has no definition: {path}:{first}")
            row = candidates[0]
            gap = "\n".join(text.splitlines()[first:row["line"] - 1])
            if "{" in re.sub(r"//[^\n]*|/\*.*?\*/", "", gap, flags=re.S):
                raise ValueError(f"Ctags missed a body after VA marker: {path}:{first}")
            if row["va"]:
                raise ValueError(f"Multiple VA markers mapped to one body: {path}:{row['line']}")
            row["va"] = marker[1].lower()
            count += 1
    return count


def reviewed(path: str, sha256: str, reviews: dict) -> tuple[str, str]:
    entry = reviews.get(path)
    if not entry:
        return "unread", ""
    if not entry.get("note") or not entry.get("sha256"):
        raise ValueError(f"Review needs a content hash and substantive note: {path}")
    if entry["sha256"] != sha256:
        return "unread", "STALE: " + entry["note"]
    return "read", entry["note"]


def tsv(rows: list[dict], columns: list[str]) -> str:
    stream = io.StringIO()
    writer = csv.DictWriter(stream, columns, delimiter="\t", lineterminator="\n")
    writer.writeheader()
    writer.writerows({key: row.get(key, "") for key in columns} for row in rows)
    return stream.getvalue()


def generate(root: Path, executable: str) -> dict[str, str]:
    paths = tracked_files(root)
    unsupported = [path for path in paths if Path(path).suffix not in (".cpp", ".h", ".asm")]
    if unsupported:
        raise ValueError(f"Uninventoried source file types: {unsupported}")
    blobs = {path: (root / path).read_bytes() for path in paths}
    tags = ctags_rows(root, [p for p in paths if not p.endswith(".asm")], executable)
    functions, macros = tag_rows(tags, blobs)
    for path in paths:
        if path.endswith(".asm"):
            functions += assembly_rows(path, blobs[path])
    marker_count = attach_addresses(functions, blobs)
    review_path = root / REPORT / "reviews.json"
    reviews = json.loads(review_path.read_text()) if review_path.exists() else {}
    if set(reviews) - set(paths):
        raise ValueError(f"Reviews reference missing files: {sorted(set(reviews) - set(paths))}")
    files = []
    for path in paths:
        sha256 = digest(blobs[path])
        status, note = reviewed(path, sha256, reviews)
        note = note or "-"
        owned = [row for row in functions if row["path"] == path]
        owned_macros = [row for row in macros if row["path"] == path]
        for row in owned + owned_macros:
            row.update(status=status, review=note)
        files.append(dict(path=path, lines=len(blobs[path].splitlines()),
                          functions=len(owned), macros=len(owned_macros),
                          status=status, sha256=sha256, review=note))
    functions.sort(key=lambda row: (row["path"], row["line"], row["name"]))
    macros.sort(key=lambda row: (row["path"], row["line"], row["name"]))
    read_functions = sum(row["status"] == "read" for row in functions)
    read_files = sum(row["status"] == "read" for row in files)
    summary = (
        "# Reading progress\n\nGenerated by `homm2.audit.readability`; do not edit this file.\n\n"
        f"- Files read: {read_files} / {len(files)}\n"
        f"- Function definitions read: {read_functions} / {len(functions)}\n"
        f"- VA-annotated definitions indexed: {marker_count}\n"
        f"- Macro definitions indexed (including guards/conditional variants): {len(macros)}\n\n"
        "Reading is human-reviewed, not inferred from searches or parser success. A file\n"
        "change invalidates its read status. Declarations without bodies are covered by\n"
        "the file checklist; compiler-generated bodies and vendor code are not invented\n"
        "as source functions. The deliberate pass must reconcile each file's index.\n"
    )
    return {
        "functions.tsv": tsv(functions, ["path", "line", "end", "name", "signature", "kind",
                                         "properties", "va", "status", "body_sha256", "review"]),
        "macros.tsv": tsv(macros, ["path", "line", "end", "name", "status", "body_sha256", "review"]),
        "files.tsv": tsv(files, ["path", "lines", "functions", "macros", "status", "sha256", "review"]),
        "progress.md": summary,
    }


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=REPO)
    parser.add_argument("--ctags", default="ctags")
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--write", action="store_true")
    mode.add_argument("--check", action="store_true")
    args = parser.parse_args(argv)
    root = args.root.resolve()
    products = generate(root, args.ctags)
    stale = []
    for name, content in products.items():
        target = root / REPORT / name
        if args.write:
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(content)
        elif not target.exists() or target.read_text() != content:
            stale.append(str(target.relative_to(root)))
    if stale:
        print("Stale or missing reading inventory: " + ", ".join(stale))
        return 1
    print(products["progress.md"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
