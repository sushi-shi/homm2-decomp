"""CP02: actual VC6 layout and both source dialects; no game object is emitted."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import units, unit_flags
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics
from homm2.build.catalog import Catalog

root = Path.cwd()
unit, = [u for u in units() if u['unit'] == 'SOURCE/GAME']
probe = Path('docs/reconstruction/casts-pointers/event-layout-probe.cpp')
code, log, timed_out = run_compile(probe, Path('build/cp02-event-layout.obj'),
                                  unit_flags(unit), depfile=False, cl_timeout=60)
print(log)
assert code == 0 and not timed_out
print('PASS VC6 event count/index types, record sizes, all owner/neighbor offsets and game size')
configure_libclang()
source = root/'src/SOURCE/GAME.cpp'
body = Catalog.load(root).render(source.read_text(), locale='ru') + '\n' + probe.read_text()
for mode in ClangMode:
    tu = ci.Index.create().parse(str(source), args=_clang_args(root, source, mode=mode)+['-Dregister='],
                                unsaved_files=[(str(source), body)])
    own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error and d.location.file
           and any(Path(d.location.file.name).resolve().is_relative_to(root/p)
                   for p in ('src', 'include', 'docs/reconstruction'))]
    errors = unreviewed_diagnostics(own, root)
    assert not errors, (mode, [str(d) for d in errors])
    print('PASS complete GAME plus event layout/type assertions:', mode.value, flush=True)

# The unchanged format duplicates indices[0]: four-byte record prefix, then
# count two-byte indices starting at offset two. This is not a two-byte header.
import struct
for capacity in (30, 50):
    for count in range(capacity + 1):
        indices = [(i * 997 + 19) & 65535 for i in range(capacity)]
        record = struct.pack('<H', count) + struct.pack('<' + 'H' * capacity, *indices)
        old_stream = record[:4] + record[2:2+count*2]
        new_stream = record[:4] + struct.pack('<' + 'H' * count, *indices[:count])
        assert old_stream == new_stream
        restored = bytearray(len(record))
        restored[:4] = new_stream[:4]
        restored[2:2+count*2] = new_stream[4:]
        assert restored[:4] == record[:4]
        assert restored[2:2+count*2] == record[2:2+count*2]
print('PASS every valid event count: identical prefix/payload stream and load overwrite order')
print('OPEN counts beyond capacity and unrelated save/load crossings are not fixed by this owner recovery')
