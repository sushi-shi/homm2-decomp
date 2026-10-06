"""Structural census of a retail image: function starts and absolute fields.

    homm2 --image editor audit census                 # report only
    homm2 --image editor audit census --write-config  # (re)write the tables

Retail instructions are decoded recursively from the PE entry point, every
direct call target, every code address an instruction operand or an
initialized data word names, the import thunks, C++ EH registration stubs and
their unwind funclets, and switch dispatch tables. Unreached code between
decoded bodies is admitted only at a literal VC6 frame prologue after the
trailing NOP/INT3 alignment fill. No decompiler output is used.

Outputs, all keyed to the selected image (`homm2 --image`):

  functions_eh.csv              the C++ EH unwind funclets
  functions.csv                 the function inventory the delinker reads
                                (entry_rva, byte_size to the next start, a
                                FUN_ placeholder name, thunk, kind); alignment
                                fill is not a function
  absolute_relocations.tsv      DIR32 fields: instruction operands whose value
                                lies inside a mapped section, switch-table
                                entries, and initialized data words that are
                                code addresses
  absolute_reference_evidence.tsv  one evidence row per field
  <image build>/gen/census.json  the report: counts, switch tables, EH
                                groups, thunks and rejected candidates

Instruction operands are admitted only at the decoded instruction boundary;
relative branches/calls are excluded. Data words are admitted here only when
they name a census code start or a switch-table target; data-to-data pointers
need typed corroboration (a neighbouring pointer word, an instruction user of
the target, or a string start), never range alone.

The game image's own tables predate this module and stay reviewed separately:
run on the game, the census is the control (`--compare
config/retail/functions.csv`); `--write-config` refuses the game.
"""

from __future__ import annotations

import argparse
import bisect
import hashlib
import json
import struct
from collections import defaultdict
from pathlib import Path

from homm2.core.paths import REPO, image_key, retail_dir, retail_exe
from homm2.core.pe import Pe

#: VC6 frame prologues that may open an otherwise unreached body.
PROLOGUES = (b"\x55\x8b\xec",)
FILL = (0x90, 0xCC)


