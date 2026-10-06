"""PE32 inspection with separate readable/mapped section extents.

Never treat the zero-filled .data tail as file bytes.
"""
from __future__ import annotations

from dataclasses import asdict, dataclass
import hashlib
import re
import struct


@dataclass(frozen=True)
class Section:
    name: str
    rva: int
    virtual_size: int
    raw_size: int
    raw_offset: int
    characteristics: int

    @property
    def size(self):
        return min(self.virtual_size, self.raw_size) or self.raw_size

    @property
    def mapped(self):
        return max(self.virtual_size, self.raw_size)

    @property
    def executable(self):
        return bool(self.characteristics & 0x20000000)


class Image:
    def __init__(self, data: bytes):
        self.data = data
        self.require(0, 64)
        if data[:2] != b'MZ':
            raise ValueError('not an MZ executable')
        pe = self.unpack('<I', 0x3c)[0]
        self.require(pe, 24)
        if data[pe:pe + 4] != b'PE\0\0':
            raise ValueError('not a PE executable')
        machine, count, self.timestamp, _, _, opt_size, _ = self.unpack('<HHIIIHH', pe + 4)
        opt = pe + 24
        self.require(opt, opt_size)
        if machine != 0x14c or opt_size < 96 or self.unpack('<H', opt)[0] != 0x10b:
            raise ValueError('expected an i386 PE32 image')
        self.linker = list(data[opt + 2:opt + 4])
        self.entry_rva = self.unpack('<I', opt + 16)[0]
        self.image_base = self.unpack('<I', opt + 28)[0]
        self.image_size = self.unpack('<I', opt + 56)[0]
        self.header_size = self.unpack('<I', opt + 60)[0]
        directory_count = self.unpack('<I', opt + 92)[0]
        if directory_count > (opt_size - 96) // 8:
            raise ValueError('truncated PE data directories')
        self.directories = [self.unpack('<II', opt + 96 + i * 8)
                            for i in range(directory_count)]
        self.sections = []
        for i in range(count):
            at = opt + opt_size + i * 40
            self.require(at, 40)
            name = data[at:at + 8].split(b'\0')[0].decode('ascii')
            virtual, rva, raw_size, raw_offset = self.unpack('<IIII', at + 8)
            flags = self.unpack('<I', at + 36)[0]
            if raw_size:
                self.require(raw_offset, raw_size)
            self.sections.append(Section(name, rva, virtual, raw_size, raw_offset, flags))

    def require(self, offset, size):
        if offset < 0 or size < 0 or offset + size > len(self.data):
            raise ValueError('truncated or out-of-bounds PE data')

    def unpack(self, fmt, offset):
        self.require(offset, struct.calcsize(fmt))
        return struct.unpack_from(fmt, self.data, offset)

    def section_of(self, rva):
        return next((s for s in self.sections if s.rva <= rva < s.rva + s.mapped), None)

    def offset(self, rva, size=1):
        if 0 <= rva < self.header_size and rva + size <= self.header_size:
            self.require(rva, size)
            return rva
        section = self.section_of(rva)
        if section is None or rva + size > section.rva + section.raw_size:
            raise ValueError(f'RVA 0x{rva:x}: not backed by file bytes')
        return section.raw_offset + rva - section.rva

    def read(self, rva, size):
        offset = self.offset(rva, size)
        return self.data[offset:offset + size]

    def cstring(self, rva):
        offset = self.offset(rva)
        section = self.section_of(rva)
        end = section.raw_offset + section.raw_size if section else self.header_size
        zero = self.data.find(b'\0', offset, end)
        if zero < 0:
            raise ValueError('unterminated PE string')
        return self.data[offset:zero].decode('latin-1')

    def directory(self, index):
        return self.directories[index] if index < len(self.directories) else (0, 0)

    def exports(self):
        rva, size = self.directory(0)
        if not rva or not size:
            return []
        values = struct.unpack('<IIHHIIIIIII', self.read(rva, 40))
        _, _, _, _, _, base, count, named, functions, names, ordinals = values
        by_ordinal = {}
        for i in range(named):
            name_rva = struct.unpack('<I', self.read(names + i * 4, 4))[0]
            ordinal = struct.unpack('<H', self.read(ordinals + i * 2, 2))[0]
            if ordinal >= count:
                raise ValueError('export ordinal outside address table')
            by_ordinal.setdefault(ordinal, []).append(self.cstring(name_rva))
        result = []
        for i in range(count):
            address = struct.unpack('<I', self.read(functions + i * 4, 4))[0]
            if address:
                result.append(dict(names=by_ordinal.get(i, []), ordinal=base + i,
                                   rva=address, va=self.image_base + address,
                                   forwarder=self.cstring(address) if rva <= address < rva + size else None))
        return result

    def imports(self):
        rva, size = self.directory(1)
        if not rva or not size:
            return []
        result = []
        for at in range(rva, rva + size - 19, 20):
            original, stamp, chain, name, iat = struct.unpack('<IIIII', self.read(at, 20))
            if not any((original, stamp, chain, name, iat)):
                return result
            symbols = []
            table = original or iat
            for i in range(len(self.data) // 4):
                value = struct.unpack('<I', self.read(table + i * 4, 4))[0]
                if not value:
                    break
                symbols.append(dict(iat_rva=iat + i * 4,
                                    name=None if value & 0x80000000 else self.cstring(value + 2),
                                    ordinal=value & 0xffff if value & 0x80000000 else None))
            else:
                raise ValueError('unterminated import thunk table')
            result.append(dict(dll=self.cstring(name), symbols=symbols))
        raise ValueError('unterminated import directory')

    def relocations(self):
        rva, size = self.directory(5)
        result = []
        if not rva or not size:
            return result
        end = rva + size
        while rva < end:
            if end - rva < 8:
                raise ValueError('truncated relocation block')
            page, block_size = struct.unpack('<II', self.read(rva, 8))
            if block_size < 8 or block_size % 2 or rva + block_size > end:
                raise ValueError('invalid relocation block size')
            for at in range(rva + 8, rva + block_size, 2):
                value = struct.unpack('<H', self.read(at, 2))[0]
                typ, offset = value >> 12, value & 0xfff
                if typ:
                    result.append(dict(rva=page + offset, type=typ))
            rva += block_size
        return result

    def source_paths(self):
        rows = []
        pattern = rb'[A-Za-z]:\\[^\x00\r\n]{1,240}\.(?:cpp|c|h)\x00'
        for section in self.sections:
            blob = self.data[section.raw_offset:section.raw_offset + section.size]
            for match in re.finditer(pattern, blob, re.IGNORECASE):
                rows.append(dict(rva=section.rva + match.start(),
                                 path=match.group()[:-1].decode('latin-1')))
        return rows

    def report(self):
        return dict(sha256=hashlib.sha256(self.data).hexdigest(), size=len(self.data),
                    image_base=self.image_base, entry_va=self.image_base + self.entry_rva,
                    timestamp=self.timestamp, linker=self.linker,
                    sections=[asdict(s) for s in self.sections],
                    exports=self.exports(), imports=self.imports(),
                    relocations=self.relocations(), source_paths=self.source_paths())
