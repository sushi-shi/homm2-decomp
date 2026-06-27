#!/usr/bin/env python3
# Introspect an embedded CodeView NB09 (VC4) blob: what debug data is actually here?
import sys, struct
from collections import Counter
data = open(sys.argv[1], "rb").read()

SST = {0x120:"sstModule",0x121:"sstTypes",0x122:"sstPublic",0x123:"sstPublicSym",
 0x124:"sstSymbols",0x125:"sstAlignSym",0x126:"sstSrcLnSeg",0x127:"sstSrcModule",
 0x128:"sstLibraries",0x129:"sstGlobalSym",0x12a:"sstGlobalPub",0x12b:"sstGlobalTypes",
 0x12c:"sstMPC",0x12d:"sstSegMap",0x12e:"sstSegName",0x12f:"sstPreComp",
 0x133:"sstFileIndex",0x134:"sstStaticSym"}
SYM = {0x0001:"S_COMPILE",0x0002:"S_REGISTER",0x0003:"S_CONSTANT",0x0004:"S_UDT",
 0x0005:"S_SSEARCH",0x0006:"S_END",0x0009:"S_OBJNAME",0x000c:"S_RETURN",
 0x0100:"S_BPREL16",0x0200:"S_BPREL32(local/arg)",0x0201:"S_LDATA32",0x0202:"S_GDATA32",
 0x0203:"S_PUB32",0x0204:"S_LPROC32",0x0205:"S_GPROC32",0x0206:"S_THUNK32",
 0x0207:"S_BLOCK32",0x0209:"S_LABEL32",0x020a:"S_CVRESERVE",0x020b:"S_REGREL32",0x020c:"S_LTHREAD32"}

tail = data.rfind(b"NB09"); lfoBase = len(data) - struct.unpack_from("<I", data, tail+4)[0]
lfoDir = struct.unpack_from("<I", data, lfoBase+4)[0]; d = lfoBase+lfoDir
cbH, cbE = struct.unpack_from("<HH", data, d); cDir = struct.unpack_from("<I", data, d+4)[0]
ents = [struct.unpack_from("<HHii", data, d+cbH+i*cbE) for i in range(cDir)]

# 1) subsection histogram
byt = Counter(); cnt = Counter()
for sst,iMod,lfo,cb in ents: cnt[sst]+=1; byt[sst]+=cb
print("== CodeView subsections present ==")
for sst in sorted(cnt):
    print(f"  {SST.get(sst,hex(sst)):16} count={cnt[sst]:5}  bytes={byt[sst]}")

# 2) symbol-record histogram across all symbol subsections
rec = Counter()
def hist(buf):
    p=0
    while p+4<=len(buf):
        rl,rt = struct.unpack_from("<HH",buf,p)
        if rl==0: break
        rec[rt]+=1; p+=2+rl
for sst,iMod,lfo,cb in ents:
    blob=data[lfoBase+lfo:lfoBase+lfo+cb]
    if sst==0x12a: cbS=struct.unpack_from("<I",blob,4)[0]; hist(blob[16:16+cbS])
    elif sst in (0x125,0x124,0x129,0x134): hist(blob[4:])
print("\n== symbol record types (locals/args = S_BPREL32) ==")
for rt,c in rec.most_common():
    print(f"  {SYM.get(rt,hex(rt)):22} {c}")

# 3) source-line modules: file names + line-pair counts
srcfiles=[]; linepairs=0; srcmods=0
for sst,iMod,lfo,cb in ents:
    if sst!=0x127: continue
    srcmods+=1; b=data[lfoBase+lfo:lfoBase+lfo+cb]
    try:
        cFile,cSeg=struct.unpack_from("<HH",b,0)
        baseSrc=[struct.unpack_from("<I",b,4+i*4)[0] for i in range(cFile)]
        for bs in baseSrc:
            csg,pad=struct.unpack_from("<HH",b,bs)
            noff=bs+4+csg*4+csg*8
            ln=b[noff]; name=b[noff+1:noff+1+ln].decode("latin1","replace")
            srcfiles.append(name)
            # count line pairs in each seg table
            for j in range(csg):
                lnbase=struct.unpack_from("<I",b,bs+4+j*4)[0]
                seg,cpair=struct.unpack_from("<HH",b,lnbase)
                linepairs+=cpair
    except Exception: pass
print(f"\n== source line info: {srcmods} sstSrcModule, {len(srcfiles)} source files, {linepairs} line<->addr pairs ==")
for n in srcfiles[:25]: print("  ",n)
if len(srcfiles)>25: print(f"  ... (+{len(srcfiles)-25} more)")