class Census:
    def __init__(self, pe: Pe):
        from capstone import CS_ARCH_X86, CS_MODE_32, Cs
        self.pe = pe
        self.base = pe.image_base
        self.text_lo, self.text_hi = pe.text_span()
        text = pe.section(".text")
        self.text = pe.data[text["rptr"]:text["rptr"] + text["vsize"]]
        self.md = Cs(CS_ARCH_X86, CS_MODE_32)
        self.md.detail = True
        self.insns: dict[int, object] = {}     # rva -> capstone insn
        self.owner: dict[int, int] = {}        # insn rva -> function start
        self.starts: dict[int, list[str]] = {}  # start -> evidence
        self.kinds: dict[int, str] = {}
        self.tables: list[dict] = []           # switch dispatch tables
        self.byte_tables: dict[int, tuple[int, int]] = {}
        self.table_bytes: set[int] = set()
        self.fields: dict[int, dict] = {}      # site rva -> evidence row
        self.iat = {slot: (dll, name or f"ordinal_{ordinal}")
                    for slot, name, dll, ordinal in _import_slots(pe)}
        self.thunks: list[dict] = []
        self.eh_groups: list[dict] = []
        self.rejected: list[dict] = []
        self.mapped = [(s["va"], s["va"] + max(s["vsize"], s["rsize"]), s["name"])
                       for s in pe.sections]

    # -- helpers ----------------------------------------------------------
    def in_text(self, rva: int) -> bool:
        return self.text_lo <= rva < self.text_hi

    def section_of(self, rva: int) -> str | None:
        return next((name for lo, hi, name in self.mapped if lo <= rva < hi), None)

    def byte(self, rva: int) -> int:
        return self.text[rva - self.text_lo]

    def u32(self, rva: int) -> int | None:
        raw = self.pe.read(rva, 4)
        return struct.unpack("<I", raw)[0] if raw is not None and len(raw) == 4 else None

    def decode(self, rva: int):
        if rva in self.insns:
            return self.insns[rva]
        off = rva - self.text_lo
        insn = next(self.md.disasm(self.text[off:off + 16], self.base + rva, 1), None)
        if insn is not None:
            self.insns[rva] = insn
        return insn

    def add_start(self, rva: int, evidence: str, kind: str = "") -> bool:
        if not self.in_text(rva):
            return False
        new = rva not in self.starts
        self.starts.setdefault(rva, []).append(evidence)
        if kind and not self.kinds.get(rva):
            self.kinds[rva] = kind
        return new

    # -- recursive descent -------------------------------------------------
    def walk(self, start: int) -> list[int]:
        """Decode the body at `start`; return newly discovered call targets."""
        from capstone.x86 import X86_OP_IMM, X86_OP_MEM
        found = []
        work = [start]
        while work:
            pc = work.pop()
            while self.in_text(pc):
                if pc in self.owner or pc in self.table_bytes:
                    break
                insn = self.decode(pc)
                if insn is None:
                    break
                self.owner[pc] = start
                mnem = insn.mnemonic
                ops = insn.operands
                nxt = pc + insn.size
                # absolute operand fields
                self._operand_fields(insn, pc)
                self._byte_table(start, insn)
                if mnem == "call":
                    if ops and ops[0].type == X86_OP_IMM:
                        target = ops[0].imm - self.base
                        if self.in_text(target):
                            found.append((target, f"call from 0x{pc:x}"))
                    pc = nxt
                    continue
                if mnem == "jmp":
                    if ops and ops[0].type == X86_OP_IMM:
                        target = ops[0].imm - self.base
                        if target in self.starts and target != start:
                            break                       # tail transfer
                        member = self.library_member(target)
                        if member is not None and target not in self.lib_starts:
                            work.append(target)
                            break
                        if target < start or self._eh_stub(start, pc):
                            # a backward transfer leaves the body; an EH
                            # registration stub jumps to the frame handler
                            found.append((target, f"jmp from 0x{pc:x}"))
                            break
                        work.append(target)
                        break
                    if ops and ops[0].type == X86_OP_MEM:
                        mem = ops[0].mem
                        disp = mem.disp & 0xFFFFFFFF
                        if mem.base == 0 and mem.index and mem.scale == 4 \
                                and self.in_text(disp - self.base):
                            work.extend(self._switch(start, pc, disp - self.base))
                        elif mem.base == 0 and mem.index == 0 \
                                and (disp - self.base) in self.iat:
                            self._thunk(pc, disp - self.base)
                    break
                if mnem.startswith("j") or mnem.startswith("loop"):
                    if ops and ops[0].type == X86_OP_IMM:
                        work.append(ops[0].imm - self.base)
                    pc = nxt
                    continue
                if mnem in ("ret", "retf", "iret", "iretd", "hlt", "int3", "ud2"):
                    break
                pc = nxt
        return found

    def _operand_fields(self, insn, pc: int) -> None:
        """Admit imm32/disp32 operand fields that name a mapped section."""
        from capstone.x86 import X86_OP_IMM, X86_OP_MEM, X86_REG_CS, X86_REG_DS
        if insn.mnemonic in ("call", "jmp") or insn.mnemonic.startswith("j") \
                or insn.mnemonic.startswith("loop"):
            # rel32 targets are not fields; the indirect forms are.
            if not any(op.type == X86_OP_MEM for op in insn.operands):
                return
        raw = bytes(insn.bytes)
        values = []
        for op in insn.operands:
            if op.type == X86_OP_IMM and op.size == 4:
                values.append(("imm32", op.imm & 0xFFFFFFFF))
            elif op.type == X86_OP_MEM and op.mem.disp and op.mem.segment in (0, X86_REG_CS, X86_REG_DS):
                values.append(("disp32", op.mem.disp & 0xFFFFFFFF))
        for kind, value in values:
            rva = value - self.base
            section = self.section_of(rva)
            if section is None or rva < self.text_lo:
                continue
            packed = struct.pack("<I", value)
            # Locate the field: capstone reports immediate/displacement
            # offsets only through x86 detail; search the encoding instead
            # and require a single occurrence of the 4-byte value.
            hits = [i for i in range(1, len(raw) - 3) if raw[i:i + 4] == packed]
            if not hits:
                continue
            offset = insn.disp_offset if kind == "disp32" and insn.disp_offset else \
                insn.imm_offset if kind == "imm32" and insn.imm_offset else hits[-1]
            site = pc + offset
            self.fields.setdefault(site, {
                "site": site, "target": rva, "channel": "instruction",
                "instruction": pc,
                "evidence": f"{kind}; {insn.mnemonic} {insn.op_str}"})

    def _byte_table(self, owner: int, insn) -> None:
        """`mov/movzx r8, byte ptr [reg + table]` with the table in .text: a
        VC6 switch index table, bounded after the walk by its dword table."""
        from capstone.x86 import X86_OP_MEM
        if insn.mnemonic not in ("mov", "movzx", "movsx") or len(insn.operands) != 2:
            return
        op = insn.operands[1]
        if op.type != X86_OP_MEM or op.size != 1 or not op.mem.index and not op.mem.base:
            return
        table = (op.mem.disp & 0xFFFFFFFF) - self.base
        if self.in_text(table):
            self.byte_tables.setdefault(table, (owner, insn.address - self.base))

    def _bound_byte_tables(self) -> None:
        counts: dict[int, int] = defaultdict(int)
        for t in self.tables:
            n = (int(t["end"], 16) - int(t["start"], 16)) // 4
            counts[int(t["owner"], 16)] = max(counts[int(t["owner"], 16)], n)
        for table, (owner, site) in sorted(self.byte_tables.items()):
            limit = counts.get(owner, 0)
            pos = table
            while self.in_text(pos) and pos not in self.owner and pos not in self.starts \
                    and pos not in self.table_bytes and self.byte(pos) < max(limit, 1):
                pos += 1
            if pos == table:
                continue
            self.table_bytes.update(range(table, pos))
            self.tables.append({"start": f"0x{table:x}", "end": f"0x{pos:x}",
                                "owner": f"0x{owner:x}", "index": f"0x{site:x}"})

    def _switch(self, owner: int, site: int, table: int) -> list[int]:
        """Entries of a `jmp [reg*4 + table]` dispatch table into the owner's
        own code; the table ends at the first word that is not one."""
        targets = []
        pos = table
        while self.in_text(pos + 3):
            value = self.u32(pos)
            target = (value or 0) - self.base
            if not (owner <= target < table):
                break
            targets.append(target)
            pos += 4
        if not targets:
            return []
        for i in range(table, pos):
            self.table_bytes.add(i)
        for i in range(table, pos, 4):
            self.fields.setdefault(i, {
                "site": i, "target": self.u32(i) - self.base,
                "channel": "text-dispatch", "instruction": site,
                "evidence": f"switch table of 0x{owner:x} at 0x{table:x}"})
        self.tables.append({"start": f"0x{table:x}", "end": f"0x{pos:x}",
                            "owner": f"0x{owner:x}", "jump": f"0x{site:x}"})
        return targets

    def _thunk(self, pc: int, slot: int) -> None:
        if self.byte(pc) == 0xFF and self.byte(pc + 1) == 0x25 \
                and self.owner.get(pc) == pc:
            dll, name = self.iat[slot]
            self.kinds[pc] = "thunk"
            self.thunks.append({"rva": f"0x{pc:x}", "slot_rva": f"0x{slot:x}",
                                "dll": dll, "import_key": name})

    # -- seeds ---------------------------------------------------------------
    def code_pointers_in_data(self) -> list[tuple[int, int]]:
        """(site, target) for every 4-aligned initialized data word that is a
        .text address."""
        out = []
        for s in self.pe.sections:
            if s["name"] in (".text", ".rsrc") or not s["rsize"]:
                continue
            raw = self.pe.data[s["rptr"]:s["rptr"] + min(s["rsize"], s["vsize"] or s["rsize"])]
            for off in range(0, len(raw) - 3, 4):
                value = struct.unpack_from("<I", raw, off)[0] - self.base
                if self.in_text(value):
                    out.append((s["va"] + off, value))
        return out

    def library_contributions(self) -> None:
        """Whole VC6 LIBCMT/OLDNAMES code contributions found in .text with
        their relocation fields masked on both sides.
        A unique hit fixes the member's function starts, its internal labels
        and its DIR32 fields independently of the instruction walk."""
        import re
        from homm2.core.coff import CoffObject, MEM_EXECUTE, RELOCATION_WIDTHS
        from homm2.tool.wine import find_ci, msvc_dir
        self.lib_ranges: list[tuple[int, int, str]] = []
        self.lib_starts: dict[int, str] = {}
        self.lib_labels: dict[int, str] = {}   # every function-typed member symbol
        self.lib_names: dict[int, str] = {}    # public LIBCMT symbol at a start
        self.lib_data_code: list[tuple[int, int]] = []
        self.library_data_ranges: list[tuple[int, int]] = []
        for library in ("libcmt.lib", "oldnames.lib"):
            path = find_ci(msvc_dir() / "lib", library)
            if path is None or not path.is_file():
                continue
            for member, payload in _ar_members(path):
                try:
                    coff = CoffObject(payload)
                except Exception:
                    continue
                for section in coff.sections:
                    if section.raw_size < 8:
                        continue
                    if not section.characteristics & MEM_EXECUTE:
                        if section.raw_offset and section.characteristics & 0x40 \
                                and section.name in (".data", ".rdata"):
                            self._library_data(coff, section, f"{library}:{member}")
                        continue
                    body = coff.section_bytes(section)
                    relocs = [r for r in coff.relocations if r.section == section.index]
                    mask = bytearray(len(body))
                    for r in relocs:
                        for i in range(r.site, min(len(body), r.site + RELOCATION_WIDTHS.get(r.typ, 4))):
                            mask[i] = 1
                    parts, i = [], 0
                    while i < len(body):
                        j = i
                        while j < len(body) and mask[j] == mask[i]:
                            j += 1
                        parts.append(b".{%d}" % (j - i) if mask[i] else re.escape(body[i:j]))
                        i = j
                    hits = [m.start() for m in re.finditer(b"".join(parts), self.text, re.S)]
                    # Byte-identical members (memcpy/memmove) place one body
                    # per member; a short body is only admitted when unique.
                    if not hits or len(hits) > (4 if len(body) >= 32 else 1):
                        continue
                    for hit in hits:
                        self._library_hit(coff, section, body, relocs, hit,
                                          f"{library}:{member}")
        # The candidate objects' own initialized data: a uniquely placed,
        # byte-identical section fixes its pointer fields the same way.
        from homm2.core.paths import IMAGE_BUILD
        BASE_OBJS = IMAGE_BUILD / "objdiff/base"
        for path in sorted(BASE_OBJS.rglob("*.obj")):
            try:
                coff = CoffObject(path.read_bytes())
            except Exception:
                continue
            name = "object:" + path.relative_to(BASE_OBJS).with_suffix("").as_posix()
            for section in coff.sections:
                if section.raw_size >= 8 and section.raw_offset \
                        and not section.characteristics & MEM_EXECUTE \
                        and section.characteristics & 0x40 \
                        and section.name in (".data", ".rdata"):
                    self._library_data(coff, section, name)
        self.lib_ranges.sort()
        self._lib_los = [lo for lo, _hi, _n in self.lib_ranges]

    def _library_hit(self, coff, section, body, relocs, hit, name) -> None:
        lo = self.text_lo + hit
        hi = lo + len(body)
        self.lib_ranges.append((lo, hi, name))
        # Public symbols are entries. A static function symbol is an entry
        # only in compiler output (which carries .debug$F frame records);
        # MASM marks its internal PROC labels as static functions too.
        compiled = any(sec.name == ".debug$F" for sec in coff.sections)
        labels = sorted({sym.value for sym in coff.symbols.values()
                         if sym.section == section.index
                         and (sym.storage_class == 2
                              or (compiled and sym.typ == 0x20))})
        for value in labels:
            if value < len(body):
                self.lib_starts[lo + value] = name
        for sym in coff.symbols.values():
            if sym.section == section.index and sym.storage_class == 2 \
                    and sym.value < len(body) and name.startswith("libcmt"):
                self.lib_names.setdefault(lo + sym.value, sym.name)
        for sym in coff.symbols.values():
            if sym.section == section.index and sym.typ == 0x20 and sym.value < len(body):
                self.lib_labels[lo + sym.value] = name
        for r in relocs:
            if r.typ != 0x6:                     # DIR32 only
                continue
            site = lo + r.site
            target = self.u32(site) - self.base
            if self.section_of(target) is None:
                continue          # absolute symbol (__except_list)
            self.fields.setdefault(site, {
                "site": site, "target": target,
                "channel": "library", "instruction": None,
                "evidence": f"DIR32 of {name} contribution 0x{lo:x}+0x{len(body):x}"})

    def _library_data(self, coff, section, name) -> None:
        """A member's initialized data placed uniquely in .rdata/.data: its
        DIR32 relocations are absolute fields at any byte alignment."""
        import re
        from homm2.core.coff import RELOCATION_WIDTHS
        body = coff.section_bytes(section)
        relocs = [r for r in coff.relocations if r.section == section.index]
        if not relocs:
            return
        mask = bytearray(len(body))
        for r in relocs:
            for i in range(r.site, min(len(body), r.site + RELOCATION_WIDTHS.get(r.typ, 4))):
                mask[i] = 1
        if sum(1 for i, b in enumerate(body) if not mask[i] and b) < 4:
            return                      # too little fixed content to place
        parts, i = [], 0
        while i < len(body):
            j = i
            while j < len(body) and mask[j] == mask[i]:
                j += 1
            parts.append(b".{%d}" % (j - i) if mask[i] else re.escape(body[i:j]))
            i = j
        pattern = re.compile(b"".join(parts), re.S)
        hits = []
        for s in self.pe.sections:
            if s["name"] not in (".rdata", ".data") or not s["rsize"]:
                continue
            raw = self.pe.data[s["rptr"]:s["rptr"] + s["rsize"]]
            hits += [s["va"] + m.start() for m in pattern.finditer(raw)]
        if len(hits) != 1:
            return
        lo = hits[0]
        self.library_data_ranges.append((lo, lo + len(body)))
        for r in relocs:
            if r.typ != 0x6:
                continue
            site = lo + r.site
            target = self.u32(site) - self.base
            if self.section_of(target) is None:
                continue
            self.fields.setdefault(site, {
                "site": site, "target": target,
                "channel": "object-data" if name.startswith("object:") else "library-data",
                "instruction": None,
                "evidence": f"DIR32 of {name} {section.name} contribution 0x{lo:x}+0x{len(body):x}"})
            if self.in_text(target):
                self.lib_data_code.append((site, target))

    def library_member(self, rva: int) -> tuple[int, int, str] | None:
        i = bisect.bisect_right(getattr(self, "_lib_los", []), rva) - 1
        if i >= 0 and self.lib_ranges[i][0] <= rva < self.lib_ranges[i][1]:
            return self.lib_ranges[i]
        return None

    def run(self) -> None:
        self.library_contributions()
        for rva, name in sorted(self.lib_starts.items()):
            lo, hi, _ = self.library_member(rva)
            self.add_start(rva, f"VC6 {name.split(':')[0].upper()} code contribution 0x{lo:x}+0x{hi - lo:x}")
        pending = []
        for rva in sorted(self.lib_starts):
            pending.extend(self.walk(rva))
        entry = struct.unpack_from("<I", self.pe.data,
                                   struct.unpack_from("<I", self.pe.data, 0x3C)[0] + 40)[0]
        pending.append((entry, "PE entry point"))
        data_ptrs = sorted(set(self.code_pointers_in_data()) | set(self.lib_data_code))
        rounds = 0
        tried: set[int] = set()
        while True:
            rounds += 1
            while pending:
                rva, why = pending.pop()
                if rva in tried:
                    self.starts.get(rva, []).append(why) if rva in self.starts else None
                    continue
                tried.add(rva)
                member = self.library_member(rva)
                # a member's function label that a data word dispatches to is
                # an entry even inside the member's decoded flow
                data_label = rva in self.lib_labels and why.startswith(
                    ("code address in data", "call from"))
                if data_label:
                    self.add_start(rva, why)
                    continue
                if member is not None and rva not in self.lib_starts:
                    self.rejected.append({"rva": f"0x{rva:x}", "reason":
                                          f"internal label within {member[2]}: {why}"})
                    continue
                if rva in self.owner and self.owner[rva] != rva:
                    # interior of a decoded body: an internal label
                    self.rejected.append({"rva": f"0x{rva:x}", "reason":
                                          f"interior of 0x{self.owner[rva]:x}: {why}"})
                    continue
                if not self.add_start(rva, why):
                    continue
                for target, how in self.walk(rva):
                    pending.append((target, how))
            # code operands naming .text (function pointers, EH stubs)
            self._bound_byte_tables()
            for site, row in sorted(self.fields.items()):
                if row["channel"] == "instruction" and self.in_text(row["target"]) \
                        and row["evidence"].startswith("imm32") \
                        and row["target"] not in self.starts \
                        and row["target"] not in self.owner \
                        and row["target"] not in self.table_bytes:
                    pending.append((row["target"],
                                    f"code address operand at 0x{row['instruction']:x}"))
            accepted = {site for site, target in data_ptrs
                        if target in self.starts}
            for site, target in data_ptrs:
                if target in self.lib_labels and target not in self.starts:
                    pending.append((target, f"code address in data at 0x{site:x}"))
                    continue
                if target in self.starts or target in self.owner \
                        or target in self.table_bytes:
                    continue
                body = self.text[target - self.text_lo:target - self.text_lo + 3]
                if body in PROLOGUES or site - 4 in accepted or site + 4 in accepted \
                        or target in self.lib_labels:
                    pending.append((target, f"code address in data at 0x{site:x}"))
            pending = [(r, w) for r, w in pending if r not in tried]
            for source in (self._funcinfo_entries, self._thunk_starts, self._gap_starts):
                if not pending:
                    pending = [(r, w) for r, w in source() if r not in tried]
            if not pending:
                break
        self._eh()
        self._pads()
        self._data_fields(data_ptrs)

    def funcinfo(self, stub: int) -> int | None:
        """The FuncInfo a `mov eax, imm32; jmp` registration stub names."""
        body = self.text[stub - self.text_lo:stub - self.text_lo + 10]
        if len(body) != 10 or body[0] != 0xB8 or body[5] != 0xE9:
            return None
        info = struct.unpack_from("<I", body, 1)[0] - self.base
        return info if self.u32(info) == 0x19930520 else None

    def _funcinfo_entries(self) -> list[tuple[int, str]]:
        """Unwind actions and catch handlers named by each stub's FuncInfo."""
        out = []
        for stub in sorted(self.starts):
            info = self.funcinfo(stub)
            if info is None:
                continue
            states, unwind = self.u32(info + 4), self.u32(info + 8) - self.base
            targets = [self.u32(unwind + i * 8 + 4) for i in range(states)]
            tries, trymap = self.u32(info + 12), self.u32(info + 16)
            for t in range(tries or 0):
                entry = trymap - self.base + t * 20
                catches, handlers = self.u32(entry + 12), self.u32(entry + 16)
                targets += [self.u32(handlers - self.base + c * 16 + 12)
                            for c in range(catches)]
            for value in targets:
                if value and self.in_text(value - self.base) \
                        and value - self.base not in self.starts:
                    out.append((value - self.base,
                                f"C++ FuncInfo 0x{info:x} unwind/catch entry"))
        return out

    def _eh_stub(self, start: int, pc: int) -> bool:
        """`mov eax, FuncInfo; jmp ___CxxFrameHandler` - a registration stub."""
        body = self.text[start - self.text_lo:start - self.text_lo + 5]
        return pc == start + 5 and len(body) == 5 and body[0] == 0xB8

    def _thunk_starts(self) -> list[tuple[int, str]]:
        """Unreferenced import thunks (`jmp [IAT slot]`): /OPT:NOREF keeps a
        thunk for every slot, so a thunk run need not be called."""
        out = []
        pos = self.text_lo
        while pos < self.text_hi - 6:
            if self.byte(pos) == 0xFF and self.byte(pos + 1) == 0x25 \
                    and pos not in self.owner and pos not in self.table_bytes:
                slot = struct.unpack_from("<I", self.text, pos - self.text_lo + 2)[0] - self.base
                if slot in self.iat and pos not in self.starts:
                    out.append((pos, "FF25 instruction"))
                    pos += 6
                    continue
            pos += 1
        return out

    def _decoded_end(self, start: int) -> int:
        ends = [pc + self.insns[pc].size for pc, owner in self.owner.items()
                if owner == start]
        return max(ends) if ends else start

    def _gap_starts(self) -> list[tuple[int, str]]:
        """Unreached bodies: a literal prologue after the previous body's
        decoded end (plus its dispatch tables) and its NOP/INT3 fill."""
        ends = {}
        for pc in self.owner:
            end = pc + self.insns[pc].size
            ends[pc] = end
        occupied = bytearray(self.text_hi - self.text_lo)
        for pc, end in ends.items():
            occupied[pc - self.text_lo:end - self.text_lo] = b"\1" * (end - pc)
        for i in self.table_bytes:
            occupied[i - self.text_lo] = 1
        out = []
        pos = self.text_lo
        while pos < self.text_hi:
            if occupied[pos - self.text_lo]:
                pos += 1
                continue
            gap = pos
            while pos < self.text_hi and not occupied[pos - self.text_lo]:
                pos += 1
            cur = gap
            while cur < pos and self.byte(cur) in FILL:
                cur += 1
            if cur < pos and any(self.text[cur - self.text_lo:cur - self.text_lo + len(p)] == p
                                 for p in PROLOGUES):
                out.append((cur, f"VC6 frame prologue after decoded code (gap 0x{gap:x})"))
            elif gap < cur < pos and cur % 16 == 0:
                out.append((cur, f"code after NOP/INT3 fill at a 16-byte object boundary "
                                 f"(gap 0x{gap:x})"))
        return out

    def _eh(self) -> None:
        """C++ EH registration stubs (`mov eax, FuncInfo; jmp handler`) and
        the unwind funclets their FuncInfo records name."""
        rdata = self.pe.section(".rdata")
        for rva in sorted(self.starts):
            body = self.text[rva - self.text_lo:rva - self.text_lo + 10]
            if len(body) == 10 and body[0] == 0xB8 and body[5] == 0xE9:
                info = struct.unpack_from("<I", body, 1)[0] - self.base
                if not (rdata["va"] <= info < rdata["va"] + rdata["vsize"]):
                    continue
                magic = self.u32(info)
                if magic != 0x19930520:
                    continue
                states = self.u32(info + 4)
                unwind = self.u32(info + 8) - self.base
                funclets = []
                for i in range(states):
                    action = self.u32(unwind + i * 8 + 4)
                    if action:
                        funclets.append(action - self.base)
                self.kinds[rva] = "eh"
                for f in funclets:
                    if f in self.starts:
                        self.kinds[f] = "eh"
                self.eh_groups.append({"stub": f"0x{rva:x}", "funcinfo": f"0x{info:x}",
                                       "states": states,
                                       "funclets": [f"0x{f:x}" for f in funclets]})

    def _pads(self) -> None:
        """Trailing NOP/INT3 alignment fill after a body becomes a pad row
        when it reaches the next start (object alignment)."""
        starts = sorted(self.starts)
        for start, nxt in zip(starts, starts[1:] + [self.text_hi]):
            if self.kinds.get(start) in ("thunk", "pad"):
                continue
            end = max(self._decoded_end_cached(start), self._table_end(start))
            pad = nxt
            while pad > max(end, start + 1) and self.byte(pad - 1) in FILL:
                pad -= 1
            if start < pad < nxt and pad >= end:
                self.add_start(pad, f"literal NOP/INT3 alignment fill through 0x{nxt:x}",
                               "pad")

    def _decoded_end_cached(self, start: int) -> int:
        if not hasattr(self, "_ends"):
            ends: dict[int, int] = defaultdict(int)
            for pc, owner in self.owner.items():
                ends[owner] = max(ends[owner], pc + self.insns[pc].size)
            self._ends = ends
        return self._ends.get(start, start)

    def _table_end(self, start: int) -> int:
        ends = [int(t["end"], 16) for t in self.tables if int(t["owner"], 16) == start]
        return max(ends) if ends else start

    def _data_fields(self, data_ptrs) -> None:
        for site, target in data_ptrs:
            if target in self.starts:
                self.fields.setdefault(site, {
                    "site": site, "target": target, "channel": "data",
                    "instruction": None,
                    "evidence": f"data word names census start 0x{target:x}"})
            else:
                self.rejected.append({"rva": f"0x{site:x}", "reason":
                                      f"data word 0x{target + self.base:x} is not a census start"})
        self._eh_record_fields()
        self._data_pointer_fields()

    def _eh_record_fields(self) -> None:
        """Pointer words of each registration stub's typed FuncInfo record:
        the unwind map, the try map, their actions, handler arrays and types."""
        def admit(site: int, why: str) -> None:
            value = self.u32(site)
            if value and self.section_of(value - self.base) is not None:
                self.fields.setdefault(site, {
                    "site": site, "target": value - self.base, "channel": "eh-record",
                    "instruction": None, "evidence": why})
        for group in self.eh_groups:
            info = int(group["funcinfo"], 16)
            states, tries = self.u32(info + 4), self.u32(info + 12)
            admit(info + 8, f"FuncInfo 0x{info:x} pUnwindMap")
            admit(info + 16, f"FuncInfo 0x{info:x} pTryBlockMap")
            unwind = self.u32(info + 8) - self.base
            for i in range(states):
                admit(unwind + i * 8 + 4, f"FuncInfo 0x{info:x} unwind action {i}")
            for t in range(tries or 0):
                entry = self.u32(info + 16) - self.base + t * 20
                admit(entry + 16, f"FuncInfo 0x{info:x} try {t} pHandlerArray")
                handlers = self.u32(entry + 16) - self.base
                for c in range(self.u32(entry + 12)):
                    admit(handlers + c * 16 + 4, f"FuncInfo 0x{info:x} try {t} handler {c} pType")
                    admit(handlers + c * 16 + 12, f"FuncInfo 0x{info:x} try {t} handler {c} address")

    def _data_pointer_fields(self) -> None:
        """4-aligned initialized words naming .rdata/.data, outside the exactly
        placed library data, admitted only with a typed corroboration: a
        neighbouring pointer word (pointer tables and records), a target an
        instruction operand also names, or a target that begins a string.
        Measured on the reviewed game manifest this keeps 1210 of its 1527
        data-to-data fields (the rest are EH records, admitted separately) with
        three false positives; review rows supersede it."""
        ranges = [(lo, hi) for lo, hi in self.library_data_ranges]
        regions = [s for s in self.pe.sections if s["name"] in (".rdata", ".data")]

        def mapped(v: int) -> bool:
            return any(s["va"] <= v < s["va"] + max(s["vsize"], s["rsize"]) for s in regions)

        def byte(rva: int) -> int:
            raw = self.pe.read(rva, 1)
            return raw[0] if raw else 0
        code_targets = {r["target"] for r in self.fields.values()
                        if r["channel"] == "instruction"}
        reviewed = _reviewed_exclusions()
        cand = {}
        for s in regions:
            raw = self.pe.data[s["rptr"]:s["rptr"] + s["rsize"]]
            for off in range(0, len(raw) - 3, 4):
                value = struct.unpack_from("<I", raw, off)[0] - self.base
                if mapped(value):
                    cand[s["va"] + off] = value
        for site, value in sorted(cand.items()):
            if site in self.fields or any(lo <= site < hi for lo, hi in ranges):
                continue
            if site in reviewed:
                self.rejected.append({"rva": f"0x{site:x}", "reason": "reviewed: " + reviewed[site]})
                continue
            string = byte(value - 1) == 0 and 32 <= byte(value) < 127 and 32 <= byte(value + 1) < 127
            why = ("neighbouring pointer word" if site - 4 in cand or site + 4 in cand
                   else "target named by an instruction operand" if value in code_targets
                   else "target begins a string" if string else None)
            if why is None:
                self.rejected.append({"rva": f"0x{site:x}", "reason":
                                      f"uncorroborated data word 0x{value + self.base:x}"})
                continue
            self.fields[site] = {"site": site, "target": value, "channel": "data-pointer",
                                 "instruction": None, "evidence": why}


