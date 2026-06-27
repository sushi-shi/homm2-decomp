#!/usr/bin/env python3
# cv-nb09-materialize.py  PE  DEMANGLED_ADDRMAP  OUTROOT
# Materialize a reconstructed source tree from an embedded CodeView NB09 stream:
#   - one .cpp/.c per compiland (translation unit), under OUTROOT mirroring paths
#   - every function annotated:  VA(0xADDR, 0xSIZE)
#       (size = bytes to next .text symbol, trailing 0xCC/0x90 padding trimmed)
#   - per-class vtable layouts (walked via relocations) with each slot labelled
#       "new virtual" / "override of Base::m" / "inherited from Base"
#     emitted into the class's home TU AND collected in OUTROOT/VTABLES.txt
import sys, struct, re, os, bisect
from collections import defaultdict
pe, addrmap_file, outroot = sys.argv[1], sys.argv[2], sys.argv[3]
d = open(pe, "rb").read()

# ---------- PE ----------
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
            off=pr+(r-va); return off if 0<=off<len(d) else None
    return None
def s2va(seg,off): return imgbase+sections[seg-1][1]+off if 1<=seg<=len(sections) else None
TVA=TEND=None
for nm,va,vs,pr,rs in sections:
    if nm.startswith(".text"): TVA,TEND=imgbase+va, imgbase+va+vs

