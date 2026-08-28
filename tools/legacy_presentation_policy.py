#!/usr/bin/env python3

from __future__ import annotations

import re
import sys
from pathlib import Path


PRESENT_CALL = re.compile(r"platform::Video\(\)\.Present\(\);")
TRAILING_TRIVIA = re.compile(r"(?:\s+|//[^\n]*(?:\n|$)|/\*.*?\*/)*\Z", re.DOTALL)


def line_number(source: str, offset: int) -> int:
    return source.count("\n", 0, offset) + 1


def function_body(source: str, declaration: re.Pattern[str], label: str) -> str:
    match = declaration.search(source)
    if match is None:
        raise ValueError(f"missing {label}")
    opening_brace = source.find("{", match.end())
    if opening_brace == -1:
        raise ValueError(f"missing body for {label}")

    depth = 1
    position = opening_brace + 1
    while position < len(source) and depth != 0:
        if source[position] == "{":
            depth += 1
        elif source[position] == "}":
            depth -= 1
        position += 1
    if depth != 0:
        raise ValueError(f"unterminated body for {label}")
    return source[opening_brace + 1 : position - 1]


def check(root: Path) -> list[str]:
    failures: list[str] = []
    misc_path = root / "src/BASE/Misc.cpp"
    low_level_path = root / "src/BASE/miscwin.cpp"
    misc = misc_path.read_text(encoding="utf-8")
    low_level = low_level_path.read_text(encoding="utf-8")

    try:
        vesa_body = function_body(
            low_level,
            re.compile(r'extern\s+"C"\s+void\s+__cdecl\s+BlitBitmapToScreenVesa\s*\('),
            "BlitBitmapToScreenVesa",
        )
        no_mouse_body = function_body(
            misc,
            re.compile(r"\bvoid\s+BlitBitmapToScreenNoMouseCheck\s*\("),
            "BlitBitmapToScreenNoMouseCheck",
        )
        composed_body = function_body(
            misc,
            re.compile(r"\bvoid\s+BlitBitmapToScreen\s*\("),
            "BlitBitmapToScreen",
        )
    except ValueError as error:
        return [str(error)]

    if PRESENT_CALL.search(vesa_body):
        failures.append(
            "BlitBitmapToScreenVesa presents an intermediate legacy blit; "
            "only its high-level owner may present the composed frame"
        )
    if "platform::Video().Blit(" not in vesa_body:
        failures.append("BlitBitmapToScreenVesa no longer owns the platform framebuffer blit")

    no_mouse_presents = list(PRESENT_CALL.finditer(no_mouse_body))
    no_mouse_blit = no_mouse_body.rfind("BlitBitmapToScreenVesa(")
    if len(no_mouse_presents) != 1:
        failures.append("BlitBitmapToScreenNoMouseCheck must present exactly once")
    elif no_mouse_blit == -1 or no_mouse_presents[0].start() < no_mouse_blit:
        failures.append("BlitBitmapToScreenNoMouseCheck must present after its low-level blit")
    elif not TRAILING_TRIVIA.fullmatch(no_mouse_body[no_mouse_presents[0].end() :]):
        failures.append("BlitBitmapToScreenNoMouseCheck presentation must terminate the update")

    try:
        monochrome_body = function_body(
            composed_body,
            re.compile(r"\bif\s*\(\s*gbColorMice\s*==\s*0\s*\)"),
            "BlitBitmapToScreen monochrome path",
        )
    except ValueError as error:
        failures.append(str(error))
        monochrome_body = ""
    monochrome_presents = list(PRESENT_CALL.finditer(monochrome_body))
    monochrome_blit = monochrome_body.rfind("BlitBitmapToScreenVesa(")
    monochrome_return = monochrome_body.rfind("return;")
    if len(monochrome_presents) != 1:
        failures.append("BlitBitmapToScreen monochrome path must present exactly once")
    elif (
        monochrome_blit == -1
        or monochrome_return == -1
        or not (monochrome_blit < monochrome_presents[0].start() < monochrome_return)
    ):
        failures.append(
            "BlitBitmapToScreen monochrome path must blit, present, then return"
        )

    composed_presents = list(PRESENT_CALL.finditer(composed_body))
    if len(composed_presents) != 2:
        failures.append(
            "BlitBitmapToScreen must have one monochrome presentation and one "
            "terminal composed-frame presentation"
        )
    else:
        final_present = composed_presents[-1]
        last_blit = composed_body.rfind("BlitBitmapToScreenVesa(")
        restore = composed_body.rfind("gpMouseManager->RestoreUnderlying()")
        if final_present.start() < max(last_blit, restore):
            failures.append(
                "BlitBitmapToScreen must present after every low-level blit and cursor restore"
            )
        if not TRAILING_TRIVIA.fullmatch(composed_body[final_present.end() :]):
            failures.append("BlitBitmapToScreen presentation must terminate the composed update")

    game_presentations: list[str] = []
    for directory in ("BASE", "EDITOR", "EXECUTABLE", "IRONFIST", "SOURCE"):
        source_root = root / "src" / directory
        if not source_root.exists():
            continue
        for path in sorted(source_root.rglob("*.cpp")):
            source = path.read_text(encoding="utf-8")
            for match in PRESENT_CALL.finditer(source):
                line = line_number(source, match.start())
                game_presentations.append(f"{path.relative_to(root)}:{line}")

    expected_presentations = [
        f"src/BASE/Misc.cpp:{line_number(misc, match.start())}"
        for match in PRESENT_CALL.finditer(misc)
    ]
    if game_presentations != expected_presentations:
        failures.append(
            "game-side Present() calls must remain owned by the high-level legacy "
            "screen blits in src/BASE/Misc.cpp; found "
            + ", ".join(game_presentations)
        )

    return failures


def main() -> int:
    root = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
    failures = check(root)
    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
