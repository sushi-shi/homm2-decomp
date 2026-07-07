# -*- coding: utf-8 -*-
# recover_structs.py - decompiler-backed struct/class LAYOUT recovery from access patterns.
#
# CodeView (NB09) gives us names, RVAs, method sets and vtables but NO type info (empty
# sstGlobalTypes): class member layouts are unknown. This script reconstructs, per class, a
# best-effort field skeleton by asking Ghidra's decompiler what offsets each method touches
# off its `this` pointer, then UNIONing those across every method of the class.
#
# Engine: ghidra.app.decompiler.util.FillOutStructureHelper (the "Auto Create Structure"
# action). For each instance method we grab the `this` HighVariable (ECX / param slot 0 under
# __thiscall - apply_names.py sets that from the MSVC mangling) and run processStructure()
# inside a transaction we ROLL BACK, so the Ghidra DB is never mutated. We read the resulting
# offset->width component map plus the load/store pcode ops (for read/write classification).
#
# Per class we also run a SIZE ORACLE: memset(this,0,N) inside the constructor and the
# operator new(N) immediate feeding the ctor at its call sites give the TRUE object size
# (FillOutStructure only ever yields a lower bound - unaccessed tail fields are invisible).
# The ??_7Class@@6B@ vtable symbol size / 4 gives the virtual-method count (vptr at offset 0).
#
# READ-ONLY: every probe transaction is rolled back; nothing is committed to the program DB.
# Flaky decompiles are skipped per-function and counted in the summary, never fatal.
#
# Reads  build/gen/symbol_names.csv
# Writes build/ghidra/exports/struct_layouts.csv  (class,offset,width,n_methods,kind)
#        build/ghidra/exports/struct_summary.csv   (class,accessed_size,ctor_size,
#                                                    vtable_entries,n_methods,n_probed,
#                                                    n_skipped,skipped_rvas)
# Runs as a GhidraScript under PyGhidra (currentProgram + monitor bound by the driver).
#@category Homm2
import csv
import os
import re

from ghidra.app.decompiler import DecompInterface, DecompileOptions
from ghidra.app.decompiler.util import FillOutStructureHelper
from ghidra.program.model.pcode import PcodeOp
from ghidra.util.task import ConsoleTaskMonitor

IMAGE_BASE = 0x400000
DECOMP_TIMEOUT = 45
MAX_CTOR_CALLERS = 12  # cap the operator-new oracle scan per ctor

prog = currentProgram
fm = prog.getFunctionManager()
af = prog.getAddressFactory().getDefaultAddressSpace()
refMgr = prog.getReferenceManager()
mon = ConsoleTaskMonitor()
ECX = prog.getRegister("ECX")

ROOT = os.environ.get("HOMM2_DIR", ".")
SYM = ROOT + "/build/gen/symbol_names.csv"
OUT = ROOT + "/build/ghidra/exports"
from java.io import File as _File
_File(OUT).mkdirs()

# NOTE: FillOutStructureHelper.computeHighVariable()/processStructure() call
# ifc.getOptions().getDefaultTimeout(); without options set that NPEs. Set them explicitly.
ifc = DecompInterface()
_opts = DecompileOptions()
try:
    _opts.grabFromProgram(prog)
except Exception:
    pass
ifc.setOptions(_opts)
ifc.toggleCCode(True)
ifc.openProgram(prog)


def toaddr(rva):
    return af.getAddress(IMAGE_BASE + rva)


def as_int(x):
    """java.lang.Long / Integer / python int -> python int."""
    try:
        return int(x.longValue())
    except AttributeError:
        return int(x)


# --- MSVC decorated-name -> (class_key, is_instance, is_ctor) ----------------------------
# We parse the mangling ourselves (independent of whether Ghidra set __thiscall) purely to
# GROUP funcs by owning class and to spot which one is the constructor.
_INSTANCE = set("ABEFIJMNQRUV")  # public/protected/private [virtual], near+far: has `this`
_STATIC = set("CDGHKLST")        # static members / global: no `this`


def parse_class(name):
    if not name or name[0] != "?":
        return None, False, False          # C symbol / plain name
    s = name[1:]
    is_ctor = False
    if s and s[0] == "?":                   # special name: ??0 ctor, ??1 dtor, ??_G/_E, ...
        rest = s[1:]
        if not rest:
            return None, False, False
        if rest[0] == "_":
            code, after = "_" + rest[1:2], rest[2:]
        else:
            code, after = rest[0], rest[1:]
        if code == "0":
            is_ctor = True
    else:                                   # normal ?leaf@Class@@sig
        i = s.find("@")
        if i < 0:
            return None, False, False
        after = s[i + 1:]
    j = after.find("@@")
    if j < 0:
        return None, False, False
    quals = [q for q in after[:j].split("@") if q != ""]
    if not quals:
        return None, False, is_ctor         # global function (no owning class)
    sig = after[j + 2:]
    prop = sig[0] if sig else ""
    is_instance = is_ctor or (prop in _INSTANCE)
    if prop in _STATIC and not is_ctor:
        is_instance = False
    class_key = "::".join(reversed(quals))  # inner..outer -> Outer::Inner
    return class_key, is_instance, is_ctor


