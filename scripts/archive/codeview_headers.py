#!/usr/bin/env python3
# cv-nb09-headers.py  PE  DEMANGLED_ADDRMAP  OUTDIR  [SRCPATHS]
# Emit one C++ header per recovered class, placed in BASE/SOURCE/EDITOR tiers (mirroring
# the reconstructed sources). Clean declarations — no RVA/size (those live in the .cpp):
#   class C : public Base { ctors; virtual ~C(); virtual methods (vtable order, OVERRIDE
#   where they override a base slot); methods; static data; };
import sys, struct, re, os, bisect
from collections import defaultdict
pe, addrmap_file, outdir = sys.argv[1], sys.argv[2], sys.argv[3]
srcpaths = sys.argv[4] if len(sys.argv)>4 else None
d = open(pe, "rb").read()
os.makedirs(outdir, exist_ok=True)

# ---------- PE + CodeView ----------
e=struct.unpack_from("<I",d,0x3c)[0]; nsec=struct.unpack_from("<H",d,e+6)[0]; osz=struct.unpack_from("<H",d,e+20)[0]
imgbase=struct.unpack_from("<I",d,e+24+28)[0]
sections=[(d[e+24+osz+i*40:e+24+osz+i*40+8].rstrip(b"\0").decode("latin1"),
    struct.unpack_from("<I",d,e+24+osz+i*40+12)[0], struct.unpack_from("<I",d,e+24+osz+i*40+8)[0],
    struct.unpack_from("<I",d,e+24+osz+i*40+20)[0], struct.unpack_from("<I",d,e+24+osz+i*40+16)[0]) for i in range(nsec)]
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
tail=d.rfind(b"NB09"); lfoBase=len(d)-struct.unpack_from("<I",d,tail+4)[0]
D=lfoBase+struct.unpack_from("<I",d,lfoBase+4)[0]
cbH,cbE=struct.unpack_from("<HH",d,D); cDir=struct.unpack_from("<I",d,D+4)[0]
ents=[struct.unpack_from("<HHii",d,D+cbH+i*cbE) for i in range(cDir)]
def pstr(buf,p): n=buf[p]; return buf[p+1:p+1+n].decode("latin1"), p+1+n

# libraries + modules (for tier placement)
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

# raw mangled name per VA — lets us read a method's virtualness straight from the MSVC
# access char instead of only from vtable slot targets. This is ESSENTIAL for pure
# virtuals: an abstract class's own slot for a pure virtual points at __purecall (not the
# method), so slot-target scanning alone misfiles pure-virtual-WITH-body methods
# (widget::Main, ~widget, resource::~resource) as non-virtual.
mangled_at={}
for va,raw in pubs: mangled_at.setdefault(va,raw)
_VIRT_ACCESS=set("EFMNUV")  # fn property char: E/F priv-virt, M/N prot-virt, U/V pub-virt
def virtual_by_mangling(va):
    r=mangled_at.get(va,"")
    if r[:4] in ("??_E","??_G"): return True   # vector/scalar deleting dtor => always virtual
    m=re.search(r'@@([A-Z])',r)                # first `@@<char>` = the function-property code
    return bool(m) and m.group(1) in _VIRT_ACCESS

# assert __FILE__ tier map: basename(lower w/ ext) -> BASE/SOURCE/EDITOR
assert_dir={}
if srcpaths and os.path.exists(srcpaths):
    for line in open(srcpaths):
        line=line.strip().lower().replace("/","\\")
        if "\\" in line:
            sub,fn=line.rsplit("\\",1); assert_dir[fn]=sub.upper()
def placement_tier(im):
    if im is None: return "SOURCE"
    name=modname.get(im,""); lib=modlib.get(im,"")
    stem=(name.replace("\\","/").split("/")[-1]); stem=(stem[:-4] if stem.lower().endswith(".obj") else stem).lower()
    if stem+".cpp" in assert_dir: return assert_dir[stem+".cpp"]
    if stem+".c"   in assert_dir: return assert_dir[stem+".c"]
    if "basewin" in lib.lower(): return "BASE"
    return "SOURCE"   # directly-linked game code, or fold lib objects into SOURCE

# ---------- vtables + inheritance ----------
def cleandecl(s):
    s=re.sub(r'^(public|protected|private)(\s*virtual)?:\s*','',s)
    return re.sub(r'\b__(thiscall|cdecl|stdcall|fastcall)\b\s*','',s).strip()
