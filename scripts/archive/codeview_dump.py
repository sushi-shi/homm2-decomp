#!/usr/bin/env python3
# Exhaustively dump an embedded CodeView NB09 (VC4) blob:
#   - tooling versions (S_COMPILE)         -> stdout
#   - linked libraries (sstLibraries)      -> <out>/pol-libraries.txt
#   - compilands + address ranges          -> <out>/pol-compilands.txt
#   - public symbols attributed to .obj    -> <out>/pol-symbols-by-module.txt
import sys, struct, subprocess, os
from collections import Counter, defaultdict
path, outdir = sys.argv[1], sys.argv[2]
os.makedirs(outdir, exist_ok=True)
d = open(path, "rb").read()

# PE sections: index(1-based) -> (name, va, vsize)
e=struct.unpack_from("<I",d,0x3c)[0]; nsec=struct.unpack_from("<H",d,e+6)[0]; osz=struct.unpack_from("<H",d,e+20)[0]
imgbase=struct.unpack_from("<I",d,e+24+28)[0]
secs=[]
for i in range(nsec):
    o=e+24+osz+i*40
    secs.append((d[o:o+8].rstrip(b"\0").decode("latin1"),
                 struct.unpack_from("<I",d,o+12)[0], struct.unpack_from("<I",d,o+8)[0]))
def s2va(seg,off): return imgbase+secs[seg-1][1]+off if 1<=seg<=len(secs) else None

tail=d.rfind(b"NB09"); lfoBase=len(d)-struct.unpack_from("<I",d,tail+4)[0]
lfoDir=struct.unpack_from("<I",d,lfoBase+4)[0]; D=lfoBase+lfoDir
cbH,cbE=struct.unpack_from("<HH",d,D); cDir=struct.unpack_from("<I",d,D+4)[0]
ents=[struct.unpack_from("<HHii",d,D+cbH+i*cbE) for i in range(cDir)]

# --- modules + their address ranges (iMod is 1-based) -----------------------
modname={}; modrange=defaultdict(list)
imod=0
for sst,iMod,lfo,cb in ents:
    if sst!=0x120: continue
    b=d[lfoBase+lfo:lfoBase+lfo+cb]
    ov,iLib,cSeg=struct.unpack_from("<HHH",b,0); p=8
    rngs=[]
    for j in range(cSeg):
        seg,pad,off,cbseg=struct.unpack_from("<HHII",b,p); p+=12
        va=s2va(seg,off)
        if va: rngs.append((va,va+cbseg))
    nl=b[p]; nm=b[p+1:p+1+nl].decode("latin1","replace")
    modname[iMod]=nm; modrange[iMod]=rngs

# --- linked libraries (sstLibraries) ----------------------------------------
libs=[]
for sst,iMod,lfo,cb in ents:
    if sst!=0x128: continue
    b=d[lfoBase+lfo:lfoBase+lfo+cb]; p=0
    while p<len(b):
        n=b[p]
        if n==0: p+=1; continue
        s=b[p+1:p+1+n].decode("latin1","replace");
        if s.strip(): libs.append(s)
        p+=1+n

# --- publics (name+addr) ----------------------------------------------------
def pstr(buf,p): n=buf[p]; return buf[p+1:p+1+n].decode("latin1"), p+1+n
pubs=[]
for sst,iMod,lfo,cb in ents:
    if sst!=0x12a: continue
    blob=d[lfoBase+lfo:lfoBase+lfo+cb]; cbS=struct.unpack_from("<I",blob,4)[0]; buf=blob[16:16+cbS]
    p=0
    while p+4<=len(buf):
        rl,rt=struct.unpack_from("<HH",buf,p)
        if rl==0: break
        body=buf[p+4:p+2+rl]
        if rt==0x0203:
            off,seg=struct.unpack_from("<IH",body,0); nm,_=pstr(body,8); va=s2va(seg,off)
            if nm and va: pubs.append((va,nm))
        p+=2+rl

def which_mod(va):
    for im,rngs in modrange.items():
        for s,en in rngs:
            if s<=va<en: return im
    return None

# attribute publics to modules
bymod=defaultdict(list)
for va,nm in pubs:
    im=which_mod(va); bymod[im].append((va,nm))

# write artifacts
with open(f"{outdir}/pol-libraries.txt","w") as f:
    f.write("# static libraries linked into PoL HEROES2W.EXE (from CodeView sstLibraries)\n")
    for l in libs: f.write(l+"\n")
with open(f"{outdir}/pol-compilands.txt","w") as f:
    f.write(f"# {len(modname)} compilands (CodeView sstModule) with address ranges\n")
    for im in sorted(modname):
        rg=" ".join(f"{s:#08x}-{e:#08x}" for s,e in modrange[im])
        f.write(f"{modname[im]:40} {rg}\n")
# demangle + group symbols by module
def dem(n):
    try: return subprocess.run(["toolbin/rabin2","-D","msvc",n],capture_output=True,text=True,timeout=5).stdout.strip() or n
    except Exception: return n
with open(f"{outdir}/pol-symbols-by-module.txt","w") as f:
    for im in sorted(bymod, key=lambda x:(x is None,x)):
        nm = modname.get(im,"<unattributed>")
        syms=sorted(bymod[im])
        f.write(f"\n===== {nm}  ({len(syms)} symbols) =====\n")
        for va,raw in syms: f.write(f"0x{va:08x}  {dem(raw)}\n")

print(f"compilands: {len(modname)}   libraries: {len(libs)}   publics: {len(pubs)}")
print("\n== linked static libraries ==")
for l in libs: print("  ",l)
print("\n== biggest game modules by symbol count ==")
top=sorted(((len(v),modname.get(k,'<none>')) for k,v in bymod.items()),reverse=True)[:20]
for c,nm in top: print(f"  {c:4}  {nm}")
