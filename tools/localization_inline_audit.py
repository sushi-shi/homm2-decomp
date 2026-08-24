#!/usr/bin/env python3
"""Inventory inline translations by aligning English and localized functions.

This is a maintainer audit tool, not part of the game build. It deliberately
reports candidates instead of rewriting source: the Buka branch contains both
localization and unrelated 2.1 code changes, so every candidate still needs a
semantic ID and a call-site review before entering ``messages.def``.
"""

import argparse
import ast
import difflib
import json
import pathlib
import re
import subprocess


STRING = re.compile(r'(?:u8|u|U|L)?"(?:\\.|[^"\\])*"', re.S)
IDENTIFIER = re.compile(r'([~A-Za-z_]\w*(?:::[~A-Za-z_]\w*)*)\s*\($')
FORMAT = re.compile(r"%(?:\d+\$)?[-+#0 .'\dhljztL]*[diuoxXfFeEgGaAcspn%]")


def revision_text(root, revision, path):
    return subprocess.check_output(
        ["git", "-C", str(root), "show", f"{revision}:{path}"],
        encoding="utf-8",
        errors="surrogateescape",
        stderr=subprocess.DEVNULL,
        text=True,
    )


def mask_non_code(text):
    """Replace comments and literal contents with spaces, preserving lines."""
    result = list(text)
    position = 0
    while position < len(text):
        if text.startswith("//", position):
            end = text.find("\n", position)
            end = len(text) if end < 0 else end
        elif text.startswith("/*", position):
            close = text.find("*/", position + 2)
            end = len(text) if close < 0 else close + 2
        elif text[position] in "\"'":
            quote = text[position]
            end = position + 1
            while end < len(text):
                if text[end] == "\\":
                    end += 2
                elif text[end] == quote:
                    end += 1
                    break
                else:
                    end += 1
        else:
            position += 1
            continue
        for index in range(position, min(end, len(text))):
            if result[index] != "\n":
                result[index] = " "
        position = end
    return "".join(result)


def matching_open_paren(code, close):
    depth = 1
    position = close - 1
    while position >= 0:
        if code[position] == ')':
            depth += 1
        elif code[position] == '(':
            depth -= 1
            if depth == 0:
                return position
        position -= 1
    return -1


def function_name(code, brace):
    position = brace - 1
    while position >= 0 and code[position].isspace():
        position -= 1
    # Skip trailing cv/ref/noexcept/override tokens. Source definitions in this
    # tree are simple enough that the rightmost ')' is the parameter list.
    close = code.rfind(')', max(0, position - 400), position + 1)
    if close < 0:
        return None
    between = code[close + 1:brace].strip()
    if between and not re.fullmatch(r"(?:const|noexcept|override|final|\s|&)*", between):
        return None
    opening = matching_open_paren(code, close)
    if opening < 0:
        return None
    prefix = code[max(0, opening - 160):opening + 1]
    match = IDENTIFIER.search(prefix)
    if not match:
        return None
    name = match.group(1)
    if name in ("if", "for", "while", "switch", "catch"):
        return None
    return name


def functions(text):
    code = mask_non_code(text)
    stack = []
    found = []
    for position, character in enumerate(code):
        if character == '{':
            name = function_name(code, position)
            stack.append((position, name))
        elif character == '}' and stack:
            opening, name = stack.pop()
            if name is not None:
                found.append((name, opening + 1, position))

    by_name = {}
    for name, start, end in sorted(found, key=lambda value: value[1]):
        by_name.setdefault(name, []).append((start, end))
    return by_name


def strip_comments(text):
    output = []
    position = 0
    while position < len(text):
        if text.startswith("//", position):
            end = text.find("\n", position)
            position = len(text) if end < 0 else end
        elif text.startswith("/*", position):
            end = text.find("*/", position + 2)
            position = len(text) if end < 0 else end + 2
        else:
            output.append(text[position])
            position += 1
    return "".join(output)


