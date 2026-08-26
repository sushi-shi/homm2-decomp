#!/usr/bin/env python3
"""Extract stable catalog entries from legacy text table initializers.

The English C++ initializer remains authoritative. ``template`` is used by
CMake; ``import`` is a maintainer command which pairs it with a historical
localized branch without compiling or copying that branch.
"""

import ast
import contextlib
import json
import pathlib
import re
import subprocess
import sys


TABLE_ENTRY = re.compile(
    r'^HOMM2_LEGACY_TABLE\("([^"]+)",\s*([A-Za-z_]\w*)\)', re.M
)
MEMBER_TABLE_ENTRY = re.compile(
    r'^HOMM2_LEGACY_MEMBER_TABLE\('
    r'"([^"]+)",\s*([A-Za-z_]\w*),\s*([A-Za-z_]\w*),\s*(\d+)\)',
    re.M,
)


def configured_tables(root):
    definition = (root / "locales/legacy_tables.def").read_text(encoding="utf-8")
    result = {}
    for path, name in TABLE_ENTRY.findall(definition):
        result.setdefault(path, []).append(name)
    return result


def configured_member_tables(root):
    definition = (root / "locales/legacy_tables.def").read_text(encoding="utf-8")
    result = {}
    for path, name, member, index in MEMBER_TABLE_ENTRY.findall(definition):
        result.setdefault(path, []).append((name, member, int(index)))
    return result


def read_source(root, revision, path):
    if revision is None:
        return (root / path).read_text(encoding="utf-8")
    return subprocess.check_output(
        ["git", "-C", str(root), "show", f"{revision}:{path}"],
        text=True,
        encoding="utf-8",
    )


def skip(text, position):
    while position < len(text):
        if text[position].isspace():
            position += 1
        elif text.startswith("//", position):
            end = text.find("\n", position)
            position = len(text) if end < 0 else end + 1
        elif text.startswith("/*", position):
            end = text.find("*/", position + 2)
            position = len(text) if end < 0 else end + 2
        else:
            break
    return position


def quoted_end(text, position, quote):
    position += 1
    while position < len(text):
        if text[position] == "\\":
            position += 2
        elif text[position] == quote:
            return position + 1
        else:
            position += 1
    raise ValueError("unterminated literal")


def expression(text, position):
    start = position
    depth = 0
    while position < len(text):
        if text.startswith("//", position):
            end = text.find("\n", position)
            position = len(text) if end < 0 else end + 1
        elif text.startswith("/*", position):
            end = text.find("*/", position + 2)
            position = len(text) if end < 0 else end + 2
        elif text[position] in "\"'":
            position = quoted_end(text, position, text[position])
        elif text[position] in "([":
            depth += 1
            position += 1
        elif text[position] in ")]":
            depth -= 1
            position += 1
        elif depth == 0 and text[position] in ",}":
            return text[start:position], position
        else:
            position += 1
    raise ValueError("unterminated expression")


def parse_list(text, position):
    if text[position] != "{":
        raise ValueError("initializer is not a list")
    position += 1
    values = []
    while True:
        position = skip(text, position)
        if text[position] == "}":
            return values, position + 1
        if text[position] == "{":
            value, position = parse_list(text, position)
        else:
            value, position = expression(text, position)
        values.append(value)
        position = skip(text, position)
        if text[position] == ",":
            position += 1


def without_comments(text):
    output = []
    position = 0
    while position < len(text):
        if text.startswith("//", position):
            end = text.find("\n", position)
            position = len(text) if end < 0 else end
        elif text.startswith("/*", position):
            end = text.find("*/", position + 2)
            position = len(text) if end < 0 else end + 2
        elif text[position] in "\"'":
            end = quoted_end(text, position, text[position])
            output.append(text[position:end])
            position = end
        else:
            output.append(text[position])
            position += 1
    return "".join(output)


STRING = re.compile(r'(?:u8|u|U|L)?"(?:\\.|[^"\\])*"', re.S)
DECLARATION = re.compile(
    r"^(?:DATA(?:_COMPGEN)?\([^\n]*\)\s*)?(?:const\s+)?char\s*\*\s*"
    r"([A-Za-z_]\w*)\s*\[[^;=]*=",
    re.M,
)


def decode(value):
    if isinstance(value, list):
        return [decode(item) for item in value]
    pieces = STRING.findall(without_comments(value))
    if not pieces:
        return None
    result = []
    for piece in pieces:
        piece = re.sub(r"^(?:u8|u|U|L)", "", piece)
        result.append(ast.literal_eval(piece))
    return "".join(result).encode("latin1")


def extract(text):
    result = {}
    for match in DECLARATION.finditer(text):
        position = skip(text, match.end())
        if position < len(text) and text[position] == "{":
            value, unused = parse_list(text, position)
            result[match.group(1)] = decode(value)
    return result