# ---------- relocations (set of fixup VAs => "this dword is a pointer") ----------
ddir=e+24+96; rrva,rsize=struct.unpack_from("<II",d,ddir+5*8); reloc=set()
if rsize:
    off=va2off(imgbase+rrva); p=off; end=off+rsize
    while p<end:
        page,blk=struct.unpack_from("<II",d,p)
        if blk==0: break
        for i in range((blk-8)//2):
            w=struct.unpack_from("<H",d,p+8+i*2)[0]
            if w>>12: reloc.add(imgbase+page+(w&0xfff))
        p+=blk

# ---------- CodeView ----------
tail=d.rfind(b"NB09"); lfoBase=len(d)-struct.unpack_from("<I",d,tail+4)[0]
D=lfoBase+struct.unpack_from("<I",d,lfoBase+4)[0]
cbH,cbE=struct.unpack_from("<HH",d,D); cDir=struct.unpack_from("<I",d,D+4)[0]
ents=[struct.unpack_from("<HHii",d,D+cbH+i*cbE) for i in range(cDir)]
# static libraries (sstLibraries) — modules carry a 1-based index (iLib) into this list
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
    modlib[iMod]= libs[iLib] if iLib<len(libs) else ""
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

pretty={}
for line in open(addrmap_file,errors="replace"):
    if line.startswith("0x"):
        a,nm=line.rstrip("\n").split("\t",1); pretty[int(a,16)]=nm
name_at={va:pretty.get(va,raw) for va,raw in pubs}
raw_at={va:raw for va,raw in pubs}

# ---------- function sizes (next .text symbol, trim 0xCC/0x90 padding) ----------
text_syms=sorted(va for va,_ in pubs if (sec_of(va-imgbase) or "").startswith(".text"))
def fsize(va):
    i=bisect.bisect_right(text_syms,va); nxt=text_syms[i] if i<len(text_syms) else TEND
    o=va2off(va)
    if o is None: return 0
    region=d[o:o+(nxt-va)].rstrip(b"\xcc\x90")
    return len(region)

# ---------- classify + attribute to modules ----------
def which(va):
    for im,rngs in modrange.items():
        for s,en in rngs:
            if s<=va<en: return im
    return None
def cleandecl(name):
    s=re.sub(r'^(public|protected|private)(\s*virtual)?:\s*','',name)
    s=re.sub(r'\b__(thiscall|cdecl|stdcall|fastcall)\b\s*','',s); return s.strip()
def cls_of(name):
    s=re.sub(r'\(.*','',name)  # drop params
    m=re.search(r'([A-Za-z_]\w*)::[~A-Za-z_]\w*\s*$', s)
    return m.group(1) if m else None

# ---------- walk vtables ----------
allsym=sorted(set(va for va,_ in pubs))
def next_sym(va):
    i=bisect.bisect_right(allsym,va); return allsym[i] if i<len(allsym) else None
vtables={}     # class -> list of (slotidx, fva, fname, fclass)
vt_addr={}     # class -> vtable VA
for va,raw in pubs:
    if not raw.startswith("??_7"): continue
    m=re.match(r'\?\?_7([A-Za-z_]\w*)@@6B', raw);
    if not m: continue
    C=m.group(1); slots=[]; cur=va; i=0; stop=next_sym(va)
    while True:
        o=va2off(cur)
        if o is None or cur not in reloc: break
        if cur!=va and stop and cur>=stop: break
        ptr=struct.unpack_from("<I",d,o)[0]
        if not (TVA and TVA<=ptr<TEND): break
        fn=name_at.get(ptr) or f"sub_{ptr:08x}"
        slots.append((i,ptr,fn,cls_of(fn))); cur+=4; i+=1
    if slots: vtables[C]=slots; vt_addr[C]=va

# infer direct base: the candidate vtable that is a compatible prefix AND shares the
# most *identical* (inherited-unchanged) slot pointers. Require shared>0, else the base
# is unprovable from vtables alone (a class that overrides every slot is ambiguous).
def infer_base(C):
    vC=vtables[C]; best=None; bestscore=(0,-1)
    for B,vB in vtables.items():
        if B==C or len(vB)>len(vC): continue
        shared=0; ok=True
        for i in range(len(vB)):
            if vC[i][1]==vB[i][1]: shared+=1; continue   # inherited, unchanged
            if vC[i][3]==C: continue                      # C overrides this slot
            if vC[i][3]==B: continue                      # still B's impl
        score=(shared,len(vB))
        if shared>0 and score>bestscore: best,bestscore=B,score
    return best
vbase={C:infer_base(C) for C in vtables}
# break 2-cycles (identical vtables): the base is the class that implements its own slots
_own=lambda X: sum(1 for s in vtables[X] if s[3]==X)
for C in list(vbase):
    B=vbase[C]
    if B and vbase.get(B)==C:
        if _own(C)>=_own(B): vbase[C]=None
        else: vbase[B]=None
# abstract interfaces = vtables that are entirely pure-virtual (__purecall); subclasses
# override every slot (sharing no concrete pointer) so link them by slot-0 convention.
def is_pure(fn): return "purecall" in fn.lower()
abstract={C for C in vtables if vtables[C] and all(is_pure(s[2]) for s in vtables[C])}
def slot0name(C):
    s=cleandecl(vtables[C][0][2]); return s.split('::')[-1].split('(')[0] if '::' in s else ''
IFACE={"Draw":"widget","Open":"baseManager"}
for C in vtables:
    if vbase.get(C) is None and C not in abstract:
        ib=IFACE.get(slot0name(C))
        if ib in abstract and ib!=C: vbase[C]=ib
def slot_label(C,B,i,fc,ptr,fn):
    if is_pure(fn): return "= 0 (pure virtual)"
    if fc==C:
        if B and i<len(vtables[B]):
            bs=vtables[B][i][2]
            if is_pure(bs): return f"override (implements {B} pure virtual)"
            return "override of %s::%s"%(B, cleandecl(bs).split('::')[-1].split('(')[0])
        return "new virtual" if B else "introduces virtual"
    return "inherited from %s"%fc if fc else "inherited"
# per-function virtual/override info  fva -> label
vlabel={}
vt_text=[]   # lines for VTABLES.txt
for C in sorted(vtables):
    B=vbase.get(C); bl=len(vtables[B]) if B else 0
    vt_text.append(f"\n===== vtable {C}  @ 0x{vt_addr[C]:08x}   ({len(vtables[C])} slots)"
                   + (f"   : public {B}" if B else "   (root)") + " =====")
    for i,ptr,fn,fc in vtables[C]:
        tag=slot_label(C,B,i,fc,ptr,fn)
        vlabel.setdefault(ptr, []).append((C,i,tag))
        vt_text.append(f"  [{i:2}] VA(0x{ptr:08x}, 0x{fsize(ptr):x})  {cleandecl(fn)}   <- {tag}")
# class -> home TU (where its ctor lives, else where its vtable lives)
home={}
for va,raw in pubs:
    m=re.match(r'\?\?0([A-Za-z_]\w*)@@', raw)   # constructor
    if m and m.group(1) not in home:
        im=which(va);  home[m.group(1)]=im if im else which(vt_addr.get(m.group(1),0))
for C in vtables:
    home.setdefault(C, which(vt_addr[C]))

# ---------- emit files ----------
# --- reconstruct the SOURCE directory tier the .obj build dir flattened away ---------
# evidence, in priority order:
#   1. assert __FILE__ paths gathered across builds (basename -> BASE/SOURCE/EDITOR)
#   2. CodeView library membership: basewin.lib = BASE tier, directly-linked = SOURCE,
#      everything else is a third-party / CRT lib object (segregated under _external/).
assert_dir={}   # basename(lower w/ ext) -> subdir (upper)
if len(sys.argv)>4 and os.path.exists(sys.argv[4]):
    for line in open(sys.argv[4]):
        line=line.strip().lower().replace("/", "\\")
        if "\\" in line:
            sub,fn=line.rsplit("\\",1); assert_dir[fn]=sub.upper()
def placement(modname_, lib):
    base=modname_.replace("\\","/").split("/")[-1]
    stem=base[:-4] if base.lower().endswith(".obj") else base
    libl=lib.lower(); libbase=lib.replace("\\","/").split("/")[-1]
    crt = ("libcmt" in libl or "oldnames" in libl or "mt_obj" in modname_.lower())
    # source name + extension
    for ext in (".cpp",".c",".cxx"):
        if (stem.lower()+ext) in assert_dir:
            return [assert_dir[stem.lower()+ext]], stem+ext
    ext=".c" if crt else ".cpp"
    cand=stem.lower()+".cpp"
    if cand in assert_dir:           return [assert_dir[cand]], stem+ext
    if "basewin" in libl:            return ["BASE"], stem+ext      # NWC base/UI library
    if lib=="":                      return ["SOURCE"], stem+ext    # iLib=0 => game source
    libnm=libbase[:-4] if libbase.lower().endswith(".lib") else libbase
    return ["_external", libnm], stem+ext
bymod=defaultdict(list)
for va,raw in pubs: bymod[which(va)].append(va)
home_by_mod=defaultdict(list)
for C,im in home.items():
    if im is not None: home_by_mod[im].append(C)

os.makedirs(outroot, exist_ok=True)
open(os.path.join(outroot,"VTABLES.txt"),"w").write(
    "# Recovered C++ vtables (PoL HEROES2W.EXE), slots labelled override/new/inherited\n"+"\n".join(vt_text)+"\n")
nfiles=0
for im,name in modname.items():
    if not name.lower().endswith(".obj"): continue
    dirs,fn=placement(name, modlib.get(im,"")); dd=os.path.join(outroot,*dirs); os.makedirs(dd,exist_ok=True)
    syms=sorted(bymod.get(im,[]))
    funcs=[v for v in syms if (sec_of(v-imgbase) or "").startswith(".text")]
    data =[v for v in syms if v not in set(funcs)]
    with open(os.path.join(dd,fn),"w") as f:
        lib=modlib.get(im,"") or "(directly linked into exe)"
        f.write(f"// Reconstructed from CodeView NB09 of {os.path.basename(pe)} — NOT original source.\n")
        f.write(f"// compiland: {name}   from: {lib}\n")
        f.write(f"// functions: {len(funcs)}   data: {len(data)}\n")
        f.write("// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.\n\n")
        for va in funcs:
            decl=cleandecl(name_at.get(va,f"sub_{va:08x}"))
            tags=vlabel.get(va)
            note=""
            if tags:
                C,i,tag=tags[0]; note=f"   // virtual [{tag}]" + (f"  (+{len(tags)-1} more)" if len(tags)>1 else "")
            f.write(f"VA(0x{va:08x}, 0x{fsize(va):x})\n{decl};{note}\n\n")
        if data:
            f.write("// ---- data / globals / vtables ----\n")
            for va in data:
                f.write(f"DATA(0x{va:08x})  // {cleandecl(name_at.get(va,hex(va)))}\n")
        # vtable layouts for classes homed here
        for C in sorted(home_by_mod.get(im,[])):
            if C not in vtables: continue
            B=vbase.get(C); bl=len(vtables[B]) if B else 0
            f.write(f"\n// ===== vtable {C}" + (f" : public {B}" if B else " (root)") + f"  ({len(vtables[C])} slots) =====\n")
            for i,ptr,fn2,fc in vtables[C]:
                tag=slot_label(C,B,i,fc,ptr,fn2)
                f.write(f"//  [{i:2}] VA(0x{ptr:08x}, 0x{fsize(ptr):x})  {cleandecl(fn2)}   <- {tag}\n")
    nfiles+=1
print(f"wrote {nfiles} source files under {outroot}/")
print(f"vtables recovered: {len(vtables)}  (see {outroot}/VTABLES.txt)")
print("classes:", ", ".join(sorted(vtables)))
