"""U15 supplement: the13 private float operands in HeroInteractionAtTown.

This fixed-site proof complements check-function.py's ordinary bytes, DIR32
sites and other ordered targets; it does not loosen the general verifier.
"""
import struct
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, sites, u32, read = _pe_layout(control)
obj = CoffObject(Path('build/objdiff/base/SOURCE/PHILAI.obj').read_bytes())
fn, = [s for s in obj.symbols.values() if s.typ == FUNCTION_TYPE
       and s.name == '?HeroInteractionAtTown@philAI@@QAEXPAVhero@@PAVtown@@HPAH@Z']
raw = obj.section_bytes(obj.symbol_section(fn))
expected = {
    0x421: (0x4ea480, 'd', 0.02), 0x427: (0x4ea790, 'd', 0.54),
    0x435: (0x4ea788, 'd', 0.01), 0x43b: (0x4ea628, 'd', 0.33),
    0x463: (0x4ea500, 'd', 0.5), 0x47a: (0x4ea610, 'd', 0.8),
    0x496: (0x4ea500, 'd', 0.5), 0x51a: (0x4ea6e8, 'd', 0.15),
    0x52c: (0x4ea488, 'd', 0.08), 0x57b: (0x4ea518, 'f', 1.0),
    0x584: (0x4ea780, 'd', 0.22), 0x590: (0x4ea518, 'f', 1.0),
    0x6e7: (0x4ea490, 'd', 0.04),
}
seen = set()
for reloc in obj.relocations:
    if reloc.section != fn.section or not fn.value <= reloc.site < fn.value + 1975:
        continue
    owner = obj.symbols[reloc.symbol_index]
    if not owner.name.startswith('__real@'):
        continue
    offset = reloc.site-fn.value
    destination, fmt, value = expected[offset]
    seen.add(offset)
    assert reloc.typ == 6 and raw[reloc.site:reloc.site+4] == bytes(4)
    assert 0x86ed0+offset in sites and u32(0x86ed0+offset) == destination
    encoded = struct.pack('<'+fmt, value)
    data = obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+len(encoded)]
    assert data == read(destination-base, len(encoded)) == encoded
    print('PASS', hex(offset), value, hex(destination), data.hex())
assert seen == set(expected)
print('PASS all13 private floating operands; no other unknown owner accepted')