def mask_comments(text):
    result = list(text)
    position = 0
    while position < len(text):
        if text[position] in "\"'":
            quote = text[position]
            position += 1
            while position < len(text):
                if text[position] == "\\":
                    position += 2
                elif text[position] == quote:
                    position += 1
                    break
                else:
                    position += 1
            continue
        if text.startswith("//", position):
            end = text.find("\n", position)
            end = len(text) if end < 0 else end
        elif text.startswith("/*", position):
            close = text.find("*/", position + 2)
            end = len(text) if close < 0 else close + 2
        else:
            position += 1
            continue
        for index in range(position, min(end, len(text))):
            if result[index] != "\n":
                result[index] = " "
        position = end
    return "".join(result)


def decode_literal(token):
    token = re.sub(r"^(?:u8|u|U|L)", "", token)
    value = ast.literal_eval(token)
    output = bytearray()
    for character in value:
        code = ord(character)
        if code <= 0xff:
            output.append(code)
        else:
            output.extend(character.encode("cp1251"))
    return bytes(output)


def literals(text, start, end):
    body = text[start:end]
    code = mask_comments(body)
    result = []
    pending = None
    for match in STRING.finditer(code):
        if pending is not None:
            gap = body[pending[1]:match.start()]
            if strip_comments(gap).strip():
                result.append(pending)
                pending = None
        value = decode_literal(body[match.start():match.end()])
        line = text.count("\n", 0, start + match.start()) + 1
        if pending is None:
            pending = [value, match.end(), line]
        else:
            pending[0] += value
            pending[1] = match.end()
    if pending is not None:
        result.append(pending)
    return [(value, line) for value, unused, line in result]


def printable_english(value):
    return bool(value) and all(byte in (9, 10, 13) or 0x20 <= byte < 0x7f for byte in value)


def translated(value):
    return any(byte >= 0x80 for byte in value)


def formats(value):
    return FORMAT.findall(value.decode("latin1"))


def candidates(path, english_text, localized_text):
    english_functions = functions(english_text)
    localized_functions = functions(localized_text)
    output = []
    for name, english_ranges in english_functions.items():
        localized_ranges = localized_functions.get(name, [])
        for occurrence, english_range in enumerate(english_ranges):
            if occurrence >= len(localized_ranges):
                continue
            english = literals(english_text, *english_range)
            localized = literals(localized_text, *localized_ranges[occurrence])
            matcher = difflib.SequenceMatcher(
                None,
                [value for value, unused in english],
                [value for value, unused in localized],
                autojunk=False,
            )
            for operation, first_a, last_a, first_b, last_b in matcher.get_opcodes():
                if operation != "replace" or last_a - first_a != last_b - first_b:
                    continue
                for offset in range(last_a - first_a):
                    source, line = english[first_a + offset]
                    target, localized_line = localized[first_b + offset]
                    if not printable_english(source) or not translated(target):
                        continue
                    source_text = source.decode("ascii")
                    if re.fullmatch(r"[a-z][a-z0-9_]*(?:\.[a-z0-9_]+)+", source_text):
                        # Stable localization IDs already migrated at this site.
                        continue
                    output.append({
                        "path": path,
                        "function": name,
                        "occurrence": occurrence,
                        "line": line,
                        "localized_line": localized_line,
                        "english": source_text,
                        "translation": target.decode("cp1251"),
                        "format_compatible": formats(source) == formats(target),
                    })
    return output


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("root", type=pathlib.Path)
    parser.add_argument("revision")
    parser.add_argument("output", type=pathlib.Path)
    args = parser.parse_args()
    root = args.root.resolve()

    result = []
    for directory in (root / "src" / "BASE", root / "src" / "SOURCE"):
        for source in sorted(directory.glob("*.cpp")):
            path = source.relative_to(root).as_posix()
            try:
                localized = revision_text(root, args.revision, path)
            except subprocess.CalledProcessError:
                continue
            english = source.read_text(encoding="utf-8", errors="surrogateescape")
            result.extend(candidates(path, english, localized))

    args.output.write_text(
        "\n".join(json.dumps(entry, ensure_ascii=False) for entry in result) + "\n",
        encoding="utf-8",
    )
    incompatible = sum(not entry["format_compatible"] for entry in result)
    print(f"wrote {len(result)} candidates ({incompatible} format-incompatible) to {args.output}")


if __name__ == "__main__":
    main()
