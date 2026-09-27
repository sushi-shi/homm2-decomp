from pathlib import Path
from homm2.core.coff import CoffObject,MEM_EXECUTE
def functions(c):
    rows = []
    for s in c.symbols.values():
        if s.section <= 0 or s.typ != 32:
            continue
        sec = c.section(s.section)
        if not sec.characteristics & MEM_EXECUTE:
            continue
        end = min([t.value for t in c.symbols.values() if t.section == s.section and t.typ == 32 and (t.value > s.value)] + [sec.raw_size])
        raw = c.section_bytes(sec)[s.value:end]
        masked = bytearray(raw)
        rel = []
        for r in c.relocations:
            if r.section == s.section and s.value <= r.site < end:
                off = r.site - s.value
                target = c.symbols[r.symbol_index]
                rel.append(dict(site=off, type=r.typ, target=target.name, target_section=target.section, target_value=target.value, addend=int.from_bytes(raw[off:off + 4], 'little')))
                masked[off:off + 4] = b'\x00' * 4
        rows.append(dict(name=s.name, section=s.section, start=s.value, size=end - s.value, bytes=raw.hex(), masked=bytes(masked).hex(), relocations=rel))
    return rows
