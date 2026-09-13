"""Supplement C35's function check with the one private floating owner.

The general function checker deliberately does not guess compiler-generated
constant identities. Verify this single operand against its actual four bytes.
"""
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

retail = Path('build/orig/HMM2PL.exe')
verify_retail(retail)
base, sites, u32, read = _pe_layout(retail)
obj = CoffObject(Path('build/objdiff/base/SOURCE/GAME.obj').read_bytes())
fn, = [s for s in obj.symbols.values()
       if s.name == '?SetupPuzzlePieces@game@@QAEHHH@Z' and s.typ == FUNCTION_TYPE]
rva, operand = 0x4cb1e, 0x53
reloc, = [r for r in obj.relocations
          if r.section == fn.section and r.site == fn.value + operand]
owner = obj.symbols[reloc.symbol_index]
assert reloc.typ == 6 and owner.name == '__real@4@40008000000000000000'
raw = obj.section_bytes(obj.symbol_section(fn))
assert raw[fn.value + operand - 2:fn.value + operand] == bytes.fromhex('d8 35')
assert raw[fn.value + operand:fn.value + operand + 4] == bytes(4)
assert read(rva + operand - 2, 2) == bytes.fromhex('d8 35')
assert rva + operand in sites
destination = u32(rva + operand) - base
assert destination == 0xea4e4
candidate = obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value + 4]
expected = bytes.fromhex('00 00 00 40')  # IEEE-754 single 2.0
assert candidate == read(destination, 4) == expected
print('PASS SetupPuzzlePieces +0x53: fdiv m32, zero-addend private float 2.0;')
print('candidate/retail payload 00000040 at retail VA 0x004ea4e4.')
