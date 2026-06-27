#!/usr/bin/env python3
# Parse an embedded CodeView NB09 (VC4) debug blob in a PE and emit name -> RVA.
# Handles sstGlobalPub (S_PUB32) and per-module sstAlignSym (S_GPROC32/S_LPROC32).
import sys, struct

path = sys.argv[1]
data = open(path, "rb").read()

# --- PE section table: section index (1-based) -> virtual address -----------
e_lfanew = struct.unpack_from("<I", data, 0x3c)[0]
assert data[e_lfanew:e_lfanew+4] == b"PE\0\0"
nsec   = struct.unpack_from("<H", data, e_lfanew+6)[0]
opt_sz = struct.unpack_from("<H", data, e_lfanew+20)[0]
imgbase= struct.unpack_from("<I", data, e_lfanew+24+28)[0]   # ImageBase in opt hdr
sec_off= e_lfanew + 24 + opt_sz
secs = []  # (name, vaddr)
for i in range(nsec):
    o = sec_off + i*40
    name = data[o:o+8].rstrip(b"\0").decode("latin1")
    vaddr= struct.unpack_from("<I", data, o+12)[0]
    secs.append((name, vaddr))

# --- locate CodeView blob via the trailing NB09 back-pointer -----------------
tail = data.rfind(b"NB09")
lfoBase = len(data) - struct.unpack_from("<I", data, tail+4)[0]
assert data[lfoBase:lfoBase+4] == b"NB09", "bad lfoBase"
lfoDir = struct.unpack_from("<I", data, lfoBase+4)[0]
d = lfoBase + lfoDir
cbDirHeader, cbDirEntry = struct.unpack_from("<HH", data, d)
cDir = struct.unpack_from("<I", data, d+4)[0]
entries = []
for i in range(cDir):
    o = d + cbDirHeader + i*cbDirEntry
    sst, iMod, lfo, cb = struct.unpack_from("<HHii", data, o)
    entries.append((sst, iMod, lfoBase+lfo, cb))

def seg_off_to_rva(seg, off):
    return (secs[seg-1][1] + off) if 1 <= seg <= len(secs) else None

def pstr(buf, p):
    n = buf[p]; return buf[p+1:p+1+n].decode("latin1"), p+1+n

syms = {}   # name -> rva
def parse_symrecs(buf):
    p = 0
    while p + 4 <= len(buf):
        reclen, rectype = struct.unpack_from("<HH", buf, p)
        if reclen == 0: break
        body = buf[p+4 : p+2+reclen]
        try:
            if rectype in (0x0203,):           # S_PUB32: off(4) seg(2) typ(2) name
                off, seg = struct.unpack_from("<IH", body, 0); nm,_ = pstr(body, 8)
                rva = seg_off_to_rva(seg, off)
                if nm and rva: syms.setdefault(nm, rva)
            elif rectype in (0x0205, 0x0204):  # S_GPROC32 / S_LPROC32 (CV4): name near end
                # layout: pparent,pend,pnext(12) procLen,debugStart,debugEnd(12) off(4) seg(2) typ(2) flags(1) name
                off, seg = struct.unpack_from("<IH", body, 24); nm,_ = pstr(body, 33)
                rva = seg_off_to_rva(seg, off)
                if nm and rva: syms.setdefault(nm, rva)
        except Exception:
            pass
        p += 2 + reclen

SST_GLOBALPUB = 0x12a
SST_ALIGNSYM  = 0x125
for sst, iMod, off, cb in entries:
    blob = data[off:off+cb]
    if sst == SST_GLOBALPUB:
        # OMFSymHash: symhash(2) addrhash(2) cbSymbol(4) cbHSym(4) cbHAddr(4) = 16
        cbSymbol = struct.unpack_from("<I", blob, 4)[0]
        parse_symrecs(blob[16:16+cbSymbol])
    elif sst == SST_ALIGNSYM:
        parse_symrecs(blob[4:])   # skip 4-byte signature

print(f"# image base 0x{imgbase:x}, {len(secs)} sections, {len(entries)} CV subsections")
print(f"# recovered {len(syms)} symbols with addresses")
for nm, rva in sorted(syms.items(), key=lambda kv: kv[1]):
    print(f"0x{imgbase+rva:08x}\t{nm}")
