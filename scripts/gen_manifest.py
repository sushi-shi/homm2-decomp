#!/usr/bin/env python3
# gen_manifest.py  PE  REPO  [SRCPATHS]
# Generate the matching build manifest straight from the CodeView NB09 debug
# stream — no Ghidra. Emits:
#   config/symbol_names.csv   rva,name,unit,size,kind   (RAW MSVC-mangled names,
#                             RVA relative to image base — what cl emits / what
#                             synth_pdb + vostok-delinker + objdiff key on)
#   config/units.toml         one [[unit]] per NWC compiland with src/<tier>/<stem>.cpp
import sys, struct, re, os, bisect
from collections import defaultdict
pe = sys.argv[1]; REPO = sys.argv[2]; srcpaths = sys.argv[3] if len(sys.argv)>3 else None
d = open(pe, "rb").read()

# ---- PE ----
e=struct.unpack_from("<I",d,0x3c)[0]; nsec=struct.unpack_from("<H",d,e+6)[0]; osz=struct.unpack_from("<H",d,e+20)[0]
imgbase=struct.unpack_from("<I",d,e+24+28)[0]
sections=[]
for i in range(nsec):
    o=e+24+osz+i*40
    sections.append((d[o:o+8].rstrip(b"\0").decode("latin1"),
        struct.unpack_from("<I",d,o+12)[0], struct.unpack_from("<I",d,o+8)[0],
        struct.unpack_from("<I",d,o+20)[0], struct.unpack_from("<I",d,o+16)[0]))
def sec_of(rva):
    for nm,va,vs,pr,rs in sections:
        if va<=rva<va+max(vs,rs): return nm
    return None
def va2off(VA):
    r=VA-imgbase
    for nm,va,vs,pr,rs in sections:
        if va<=r<va+max(vs,rs):
            o=pr+(r-va); return o if 0<=o<len(d) else None
    return None
def s2va(seg,off): return imgbase+sections[seg-1][1]+off if 1<=seg<=len(sections) else None
TVA=TEND=None
for nm,va,vs,pr,rs in sections:
    if nm.startswith(".text"): TVA,TEND=imgbase+va,imgbase+va+vs

# ---- CodeView ----
tail=d.rfind(b"NB09"); lfoBase=len(d)-struct.unpack_from("<I",d,tail+4)[0]
D=lfoBase+struct.unpack_from("<I",d,lfoBase+4)[0]
cbH,cbE=struct.unpack_from("<HH",d,D); cDir=struct.unpack_from("<I",d,D+4)[0]
ents=[struct.unpack_from("<HHii",d,D+cbH+i*cbE) for i in range(cDir)]
libs=[""]
for sst,iMod,lfo,cb in ents:
    if sst!=0x128: continue
    b=d[lfoBase+lfo:lfoBase+lfo+cb]; p=0
    while p<len(b):
        n=b[p]
        if n==0: p+=1; continue
        libs.append(b[p+1:p+1+n].decode("latin1","replace")); p+=1+n
modname={}; modrange={}; modlib={}
for sst,iMod,lfo,cb in ents:
    if sst!=0x120: continue
    b=d[lfoBase+lfo:lfoBase+lfo+cb]
    iLib=struct.unpack_from("<H",b,2)[0]; cSeg=struct.unpack_from("<H",b,4)[0]; p=8; rngs=[]
    for j in range(cSeg):
        seg,pad,off,cbseg=struct.unpack_from("<HHII",b,p); p+=12
        va=s2va(seg,off)
        if va: rngs.append((va,va+cbseg))
    nl=b[p]; modname[iMod]=b[p+1:p+1+nl].decode("latin1","replace"); modrange[iMod]=rngs
    modlib[iMod]=libs[iLib] if iLib<len(libs) else ""
def which(va):
    for im,rngs in modrange.items():
        for s,en in rngs:
            if s<=va<en: return im
    return None
def pstr(buf,p): n=buf[p]; return buf[p+1:p+1+n].decode("latin1"), p+1+n
pubs=[]
for sst,iMod,lfo,cb in ents:
    if sst!=0x12a: continue
    blob=d[lfoBase+lfo:lfoBase+lfo+cb]; cbS=struct.unpack_from("<I",blob,4)[0]; buf=blob[16:16+cbS]; p=0
    while p+4<=len(buf):
        rl,rt=struct.unpack_from("<HH",buf,p)
        if rl==0: break
        body=buf[p+4:p+2+rl]
        if rt==0x0203:
            off,seg=struct.unpack_from("<IH",body,0); nm,_=pstr(body,8); va=s2va(seg,off)
            if nm and va: pubs.append((va,nm))
        p+=2+rl
pubs.sort()

