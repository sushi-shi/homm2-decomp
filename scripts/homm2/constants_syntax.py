"""Lightweight lexical helpers for the live constants audit."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Token:
    text: str
    line: int
    start: int
    end: int


@dataclass(frozen=True)
class EnumDeclaration:
    line: int
    end_line: int


def lex(text: str) -> list[Token]:
    tokens: list[Token] = []
    index = 0
    line = 1
    two_character = {"<<", ">>", "<=", ">=", "==", "!=", "&&", "||", "++", "--", "->"}
    while index < len(text):
        character = text[index]
        if character.isspace():
            line += character == "\n"
            index += 1
            continue
        if text.startswith("//", index):
            end = text.find("\n", index)
            index = len(text) if end < 0 else end
            continue
        if text.startswith("/*", index):
            end = text.find("*/", index + 2)
            end = len(text) - 2 if end < 0 else end
            line += text.count("\n", index, end + 2)
            index = end + 2
            continue
        start = index
        token_line = line
        if character.isalpha() or character == "_":
            index += 1
            while index < len(text) and (text[index].isalnum() or text[index] == "_"):
                index += 1
        elif character.isdigit():
            index += 1
            while index < len(text) and (text[index].isalnum() or text[index] in "._"):
                index += 1
        elif character in "\"'":
            quote = character
            index += 1
            while index < len(text):
                if text[index] == "\\":
                    index += 2
                    continue
                if text[index] == quote:
                    index += 1
                    break
                line += text[index] == "\n"
                index += 1
        elif text[index:index + 2] in two_character:
            index += 2
        else:
            index += 1
        tokens.append(Token(text[start:index], token_line, start, index))
    return tokens


def _matching_token(tokens: list[Token], start: int, opening: str, closing: str) -> int:
    depth = 0
    for index in range(start, len(tokens)):
        if tokens[index].text == opening:
            depth += 1
        elif tokens[index].text == closing:
            depth -= 1
            if depth == 0:
                return index
    raise ValueError(f"unclosed {opening} at line {tokens[start].line}")


def parse_enum_declarations(path: Path, text: str | None = None) -> list[EnumDeclaration]:
    if path.name == "Ints.h":
        return []
    text = path.read_text(errors="replace") if text is None else text
    tokens = lex(text)
    result: list[EnumDeclaration] = []
    begin_to_end = {
        "H2_ENUM_BEGIN": "H2_ENUM_END",
        "H2_ENUM_CLASS_BEGIN": "H2_ENUM_CLASS_END",
        "H2_ENUM_CLASS_BEGIN_T": "H2_ENUM_CLASS_END_T",
    }
    index = 0
    while index < len(tokens):
        token = tokens[index].text
        if token in begin_to_end:
            end_name = begin_to_end[token]
            end = next(
                (cursor for cursor in range(index + 1, len(tokens)) if tokens[cursor].text == end_name),
                None,
            )
            if end is None:
                raise ValueError(f"{path}:{tokens[index].line}: missing {end_name}")
            result.append(EnumDeclaration(tokens[index].line, tokens[end].line))
            index = end + 1
            continue
        if token == "typedef" and index + 1 < len(tokens) and tokens[index + 1].text == "enum":
            enum_index = index + 1
        elif token == "enum" and (index == 0 or tokens[index - 1].text != "typedef"):
            enum_index = index
        else:
            index += 1
            continue
        opening = next(
            (cursor for cursor in range(enum_index + 1, len(tokens)) if tokens[cursor].text in ("{", ";")),
            None,
        )
        if opening is None or tokens[opening].text != "{":
            index += 1
            continue
        closing = _matching_token(tokens, opening, "{", "}")
        result.append(EnumDeclaration(tokens[index].line, tokens[closing].line))
        index = closing + 1
    return result
