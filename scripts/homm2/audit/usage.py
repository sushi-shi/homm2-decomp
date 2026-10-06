"""Check that every tooling entry point keeps usage logging.

Every module-level ``main``/``cli_main`` under scripts/homm2 must carry the
``homm2.core.usage.logged`` decorator, so direct module runs, ninja-invoked
wrappers and batch commands are recorded like CLI commands. A module that runs
as a program (``if __name__ == "__main__":``) must do so through such a
``main``. Test modules are exempt, and so is ``clean/project/``: it is copied
into the generated source tree, which has no homm2 package.
"""
from __future__ import annotations

import argparse
import ast
import sys

from homm2.core.paths import REPO
from homm2.core.usage import logged

ENTRY_POINTS = ("main", "cli_main")


def _is_main_guard(node: ast.stmt) -> bool:
    if not isinstance(node, ast.If) or not isinstance(node.test, ast.Compare):
        return False
    names = [node.test.left, *node.test.comparators]
    return any(isinstance(n, ast.Name) and n.id == "__name__" for n in names) and any(
        isinstance(n, ast.Constant) and n.value == "__main__" for n in names)


def _has_logged(node: ast.FunctionDef) -> bool:
    return any(isinstance(d, ast.Name) and d.id == "logged" for d in node.decorator_list)


def uninstrumented() -> list[str]:
    missing = []
    for path in sorted((REPO / "scripts/homm2").rglob("*.py")):
        if path.name.startswith("test_"):
            continue
        if path.is_relative_to(REPO / "scripts/homm2/clean/project"):
            continue
        tree = ast.parse(path.read_text())
        relative = str(path.relative_to(REPO))
        entries = [n for n in tree.body if isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef))
                   and n.name in ENTRY_POINTS]
        for node in entries:
            if not _has_logged(node):
                missing.append(f"{relative}: {node.name}() lacks @logged")
        if not entries and any(_is_main_guard(n) for n in tree.body):
            missing.append(f"{relative}: runs as a program without a logged main()")
    return missing


@logged
def main(argv=None) -> int:
    argparse.ArgumentParser(prog="homm2 audit usage", description=__doc__).parse_args(argv)
    missing = uninstrumented()
    for line in missing:
        print(f"[usage] {line}")
    print(f"[usage] {'ok' if not missing else f'{len(missing)} uninstrumented entry point(s)'}")
    return 1 if missing else 0


if __name__ == "__main__":
    sys.exit(main())
