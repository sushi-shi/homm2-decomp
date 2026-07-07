#!/usr/bin/env python3
# Rigorous inheritance + member-subobject recovery from the retail binary.
# For each class ctor, track ecx symbolically as (this + offset). At every ??0X ctor call:
#   ecx == this+0   -> X is a BASE  (base subobject lives at offset 0)
#   ecx == this+N>0 -> X is a MEMBER subobject at offset N (composition, NOT inheritance)
#   ecx unknown     -> report as '?'
# The base is the offset-0 ctor call (before the class stores its own ??_7 vtable).
import csv, subprocess, re, os
CSV = "build/gen/symbol_names.csv"
ctors = {}
for r in csv.reader(open(CSV)):
    if len(r) < 5 or r[4] != 'func': continue
    m = re.match(r'\?\?0(\w+)@@', r[1])
    if m: ctors.setdefault(m.group(1), []).append((r[1], r[2]))
def called(mangled):  # ??0X@@... -> X
    m = re.match(r'\?\?0(\w+)@@', mangled); return m.group(1) if m else None
def analyze(cls):
    bases, members = set(), {}
    for name, unit in ctors.get(cls, []):
        obj = "build/delink/%s.c.obj" % unit
        if not os.path.exists(obj): continue
        raw = subprocess.run(["llvm-objdump","-dr","--disassemble-symbols="+name, obj],
                             capture_output=True, text=True).stdout.splitlines()
        this_slot = None; ecx = 0      # ecx starts = incoming `this` (offset 0); None once clobbered
        for i, ln in enumerate(raw):
            s = re.sub(r'\t',' ', ln)
            m = re.search(r'movl %ecx, -0x([0-9a-f]+)\(%ebp\)', s)      # spill this -> slot
            if m and this_slot is None: this_slot = m.group(1); continue
            m = re.search(r'movl -0x([0-9a-f]+)\(%ebp\), %ecx', s)      # reload
            if m: ecx = 0 if m.group(1) == this_slot else None; continue
            m = re.search(r'addl \$0x([0-9a-f]+), %ecx', s)             # this += N
            if m and ecx is not None: ecx += int(m.group(1),16); continue
            if re.search(r', %ecx', s) or re.search(r'leal .*%ecx', s): # other ecx write
                if 'movl %ecx,' not in s: ecx = None
            if 'calll' in s:                                            # call: reloc on next line
                nxt = re.sub(r'\t', ' ', raw[i+1]) if i+1 < len(raw) else ''
                mm = re.search(r'REL32\s+(\?\?0\w+@@\S*)', nxt)
                if mm:
                    X = called(mm.group(1))
                    if X and X != cls:
                        if ecx == 0: bases.add(X)
                        elif ecx: members.setdefault(ecx, X)
                        else: members.setdefault('?', X)
                ecx = None                                              # ret value clobbers ecx
    return bases, members
print("%-20s %-14s %s" % ("class","BASE(off0)","members(off:type)"))
for cls in sorted(ctors):
    b, mem = analyze(cls)
    memstr = " ".join("%s:%s" % (hex(o) if o!='?' else '?', t) for o,t in sorted(mem.items(), key=lambda x:(x[0]=='?',x[0])))
    if b or mem:
        print("%-20s %-14s %s" % (cls, ",".join(sorted(b)) or "-", memstr))