_VT_RE = re.compile(r"^\?\?_7(.+)@@6B@$")


def parse_vtable(name):
    m = _VT_RE.match(name or "")
    if not m:
        return None
    quals = [q for q in m.group(1).split("@") if q != ""]
    if not quals:
        return None
    return "::".join(reversed(quals))


# --- read CodeView symbol table ----------------------------------------------------------
methods = {}       # class_key -> list of rva (instance methods, incl. ctor/dtor)
ctors = {}         # class_key -> rva of a constructor (default preferred)
vtable_ents = {}   # class_key -> virtual-method count
opnew_rva = None

with open(SYM) as f:
    for r in csv.DictReader(f):
        name = r.get("name") or ""
        kind = r.get("kind") or "func"
        try:
            rva = int(r["rva"], 16)
        except (ValueError, KeyError, TypeError):
            continue
        if kind == "func":
            if name == "??2@YAPAXI@Z":     # operator new(unsigned int)
                opnew_rva = rva
            ck, is_inst, is_ctor = parse_class(name)
            if ck and is_inst:
                methods.setdefault(ck, []).append(rva)
                if is_ctor:
                    # prefer the default ctor ??0X@@QAE@XZ (no args) if we can tell
                    if ck not in ctors or name.endswith("@XZ"):
                        ctors[ck] = rva
        elif kind == "data":
            ck = parse_vtable(name)
            if ck is not None:
                try:
                    vtable_ents[ck] = int(r["size"], 16) // 4
                except (ValueError, KeyError, TypeError):
                    pass

opnew_addr = toaddr(opnew_rva) if opnew_rva is not None else None
print("[recover_structs] %d classes with instance methods; operator new @ %s"
      % (len(methods), ("0x%x" % opnew_rva) if opnew_rva is not None else "?"))


# --- per-method probe: offsets touched off `this` ----------------------------------------
def probe(fn):
    """Return (offset->width dict, read-offset set, write-offset set) for this method, or None.

    Runs entirely inside a rolled-back transaction. We first force the method to __thiscall so
    the decompiler reliably models `this` as param 0 in ECX (Ghidra's demangler often leaves
    the signature unset), decompile, take param-0's HighVariable (ECX-input fallback), then
    FillOutStructure. The rollback reverts both the convention change and the created struct."""
    helper = FillOutStructureHelper(prog, mon)
    tx = prog.startTransaction("struct-probe")
    try:
        try:
            if fn.getCallingConventionName() != "__thiscall":
                fn.setCallingConvention("__thiscall")
        except Exception:
            pass
        res = ifc.decompileFunction(fn, DECOMP_TIMEOUT, mon)
        if not (res and res.decompileCompleted()):
            return None
        psym = res.getHighFunction().getLocalSymbolMap().getParamSymbol(0)
        thv = psym.getHighVariable() if psym is not None else None
        if thv is None:                       # fallback: ECX-input high variable at entry
            thv = helper.computeHighVariable(ECX.getAddress(), fn, ifc)
        if thv is None:
            return None
        helper.processStructure(thv, fn, True, False, ifc)  # createNew=True, removeExisting=False
        widths = {}
        nb = helper.getComponentMap()
        it = nb.iterator()
        while it.hasNext():
            e = it.next()
            off = as_int(e.getKey())
            dt = e.getValue()
            w = dt.getLength() if dt is not None else 0
            if w <= 0:
                w = 1
            widths[off] = max(widths.get(off, 0), w)
        reads = set(as_int(p.getOffset()) for p in helper.getLoadPcodeOps())
        writes = set(as_int(p.getOffset()) for p in helper.getStorePcodeOps())
        return widths, reads, writes
    except Exception as e:
        print("[recover_structs]   probe failed @0x%x: %s"
              % (fn.getEntryPoint().getOffset() - IMAGE_BASE, e))
        return None
    finally:
        prog.endTransaction(tx, False)  # ROLL BACK - read-only w.r.t. the DB


# --- size oracle: memset(this,0,N) in ctor + operator new(N) at ctor call sites ----------
_MEMSET_RE = re.compile(
    r"memset\s*\(\s*[^,]+,\s*0\s*,\s*(0x[0-9a-fA-F]+|\d+)\s*\)")


def ctor_memset_size(ctor_fn):
    try:
        res = ifc.decompileFunction(ctor_fn, DECOMP_TIMEOUT, mon)
        if not (res and res.decompileCompleted()):
            return None
        c = str(res.getDecompiledFunction().getC())
        best = None
        for m in _MEMSET_RE.finditer(c):
            v = int(m.group(1), 16) if m.group(1).lower().startswith("0x") else int(m.group(1))
            best = v if best is None or v > best else best
        return best
    except Exception:
        return None


def _call_target_off(op):
    """VA of a CALL pcode op's direct target, or None (compare by offset - JPype '!=' on
    Address objects is identity-based, not value-based)."""
    if op.getOpcode() != PcodeOp.CALL or op.getNumInputs() < 1:
        return None
    t = op.getInput(0)
    if not t.isAddress():
        return None
    return t.getAddress().getOffset()