def cls_of(name):
    s=re.sub(r'\(.*','',name); m=re.search(r'([A-Za-z_]\w*)::[~A-Za-z_]\w*\s*$', s); return m.group(1) if m else None
allsym=sorted(set(va for va,_ in pubs))
def next_sym(va):
    i=bisect.bisect_right(allsym,va); return allsym[i] if i<len(allsym) else None
vtables={}; vt_addr={}
for va,raw in pubs:
    m=re.match(r'\?\?_7([A-Za-z_]\w*)@@6B', raw)
    if not m: continue
    C=m.group(1); slots=[]; cur=va; i=0; stop=next_sym(va)
    while True:
        o=va2off(cur)
        if o is None or cur not in reloc: break
        if cur!=va and stop and cur>=stop: break
        ptr=struct.unpack_from("<I",d,o)[0]
        if not (TVA and TVA<=ptr<TEND): break
        slots.append((i,ptr,name_at.get(ptr,f"sub_{ptr:08x}"),cls_of(name_at.get(ptr,"")))); cur+=4; i+=1
    if slots: vtables[C]=slots; vt_addr[C]=va
is_pure=lambda fn:"purecall" in fn.lower()
def infer_base(C):
    vC=vtables[C]; best=None; bestscore=(0,-1)
    for B,vB in vtables.items():
        if B==C or len(vB)>len(vC): continue
        # Only count slots that share a REAL fn pointer. Matching __purecall is meaningless:
        # two UNRELATED abstract classes coincidentally share the one __purecall thunk, which
        # used to invent bogus bases like `baseManager : widget` (both are all-__purecall).
        shared=sum(1 for i in range(len(vB))
                   if vC[i][1]==vB[i][1] and not is_pure(vB[i][2]))
        if shared>0 and (shared,len(vB))>bestscore: best,bestscore=B,(shared,len(vB))
    return best
vbase={C:infer_base(C) for C in vtables}
_own=lambda X:sum(1 for s in vtables[X] if s[3]==X)
for C in list(vbase):
    B=vbase[C]
    if B and vbase.get(B)==C: vbase[C if _own(C)>=_own(B) else B]=None
abstract={C for C in vtables if vtables[C] and all(is_pure(s[2]) for s in vtables[C])}
def slot0name(C):
    s=cleandecl(vtables[C][0][2]); return s.split('::')[-1].split('(')[0] if '::' in s else ''
for C in vtables:
    if vbase.get(C) is None and C not in abstract:
        ib={"Draw":"widget","Open":"baseManager"}.get(slot0name(C))
        if ib in abstract and ib!=C: vbase[C]=ib
ownv=defaultdict(dict)
for C,sl in vtables.items():
    for i,ptr,fn,fc in sl:
        if fc==C: ownv[C][ptr]=i
def is_override(C,va):
    B=vbase.get(C)
    if not B: return False
    i=ownv.get(C,{}).get(va)
    return i is not None and i < len(vtables.get(B,[]))

# ---------- collect members ----------
def is_dtor(name): return name.startswith("~") or name in ("scalar_dtor","vector_dtor","vbase_dtor","destructor")
def split_method(s):
    par=s.find('(')
    if par<0: return None
    head=s[:par]; params=s[par+1:s.rfind(')')]
    m=re.search(r'(\b[A-Za-z_]\w*)::(~?[A-Za-z_]\w*|constructor|scalar_dtor|vector_dtor|vbase_dtor)\s*$', head)
    if not m: return None
    return head[:m.start()].strip(), m.group(1), m.group(2), params
def split_data(s):
    m=re.match(r'^(.*?)\s*\b([A-Za-z_]\w*)::([A-Za-z_]\w*)\s*$', s)
    return (m.group(2), m.group(1).strip(), m.group(3)) if m else None
methods=defaultdict(list); statics=defaultdict(list); seenva=set()
for va,raw in pubs:
    if va in seenva: continue
    seenva.add(va)
    s=cleandecl(name_at.get(va,raw))
    if "vftable" in s or "vbtable" in s or "RTTI" in s or s.startswith("??"): continue
    if "(" in s:
        r=split_method(s)
        if r: methods[r[1]].append((va,r[0],r[2],r[3]))
    else:
        r=split_data(s)
        if r and r[2] not in ("vftable","vbtable"): statics[r[0]].append((va,r[1],r[2]))
classes=sorted(set(methods)|set(statics)|set(vtables))

