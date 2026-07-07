# -*- coding: utf-8 -*-
# export_functions.py - dump build/ghidra/exports/functions.csv from the analyzed DB.
#
# functions.csv (entry_rva hex, byte_size decimal, name) is the WHOLE-.text function
# boundary map homm2.analysis.xref reads to attribute caller sites that land in the
# library/runtime functions CodeView omits. Read-only over the FunctionManager.
#
# Runs as a GhidraScript under PyGhidra (currentProgram bound by the driver).
#@category Homm2
import os

IMAGE_BASE = 0x400000
prog = currentProgram
fm = prog.getFunctionManager()

ROOT = os.environ.get("HOMM2_DIR", ".")
OUT = ROOT + "/build/ghidra/exports"
from java.io import File as _File
_File(OUT).mkdirs()

n = 0
fh = open(OUT + "/functions.csv", "w")
try:
    fh.write("entry_rva,byte_size,name\n")
    for fn in fm.getFunctions(True):           # True = forward (entry order)
        ep = fn.getEntryPoint()
        if ep is None or not ep.isMemoryAddress():
            continue
        rva = ep.getOffset() - IMAGE_BASE
        if rva < 0:
            continue
        size = fn.getBody().getNumAddresses()  # bytes of code in the body
        fh.write("0x%x,%d,%s\n" % (rva, size, fn.getName()))
        n += 1
finally:
    fh.close()

print("[export_functions] wrote %d functions -> %s/functions.csv" % (n, OUT))
