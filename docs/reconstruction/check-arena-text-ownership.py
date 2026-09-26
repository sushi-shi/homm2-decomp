"""C10: fixed retail operands establish static text entering the deleting widget path.

This is ownership evidence, not a claim about an observed runtime crash or
a complete certificate for the constructors' private exception graphs.
"""
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.core.coff import CoffObject
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, _, read_u32, read_bytes = _pe_layout(control)
for index, name in enumerate(('Атака', 'Защита', 'Сила магии', 'Знания')):
    pointer = read_u32(0xfdc80+index*4)
    expected = name.encode('cp1251')+b'\0'
    assert read_bytes(pointer-base, len(expected)) == expected
    print('PASS image stat-name', index, hex(pointer), len(expected), 'bytes')
assert read_u32(0xb2ba0+0x25b) == 0x4fdc80
def call_operand(rva, offset, destination):
    assert read_bytes(rva+offset-1, 1) == b'\xe8'
    assert (read_u32(rva+offset)+rva+offset+4) & 0xffffffff == destination
call_operand(0xb2ba0, 0x27a, 0xc2fe0)
assert read_bytes(0xc2fe0+0x6a, 6) == bytes.fromhex('8b4d18894820')
assert read_bytes(0xba700+0x100, 6) == bytes.fromhex('8b512080ce40')
assert read_bytes(0xba700+0x10c, 3) == bytes.fromhex('895020')
assert read_bytes(0xbaf20+0x55, 6) == bytes.fromhex('81e100400000')
assert read_bytes(0xbaf20+0x7b, 3) == bytes.fromhex('ff5204')
assert read_u32(0xea9a0+4) == 0x4c2fb0
call_operand(0xc2fb0, 0xb, 0xc31c0)
call_operand(0xc2fb0, 0x1e, 0xd68c2)
assert read_bytes(0xc31c0+0x43, 3) == bytes.fromhex('8b4820')
call_operand(0xc31c0, 0x4e, 0xd68c2)
print('PASS stat-name table operand, text constructor/store, owning-window flag, virtual-delete slot and text delete operand')

# C04's generic checker does not recognize this named (rather than $SG)
# compiler string. Check that sole literal identity explicitly, without
# accepting any of the separate unresolved EH operands.
obj = CoffObject(Path('build/objdiff/base/BASE/WINMGR.obj').read_bytes())
symbol, = [s for s in obj.symbols.values() if s.name == '??_C@_0BC@JFEN@heroWindowManager?$AA@' and s.section > 0]
literal = b'heroWindowManager\0'
candidate = obj.section_bytes(obj.symbol_section(symbol))[symbol.value:symbol.value+len(literal)]
assert candidate == read_bytes(0x11dfe8, len(literal)) == literal
print('PASS C04 named manager literal at0051dfe8:', len(literal), 'complete bytes')
