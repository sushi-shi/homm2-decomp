#!/usr/bin/env python3
"""Reject raw integer type keywords in reconstructed game code.

The project uses Ints.h aliases so recovered storage and APIs retain explicit widths
when the game is ported away from 32-bit MSVC. SDK headers keep their native ABI
spellings, and plain char remains the text/byte-string character type.
"""
from pathlib import Path
import sys


FORBIDDEN = {"signed", "unsigned", "short", "int", "long", "__int64"}
SOURCE_ROOTS = (
    Path("src/SOURCE"),
    Path("src/BASE"),
    Path("src/EDITOR"),
    Path("include/SOURCE"),
    Path("include/BASE"),
    Path("include/EDITOR"),
)
SHARED_FILES = ()


def code_identifiers(text):
    """Yield identifiers with source positions outside comments and literals."""
    index = 0
    line = 1
    column = 1
    length = len(text)

    def advance(value):
        nonlocal line, column
        if value == "\n":
            line += 1
            column = 1
        else:
            column += 1

    while index < length:
        char = text[index]
        following = text[index + 1] if index + 1 < length else ""

        if char == "/" and following == "/":
            while index < length and text[index] != "\n":
                advance(text[index])
                index += 1
            continue

        if char == "/" and following == "*":
            advance(char)
            advance(following)
            index += 2
            while index < length:
                char = text[index]
                following = text[index + 1] if index + 1 < length else ""
                advance(char)
                index += 1
                if char == "*" and following == "/":
                    advance(following)
                    index += 1
                    break
            continue

        if char in ('"', "'"):
            quote = char
            advance(char)
            index += 1
            while index < length:
                char = text[index]
                advance(char)
                index += 1
                if char == "\\" and index < length:
                    advance(text[index])
                    index += 1
                elif char == quote:
                    break
            continue

        if char.isalpha() or char == "_":
            start = index
            start_line = line
            start_column = column
            while index < length and (text[index].isalnum() or text[index] == "_"):
                advance(text[index])
                index += 1
            yield text[start:index], start_line, start_column, start, index
            continue

        advance(char)
        index += 1


def violations(path):
    text = path.read_text(errors="replace")
    identifiers = list(code_identifiers(text))
    result = []
    for index, item in enumerate(identifiers):
        name, line, column, _, end = item
        if name not in FORBIDDEN:
            continue
        # long double is a floating-point type, not a raw integer spelling.
        if name == "long" and index + 1 < len(identifiers) \
                and identifiers[index + 1][0] == "double" \
                and text[end:identifiers[index + 1][3]].strip() == "":
            continue
        result.append((line, column, name))
    return result


def default_files():
    files = list(SHARED_FILES)
    for root in SOURCE_ROOTS:
        files.extend(root.rglob("*.cpp"))
        files.extend(root.rglob("*.h"))
    return sorted(set(files))


def main(argv=None):
    paths = [Path(value) for value in (sys.argv[1:] if argv is None else argv)]
    files = paths or default_files()
    total = 0
    bad_files = 0
    for path in files:
        found = violations(path)
        if not found:
            continue
        bad_files += 1
        total += len(found)
        print("%s: %d raw integer spelling(s)" % (path, len(found)))
        for line, column, name in found[:20]:
            print("    %d:%d: %s" % (line, column, name))
    if total:
        print("\nFIXED-WIDTH INTEGER FAIL: %d raw spelling(s) in %d game file(s); "
              "use i8/u8 through i64/u64." % (total, bad_files))
        return 1
    print("fixed-width integers OK: reconstructed game code uses Ints.h aliases.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