def ctor_new_size(ctor_addr):
    """operator new(N) whose result is the `this` passed to the ctor at a call site."""
    if opnew_addr is None:
        return None
    ctor_off = ctor_addr.getOffset()
    opnew_off = opnew_addr.getOffset()
    sizes = {}
    seen = 0
    for ref in refMgr.getReferencesTo(ctor_addr):
        if not ref.getReferenceType().isCall():
            continue
        caller = fm.getFunctionContaining(ref.getFromAddress())
        if caller is None:
            continue
        seen += 1
        if seen > MAX_CTOR_CALLERS:
            break
        try:
            res = ifc.decompileFunction(caller, DECOMP_TIMEOUT, mon)
            if not (res and res.decompileCompleted()):
                continue
            ops = res.getHighFunction().getPcodeOps()
            while ops.hasNext():
                op = ops.next()
                if _call_target_off(op) != ctor_off or op.getNumInputs() < 2:
                    continue
                d = op.getInput(1).getDef()      # def of the `this` argument
                if d is None or _call_target_off(d) != opnew_off or d.getNumInputs() < 2:
                    continue
                sz = d.getInput(1)               # operator new(size)
                if sz.isConstant():
                    v = int(sz.getOffset())
                    sizes[v] = sizes.get(v, 0) + 1
        except Exception:
            continue
    if not sizes:
        return None
    # most frequently observed allocation size
    return sorted(sizes.items(), key=lambda kv: -kv[1])[0][0]


# --- drive all classes -------------------------------------------------------------------
lay = open(OUT + "/struct_layouts.csv", "w")
summ = open(OUT + "/struct_summary.csv", "w")
lay.write("class,offset,width,n_methods,kind\n")
summ.write("class,accessed_size,ctor_size,vtable_entries,n_methods,n_probed,n_skipped,"
           "skipped_rvas\n")

n_classes = len(methods)
n_with_layout = 0
n_with_ctor_size = 0
done = 0
for ck in sorted(methods):
    done += 1
    rvas = sorted(set(methods[ck]))
    width_of = {}   # off -> set(widths)
    touched = {}    # off -> set(rva)
    read_of = {}    # off -> bool
    write_of = {}   # off -> bool
    n_probed = 0
    skipped = []
    for rva in rvas:
        fn = fm.getFunctionAt(toaddr(rva))
        if fn is None:
            skipped.append(rva)
            continue
        r = probe(fn)
        if r is None:
            skipped.append(rva)
            continue
        widths, reads, writes = r
        touched_any = False
        for off, w in widths.items():
            width_of.setdefault(off, set()).add(w)
            touched.setdefault(off, set()).add(rva)
            touched_any = True
        for off in reads:
            read_of[off] = True
        for off in writes:
            write_of[off] = True
        if touched_any or reads or writes:
            n_probed += 1

    all_offs = set(width_of) | set(read_of) | set(write_of)
    accessed_size = 0
    for off in sorted(all_offs):
        ws = width_of.get(off, set())
        w = max(ws) if ws else 1
        n = len(touched.get(off, set()))
        rd = read_of.get(off, False)
        wr = write_of.get(off, False)
        kind = "rw" if (rd and wr) else "r" if rd else "w" if wr else "?"
        if len(ws) > 1:
            kind += "!union"       # multiple distinct widths at one offset -> candidate union
        off_s = ("-0x%x" % (-off)) if off < 0 else ("0x%x" % off)
        lay.write("%s,%s,%d,%d,%s\n" % (ck, off_s, w, n, kind))
        if off >= 0:
            accessed_size = max(accessed_size, off + w)

    if all_offs:
        n_with_layout += 1

    ctor_size = ""
    ctor_addr = None
    if ck in ctors:
        ctor_addr = toaddr(ctors[ck])
        ms = ctor_memset_size(fm.getFunctionAt(ctor_addr)) if fm.getFunctionAt(ctor_addr) else None
        ns = ctor_new_size(ctor_addr)
        # prefer a size confirmed by both; else operator-new; else memset
        cand = None
        if ms is not None and ns is not None:
            cand = ns if ns >= ms else ms
        else:
            cand = ns if ns is not None else ms
        if cand is not None:
            ctor_size = "0x%x" % cand
            n_with_ctor_size += 1

    vt = vtable_ents.get(ck, "")
    summ.write("%s,0x%x,%s,%s,%d,%d,%d,%s\n" % (
        ck, accessed_size, ctor_size, str(vt), len(rvas), n_probed, len(skipped),
        " ".join("0x%x" % s for s in skipped)))
    if done % 25 == 0 or done == n_classes:
        print("[recover_structs] %d/%d classes ... (last: %s, %d offsets, %d skipped)"
              % (done, n_classes, ck, len(all_offs), len(skipped)))

lay.close()
summ.close()
print("[recover_structs] wrote struct_layouts.csv + struct_summary.csv -> %s" % OUT)
print("[recover_structs] %d classes, %d with a recovered layout, %d with a ctor-exact size"
      % (n_classes, n_with_layout, n_with_ctor_size))