def _reviewed_exclusions() -> dict[int, str]:
    """Data words the image's reloc_exclusions.tsv reviews as ordinary payload
    whose value merely falls inside the image (site_rva, reason)."""
    path = retail_dir() / "reloc_exclusions.tsv"
    rows = {}
    if path.exists():
        for line in path.read_text().splitlines():
            if not line.startswith("0x"):
                continue
            site, reason = line.split("\t")[:2]
            rows[int(site, 16)] = reason
    return rows


def _ar_members(path: Path):
    """Yield (member-name, body) for each member of a `!<arch>` library."""
    data = Path(path).read_bytes()
    if data[:8] != b"!<arch>\n":
        return
    i = 8
    while i + 60 <= len(data):
        name = data[i:i + 16].decode("latin1").strip()
        try:
            size = int(data[i + 48:i + 58].decode("latin1").strip() or 0)
        except ValueError:
            return
        yield name, data[i + 60:i + 60 + size]
        i += 60 + size + (size & 1)


def _import_slots(pe: Pe):
    """[(iat_slot_rva, name_or_None, dll, ordinal_or_None)] from the PE."""
    from homm2.core.image import Image
    out = []
    for library in Image(pe.data).imports():
        for symbol in library["symbols"]:
            out.append((symbol["iat_rva"], symbol["name"], library["dll"], symbol["ordinal"]))
    return out


