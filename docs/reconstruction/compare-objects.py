"""Strict native before/after comparison, not an assertion of retail exactness.

Usage: PYTHONPATH=scripts python3 docs/reconstruction/compare-objects.py BEFORE AFTER
Only established anonymous/compiler-private label normalization is permitted.
"""
import argparse
from pathlib import Path
from homm2.core.coff import CoffObject, MEM_EXECUTE, FUNCTION_TYPE
from homm2.build.canonicalize_data_symbols import canonicalize_coff

def census(path):
    obj=CoffObject(canonicalize_coff(path.read_bytes()).data)
    def target_name(symbol):
        if symbol.name.startswith(('$L','$anon_data_')) and symbol.section>0:
            return f'local:{symbol.section}:{symbol.value}'
        return symbol.name
    def relocations(section,start,end):
        return [(r.site-start,r.typ,target_name(obj.symbols[r.symbol_index]))
                for r in obj.relocations
                if r.section==section and start<=r.site<end]
    functions={}
    allocated={}
    for section in obj.sections:
        if section.characteristics & 0xe0000000:
            allocated[section.index]=(section.name,section.characteristics,
              obj.section_bytes(section),relocations(section.index,0,section.raw_size))
    for symbol in obj.symbols.values():
        section=obj.symbol_section(symbol)
        if not section or not section.characteristics & MEM_EXECUTE or symbol.typ!=FUNCTION_TYPE:
            continue
        end=min([s.value for s in obj.symbols.values() if s.section==symbol.section
          and s.typ==FUNCTION_TYPE and s.value>symbol.value],default=section.raw_size)
        functions[symbol.name]=(obj.section_bytes(section)[symbol.value:end],
          relocations(section.index,symbol.value,end))
    return functions,allocated

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('before',type=Path)
    parser.add_argument('after',type=Path)
    args=parser.parse_args()
    before,old_sections=census(args.before)
    after,new_sections=census(args.after)
    bad=[]
    for name in sorted(before.keys()|after.keys()):
        a,b=before.get(name),after.get(name)
        equal=a==b
        print(name,'identical' if equal else 'DIFFERENT',
          'sizes',len(a[0]) if a else None,len(b[0]) if b else None)
        if not equal:
            bad.append(name)
    sections=[i for i in old_sections.keys()|new_sections.keys()
              if old_sections.get(i)!=new_sections.get(i)]
    print('Allocated-section differences:',sections)
    print('Function differences:',len(bad),'/',len(before.keys()|after.keys()))
    return int(bool(bad or sections))

if __name__=='__main__':
    raise SystemExit(main())
