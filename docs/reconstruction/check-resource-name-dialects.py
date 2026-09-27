"""C14/C62: conditional-literal casts needed by annotation C++98, not C++20."""
from pathlib import Path
import clang.cindex as ci
from homm2.build.annotated_data import ClangMode, _clang_args, configure_libclang
from homm2.build.source_symbols import unreviewed_diagnostics

root = Path.cwd()
configure_libclang()
sites = {
    'src/SOURCE/RECRUIT.cpp': [
        'm_resourceType == RECRUIT_NO_RESOURCE ? "recruit0.bin" : "recruit1.bin"',
        'resourceType == RECRUIT_NO_RESOURCE ? "recruiq0.bin" : "recruiq1.bin"',
    ],
    'src/SOURCE/CMBTMGR.cpp': [
        'missShot19 ? "smalclod.icn" : "lichclod.icn"',
    ],
}
for name, expressions in sites.items():
    path = root / name
    original = path.read_text()
    removed = original
    for expression in expressions:
        cast = 'const_cast<char*>(' + expression + ')'
        assert removed.count(cast) == 1, (name, cast)
        removed = removed.replace(cast, expression)
    for mode in ClangMode:
        for label, body in [('retained', original), ('removed', removed)]:
            args = _clang_args(root, path, mode=mode) + ['-Dregister=']
            tu = ci.Index.create().parse(str(path), args=args,
                                        unsaved_files=[(str(path), body)])
            own = [d for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error
                   and d.location.file and any(
                       Path(d.location.file.name).resolve().is_relative_to(root / part)
                       for part in ('src', 'include'))]
            errors = unreviewed_diagnostics(own, root)
            expected = len(expressions) if label == 'removed' and mode == ClangMode.RETAIL_ANALYSIS else 0
            assert len(errors) == expected, (name, mode, label, [str(d) for d in errors])
            if errors:
                assert all(Path(d.location.file.name).resolve() == path and
                           ('no matching' in d.spelling) for d in errors)
            print('PASS', name, mode.value, label, len(errors), 'expected project errors')
