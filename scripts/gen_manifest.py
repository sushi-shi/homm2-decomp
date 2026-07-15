#!/usr/bin/env python3
# gen_manifest.py  PE  REPO  [SRCPATHS]
# Generate the matching build manifest straight from the CodeView NB09 debug
# stream — no Ghidra. Emits:
#   build/gen/symbol_names.csv rva,name,unit,size,kind,provenance (RAW MSVC names,
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

# ---- per-module procedures -------------------------------------------------------
# Public symbols do not carry lengths. Prefer the true pLen from a per-module
# S_GPROC32/S_LPROC32/S_THUNK32 record whenever NB09 provides one. This retail
# happens to contain only import S_THUNK32 records, but the parser is complete:
# never use a next-public span when a procedure record exists at that RVA.
proc_records=[]
for sst,iMod,lfo,cb in ents:
    if sst!=0x125: continue                       # sstAlignSym
    blob=d[lfoBase+lfo:lfoBase+lfo+cb]; p=4      # four-byte alignment signature
    while p+4<=len(blob):
        rl,rt=struct.unpack_from("<HH",blob,p)
        if rl==0: break
        body=blob[p+4:p+2+rl]
        try:
            if rt in (0x0204,0x0205):            # S_LPROC32 / S_GPROC32 (CV4)
                length=struct.unpack_from("<I",body,12)[0]
                off,seg=struct.unpack_from("<IH",body,24)
                nm,_=pstr(body,33); va=s2va(seg,off)
                if va and length:
                    proc_records.append((va,length,nm,iMod,
                                         "cv-lproc32" if rt==0x0204 else "cv-gproc32"))
            elif rt==0x0206:                     # S_THUNK32 (CV4)
                off,seg,length=struct.unpack_from("<IHH",body,12)
                nm,_=pstr(body,21); va=s2va(seg,off)
                if va and length:
                    proc_records.append((va,length,nm,iMod,"cv-thunk32"))
        except (IndexError,struct.error):
            pass
        p+=2+rl

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

