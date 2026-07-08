#!/usr/bin/env python3
# name_strings.py - give string-literal constants their real MSVC ??_C@ name.
#
# VC4.2 names every string literal ??_C@_0<len>@<hash>@<chars> AT THE OBJECT LEVEL
# (verified: even with no /Gf|/GF). objdiff compares .objs, so if a delinked target
# constant gets the SAME ??_C@ name our base obj will emit for that literal, objdiff
# pairs them by name (no reloc-masking). The ??_C@ hash isn't a standard CRC, so we
# use cl as the exact oracle: compile each unique string, read which ??_C@ symbol its
# global points to via the .data relocation. Run once by `homm2 init` (needs wine cl).
#
# Rewrites build/gen/symbol_names.csv in place: rows whose unit is "_const" and whose
# bytes are a printable null-terminated string get name = the real ??_C@ symbol.
import os, re, struct, subprocess, sys, tempfile
from pathlib import Path
REPO = next((p for p in Path(__file__).resolve().parents if (p/"flake.nix").exists()), Path.cwd())
EXE = REPO/"build/orig/HEROES2W.EXE"; CSV = REPO/"build/gen/symbol_names.csv"
MSVC = os.environ.get("MSVC_DIR", str(REPO/"build/toolchain/msvc"))

def sections(d):
    e=struct.unpack_from("<I",d,0x3c)[0]; n=struct.unpack_from("<H",d,e+6)[0]; osz=struct.unpack_from("<H",d,e+20)[0]
    out=[]
    for i in range(n):
        o=e+24+osz+i*40; nm=d[o:o+8].rstrip(b"\0").decode("latin1")
        vs,va=struct.unpack_from("<II",d,o+8); pr=struct.unpack_from("<I",d,o+20)[0]
        out.append((nm,va,va+vs,pr))
    return out
def va2off(secs,rva):
    for nm,va,en,pr in secs:
        if va<=rva<en: return pr+(rva-va)
    return None
def read_str(d,secs,rva):
    o=va2off(secs,rva)
    if o is None: return None
    b=d[o:o+512]; z=b.find(b"\0")
    if 1<=z and all(c>=9 for c in b[:z]): return b[:z]
    return None
def cesc(b):
    r=[]
    for c in b:
        if c==0x22: r.append('\\"')
        elif c==0x5c: r.append('\\\\')
        elif 32<=c<127: r.append(chr(c))
        else: r.append('\\%03o'%c)
    return "".join(r)

def main():
    d=EXE.read_bytes(); secs=sections(d)
    rows=[ln.rstrip("\n") for ln in open(CSV)]
    hdr=rows[0]; body=rows[1:]
    # collect string constants: rva -> bytes  (unit == _const, is a string)
    const_rows=[]; uniq={}
    for i,ln in enumerate(body):
        f=ln.split(",")
        if len(f)<5 or f[2]!="_const": continue
        if not f[1].startswith("const_"): continue  # skip gen_manifest CONST_ALIASES (real globals inside a struct, e.g. gCdMusic in gConfig) — don't re-label them as strings
        rva=int(f[0],16); s=read_str(d,secs,rva)
        if s is None: continue
        const_rows.append((i,rva,s)); uniq.setdefault(bytes(s),None)
    keys=list(uniq)
    print(f"[name_strings] {len(const_rows)} string constants, {len(keys)} unique")
    # one global per unique string, in order -> .data reloc gives its ??_C@
    src="\n".join('const char* g%d = "%s";'%(i,cesc(k)) for i,k in enumerate(keys))+"\n"
    with tempfile.TemporaryDirectory() as td:
        td=Path(td); (td/"s.c").write_text(src)
        env=dict(os.environ, INCLUDE=f"{MSVC}/include")
        subprocess.run(["wine",f"{MSVC}/bin/CL.EXE","/nologo","/c","/O2","/MT","s.c"],
                       cwd=td, env=env, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        out=subprocess.run(["llvm-objdump","-r",str(td/"s.obj")],capture_output=True,text=True,errors="replace").stdout
    # relocs in .data: offset -> ??_C@ name; g<i> sits at .data offset 4*i (decl order)
    relocs={}
    for m in re.finditer(r"^([0-9a-f]+)\s+\S+\s+(\?\?_C@\S+)", out, re.M):
        relocs[int(m.group(1),16)]=m.group(2)
    offs=sorted(relocs)
    if len(offs)!=len(keys):
        print(f"[name_strings] WARN: {len(offs)} relocs vs {len(keys)} strings - mapping by sorted offset")
    name_of={keys[i]:relocs[off] for i,off in enumerate(offs) if i<len(keys)}
    # rewrite the rows
    n=0
    for i,rva,s in const_rows:
        nm=name_of.get(bytes(s))
        if nm:
            f=body[i].split(","); f[1]=nm; body[i]=",".join(f); n+=1
    Path(CSV).write_text(hdr+"\n"+"\n".join(body)+"\n")
    print(f"[name_strings] renamed {n} string constants to ??_C@ symbols")
    ex=next((rva for i,rva,s in const_rows if bytes(s)==b"brothers.icn"),None)
    if ex: print(f"[name_strings] e.g. brothers.icn -> {name_of.get(b'brothers.icn')}")

if __name__=="__main__": main()
