# -*- coding: utf-8 -*-
# decomp_export.py - Ghidra decompiler C + caller/callee xrefs for a list of RVAs.
#
# Backs `homm2 sema decomp`: for each target RVA, the decompiler C output plus its callers
# and callees (names come from apply_names.py's CodeView seeding). READ-ONLY unless
# HOMM2_DECOMP_FORCE=1, which creates a function at an RVA Ghidra left unwrapped first.
#
# Reads  /tmp/homm2_decomp_targets.txt  (one 0xRVA per line)
# Writes /tmp/homm2_decomp_out.txt
# Runs as a GhidraScript under PyGhidra (currentProgram + monitor bound by the driver).
#@category Homm2
import os

from ghidra.app.decompiler import DecompInterface
from ghidra.util.task import ConsoleTaskMonitor

IMAGE_BASE = 0x400000
FORCE = os.environ.get("HOMM2_DECOMP_FORCE") == "1"
prog = currentProgram
fm = prog.getFunctionManager()
af = prog.getAddressFactory().getDefaultAddressSpace()
mon = ConsoleTaskMonitor()
ifc = DecompInterface()
ifc.openProgram(prog)

targets = [int(ln.strip(), 16) for ln in open("/tmp/homm2_decomp_targets.txt")
           if ln.strip().startswith("0x")]
out = open("/tmp/homm2_decomp_out.txt", "w")


def w(s):
    out.write(s)
    out.write("\n")


for rva in targets:
    addr = af.getAddress(IMAGE_BASE + rva)
    fn = fm.getFunctionContaining(addr)
    if fn is None and FORCE:
        from ghidra.app.cmd.function import CreateFunctionCmd
        tx = prog.startTransaction("homm2-decomp-force")
        try:
            CreateFunctionCmd(addr).applyTo(prog, mon)
        finally:
            prog.endTransaction(tx, True)
        fn = fm.getFunctionContaining(addr)
    w("\n" + "=" * 78)
    if fn is None:
        w("### 0x%08x  <no function here - retry with --force>" % rva)
        continue
    ep = fn.getEntryPoint().getOffset() - IMAGE_BASE
    w("### 0x%08x  %s  cc=%s  sig=%s" % (
        ep, fn.getName(), fn.getCallingConventionName(),
        fn.getSignature(False).getPrototypeString()))
    callers = []
    for r in getReferencesTo(fn.getEntryPoint()):
        if r.getReferenceType().isCall():
            cf = fm.getFunctionContaining(r.getFromAddress())
            callers.append(
                (cf.getName() + "@0x%08x" % (cf.getEntryPoint().getOffset() - IMAGE_BASE))
                if cf else "0x%08x" % (r.getFromAddress().getOffset() - IMAGE_BASE))
    w("CALLERS(%d): %s" % (len(callers), ", ".join(sorted(set(callers))[:25])))
    callees = sorted(set(
        c.getName() + "@0x%08x" % (c.getEntryPoint().getOffset() - IMAGE_BASE)
        for c in fn.getCalledFunctions(mon)))
    w("CALLEES(%d): %s" % (len(callees), ", ".join(callees[:40])))
    try:
        res = ifc.decompileFunction(fn, 60, mon)
        if res and res.decompileCompleted():
            w(res.getDecompiledFunction().getC())
        else:
            w("/* decompile failed: %s */" % (res.getErrorMessage() if res else "null"))
    except Exception as e:
        w("/* decompile exception: %s */" % e)

out.close()
print("[decomp_export] wrote /tmp/homm2_decomp_out.txt for %d target(s)" % len(targets))