# ---------- class -> tier ----------
home={}
for va,raw in pubs:
    m=re.match(r'\?\?0([A-Za-z_]\w*)@@', raw)
    if m and m.group(1) not in home: home[m.group(1)]=which(va)
for C in vtables: home.setdefault(C, which(vt_addr[C]))
for C in classes:
    if C not in home:
        pool=methods.get(C) or statics.get(C)
        if pool: home[C]=which(sorted(pool)[0][0])
class_tier={C: placement_tier(home.get(C)) for C in classes}

def sanitize(c): return re.sub(r'[^A-Za-z0-9_]','_',c)
def render_member(ret,name,params,virtual,override,cls,pure=False):
    if name=="constructor": core=f"{cls}({params})"
    elif is_dtor(name): core=f"~{cls}()"
    else: core=f"{ret+' ' if ret else ''}{name}({params})"
    tail=" = 0" if pure else (" OVERRIDE" if override else "")
    return (("virtual " if virtual else "")+core+tail)+";"
def referenced_types(sigs):
    t=set()
    for s in sigs:
        for kw,nm in re.findall(r'\b(class|struct|enum|union)\s+([A-Za-z_]\w*)', s): t.add((kw,nm))
    return t
def inc_path(frm, cls):
    t=class_tier.get(cls)
    if not t: return None
    return f'"{sanitize(cls)}.h"' if t==frm else f'"../{t}/{sanitize(cls)}.h"'

# is a symbol-backed method virtual / an override / pure, for class C?  A method whose own
# vtable slot holds real code is a concrete virtual (fc==C => in ownv); one that is virtual
# by mangling but is NOT its own slot's target has a __purecall slot => pure-virtual-with-body.
def method_flags(C,va):
    inv = va in ownv.get(C,{})
    virtual = inv or virtual_by_mangling(va)
    return virtual, (inv and is_override(C,va)), (virtual and not inv)

# ---------- recover an abstract base's pure virtuals from its derived overrides ----------
# An abstract class's own vtable is all __purecall, so its virtuals cannot be read from its
# own slots. Each direct child overrides those slots with real fns, revealing slot order +
# signatures. For a bodyless pure virtual (NO symbol anywhere, e.g. widget::Draw or
# baseManager::Open/Close/Main) this is the ONLY recovery path; for a pure-virtual-WITH-body
# (widget::Main, widget/resource ~dtor) we prefer the base's own symbol signature.
def child_override_at(C,i):
    for D,sl in vtables.items():
        if D==C or vbase.get(D)!=C: continue
        if i<len(sl) and sl[i][3]==D and not is_pure(sl[i][2]): return sl[i][2]
    return None
abstract_pv=defaultdict(list)   # C -> [(slot_i, ret, name, params, is_dtor)] in vtable order
for C in abstract:
    own_by_name={m[2]:m for m in methods.get(C,[])}
    own_virt=[m for m in methods.get(C,[]) if virtual_by_mangling(m[0])]
    for i,ptr,fn,fc in vtables[C]:
        ov=child_override_at(C,i)
        r=split_method(cleandecl(ov)) if ov else None
        if r:
            ret,_dc,mname,params=r
            if is_dtor(mname):                    abstract_pv[C].append((i,"","~"+C,"",True))
            elif mname in own_by_name:            # pure-with-body: prefer base's own signature
                _,oret,onm,opar=own_by_name[mname]; abstract_pv[C].append((i,oret,onm,opar,False))
            else:                                 abstract_pv[C].append((i,ret,mname,params,False))
        else:
            # no wired child override (e.g. resource, whose asset subclasses aren't linked to
            # it yet). Fall back to the base's own virtual symbols; a lone slot is unambiguous.
            dtor_syms=[m for m in own_virt if is_dtor(m[2])]
            if len(vtables[C])==1 and dtor_syms:  abstract_pv[C].append((i,"","~"+C,"",True))
            elif len(vtables[C])==1 and own_virt:
                _,oret,onm,opar=own_virt[0];      abstract_pv[C].append((i,oret,onm,opar,False))
            else:                                 abstract_pv[C].append((i,"void",f"vfn{i}","",False))

# ---------- emit ----------
open(os.path.join(outdir,"_macros.h"),"w").write(
    "#pragma once\n// No-op annotation macros.\n"
    "#define VA(addr, size)\n#define DATA(addr)\n#define OVERRIDE\n")
