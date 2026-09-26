"""O03 icon-family retail check: exact bytes, DIR32 sites, scoped targets."""
import csv
from pathlib import Path

from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
with Path('build/gen/symbol_names.csv').open() as stream:
    rows = list(csv.DictReader(stream))
global_values = {}
for row in rows:
    global_values.setdefault(row['name'], set()).add(int(row['rva'], 0))
shared = {name: next(iter(values)) for name, values in global_values.items()
          if len(values) == 1}
base, sites, read_u32, read_bytes = _pe_layout(control)
failed = False
for unit in ('BASE/Icond2b', 'BASE/Icondf2b'):
    owners = dict(shared)
    local_rows = [row for row in rows if row['unit'] == unit]
    owners.update({row['name']: int(row['rva'], 0) for row in local_rows})
    obj = CoffObject(Path('build/objdiff/base/' + unit + '.obj').read_bytes())
    for row in local_rows:
        if row['kind'] != 'func':
            continue
        matches = [s for s in obj.symbols.values()
                   if s.name == row['name'] and s.typ == FUNCTION_TYPE]
        if len(matches) != 1:
            raise ValueError((unit, row['name'], 'missing/ambiguous function'))
        symbol, = matches
        section = obj.symbol_section(symbol)
        end = min([s.value for s in obj.symbols.values()
                   if s.section == symbol.section and s.typ == FUNCTION_TYPE
                   and s.value > symbol.value], default=section.raw_size)
        raw = bytearray(obj.section_bytes(section)[symbol.value:end])
        rva, size = int(row['rva'], 0), int(row['size'], 0)
        target = bytearray(read_bytes(rva, size))
        errors, absolute_sites = [], []
        for reloc in obj.relocations:
            if reloc.section != symbol.section or not symbol.value <= reloc.site < end:
                continue
            offset = reloc.site - symbol.value
            name = obj.symbols[reloc.symbol_index].name
            addend = int.from_bytes(raw[offset:offset + 4], 'little')
            actual = read_u32(rva + offset)
            if reloc.typ == 6:
                actual = (actual - base) & 0xffffffff
                absolute_sites.append(offset)
            elif reloc.typ == 20:
                actual = (actual + rva + offset + 4) & 0xffffffff
            else:
                raise ValueError(('unsupported relocation', reloc.typ))
            expected = owners.get(name)
            if expected is None or ((expected + addend) & 0xffffffff) != actual:
                errors.append((hex(offset), name, hex(addend), hex(actual), expected))
            raw[offset:offset + 4] = b'\0' * 4
            target[offset:offset + 4] = b'\0' * 4
        correct_sites = absolute_sites == [s - rva for s in sites if rva <= s < rva + size]
        print(unit, row['name'], 'bytes', raw == target, 'DIR32 sites', correct_sites,
              'ordered target errors', errors)
        failed |= raw != target or not correct_sites or bool(errors)
raise SystemExit(int(failed))
