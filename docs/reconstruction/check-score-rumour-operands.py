"""Prove fixed private double operands after strict whole-function retail checks.

This does not relax check-function.py: only the listed sites, addresses and
eight-byte constants of CalcBaseScore and SetupNewRumour are accepted here.
"""
import ast
import struct
import subprocess
import sys
from pathlib import Path
from homm2.core.coff import CoffObject, FUNCTION_TYPE
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control=Path('build/orig/HMM2PL.exe')
verify_retail(control)
base,sites,u32,read=_pe_layout(control)
obj=CoffObject(Path('build/objdiff/base/SOURCE/GAME.obj').read_bytes())
expected_score={
    0x2a:(0x4ea618,0.6),0x50:(0x4ea610,0.8),0x76:(0x4ea4b8,1.0),
    0x9b:(0x4ea608,1.4),0xdb:(0x4ea500,0.5),0xf3:(0x4ea600,30.0),
    0x119:(0x4ea5a0,0.25),0x12e:(0x4ea5f8,60.0),0x14d:(0x4ea5f0,0.125),
}
expected_rumour={offset:(0x4ea628 if value==0.33 else 0x4ea620,value) for offset,value in {
    0x2b3:0.33,0x2e3:0.33,0x31f:0.33,0x34f:0.66,
    0x38b:0.33,0x3c7:0.66,0x3f7:0.33,0x433:0.66,
    0x469:0.66,0x4b1:0.66,0x4f9:0.33,0x53e:0.66,
}.items()}
for name,rva,size,expected in [
    ('?CalcBaseScore@@YIHH@Z',0x5efc5,375,expected_score),
    ('?SetupNewRumour@game@@QAEXXZ',0x5fdd7,1613,expected_rumour),
]:
    result=subprocess.run([sys.executable,'docs/reconstruction/check-function.py',
                           'SOURCE/GAME',hex(rva)],text=True,capture_output=True)
    print(result.stdout,end='')
    assert result.returncode==1 and not result.stderr
    summary=result.stdout.splitlines()[-1]
    assert summary.startswith(f'{name} bytes True size {size} {size} DIR32 sites True ordered target errors ')
    errors=ast.literal_eval(summary.split('ordered target errors ',1)[1])
    assert len(errors)==len(expected)
    assert {int(error[0],16) for error in errors}==set(expected)
    fn,=[s for s in obj.symbols.values() if s.typ==FUNCTION_TYPE and s.name==name]
    raw=obj.section_bytes(obj.symbol_section(fn));seen=set()
    for reloc in obj.relocations:
        if reloc.section!=fn.section or not fn.value<=reloc.site<fn.value+size:continue
        owner=obj.symbols[reloc.symbol_index]
        if not owner.name.startswith('__real@'):continue
        offset=reloc.site-fn.value
        destination,value=expected[offset];seen.add(offset)
        assert reloc.typ==6 and raw[reloc.site:reloc.site+4]==bytes(4)
        assert rva+offset in sites and u32(rva+offset)==destination
        data=obj.section_bytes(obj.symbol_section(owner))[owner.value:owner.value+8]
        assert data==struct.pack('<d',value)==read(destination-base,8)
        error,=[error for error in errors if int(error[0],16)==offset]
        assert error==(hex(offset),owner.name,'0x0',hex(destination-base),None)
        print('PASS fixed double',name,hex(offset),hex(destination),value,data.hex())
    assert seen==set(expected)
    print('PASS complete bytes, size, DIR32 sites and every ordered target:',name)
