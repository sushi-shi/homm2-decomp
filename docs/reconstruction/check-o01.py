"""Compare O01 field owners, keeping instruction bytes and ordered addends."""
from pathlib import Path
import json
from homm2.core.coff import CoffObject, FUNCTION_TYPE, MEM_EXECUTE
from homm2.build.canonicalize_data_symbols import canonicalize_coff
from homm2.build.candidate_data_manifest import _symbol_inventory, _pe_layout
from homm2.core.retail import verify_retail

verify_retail(Path('build/orig/HMM2PL.exe'))

OWNERS = {
    '_gMilesSamplesReady': ('_gMilesSamples', 0),
    '_gMilesSampleHandles': ('_gMilesSamples', 4),
    '_gMilesSampleHandleCount': ('_gMilesSamples', 0x44),
}

def census(path):
    obj = CoffObject(canonicalize_coff(path.read_bytes()).data)
    result = {}
    for s in obj.symbols.values():
        sec = obj.symbol_section(s)
        if not sec or not sec.characteristics & MEM_EXECUTE or s.typ != FUNCTION_TYPE:
            continue
        end = min([t.value for t in obj.symbols.values()
                   if t.section == s.section and t.typ == FUNCTION_TYPE
                   and t.value > s.value], default=sec.raw_size)
        raw = bytearray(obj.section_bytes(sec)[s.value:end])
        relocs = []
        for r in obj.relocations:
            if r.section != s.section or not s.value <= r.site < end:
                continue
            if r.typ not in (6, 20):
                raise ValueError(r)
            off = r.site - s.value
            target = obj.symbols[r.symbol_index]
            name, shift = OWNERS.get(target.name, (target.name, 0))
            addend = (int.from_bytes(raw[off:off+4], 'little') + shift) & 0xffffffff
            if name.startswith(('$L', '$anon_data_')) and target.section > 0:
                name = f'local:{target.section}:{target.value}'
            relocs.append((off, r.typ, name, addend))
            raw[off:off+4] = b'\0'*4
        result[s.name] = (bytes(raw), relocs)
    return result

before = census(Path('build/o01-before/MilesSound.obj'))
after = census(Path('build/objdiff/base/BASE/MilesSound.obj'))
diffs = [name for name in sorted(before.keys() | after.keys())
         if before.get(name) != after.get(name)]
print('Emitted functions:', len(after), 'before/after differences:', diffs)

public, data, functions = _symbol_inventory(Path('build/gen/symbol_names.csv'))
public['_gMilesSamples'] = 0x1396d8
public['_gMilesSampleVolumes'] = 0x139720
base, sites, read_u32, read_bytes = _pe_layout(Path('build/orig/HMM2PL.exe'))
results = []
for f in functions['BASE/MilesSound']:
    if f['name'] not in after:
        continue
    raw, relocs = after[f['name']]
    target = bytearray(read_bytes(f['rva'], f['size']))
    mismatches = []
    for off, typ, name, addend in relocs:
        actual = read_u32(f['rva']+off)
        if typ == 6:
            expected = public.get(name)
            actual = (actual-base) & 0xffffffff
            if expected is not None:
                expected = (expected + addend) & 0xffffffff
        else:
            expected = public.get(name)
            actual = (actual+f['rva']+off+4) & 0xffffffff
            if expected is not None:
                expected = (expected + addend) & 0xffffffff
        if expected is None or expected != actual:
            mismatches.append((hex(off), name, hex(actual),
                               None if expected is None else hex(expected)))
        target[off:off+4] = b'\0'*4
    results.append(dict(function=f['name'], va=hex(f['rva']+base),
                        bytes_equal=raw == bytes(target), relocations=mismatches))
print(json.dumps(results, indent=2))
Path('build/o01-verification.json').write_text(json.dumps(results, indent=2)+'\n')