def write_tables(census: Census, out: Path) -> dict:
    digest = hashlib.sha256(census.pe.data).hexdigest()
    out.mkdir(parents=True, exist_ok=True)
    starts = sorted(census.starts)
    ends = starts[1:] + [census.text_hi]
    lines = [
        f"# {census.pe.path.name} structural census: homm2 audit census "
        "(scripts/homm2/audit/census.py).",
        f"# image-sha256: {digest}",
        "# Starts from retail instructions alone; names are placeholders and strict",
        "# source claims are separate. byte_size runs to the next start (alignment",
        "# fill is its own start and is not listed). kind: '' | thunk | eh.",
        "entry_rva,byte_size,name,thunk,chunks,max_gap,total_gap,extent,kind",
    ]
    stubs = {int(group["stub"], 16) for group in census.eh_groups}
    funclets = []
    for rva, end in zip(starts, ends):
        kind = census.kinds.get(rva, "")
        if kind == "pad" or rva in stubs:
            # Alignment fill is not a function; an EH registration stub
            # (`mov eax, FuncInfo; jmp handler`) is compiler-local .text$x
            # data of its parent, named by the relocation that loads it.
            continue
        if kind == "eh":
            funclets.append((rva, end - rva))
        size = end - rva
        lines.append(f"0x{rva:x},{size},FUN_{census.base + rva:08x},"
                     f"{int(kind == 'thunk')},1,0,0,{size},{kind}")
    (out / "functions.csv").write_text("\n".join(lines) + "\n")
    (out / "functions_eh.csv").write_text("\n".join([
        "# SEH unwind funclets named by their C++ FuncInfo unwind maps (homm2 audit",
        "# census). Names are the funclets' own addresses until each is attributed",
        "# to its parent function.",
        "entry_rva,size,name",
    ] + [f"0x{rva:x},{size},Unwind@{census.base + rva:08x}" for rva, size in funclets]) + "\n")
    fields = sorted(census.fields.values(), key=lambda r: r["site"])
    (out / "absolute_relocations.tsv").write_text("\n".join([
        f"# image-sha256: {digest}",
        "# Instruction operands, dispatch slots, EH records and pointer data words",
        "# (homm2 audit census). Evidence: absolute_reference_evidence.tsv",
        "site_rva\tkind",
    ] + [f"0x{r['site']:x}\tdir32" for r in fields]) + "\n")
    (out / "absolute_reference_evidence.tsv").write_text("\n".join([
        f"# image-sha256: {digest}",
        "site_rva\ttarget_rva\tchannel\tinstruction_rva\tevidence",
    ] + [f"0x{r['site']:x}\t0x{r['target']:x}\t{r['channel']}\t"
         f"{'' if r['instruction'] is None else hex(r['instruction'])}\t{r['evidence']}"
         for r in fields]) + "\n")
    summary = summarize(census)
    payload = {"image_sha256": digest, "method": __doc__.split("\n\n")[1].split("\n"),
               "counts": summary,
               "starts": [{"rva": f"0x{r:08x}", "kind": census.kinds.get(r, ""),
                           "evidence": census.starts[r][:3],
                           **({"library_symbol": census.lib_names[r]}
                              if r in census.lib_names else {})} for r in starts],
               "switch_tables": census.tables, "eh_groups": census.eh_groups,
               "import_thunks": census.thunks, "rejected": census.rejected}
    from homm2.core.paths import gen_dir
    report = gen_dir() / "census.json"
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps(payload, indent=1) + "\n")
    return summary