emitted=[]
for C in classes:
    tier=class_tier[C]; base=vbase.get(C)
    os.makedirs(os.path.join(outdir,tier),exist_ok=True)
    ms=sorted(methods.get(C,[]), key=lambda x:x[0]); ds=sorted(statics.get(C,[]), key=lambda x:x[0])
    is_abs=C in abstract; apv=abstract_pv.get(C,[]) if is_abs else []
    apv_names={name for _,_,name,_,isd in apv if not isd}   # own-symbol virtuals folded into apv
    ctors=[m for m in ms if m[2]=="constructor"]; dtors=[m for m in ms if is_dtor(m[2])]
    rest =[m for m in ms if m not in ctors and m not in dtors and not (is_abs and m[2] in apv_names)]
    virt =sorted([m for m in rest if method_flags(C,m[0])[0]], key=lambda m:ownv.get(C,{}).get(m[0],1<<20))
    nonv =[m for m in rest if not method_flags(C,m[0])[0]]
    apv_sigs=[f"{r} {n}({p})" for _,r,n,p,isd in apv if not isd]
    refs={(k,n) for (k,n) in referenced_types([f"{r} {n}({p})" for _,r,n,p in ms]+[t for _,t,_ in ds]+apv_sigs) if n!=C}
    n_own_virtual=len(apv) if is_abs else len(virt)
    L=["#pragma once",
       f"// Reconstructed class ({tier}) from CodeView NB09 of {os.path.basename(pe)} — NOT original source.",
       f"// {len(ms)} methods, {n_own_virtual} own-virtual, {len(ds)} static data.",
       '#include "../_macros.h"']
    if base and inc_path(tier,base): L.append(f"#include {inc_path(tier,base)}")
    fwd=sorted(refs-({("class",base)} if base else set()))
    if fwd: L.append("// forward declarations:"); L+= [f"{kw} {nm};" for kw,nm in fwd]
    L.append("")
    L.append(f"class {C}"+(f" : public {base}" if base else "")+(" /* abstract */" if C in abstract else "")+" {")
    L.append("public:")
    def emit(group, hdr):
        if not group: return
        L.append(f"    // --- {hdr} ---")
        for va,ret,name,params in group:
            v,o,p=method_flags(C,va)
            L.append("    "+render_member(ret,name,params,v,o,C,pure=p))
    emit(ctors,"constructors")
    if is_abs:
        # abstract base: emit the pure virtuals recovered from derived overrides, in slot order
        if apv:
            L.append("    // --- virtual methods (vtable order) ---")
            for i,ret,name,params,isd in apv:
                if isd: L.append(f"    virtual ~{C}(void) = 0;")
                else:   L.append("    "+render_member(ret,name,params,True,False,C,pure=True))
        emit(nonv,"methods")
    else:
        if dtors:
            dv=any(method_flags(C,m[0])[0] for m in dtors); do=any(method_flags(C,m[0])[1] for m in dtors)
            L.append("    "+("virtual " if dv else "")+f"~{C}()"+(" OVERRIDE" if do else "")+";")
        emit(virt,"virtual methods (vtable order)"); emit(nonv,"methods")
    if ds:
        L.append("    // --- static data members ---")
        L+=[f"    static {t} {n};" for _,t,n in ds]
    L.append("};")
    open(os.path.join(outdir,tier,sanitize(C)+".h"),"w").write("\n".join(L)+"\n")
    emitted.append(C)

# master include (base-before-derived) + hierarchy
order=[]; seen=set()
def visit(C):
    if C in seen or C not in set(emitted): return
    if vbase.get(C): visit(vbase[C])
    seen.add(C); order.append(C)
for C in emitted: visit(C)
with open(os.path.join(outdir,"_all.h"),"w") as f:
    f.write("#pragma once\n// every reconstructed class, base-before-derived\n")
    for C in order: f.write(f'#include "{class_tier[C]}/{sanitize(C)}.h"\n')
with open(os.path.join(outdir,"_hierarchy.txt"),"w") as f:
    kids=defaultdict(list)
    for C in emitted: kids[vbase.get(C)].append(C)
    def tree(C,ind=0):
        f.write("  "*ind+f"{C} [{class_tier[C]}]"+(" (abstract)" if C in abstract else "")+f"  {len(methods.get(C,[]))} methods\n")
        for k in sorted(kids.get(C,[])): tree(k,ind+1)
    for root in sorted(kids.get(None,[])): tree(root)
print(f"emitted {len(emitted)} class headers into {outdir}/ (BASE/SOURCE/EDITOR tiers)")
by=defaultdict(int)
for C in emitted: by[class_tier[C]]+=1
print("by tier:", dict(by))