# ---- harvest reloc-target constants (vostok-delinker: "all constants must be
# named"). Every .reloc fixup stores an absolute VA; targets landing in a data
# section that aren't already a named symbol get a synthetic name so the delinker
# can emit a named external ref (the name mismatch vs our base obj is absorbed by
# reloc-masking during matching). This is a CodeView-side constant inventory.
named_va = set(v for v, _ in pubs)
ddir = e + 24 + 96
rrva, rsize = struct.unpack_from("<II", d, ddir + 5 * 8)
consts = set()
if rsize:
    o = va2off(imgbase + rrva); p = o; end = o + rsize
    while p < end:
        page, blk = struct.unpack_from("<II", d, p)
        if blk == 0: break
        for i in range((blk - 8) // 2):
            w = struct.unpack_from("<H", d, p + 8 + i * 2)[0]
            if w >> 12:
                fo = va2off(imgbase + page + (w & 0xfff))
                if fo is not None and fo + 4 <= len(d):
                    tgt = struct.unpack_from("<I", d, fo)[0]
                    sc = sec_of(tgt - imgbase)
                    if sc in (".rdata", ".data") and tgt not in named_va:
                        consts.add(tgt)
        p += blk

# ---- sizes ----
text_syms=sorted(va for va,_ in pubs if (sec_of(va-imgbase) or "").startswith(".text"))
def fsize(va):
    i=bisect.bisect_right(text_syms,va); nxt=text_syms[i] if i<len(text_syms) else TEND
    o=va2off(va)
    return len(d[o:o+(nxt-va)].rstrip(b"\xcc\x90")) if o is not None else 0
allva=sorted(set(v for v,_ in pubs))
def dsize(va):  # data extent = gap to next symbol (best effort)
    i=bisect.bisect_right(allva,va); return (allva[i]-va) if i<len(allva) else 4

# ---- tier placement: the committed src/<TIER>/ tree IS the tier map (every
# reconstruction compiland has a src/<TIER>/<stem>.cpp). lib-based fallback for any
# compiland that has no src file yet (basewin.lib -> BASE; directly-linked -> SOURCE).
src_tier={}
for _t in ("BASE","SOURCE","EDITOR"):
    _dd=os.path.join(REPO,"src",_t)
    if os.path.isdir(_dd):
        for _fn in os.listdir(_dd):
            if _fn.lower().endswith((".cpp",".c")): src_tier[_fn.rsplit(".",1)[0].lower()]=_t
def stem_of(name):
    b=name.replace("\\","/").split("/")[-1]; return b[:-4] if b.lower().endswith(".obj") else b
def tier_of(im):
    if im is None: return None
    lib=modlib.get(im,""); st=stem_of(modname.get(im,"")).lower()
    if st in src_tier: return src_tier[st]
    if "basewin" in lib.lower(): return "BASE"
    if lib=="": return "SOURCE"
    return None   # external/CRT — not a reconstruction unit
def unit_of(im):
    # unit identifier carries its tier folder so the delinker mirrors src/:
    # NWC reconstruction units -> "<TIER>/<stem>" (-> build/delink/<TIER>/<stem>.c.obj);
    # external/CRT/synthetic stay flat.
    if im is None: return "unknown"
    t=tier_of(im); st=stem_of(modname.get(im,""))
    return f"{t}/{st}" if t in ("BASE","SOURCE","EDITOR") else st

# ---- per-TU optimization level (empirical, from the PE itself) -------------
# Retail optimization is PER-COMPILAND, not uniform /Od: the basewin.lib UI
# framework (most of BASE) + a couple SOURCE units shipped /O2 (FPO, register
# allocation, intrinsic strlen), while the game logic (SOURCE/*) and EDITOR are
# /Od (full ebp frames). The CodeView S_COMPILE record does NOT encode opt level,
# so classify by the prologue: a /Od function opens `push ebp; mov ebp,esp`
# (55 8b ec); an /O2 (/Oy) function omits the frame pointer. The split is clean
# per-TU (no mixed compilands), so a simple majority vote is exact.
def is_od_func(va):
    o=va2off(va)
    return o is not None and o+3<=len(d) and d[o]==0x55 and d[o+1]==0x8b and d[o+2]==0xec
unit_funcs=defaultdict(list)
for _va,_raw in pubs:
    if (sec_of(_va-imgbase) or "").startswith(".text"):
        unit_funcs[unit_of(which(_va))].append(_va)
def profile_of(unit):
    fns=unit_funcs.get(unit,[])
    if not fns: return "base"            # data-only TU: harmless default
    od=sum(1 for va in fns if is_od_func(va))
    return "base" if od*2>=len(fns) else "o2"

# existing src files: stem(lower) -> repo-relative path
srcfiles={}
for tier in ("BASE","SOURCE","EDITOR"):
    dd=os.path.join(REPO,"src",tier)
    if os.path.isdir(dd):
        for fn in os.listdir(dd):
            if fn.lower().endswith((".cpp",".c")): srcfiles[fn.rsplit(".",1)[0].lower()]=f"src/{tier}/{fn}"

# ---- emit symbol_names.csv ----
os.makedirs(os.path.join(REPO,"build","gen"),exist_ok=True); os.makedirs(os.path.join(REPO,"config"),exist_ok=True)
n_func=n_data=0
with open(os.path.join(REPO,"build","gen","symbol_names.csv"),"w") as f:
    f.write("rva,name,unit,size,kind\n")
    for va,raw in pubs:
        im=which(va); unit=unit_of(im)
        rva=va-imgbase; s=sec_of(rva) or ""
        if raw.startswith("??_7") or raw.startswith("??_R") or not s.startswith(".text"):
            kind="data"; size=dsize(va); n_data+=1
        else:
            kind="func"; size=fsize(va); n_func+=1
        f.write(f"0x{rva:x},{raw},{unit},0x{size:x},{kind}\n")
    n_const = 0
    for tgt in sorted(consts):
        rva = tgt - imgbase
        f.write(f"0x{rva:x},const_{rva:08x},_const,0x0,data\n"); n_const += 1; n_data += 1

# ---- emit units.toml (NWC reconstruction units only) ----
units=[]
for im,name in modname.items():
    if not name.lower().endswith(".obj"): continue
    tier=tier_of(im)
    if tier not in ("BASE","SOURCE","EDITOR"): continue
    st=stem_of(name)
    src=srcfiles.get(st.lower())
    if src: units.append((f"{tier}/{st}", src))
units.sort()
with open(os.path.join(REPO,"config","units.toml"),"w") as f:
    f.write("# units.toml - per-TU build manifest (generated from CodeView by scripts/gen_manifest.py).\n")
    f.write("# unit MUST match the unit column in config/symbol_names.csv.\n\n")
    f.write('[build]\ncompiler = "msvc4.2"\nplatform = "win32"\n\n')
    f.write("[flags]\n")
    # Retail is a DEBUG build: /Od (full ebp frames, every local spilled) + /Gr
    # (__fastcall default; 458 free fns mangle @@YI vs 6 cdecl) + /MT (static LIBCMT)
    # + /G5 (Pentium target) + /Ob1 (inline expansion ON). No C++ EH, no RTTI, no /O2.
    #
    # /G5 is GLOBAL (all three NWC tiers), proven by the Pentium "avoid MOVZX" rule:
    # /G5 zero-extends unsigned 16->32 with AND, not MOVZX (non-pairable/slow on P5) -
    # `movw;shrw;andw $mask;andl $0xffff` where /GB-blend emits `...;movzwl`. The .text
    # has 190 AND-zero-extends spanning the whole SOURCE+BASE range vs only 3 MOVZX in
    # ~1500 NWC funcs (the linked CRT, Microsoft-built, still uses MOVZX - a built-in
    # control).
    #
    # /Ob1 (inline expansion) is a SEPARATE axis from /Od (optimization): unoptimized
    # but inlined. Proven on EDITOR/mapcell - the retail .text is littered with `jmp $+0`
    # (e9 00000000, jump-to-next) that plain /Od never emits; they are the per-call-site
    # continuation jumps of inlined in-class accessors. Reconstructing those accessors
    # (fullMap::Row/Extra) + /Ob1 lifts GetNewCellExtra* from a structurally-capped ~91%
    # to ~97% (addressing now exact; residual is jmp-placement). /Ob1 not /Ob2: retail
    # still emits real calls to out-of-line methods (e.g. GetNewCellExtraIndex), which
    # /Ob1 leaves alone.
    #
    # Optimization is PER-TU (profile_of, above). The basewin.lib UI framework (most of
    # BASE) + SOURCE/{FINDPATH,SEARCH} shipped /O2 (FPO + register allocation + intrinsic
    # strlen via `repne scasb`); the rest (SOURCE game logic, EDITOR) is /Od. /O2 already
    # implies /Oy/Oi/Og/Ot/Ob1, so the o2 profile drops the explicit /Od and /Ob1.
    # Proven on BASE/Misc: switching it to o2 took MemSize/SRand/Random/MAKEFILEID/... to
    # byte-exact (they were ~0% under /Od - wrong frame shape).
    f.write('base = ["/nologo", "/c", "/Od", "/MT", "/Gr", "/G5", "/Ob1"]\n')
    f.write('o2 = ["/nologo", "/c", "/O2", "/MT", "/Gr", "/G5"]\n\n')
    for st,src in units:
        f.write(f'[[unit]]\nunit = "{st}"\nsource = "{src}"\nflags = "{profile_of(st)}"\n\n')

print(f"symbol_names.csv: {n_func} funcs + {n_data} data = {n_func+n_data} symbols")
print(f"units.toml: {len(units)} NWC reconstruction units")
miss=[stem_of(modname[im]) for im in modname if modname[im].lower().endswith('.obj')
      and tier_of(im) in ('BASE','SOURCE','EDITOR') and stem_of(modname[im]).lower() not in srcfiles]
if miss: print(f"WARN: {len(miss)} units have no src file: {miss[:8]}")