def extract_member_table(text, name, member_index):
    declaration = re.compile(
        r"^(?:DATA(?:_COMPGEN)?\([^\n]*\)\s*)?"
        r"[A-Za-z_]\w*\s+" + re.escape(name) + r"\s*\[[^;=]*=",
        re.M,
    )
    match = declaration.search(text)
    if match is None:
        raise ValueError(f"cannot find {name}")
    position = skip(text, match.end())
    value, unused = parse_list(text, position)
    result = []
    for row in value:
        if not isinstance(row, list) or member_index >= len(row):
            raise ValueError(f"{name} has an invalid member-table row")
        result.append(decode(row[member_index]))
    return result


def flatten(value, prefix=()):
    if isinstance(value, list):
        for index, child in enumerate(value):
            yield from flatten(child, prefix + (index,))
    else:
        yield prefix, value


def po_quote(value):
    return json.dumps(value, ensure_ascii=False)


def header(language):
    fields = [
        "MIME-Version: 1.0\n",
        "Content-Type: text/plain; charset=UTF-8\n",
        "Content-Transfer-Encoding: 8bit\n",
    ]
    if language:
        fields.insert(0, f"Language: {language}\n")
    print('msgid ""')
    print('msgstr ""')
    for field in fields:
        print(po_quote(field))
    print()


def entries(root, revision):
    for path, wanted in configured_tables(root).items():
        english_tables = extract(read_source(root, None, path))
        translated_tables = (
            extract(read_source(root, revision, path)) if revision else None
        )
        for name in wanted:
            if name not in english_tables:
                raise ValueError(f"cannot find {name} in {path}")
            english = list(flatten(english_tables[name]))
            translated = (
                list(flatten(translated_tables[name])) if translated_tables else None
            )
            if (translated is not None and len(english) == len(translated) + 1
                and english[-1][1] is None):
                english.pop()
            if translated is not None and len(english) != len(translated):
                raise ValueError(f"{name} shape differs between source and {revision}")
            for offset, (indices, source_bytes) in enumerate(english):
                if not source_bytes:
                    continue
                target_bytes = translated[offset][1] if translated else None
                if translated is not None and target_bytes is None:
                    raise ValueError(f"{name}{indices} has no translated literal")
                identifier = "table." + name + "." + ".".join(map(str, indices))
                yield path, identifier, source_bytes, target_bytes
    for path, wanted in configured_member_tables(root).items():
        english_text = read_source(root, None, path)
        translated_text = read_source(root, revision, path) if revision else None
        for name, unused_member, member_index in wanted:
            english = extract_member_table(english_text, name, member_index)
            translated = (
                extract_member_table(translated_text, name, member_index)
                if translated_text is not None else None
            )
            if translated is not None and len(english) != len(translated):
                raise ValueError(f"{name} shape differs between source and {revision}")
            for index, source_bytes in enumerate(english):
                if not source_bytes:
                    continue
                target_bytes = translated[index] if translated is not None else None
                if translated is not None and target_bytes is None:
                    raise ValueError(f"{name}[{index}] has no translated literal")
                identifier = f"table.{name}.{index}"
                yield path, identifier, source_bytes, target_bytes


def emit(root, revision):
    header("ru" if revision else None)
    for path, identifier, source_bytes, target_bytes in entries(root, revision):
        source = source_bytes.decode("ascii")
        target = target_bytes.decode("cp1251") if target_bytes is not None else ""
        # Do not classify ordinary percentage prose such as "150% damage"
        # as the legal-but-unintended printf sequence "% d".
        format_pattern = re.compile(
            r"(?<!\d)%(?:\d+\$)?[-+#0 .'\dhljztL]*[diuoxXfFeEgGaAcspn]"
        )
        source_formats = format_pattern.findall(source)
        target_formats = format_pattern.findall(target)
        if target and source_formats != target_formats:
            print(
                f"warning: skipping placeholder-incompatible {identifier}",
                file=sys.stderr,
            )
            target = ""
        print("#. Legacy table entry; English is authoritative in the C++ initializer.")
        print(f"#: {path}")
        if source_formats:
            print("#, c-format")
        print("msgctxt", po_quote(identifier))
        print("msgid", po_quote(source))
        print("msgstr", po_quote(target))
        print()


def main():
    if len(sys.argv) != 4 or sys.argv[1] not in ("template", "import"):
        raise SystemExit(
            "usage: localization_tables.py template ROOT OUTPUT | "
            "localization_tables.py import ROOT REVISION"
        )
    root = pathlib.Path(sys.argv[2]).resolve()
    if sys.argv[1] == "template":
        with open(sys.argv[3], "w", encoding="utf-8", newline="\n") as output:
            with contextlib.redirect_stdout(output):
                emit(root, None)
    else:
        emit(root, sys.argv[3])


if __name__ == "__main__":
    main()
