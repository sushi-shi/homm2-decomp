"""Census shipping PE/CodeView evidence without treating generated PDBs as retail.

Reads NB09 linker subsections, counts every record kind and emits exact public
spellings with their file offsets. It does not infer procedure lengths from the
next public or recover parameter names from decorated signatures.
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import sys

SUBSECTIONS = {0x120: 'sstModule', 0x125: 'sstAlignSym', 0x128: 'sstLibraries',
    0x129: 'sstGlobalSym', 0x12a: 'sstGlobalPub', 0x12b: 'sstGlobalTypes',
    0x12d: 'sstSegMap', 0x134: 'sstStaticSym'}
RECORDS = {0x1: 'S_COMPILE', 0x6: 'S_END', 0x9: 'S_OBJNAME',
    0x200: 'S_BPREL32', 0x201: 'S_LDATA32', 0x202: 'S_GDATA32',
    0x203: 'S_PUB32', 0x204: 'S_LPROC32', 0x205: 'S_GPROC32',
    0x206: 'S_THUNK32', 0x209: 'S_LABEL32', 0x20c: 'S_REGREL32',
    0x402: 'S_ALIGN'}


def census(data):
    def region(offset, size):
        if offset < 0 or size < 0 or offset + size > len(data):
            raise ValueError(f'truncated range at 0x{offset:x} ({size} bytes)')
        return data[offset:offset + size]

    def unpack(fmt, offset):
        return struct.unpack(fmt, region(offset, struct.calcsize(fmt)))

    if region(0, 2) != b'MZ':
        raise ValueError('expected a PE executable')
    pe, = unpack('<I', 0x3c)
    if region(pe, 4) != b'PE\0\0':
        raise ValueError('invalid PE signature')
    nsections, = unpack('<H', pe + 6)
    optional_size, = unpack('<H', pe + 20)
    optional = pe + 24
    magic, = unpack('<H', optional)
    if magic != 0x10b or optional_size < 96:
        raise ValueError('expected PE32 optional header')
    image_base, = unpack('<I', optional + 28)
    ndirs, = unpack('<I', optional + 92)
    directories = {}
    for index, name in ((0, 'export'), (5, 'relocation'), (6, 'debug')):
        if index < ndirs:
            if 96 + (index + 1) * 8 > optional_size:
                raise ValueError('data directory exceeds optional header')
            rva, size = unpack('<II', optional + 96 + index * 8)
        else:
            rva, size = 0, 0
        directories[name] = {'rva': rva, 'size': size}
    sections = []
    for index in range(nsections):
        off = optional + optional_size + index * 40
        name = region(off, 8).rstrip(b'\0').decode('ascii', errors='replace')
        virtual_size, rva, raw_size, raw_offset = unpack('<IIII', off + 8)
        sections.append({'name': name, 'rva': rva, 'virtual_size': virtual_size,
                         'raw_size': raw_size, 'raw_offset': raw_offset})
    report = {'schema_version': 1, 'sha256': hashlib.sha256(data).hexdigest(),
        'size': len(data), 'image_base': image_base, 'directories': directories,
        'sections': sections, 'debug_records': [], 'codeview': None}
    debug = directories['debug']
    if debug['size']:
        if debug['size'] % 28:
            raise ValueError('partial PE debug directory entry')
        matches = [sec for sec in sections if sec['rva'] <= debug['rva']
                   and debug['rva'] + debug['size'] <= sec['rva'] + sec['raw_size']]
        if len(matches) != 1:
            raise ValueError('debug directory has no unique file-backed section')
        sec = matches[0]
        debug_offset = sec['raw_offset'] + debug['rva'] - sec['rva']
        for offset in range(debug_offset, debug_offset + debug['size'], 28):
            _, stamp, major, minor, kind, size, rva, pointer = unpack('<IIHHIIII', offset)
            payload = region(pointer, size)
            record = {'type': kind, 'size': size, 'offset': pointer}
            if kind == 2:
                record['signature'] = payload[:4].decode('ascii', errors='replace')
                if payload[:4] == b'NB10':
                    if size < 17 or payload[-1:] != b'\0':
                        raise ValueError('truncated NB10 PDB reference')
                    _, timestamp, age = struct.unpack_from('<III', payload, 4)
                    record.update(timestamp=timestamp, age=age,
                                  pdb_path=payload[16:-1].decode('latin1'))
            report['debug_records'].append(record)
    # NB09 shipping streams have a final signature/back-pointer. Never scan
    # arbitrary occurrences inside strings or accept a generated sidecar PDB.
    if len(data) < 8 or data[-8:-4] != b'NB09':
        return report
    distance, = unpack('<I', len(data) - 4)
    base = len(data) - distance
    if region(base, 4) != b'NB09':
        raise ValueError('invalid NB09 trailer back-pointer')
    directory, = unpack('<I', base + 4)
    directory += base
    header_size, entry_size, count = unpack('<HHI', directory)
    if header_size < 16 or entry_size < 12:
        raise ValueError('invalid NB09 directory layout')
    region(directory, header_size + entry_size * count)
    next_directory, = unpack('<I', directory + 8)
    if next_directory:
        raise ValueError('chained NB09 directories are not supported')
    subsections, publics, records, modules, types = [], [], Counter(), [], []
    for index in range(count):
        kind, module, relative, size = unpack('<HHII', directory + header_size + index * entry_size)
        offset = base + relative
        blob = region(offset, size)
        label = SUBSECTIONS.get(kind, f'0x{kind:04x}')
        subsection = {'kind': label, 'module': module, 'offset': offset, 'size': size}
        subsections.append(subsection)
        if kind == 0x12b:
            if size < 8:
                raise ValueError('truncated global type header')
            type_count, = struct.unpack_from('<I', blob, 4)
            types.append(type_count)
        if kind == 0x120:
            if size < 8:
                raise ValueError('truncated module header')
            library, segments = struct.unpack_from('<HH', blob, 2)
            pos = 8 + segments * 12
            if pos >= size or pos + 1 + blob[pos] > size:
                raise ValueError('truncated module name')
            modules.append({'index': module, 'library': library,
                            'name': blob[pos + 1:pos + 1 + blob[pos]].decode('latin1')})
        if kind in (0x129, 0x12a, 0x134):
            if size < 16:
                raise ValueError('truncated symbol hash header')
            symbol_size, = struct.unpack_from('<I', blob, 4)
            start, end = 16, 16 + symbol_size
            if end > size:
                raise ValueError('symbol records exceed subsection')
        elif kind == 0x125:
            if size < 4:
                raise ValueError('truncated aligned symbol header')
            start, end = 4, size
        else:
            continue
        counts = Counter()
        while start + 4 <= end:
            length, record = struct.unpack_from('<HH', blob, start)
            if length == 0:
                if any(blob[start:end]):
                    raise ValueError('nonzero data after symbol terminator')
                break
            if length < 2 or start + 2 + length > end:
                raise ValueError('truncated CodeView symbol record')
            label = RECORDS.get(record, f'0x{record:04x}')
            counts[label] += 1
            if record == 0x203:
                body = blob[start + 4:start + 2 + length]
                if len(body) < 9 or 9 + body[8] > len(body):
                    raise ValueError('truncated S_PUB32')
                symbol_offset, segment, type_index = struct.unpack_from('<IHH', body)
                rva = (sections[segment - 1]['rva'] + symbol_offset
                       if 1 <= segment <= len(sections) else None)
                publics.append({'name': body[9:9 + body[8]].decode('latin1'),
                    'segment': segment, 'segment_offset': symbol_offset, 'rva': rva,
                    'type_index': type_index, 'record_offset': offset + start})
            start += 2 + length
        if start < end and end - start < 4 and any(blob[start:end]):
            raise ValueError('trailing partial symbol record')
        records.update(counts)
        subsection['records'] = dict(sorted(counts.items()))
    report['codeview'] = {'signature': 'NB09', 'offset': base,
        'subsections': subsections, 'records': dict(sorted(records.items())),
        'global_type_count': sum(types), 'modules': modules, 'publics': publics}
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args(argv)
    try:
        report = census(args.exe.read_bytes())
        text = json.dumps(report, indent=2) + '\n'
        if args.output:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(text)
        else:
            print(text, end='')
        return 0
    except (OSError, ValueError, struct.error) as error:
        print(f'symbols: {error}', file=sys.stderr)
        return 2


if __name__ == '__main__':
    raise SystemExit(main())