# Procedures absent from CodeView are admitted only through this reviewed
# manifest. Each row has an explicit executable span and evidence provenance;
# no address is inferred from padding, jump-table bytes, or a source-only label.
validated_procs=[]
vp=os.path.join(REPO,"config","delink_procedures.csv")
if os.path.exists(vp):
    import csv
    with open(vp,newline="") as vf:
        rows=(ln for ln in vf if not ln.lstrip().startswith("#"))
        for row in csv.DictReader(rows):
            rva=int(row["rva"],16); va=imgbase+rva; size=int(row["size"],16)
            name=row["name"].strip() or ("delink_proc_%08x"%rva)
            unit=row["unit"].strip()
            proof=row["provenance"].strip()
            if size<=0 or not name or not unit or not proof:
                raise SystemExit("invalid delink procedure row at 0x%x"%rva)
            if sec_of(rva)!=".text":
                raise SystemExit("delink procedure outside .text: 0x%x"%rva)
            contribution_unit=unit_of(which(va))
            if contribution_unit!=unit:
                raise SystemExit("delink procedure 0x%x belongs to %s, not %s"%
                                 (rva,contribution_unit,unit))
            validated_procs.append((va,size,name,unit,"validated-"+proof))
    seen=set()
    for va,size,name,unit,proof in sorted(validated_procs):
        key=(va,name)
        if key in seen:
            raise SystemExit("duplicate delink procedure: 0x%x %s"%(va-imgbase,name))
        seen.add(key)
        o=va2off(va); body=d[o:o+size] if o is not None else b""
        if len(body)!=size or body[:1] in (b"\xcc",b"\x90"):
            raise SystemExit("delink procedure is padding/truncated: 0x%x %s"%(va-imgbase,name))
    spans=sorted((va,va+size,name) for va,size,name,_,_ in validated_procs)
    for (_,end,name),(start2,_,name2) in zip(spans,spans[1:]):
        if start2<end:
            raise SystemExit("overlapping delink procedures: %s and %s"%(name,name2))

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
    # A frame-pointer majority is not a complete optimization fingerprint. All
    # public functions in these four units currently have 55 8b ec prologues,
    # yet the empirically selected reproduction profiles below differ (BITS
    # /O1 with frame pointers; TILE /O2; FONT/RESMGR /Od plus /Oi). CodeView does
    # not prove the literal optimization flags. Keep the selected profiles across init.
    established={"BASE/BITS":"o1_frame", "BASE/FONT":"base_oi",
                 "BASE/RESMGR":"base_oi", "BASE/TILE":"o2"}
    if unit in established: return established[unit]
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
    f.write("rva,name,unit,size,kind,provenance\n")
    procs_by_va=defaultdict(list)
    for rec in proc_records: procs_by_va[rec[0]].append(rec)
    validated_by_key={(va,name):(size,unit,proof)
                      for va,size,name,unit,proof in validated_procs}
    emitted=set()
    for va,raw in pubs:
        im=which(va); unit=unit_of(im)
        rva=va-imgbase; s=sec_of(rva) or ""
        if raw.startswith("??_7") or raw.startswith("??_R") or not s.startswith(".text"):
            kind="data"; size=dsize(va); provenance="cv-public-data"; n_data+=1
        else:
            kind="func"; n_func+=1
            override=validated_by_key.get((va,raw))
            records=procs_by_va.get(va,[])
            if override:
                size,unit,provenance=override
                provenance="cv-public-"+provenance
            elif records:
                lengths={rec[1] for rec in records}
                if len(lengths)!=1:
                    raise SystemExit("conflicting CodeView pLen at 0x%x"%rva)
                size=next(iter(lengths)); unit=unit_of(records[0][3]); provenance=records[0][4]
            else:
                size=fsize(va); provenance="cv-public-gap"
        f.write(f"0x{rva:x},{raw},{unit},0x{size:x},{kind},{provenance}\n")
        emitted.add((va,raw))
    public_rvas={va for va,_ in pubs}
    for va,size,name,im,provenance in sorted(proc_records):
        if va in public_rvas: continue            # public decorated alias is the COFF name
        raw=name or ("delink_%s_%08x"%(provenance[3:],va-imgbase))
        if (va,raw) in emitted: continue
        f.write(f"0x{va-imgbase:x},{raw},{unit_of(im)},0x{size:x},func,{provenance}\n")
        emitted.add((va,raw)); n_func+=1
    for va,size,raw,unit,provenance in sorted(validated_procs):
        if (va,raw) in emitted: continue           # reviewed same-RVA size override
        f.write(f"0x{va-imgbase:x},{raw},{unit},0x{size:x},func,{provenance}\n")
        emitted.add((va,raw)); n_func+=1
    n_const = 0
    for tgt in sorted(consts):
        rva = tgt - imgbase
        f.write(f"0x{rva:x},const_{rva:08x},_const,0x0,data,pe-reloc-constant\n")
        n_const += 1; n_data += 1

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
    f.write("# unit MUST match the unit column in build/gen/symbol_names.csv.\n\n")
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
    #
    # /QIfdiv (Pentium FDIV-bug workaround) is GLOBAL (both tiers): retail wraps every
    # float divide with the `cmp __adjust_fdiv,0 / jne / __adj_fdiv_r` guard (PHILAI alone
    # has 53 such sites; "__adjust_fdiv" is in the exe's string table). Without it our objs
    # emit 0 guards. Adding it lifted philAI::ManaRefreshValue 91% -> 100% with zero
    # regressions across all 95 units (status check clean) - guards only appear around
    # float divides, so a function that already matches without one is unaffected.
    f.write('base = ["/nologo", "/c", "/Od", "/MT", "/Gr", "/G5", "/Ob1", "/QIfdiv"]\n')
    f.write('o2 = ["/nologo", "/c", "/O2", "/MT", "/Gr", "/G5", "/QIfdiv"]\n')
    f.write('o1_frame = ["/nologo", "/c", "/O1", "/Oy-", "/MT", "/Gr", "/G5", "/QIfdiv"]\n')
    f.write('base_oi = ["/nologo", "/c", "/Od", "/MT", "/Gr", "/G5", "/Ob1", "/QIfdiv", "/Oi"]\n\n')
    for index,(st,src) in enumerate(units):
        if index:
            f.write('\n')
        f.write(f'[[unit]]\nunit = "{st}"\nsource = "{src}"\nflags = "{profile_of(st)}"\n')

print(f"symbol_names.csv: {n_func} funcs + {n_data} data = {n_func+n_data} symbols")
print(f"units.toml: {len(units)} NWC reconstruction units")
miss=[stem_of(modname[im]) for im in modname if modname[im].lower().endswith('.obj')
      and tier_of(im) in ('BASE','SOURCE','EDITOR') and stem_of(modname[im]).lower() not in srcfiles]
if miss: print(f"WARN: {len(miss)} units have no src file: {miss[:8]}")