def summarize(census: Census) -> dict:
    kinds = defaultdict(int)
    for r in census.starts:
        kinds[census.kinds.get(r, "") or "function"] += 1
    channels = defaultdict(int)
    for r in census.fields.values():
        channels[r["channel"]] += 1
    return {"starts": len(census.starts), **{f"kind_{k}": v for k, v in sorted(kinds.items())},
            "switch_tables": len(census.tables), "eh_groups": len(census.eh_groups),
            "fields": len(census.fields), **{f"fields_{k}": v for k, v in sorted(channels.items())},
            "rejected": len(census.rejected)}


def compare_with_reviewed(census: Census, path: Path) -> dict:
    """Starts against a reviewed table: functions.csv (entry_rva first) or a
    tab-separated rva table."""
    reviewed = set()
    for line in path.read_text().splitlines():
        if line.startswith(("#", "rva", "entry_rva")) or not line.strip():
            continue
        reviewed.add(int(line.replace(",", "\t").split("\t")[0], 16))
    found = set(census.starts)
    return {"reviewed": len(reviewed), "found": len(found),
            "both": len(reviewed & found),
            "missing": [f"0x{r:x}" for r in sorted(reviewed - found)],
            "extra": [f"0x{r:x}" for r in sorted(found - reviewed)]}


