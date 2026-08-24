#!/usr/bin/env python3
"""Shim: the generator lives in homm2.build.configure (rules / compile_graph /
link_graph). This entry point stays at the repo root because `homm2 build`,
`homm2 redelink`, and muscle memory all invoke `python3 configure.py`."""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent / "scripts"))

from homm2.build.configure import main

if __name__ == "__main__":
    main()
