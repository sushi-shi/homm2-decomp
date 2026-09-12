"""Per-function reconstruction-pattern review inventory.

Run with Universal Ctags 6 on PATH:
  python3 -m homm2.audit.reconstruction --write
  python3 -m homm2.audit.reconstruction --check

Ctags indexes physical definitions, including header bodies and inactive branches.
An independent preprocessor pass covers macros in branches the C++ parser skips.
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
REPORT = Path("docs/reconstruction")
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
               "--kinds-C++=fp", "--language-force=C++", "--if0=yes", "-I", IGNORES]
    for definition in DEFINES:
        command.extend(("-D", definition))
    command += ["-o", "-"] + paths
    # The C++ parser can suppress an #else macro after an inline body even with
    # --if0=yes (NextCreatureType in KB_TYPES.h). Index physical macro definitions
    # independently, without the enum expansions used only to expose C++ bodies.
    macro_command = [executable, "--options=NONE", "--output-format=json", "--sort=no",
                     "--fields=+neKSt", "--kinds-CPreProcessor=d",
                     "--language-force=CPreProcessor", "--if0=yes", "-o", "-"] + paths
    rows = []
    for invocation in (command, macro_command):
        result = subprocess.run(invocation, cwd=root, check=True,
                                capture_output=True, text=True)
        rows.extend(row for line in result.stdout.splitlines()
                    if (row := json.loads(line)).get("_type") == "tag")
    return rows


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


PATTERNS = ("casts", "unions", "gotos", "address", "owner", "outside",
            "varargs", "reuse", "stack", "bounds")
VERDICTS = {"absent", "retain", "applied", "open"}


def inventory(root: Path, executable: str) -> tuple[list, list, dict]:
    paths = tracked_files(root)
    unsupported = [p for p in paths if Path(p).suffix not in (".cpp", ".h", ".asm")]
    if unsupported:
        raise ValueError(f"Uninventoried source file types: {unsupported}")
    blobs = {p: (root / p).read_bytes() for p in paths}
    tags = ctags_rows(root, [p for p in paths if not p.endswith(".asm")], executable)
    functions, macros = tag_rows(tags, blobs)
    for path in paths:
        if path.endswith(".asm"):
            functions += assembly_rows(path, blobs[path])
    attach_addresses(functions, blobs)
    functions.sort(key=lambda r: (r["path"], r["line"], r["name"]))
    # Occurrence distinguishes physical definitions in conditional branches.
    counts = {}
    for row in functions:
        base = row["path"] + ":" + row["name"] + row["signature"]
        counts[base] = counts.get(base, 0) + 1
        row["id"] = base + "#" + str(counts[base])
    return functions, macros, blobs


def assess(row: dict, reviews: dict) -> dict:
    result = dict(row, status="pending", note="Not yet reviewed.")
    result.update({p: "pending" for p in PATTERNS})
    review = reviews.get(row["id"])
    if review is None:
        return result
    if review.get("body_sha256") != row["body_sha256"]:
        result["note"] = "STALE: " + review.get("note", "")
        return result
    verdicts = review.get("patterns", {})
    if set(verdicts) != set(PATTERNS) or not set(verdicts.values()) <= VERDICTS:
        raise ValueError(f"Incomplete pattern review: {row['id']}")
    if not review.get("note"):
        raise ValueError(f"Review needs a substantive note: {row['id']}")
    result.update(verdicts, status="reviewed", note=review["note"])
    return result


def tsv(rows: list[dict], columns: list[str]) -> str:
    stream = io.StringIO()
    writer = csv.DictWriter(stream, columns, delimiter="\t", lineterminator="\n")
    writer.writeheader()
    writer.writerows({k: r.get(k, "") for k in columns} for r in rows)
    return stream.getvalue()


def generate(root: Path, executable: str) -> dict[str, str]:
    functions, macros, blobs = inventory(root, executable)
    review_path = root / REPORT / "reviews.json"
    reviews = json.loads(review_path.read_text()) if review_path.exists() else {}
    missing = set(reviews) - {r["id"] for r in functions}
    if missing:
        raise ValueError(f"Reviews reference missing functions: {sorted(missing)}")
    rows = [assess(r, reviews) for r in functions]
    files = []
    file_review_path = root / REPORT / "file-reviews.json"
    file_reviews = json.loads(file_review_path.read_text()) if file_review_path.exists() else {}
    if set(file_reviews) - set(blobs):
        raise ValueError("File reviews reference missing source paths")
    for path, blob in blobs.items():
        owned = [r for r in rows if r["path"] == path]
        review = file_reviews.get(path, {})
        status = ("reviewed" if review.get("sha256") == digest(blob) and review.get("note")
                  else "pending")
        files.append(dict(path=path, sha256=digest(blob), functions=len(owned),
                          reviewed=sum(r["status"] == "reviewed" for r in owned),
                          status=status, note=review.get("note", "Not yet reviewed.")))
    reviewed = sum(r["status"] == "reviewed" for r in rows)
    summary = (
        "# Reconstruction-pattern review progress\n\n"
        "Generated by `python3 -m homm2.audit.reconstruction --write`.\n\n"
        f"- Function definitions reviewed: {reviewed} / {len(rows)}\n"
        f"- VA-annotated definitions indexed: {sum(bool(r['va']) for r in rows)}\n"
        f"- Files fully reviewed: {sum(r['status'] == 'reviewed' for r in files)} / {len(files)}\n"
        f"- Physical macro definitions indexed: {len(macros)}\n\n"
        "A parser hit is not a review. Each reviewed function needs all ten verdicts,\n"
        "a body hash and a substantive manual note. A changed body becomes pending.\n"
        "File reviews cover declarations, aggregate layouts, macros and the manual\n"
        "reconciliation of parsed bodies. Header changes require reviewing affected\n"
        "callers as well; a matching body hash alone does not prove dependency stability.\n"
    )
    return {
        "functions.tsv": tsv(rows, ["id", "path", "line", "end", "name", "signature",
                                     "kind", "va", "body_sha256", "status", *PATTERNS, "note"]),
        "files.tsv": tsv(files, ["path", "sha256", "functions", "reviewed", "status", "note"]),
        "macros.tsv": tsv(macros, ["path", "line", "end", "name", "body_sha256"]),
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
        print("Stale or missing review inventory: " + ", ".join(stale))
        return 1
    print(products["progress.md"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