from homm2.core.usage import logged


@logged
def main(argv=None) -> int:
    ap = argparse.ArgumentParser(prog="homm2 audit census", description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--write-config", action="store_true",
                    help="write the census tables under the image's config/retail directory")
    ap.add_argument("--compare", type=Path,
                    help="compare starts with a reviewed functions table (control)")
    ap.add_argument("--out", type=Path,
                    help="write the tables to DIR instead (controls, review)")
    a = ap.parse_args(argv)
    census = Census(Pe(retail_exe()))
    census.run()
    summary = summarize(census)
    print(f"[census] {image_key()}: " + ", ".join(f"{k}={v}" for k, v in summary.items()))
    if a.compare:
        diff = compare_with_reviewed(census, a.compare)
        print(f"[census] control {a.compare}: reviewed={diff['reviewed']} "
              f"found={diff['found']} both={diff['both']} "
              f"missing={len(diff['missing'])} extra={len(diff['extra'])}")
        print("[census] missing: " + " ".join(diff["missing"][:40]))
        print("[census] extra: " + " ".join(diff["extra"][:40]))
    if a.out:
        write_tables(census, a.out)
        print(f"[census] wrote {a.out}")
    if a.write_config:
        if image_key() == "game":
            ap.error("the game census is reviewed separately; select --image")
        write_tables(census, retail_dir())
        print(f"[census] wrote {retail_dir().relative_to(REPO)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
