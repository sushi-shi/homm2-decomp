"""Check one function's bytes, complete DIR32 sites and ordered retail targets.

Usage: PYTHONPATH=scripts python3 docs/reconstruction/check-function.py BASE/RESMGR 0xb89b0
Private C-string identities are proven by their exact content at each retail
operand destination, not by VC6's unstable $SG counter. No other unknown owner
is accepted; this is deliberately narrower than a general relocation verifier.
"""
import argparse
import csv
from pathlib import Path

from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('unit')
parser.add_argument('rva', type=lambda value: int(value, 0))
args = parser.parse_args()
control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
with Path('build/gen/symbol_names.csv').open() as stream:
    rows = list(csv.DictReader(stream))
values = {}
for row in rows:
    values.setdefault(row['name'], set()).add(int(row['rva'], 0))
owners = {name: next(iter(v)) for name, v in values.items() if len(v) == 1}
owners.update({row['name']: int(row['rva'], 0) for row in rows if row['unit'] == args.unit})
row, = [r for r in rows if r['unit'] == args.unit and r['kind'] == 'func'
        and int(r['rva'], 0) == args.rva]
obj = CoffObject(Path('build/objdiff/base/' + args.unit + '.obj').read_bytes())
symbol, = [s for s in obj.symbols.values() if s.name == row['name'] and s.typ == FUNCTION_TYPE]
section = obj.symbol_section(symbol)
end = min([s.value for s in obj.symbols.values() if s.section == symbol.section
           and s.typ == FUNCTION_TYPE and s.value > symbol.value], default=section.raw_size)
raw = bytearray(obj.section_bytes(section)[symbol.value:end])
base, sites, read_u32, read_bytes = _pe_layout(control)
rva, size = args.rva, int(row['size'], 0)
target = bytearray(read_bytes(rva, size))
errors, absolute_sites = [], []
for reloc in obj.relocations:
    if reloc.section != symbol.section or not symbol.value <= reloc.site < end:
        continue
    offset = reloc.site - symbol.value
    owner = obj.symbols[reloc.symbol_index]
    addend = int.from_bytes(raw[offset:offset + 4], 'little')
    actual = read_u32(rva + offset)
    if reloc.typ == 6:
        actual = (actual - base) & 0xffffffff
        absolute_sites.append(offset)
    elif reloc.typ == 20:
        actual = (actual + rva + offset + 4) & 0xffffffff
    else:
        raise ValueError(('unsupported relocation', reloc.typ))
    expected = owners.get(owner.name)
    valid = expected is not None and ((expected + addend) & 0xffffffff) == actual
    if expected is None and reloc.typ == 6 and owner.name.startswith('$SG') and owner.section > 0:
        data = obj.section_bytes(obj.symbol_section(owner))[owner.value:]
        terminal = data.find(b'\0')
        if terminal < 0 or addend != 0:
            raise ValueError(('not a complete zero-addend C string', owner.name))
        string = data[:terminal + 1]
        valid = read_bytes(actual, len(string)) == string
        print(hex(offset), owner.name, 'C-string bytes', len(string), 'retail', hex(actual + base), valid)
    else:
        print(hex(offset), owner.name, 'addend', hex(addend), 'retail', hex(actual + base), valid)
    if not valid:
        errors.append((hex(offset), owner.name, hex(addend), hex(actual), expected))
    raw[offset:offset + 4] = b'\0' * 4
    target[offset:offset + 4] = b'\0' * 4
correct_sites = absolute_sites == [s - rva for s in sites if rva <= s < rva + size]
print(row['name'], 'bytes', raw == target, 'size', len(raw), size,
      'DIR32 sites', correct_sites, 'ordered target errors', errors)
raise SystemExit(int(raw != target or not correct_sites or bool(errors)))
