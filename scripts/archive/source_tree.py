#!/usr/bin/env python3
# Reconstruct the project file structure from an embedded CodeView NB09 blob and
# hang every attributed public symbol under its source compiland (.obj).
#   usage: cv-nb09-tree.py PE_FILE  DEMANGLED_ADDR_MAP  > tree.txt
# DEMANGLED_ADDR_MAP = "0x........<TAB>demangled name" lines (reports/pol-symbols-addresses.txt)
import sys, struct, re
from collections import defaultdict
d = open(sys.argv[1], "rb").read()

# demangled name cache: VA -> pretty name
pretty = {}
for line in open(sys.argv[2], errors="replace"):
    if line.startswith("0x"):
        a, nm = line.rstrip("\n").split("\t", 1); pretty[int(a, 16)] = nm

# PE sections
e=struct.unpack_from("<I",d,0x3c)[0]; nsec=struct.unpack_from("<H",d,e+6)[0]; osz=struct.unpack_from("<H",d,e+20)[0]
imgbase=struct.unpack_from("<I",d,e+24+28)[0]
secs=[(struct.unpack_from("<I",d,e+24+osz+i*40+12)[0], struct.unpack_from("<I",d,e+24+osz+i*40+8)[0]) for i in range(nsec)]
def s2va(seg,off): return imgbase+secs[seg-1][0]+off if 1<=seg<=len(secs) else None

# CodeView directory
tail=d.rfind(b"NB09"); lfoBase=len(d)-struct.unpack_from("<I",d,tail+4)[0]
D=lfoBase+struct.unpack_from("<I",d,lfoBase+4)[0]
cbH,cbE=struct.unpack_from("<HH",d,D); cDir=struct.unpack_from("<I",d,D+4)[0]
ents=[struct.unpack_from("<HHii",d,D+cbH+i*cbE) for i in range(cDir)]

# modules + ranges
modname={}; modrange={}
for sst,iMod,lfo,cb in ents:
    if sst!=0x120: continue
    b=d[lfoBase+lfo:lfoBase+lfo+cb]; cSeg=struct.unpack_from("<H",b,4)[0]; p=8; rngs=[]
    for j in range(cSeg):
        seg,pad,off,cbseg=struct.unpack_from("<HHII",b,p); p+=12
        va=s2va(seg,off)
        if va: rngs.append((va,va+cbseg))
    nl=b[p]; modname[iMod]=b[p+1:p+1+nl].decode("latin1","replace"); modrange[iMod]=rngs

# publics
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

# attribute
def which(va):
    for im,rngs in modrange.items():
        for s,en in rngs:
            if s<=va<en: return im
    return None
bymod=defaultdict(list)
for va,nm in pubs: bymod[which(va)].append((va, pretty.get(va, nm)))

# build dir tree of module paths; leaf node carries iMod
def nn(): return {"dirs":{}, "mods":{}}   # mods: leafname -> iMod
root=nn()
for im,name in modname.items():
    parts=[c for c in re.split(r'[\\/]', name) if c]
    if not parts: continue
    node=root
    for c in parts[:-1]: node=node["dirs"].setdefault(c.lower(), nn())["__self__"] if False else node["dirs"].setdefault(c, nn())
    node["mods"][parts[-1]]=im

def render(node, pre=""):
    out=[]
    dirs=sorted(node["dirs"]); mods=sorted(node["mods"])
    items=[("d",x) for x in dirs]+[("m",x) for x in mods]
    for i,(k,name) in enumerate(items):
        last=(i==len(items)-1); br="└── " if last else "├── "
        if k=="d":
            out.append(pre+br+name+"\\"); out+=render(node["dirs"][name], pre+("    " if last else "│   "))
        else:
            syms=sorted(bymod.get(node["mods"][name], []))
            out.append(pre+br+name+f"   ({len(syms)} symbols)")
            sp=pre+("    " if last else "│   ")
            for va,nm in syms: out.append(f"{sp}    0x{va:08x}  {nm}")
    return out

print("# Reconstructed project structure of PoL HEROES2W.EXE (from CodeView NB09)")
print(f"# {len(modname)} compilands, {len(pubs)} attributed symbols")
print("\n".join(render(root)))
