"""homm2.verify - `homm2 verify <gate>` - gates over the reconstructed program.

    homm2 verify behaviour      game-behaviour tests of the reconstructed code
                                (homm2.verify.behaviour)

A gate exits non-zero on a finding. `homm2 build verify` runs every gate.
"""
from __future__ import annotations

import importlib
import sys

from homm2.core.usage import logged

GATES = {
    "behaviour": "homm2.verify.behaviour",
}


def usage(stream=sys.stderr) -> None:
    print(__doc__.strip(), file=stream)


@logged
def main(argv=None) -> int:
    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0] in ("-h", "--help"):
        usage(sys.stdout if argv else sys.stderr)
        return 0 if argv else 2
    if argv[0] not in GATES:
        print(f"homm2 verify: unknown gate {argv[0]!r} (have: {', '.join(GATES)})",
              file=sys.stderr)
        return 2
    return importlib.import_module(GATES[argv[0]]).main(argv[1:])


if __name__ == "__main__":
    sys.exit(main())
