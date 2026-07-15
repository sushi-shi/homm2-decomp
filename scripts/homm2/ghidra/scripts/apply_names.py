# -*- coding: utf-8 -*-
# apply_names.py - seed the Ghidra DB with OUR CodeView names ("set the symbols we know").
#
# CodeView is authoritative for each retained public name and start RVA. Recovered boundaries
# and ownership are carried in build/gen/symbol_names.csv. This creates a function at each RVA
# Ghidra missed and applies the demangled name, so:
#   - functions.csv carries real names for game funcs (Ghidra fills the library remainder), and
#   - decompiler output reads with real names instead of FUN_xxxx.
# No structs/prototypes/enums - names only (that's all decomp readability needs). One-shot;
# nothing is discovered.
#
# Runs as a GhidraScript under PyGhidra (currentProgram + monitor bound by the driver).
#@category Homm2
import csv
import os

IMAGE_BASE = 0x400000
prog = currentProgram
fm = prog.getFunctionManager()
af = prog.getAddressFactory().getDefaultAddressSpace()

from ghidra.program.model.symbol import SourceType
from ghidra.app.cmd.function import CreateFunctionCmd
from ghidra.app.cmd.label import DemanglerCmd
from ghidra.util.task import TaskMonitor

ROOT = os.environ.get("HOMM2_DIR", ".")
SYM = ROOT + "/build/gen/symbol_names.csv"

created = named = missing = 0
tx = prog.startTransaction("homm2-apply-names")
ok = True
try:
    with open(SYM) as f:
        for r in csv.DictReader(f):
            if (r.get("kind") or "func") != "func":
                continue
            try:
                rva = int(r["rva"], 16)
            except (ValueError, KeyError):
                continue
            addr = af.getAddress(IMAGE_BASE + rva)
            fn = fm.getFunctionAt(addr)
            if fn is None:
                CreateFunctionCmd(addr).applyTo(prog, monitor)
                fn = fm.getFunctionAt(addr)
                if fn is not None:
                    created += 1
            if fn is None:
                missing += 1
                continue
            mangled = r["name"]
            # DemanglerCmd applies the demangled name (leaf + class namespace) at addr;
            # on failure (non-MSVC-mangled label) fall back to the literal mangled name.
            if not DemanglerCmd(addr, mangled).applyTo(prog, TaskMonitor.DUMMY):
                try:
                    fn.setName(mangled, SourceType.USER_DEFINED)
                except Exception:
                    pass
            named += 1
except Exception as e:
    ok = False
    print("[apply_names] ERROR: %s" % e)
finally:
    prog.endTransaction(tx, ok)

print("[apply_names] created %d functions, named %d (%d RVAs uncreatable) from CodeView"
      % (created, named, missing))
