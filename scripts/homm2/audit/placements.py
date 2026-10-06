"""Place the game's claimed identities in another image by retail byte correspondence.

    homm2 --image editor audit placements                 # report only
    homm2 --image editor audit placements --write-config  # (re)write placements.tsv
    homm2 --image editor audit placements --check         # fail on drift

The game's claimed inventory (build/gen/symbol_names.csv, from source `VA`,
`DATA` and `VTBL` markers) is joined to the selected image's census
(config/retail/<image>/functions.csv and absolute_relocations.tsv):

- A function is placed where its game body, with every absolute field of the
  game's relocation manifest and every rel32 operand masked, occurs at a census
  start of the image. A body found more than once is placed at the offset its
  unit's other bodies share (one object's code moves as a block). Two game
  bodies placed at one image address are byte-identical and stay unnamed.
- A datum is placed by its code users: each absolute field of a placed body
  pairs the game target with the image target at the same body offset. Every
  pair a datum's users produce must agree (owner-relative), or the datum is not
  placed. Literal-pool names derived from a game address (`$SG`, `$T`) and
  compiler-generated names never carry over.

The table names identities only; the image's source claims stay separate.
`units` in the report lists, per game unit, how many claimed bodies placed and
at which block delta - the evidence for which shared units an image links.
"""
from __future__ import annotations

import argparse
import bisect
import csv
import hashlib
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

from homm2.core.paths import BUILD, REPO, gen_dir, image_key, retail_dir, retail_exe
from homm2.core.pe import Pe

FILL = (0x90, 0xCC)
#: Compiler-private literal names derived from the game's own layout.
LAYOUT_NAMED = re.compile(r"^(?:\$SG|\$T|\?\?_C@|const_|string_|data_|bss_)")
#: A string literal named by the SHA-256 of its bytes (`homm2.retail_labels.name_strings`).
ANON_STR = re.compile(r"^\$anon_str_([0-9a-f]{64})_[0-9]+$")
#: The identification modules of a game claim (runtime members, import
#: thunks): not reconstruction targets in either program.
IDENTIFIED_MODULES = ("(libcmt)", "(imports)")


#: A function-local static: `?name@?<scope>??<function>`; VC6 numbers the
#: scope per compile (`?BP@`), the claims spell the source scope (`?1`).
LOCAL_STATIC = re.compile(r"^_?(\?[^@]+@\?)(?:[0-9]|[A-P]+@)(\?\?.*)$")


def _local_static_key(name: str) -> str:
    match = LOCAL_STATIC.match(name)
    return f"{match.group(1)}#@{match.group(2)}" if match else name


def _text(pe: Pe) -> tuple[bytes, int]:
    t = pe.section(".text")
    return pe.data[t["rptr"]:t["rptr"] + t["vsize"]], t["va"]


def _sites(manifest: Path) -> list[int]:
    return sorted(int(line.split("\t")[0], 16) for line in manifest.read_text().splitlines()
                  if line.startswith("0x"))


def _rel32_sites(body: bytes, rva: int) -> list[int]:
    """Body offsets of rel32 branch/call operands (E8/E9/0F 8x)."""
    from capstone import CS_ARCH_X86, CS_MODE_32, Cs
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    out = []
    for insn in md.disasm(body, 0x400000 + rva):
        raw = bytes(insn.bytes)
        if raw[0] in (0xE8, 0xE9) and len(raw) == 5:
            out.append(insn.address - 0x400000 - rva + 1)
        elif len(raw) == 6 and raw[0] == 0x0F and 0x80 <= raw[1] <= 0x8F:
            out.append(insn.address - 0x400000 - rva + 2)
    return out


def _pattern(body: bytes, masked: list[int]) -> re.Pattern:
    mask = bytearray(len(body))
    for site in masked:
        for i in range(max(site, 0), min(len(body), site + 4)):
            mask[i] = 1
    parts, i = [], 0
    while i < len(body):
        j = i
        while j < len(body) and mask[j] == mask[i]:
            j += 1
        parts.append(b".{%d}" % (j - i) if mask[i] else re.escape(body[i:j]))
        i = j
    return re.compile(b"".join(parts), re.S)


def _functions_csv(path: Path) -> dict[int, dict]:
    rows = (line for line in path.read_text().splitlines() if not line.startswith("#"))
    return {int(r["entry_rva"], 16): r for r in csv.DictReader(rows)}


class Placer:
    def __init__(self, image: str):
        self.image = image
        self.game = Pe(retail_exe("game"))
        self.pe = Pe(retail_exe(image))
        self.base = self.pe.image_base
        self.gtext, self.gva = _text(self.game)
        self.etext, self.eva = _text(self.pe)
        self.gsites = _sites(retail_dir("game") / "absolute_relocations.tsv")
        self.esites = set(_sites(retail_dir(image) / "absolute_relocations.tsv"))
        self.inventory = _functions_csv(retail_dir(image) / "functions.csv")
        self.starts = set(self.inventory)
        symbols = BUILD / "gen/symbol_names.csv"
        if not symbols.is_file():
            raise SystemExit(f"{symbols} is missing; run `homm2 labels` (or `homm2 delink`)")
        self.claims = [dict(r, rva=int(r["rva"], 16), size=int(r["size"], 16))
                       for r in csv.DictReader(open(symbols))]
        self.functions: dict[int, tuple[int, str]] = {}   # game rva -> (image rva, why)
        self.data: dict[int, tuple[int, str]] = {}        # game rva -> (image rva, why)
        self.problems: list[str] = []

    # -- functions ----------------------------------------------------------
    def body(self, rva: int, size: int) -> tuple[bytes, list[int]]:
        raw = self.gtext[rva - self.gva:rva - self.gva + size]
        end = len(raw)
        while end > 1 and raw[end - 1] in FILL:
            end -= 1
        raw = raw[:end]
        i = bisect.bisect_left(self.gsites, rva)
        j = bisect.bisect_left(self.gsites, rva + end)
        masked = [s - rva for s in self.gsites[i:j]] + _rel32_sites(raw, rva)
        return raw, sorted(set(masked))

    def place_functions(self) -> None:
        candidates: dict[int, list[int]] = {}
        meta = {}
        for c in self.claims:
            if c["kind"] != "func" or not (self.gva <= c["rva"] < self.gva + len(self.gtext)):
                continue
            if c["unit"] == "(funclets)":
                continue        # named by the game's addresses; the census names the image's
            raw, masked = self.body(c["rva"], c["size"])
            if not raw:
                continue
            hits = [self.eva + m.start() for m in _pattern(raw, masked).finditer(self.etext)]
            hits = [h for h in hits if h in self.starts]
            if hits:
                candidates[c["rva"]] = hits
                meta[c["rva"]] = (c, len(raw), len(masked))
        self.meta = meta
        for grva, hits in candidates.items():
            c, n, m = meta[grva]
            if len(hits) == 1 and n >= 16:
                self.functions[grva] = (hits[0], f"masked game body ({n} bytes, {m} fields "
                                                 "masked) unique at a census start")
        by_unit: dict[str, Counter] = defaultdict(Counter)
        for grva, hits in candidates.items():
            for h in set(hits):
                by_unit[meta[grva][0]["unit"]][h - grva] += 1
        for grva, hits in sorted(candidates.items()):
            if grva in self.functions:
                continue
            c, n, m = meta[grva]
            block = by_unit[c["unit"]]
            ranked = sorted(((block.get(h - grva, 0), h) for h in set(hits)), reverse=True)
            best = ranked[0]
            runner = ranked[1][0] if len(ranked) > 1 else 0
            if best[0] >= 2 and best[0] > runner:
                self.functions[grva] = (best[1], f"masked game body ({n} bytes) at its "
                                                 f"unit's block offset, shared by {best[0]} "
                                                 f"bodies ({len(hits)} occurrence(s))")
        # Two game bodies at one image address are byte-identical: unnamed.
        at = defaultdict(list)
        for grva, (erva, _why) in self.functions.items():
            at[erva].append(grva)
        from homm2.manifest import units as image_units
        linked = {u["unit"] for u in image_units(image=self.image)}
        for erva, grvas in at.items():
            if len(grvas) > 1:
                # Identical game bodies: the one whose unit this image links
                # names the address; otherwise the bytes cannot choose.
                own = [g for g in grvas if meta[g][0]["unit"] in linked]
                keep = own[0] if len(own) == 1 else None
                names = ", ".join(meta[g][0]["name"] for g in grvas)
                if keep is None:
                    self.problems.append(f"0x{erva:x}: identical game bodies {names}; unnamed")
                for g in grvas:
                    if g != keep:
                        del self.functions[g]
                if keep is not None:
                    erva_, why = self.functions[keep]
                    self.functions[keep] = (erva_, why + "; the only identical body "
                                            "whose unit this image links")

    # -- calls --------------------------------------------------------------
    def call_votes(self) -> dict[int, Counter]:
        """{game callee: Counter(image callee)} over the rel32 calls of every
        body placed by its own masked bytes (offsets then correspond)."""
        import struct
        from capstone import CS_ARCH_X86, CS_MODE_32, Cs
        md = Cs(CS_ARCH_X86, CS_MODE_32)
        votes: dict[int, Counter] = defaultdict(Counter)
        for grva, (erva, why) in sorted(self.functions.items()):
            if not why.startswith("masked game body"):
                continue
            raw, _masked = self.body(grva, self.meta[grva][0]["size"])
            for insn in md.disasm(raw, 0x400000 + grva):
                if insn.bytes[0] not in (0xE8, 0xE9) or insn.size != 5:
                    continue        # rel32 calls and tail jumps
                gt = insn.address + 5 + struct.unpack_from("<i", bytes(insn.bytes), 1)[0] - 0x400000
                esite = erva + insn.address - 0x400000 - grva + 1
                et = esite + 4 + struct.unpack_from("<i", self.etext, esite - self.eva)[0]
                votes[gt][et] += 1
            # Absolute code pointers (`push offset f`, function tables): the
            # game field and the image field at one body offset name one
            # function.
            i = bisect.bisect_left(self.gsites, grva)
            j = bisect.bisect_left(self.gsites, grva + len(raw))
            for site in self.gsites[i:j]:
                esite = erva + site - grva
                if esite not in self.esites:
                    continue
                gt = int.from_bytes(self.game.read(site, 4), "little") - self.game.image_base
                et = int.from_bytes(self.pe.read(esite, 4), "little") - self.base
                if self.gva <= gt < self.gva + len(self.gtext):
                    votes[gt][et] += 1
        return votes

    def resolve_calls(self) -> None:
        """One source call names one function. A callee placed only at its
        unit's block offset moves to the address every placed caller reaches;
        an unplaced game callee is named where the calls land."""
        votes = self.call_votes()
        claims = {c["rva"]: c for c in self.claims if c["kind"] == "func"}
        placed_at = {erva: grva for grva, (erva, _w) in self.functions.items()}
        self.callees: dict[int, tuple[str, str, str]] = {}
        for gt, counter in sorted(votes.items()):
            if gt not in claims:
                continue
            if len(counter) != 1:
                self.problems.append(f"callee {claims[gt]['name']} (0x{gt:x}): calls reach "
                                     + ", ".join(f"0x{e:x}" for e in counter))
                continue
            (et, n), = counter.items()
            if et not in self.starts:
                continue
            if gt in self.functions:
                erva, why = self.functions[gt]
                if erva != et and "block offset" in why and et not in placed_at:
                    self.functions[gt] = (et, f"{why}; moved to the address {n} placed "
                                              "call(s) reach")
                    placed_at.pop(erva, None)
                    placed_at[et] = gt
                elif erva != et:
                    self.problems.append(f"callee {claims[gt]['name']}: placed at 0x{erva:x}, "
                                         f"calls reach 0x{et:x}")
                continue
            if et in placed_at:
                continue
            self.callees[et] = (claims[gt]["name"], f"callee of {n} placed call(s); game "
                                                   f"callee 0x{gt:x} ({claims[gt]['unit']})",
                                claims[gt]["unit"])

    # -- data ---------------------------------------------------------------
    def place_data(self) -> None:
        """Pair each absolute field of a placed body with the image's field at
        the same body offset; a datum is placed when all its users agree."""
        data = sorted((c for c in self.claims if c["kind"] == "data"
                       and not LAYOUT_NAMED.match(c["name"])), key=lambda c: c["rva"])
        los = [c["rva"] for c in data]

        def owner(rva):
            i = bisect.bisect_right(los, rva) - 1
            if i >= 0 and los[i] <= rva < los[i] + max(data[i]["size"], 1):
                return data[i]
            return None

        votes: dict[int, Counter] = defaultdict(Counter)
        for grva, (erva, _why) in self.functions.items():
            c = self.meta[grva][0]
            i = bisect.bisect_left(self.gsites, grva)
            j = bisect.bisect_left(self.gsites, grva + c["size"])
            for site in self.gsites[i:j]:
                offset = site - grva
                esite = erva + offset
                if esite not in self.esites:
                    continue
                gvalue = self.game.read(site, 4)
                evalue = self.pe.read(esite, 4)
                if gvalue is None or evalue is None:
                    continue
                gtarget = int.from_bytes(gvalue, "little") - self.game.image_base
                etarget = int.from_bytes(evalue, "little") - self.base
                datum = owner(gtarget)
                if datum is None:
                    continue
                votes[datum["rva"]][etarget - (gtarget - datum["rva"])] += 1
        by_rva = {c["rva"]: c for c in data}
        for grva, counter in votes.items():
            if len(counter) != 1:
                self.problems.append(f"data {by_rva[grva]['name']} (0x{grva:x}): users disagree "
                                     f"({', '.join(f'0x{e:x}' for e in counter)})")
                continue
            (erva, n), = counter.items()
            if self.pe.read(erva, 1) is None:
                continue
            if not self._same_content_name(by_rva[grva]["name"], erva):
                continue
            self.data[grva] = (erva, f"{n} field(s) of placed bodies")
        self.place_variant_users(data)
        self.place_pointees(data, owner)
        self.place_bracketed(data)
        self.place_by_candidate_sections(data)

    def _same_content_name(self, name: str, erva: int) -> bool:
        """A content-named string (`$anon_str_<sha256 of its bytes>_<n>`) names
        the image's cell only when the cell holds those bytes: a variant
        literal (the editor's own assertion path) keeps its own content name."""
        match = ANON_STR.match(name)
        if match is None or self.eva <= erva < self.eva + len(self.etext):
            return True         # a code label the game's string pass misnamed
        raw = self.pe.read(erva, 1)
        text = b""
        while raw not in (None, b"\0"):
            text += raw
            raw = self.pe.read(erva + len(text), 1)
        return raw is not None and hashlib.sha256(text + b"\0").hexdigest() == match.group(1)

    def _same_datum(self, claim, erva: int) -> bool:
        """The datum's game bytes, pointer fields masked on both sides, equal
        the image's bytes at the candidate address."""
        size = claim["size"]
        gbytes, ebytes = self.game.read(claim["rva"], size), self.pe.read(erva, size)
        if not size or gbytes is None or ebytes is None:
            return False
        gmask, emask = bytearray(gbytes), bytearray(ebytes)
        i = bisect.bisect_left(self.gsites, claim["rva"] - 3)
        for site in self.gsites[i:]:
            if site >= claim["rva"] + size:
                break
            for k in range(max(0, site - claim["rva"]), min(size, site - claim["rva"] + 4)):
                gmask[k] = emask[k] = 0
        return gmask == emask

    def place_variant_users(self, data) -> None:
        """A shared unit's own body in this image (`VA_AT`: an editor variant
        such as kbwin's AppWndProc) is a code user as well, once its compiled
        object equals the image: every DIR32 field of the candidate object
        names its datum, and the image field at the same body offset gives the
        datum's address. All users must agree."""
        from homm2.core.coff import CoffObject
        from homm2.core.paths import gen_dir, image_build
        symbols = gen_dir(self.image) / "symbol_names.csv"
        if not symbols.is_file():
            return
        shared = {c["unit"] for c in self.claims if c["kind"] == "func"}
        variants = [r for r in csv.DictReader(open(symbols))
                    if r["kind"] == "func" and r["unit"] in shared
                    and r["provenance"] == f"source-annotation:{self.image}"]
        by_name = defaultdict(list)
        for c in data:
            by_name[_local_static_key(c["name"])].append(c)
        votes: dict[int, Counter] = defaultdict(Counter)
        for row in variants:
            path = image_build(self.image) / "objdiff/base" / f"{row['unit']}.obj"
            if not path.is_file():
                continue
            coff = CoffObject(path.read_bytes())
            symbol = next((sym for sym in coff.symbols.values()
                           if sym.name == row["name"] and sym.section > 0), None)
            if symbol is None:
                continue
            section = coff.sections[symbol.section - 1]
            ends = [sym.value for sym in coff.symbols.values()
                    if sym.section == symbol.section and sym.typ == 0x20
                    and sym.value > symbol.value]
            body = coff.section_bytes(section)[symbol.value:min(ends, default=section.raw_size)]
            rva = int(row["rva"], 16)
            relocs = [r for r in coff.relocations if r.section == symbol.section
                      and symbol.value <= r.site < symbol.value + len(body)]
            image = self.pe.read(rva, len(body))
            if image is None or len(body) != int(row["size"], 16):
                continue
            masked = bytearray(body), bytearray(image)
            for r in relocs:
                for k in range(r.site - symbol.value, r.site - symbol.value + 4):
                    masked[0][k] = masked[1][k] = 0
            if masked[0] != masked[1]:
                continue        # not yet exact: its fields prove nothing
            for r in relocs:
                if r.typ != 0x6:
                    continue
                target = _local_static_key(coff.symbols[r.symbol_index].name)
                claims = [c for c in by_name.get(target, ())
                          if c["unit"] == row["unit"]] or by_name.get(target, [])
                if len(claims) != 1:
                    continue
                offset = r.site - symbol.value
                addend = int.from_bytes(body[offset:offset + 4], "little")
                field = int.from_bytes(image[offset:offset + 4], "little") - self.base
                votes[claims[0]["rva"]][field - addend] += 1
        for grva, counter in votes.items():
            if grva in self.data:
                (erva, _n), = counter.most_common(1)
                if len(counter) != 1 or erva != self.data[grva][0]:
                    self.problems.append(f"data 0x{grva:x}: variant users disagree")
                continue
            if len(counter) != 1:
                self.problems.append(f"data 0x{grva:x}: variant users disagree")
                continue
            (erva, n), = counter.items()
            self.data[grva] = (erva, f"{n} field(s) of this image's exact variant bodies")

    def place_by_candidate_sections(self, data) -> None:
        """This image's own compile of a shared unit fixes each data section's
        layout (the editor's kbwin titles are longer than the game's, so the
        game's offsets do not carry over). A section whose placed members all
        give one base places its other members at their candidate offsets
        when their candidate bytes equal the image's, relocated fields masked."""
        from homm2.core.coff import CoffObject
        from homm2.core.paths import image_build
        shared = sorted({c["unit"] for c in self.claims if c["kind"] == "func"})
        by_unit_name = {(c["unit"], c["name"]): c for c in data}
        for unit in shared:
            path = image_build(self.image) / "objdiff/base" / f"{unit}.obj"
            if not path.is_file():
                continue
            coff = CoffObject(path.read_bytes())
            for section in coff.sections:
                if section.characteristics & 0x20000000 or section.name not in (
                        ".data", ".bss", ".rdata"):
                    continue
                members = [(sym, by_unit_name.get((unit, sym.name)))
                           for sym in coff.symbols.values()
                           if sym.section == section.index and sym.storage_class in (2, 3)
                           and not sym.name.startswith(".")]
                members = [(sym, claim) for sym, claim in members if claim is not None]
                bases = {self.data[claim["rva"]][0] - sym.value
                         for sym, claim in members if claim["rva"] in self.data}
                if len(bases) != 1:
                    continue
                (base,) = bases
                raw = coff.section_bytes(section)
                fields = {r.site for r in coff.relocations if r.section == section.index}
                for sym, claim in members:
                    if claim["rva"] in self.data or not claim["size"]:
                        continue
                    size = claim["size"]
                    candidate = bytearray(raw[sym.value:sym.value + size])
                    image = self.pe.read(base + sym.value, size)
                    if image is None or len(candidate) != size:
                        continue
                    image = bytearray(image)
                    for site in fields:
                        for k in range(site - sym.value, site - sym.value + 4):
                            if 0 <= k < size:
                                candidate[k] = image[k] = 0
                    if candidate != image:
                        continue
                    self.data[claim["rva"]] = (
                        base + sym.value, f"{unit} candidate section layout, bytes equal")

    def place_bracketed(self, data) -> None:
        """Data nothing addresses (`i32 iCurSwapPalette = 0;`) still sit in
        their object's section: a run of a unit's unplaced data is placed when
        the unit's data immediately before and after the run are placed with
        one delta and every datum's bytes equal the game's. An object's
        section moves as a block."""
        i = 0
        while i < len(data):
            if data[i]["rva"] in self.data:
                i += 1
                continue
            j = i
            while j < len(data) and data[j]["rva"] not in self.data:
                j += 1
            run = data[i:j]
            i = j
            if i - len(run) == 0 or j >= len(data):
                continue
            before, after = data[i - len(run) - 1], data[j]
            unit = before["unit"]
            if after["unit"] != unit or any(c["unit"] != unit for c in run):
                continue
            delta = self.data[before["rva"]][0] - before["rva"]
            if self.data[after["rva"]][0] - after["rva"] != delta:
                continue
            ends = [c["rva"] + c["size"] for c in (before, *run)]
            starts = [c["rva"] for c in (*run, after)]
            if any(end > start for end, start in zip(ends, starts)):
                continue
            if not all(c["size"] and self._same_datum(c, c["rva"] + delta) for c in run):
                continue
            for c in run:
                self.data[c["rva"]] = (c["rva"] + delta,
                                       "between its unit's placed neighbours, bytes equal")

    def _same_content_name(self, name: str, erva: int) -> bool:
        """A content-named string (`$anon_str_<sha256 of its bytes>_<n>`) names
        the image's cell only when the cell holds those bytes: a variant
        literal (the editor's own assertion path) keeps its own content name."""
        match = ANON_STR.match(name)
        if match is None or self.eva <= erva < self.eva + len(self.etext):
            return True         # a code label the game's string pass misnamed
        raw = self.pe.read(erva, 1)
        text = b""
        while raw not in (None, b"\0"):
            text += raw
            raw = self.pe.read(erva + len(text), 1)
        return raw is not None and hashlib.sha256(text + b"\0").hexdigest() == match.group(1)

    def _same_datum(self, claim, erva: int) -> bool:
        """The datum's game bytes, pointer fields masked on both sides, equal
        the image's bytes at the candidate address."""
        size = claim["size"]
        gbytes, ebytes = self.game.read(claim["rva"], size), self.pe.read(erva, size)
        if not size or gbytes is None or ebytes is None:
            return False
        gmask, emask = bytearray(gbytes), bytearray(ebytes)
        i = bisect.bisect_left(self.gsites, claim["rva"] - 3)
        for site in self.gsites[i:]:
            if site >= claim["rva"] + size:
                break
            for k in range(max(0, site - claim["rva"]), min(size, site - claim["rva"] + 4)):
                gmask[k] = emask[k] = 0
        return gmask == emask

    def place_bracketed(self, data) -> None:
        """A datum nothing addresses (`i32 iCurSwapPalette = 0;`) still sits in
        its object's section: it is placed when the unit's data immediately
        before and after it are placed with one delta and its bytes equal the
        game's. An object's section moves as a block."""
        changed = True
        while changed:
            changed = False
            for before, claim, after in zip(data, data[1:], data[2:]):
                if claim["rva"] in self.data or not claim["size"]:
                    continue
                if not (before["unit"] == claim["unit"] == after["unit"]
                        and before["rva"] in self.data and after["rva"] in self.data
                        and before["rva"] + before["size"] <= claim["rva"]
                        and claim["rva"] + claim["size"] <= after["rva"]):
                    continue
                delta = self.data[before["rva"]][0] - before["rva"]
                if self.data[after["rva"]][0] - after["rva"] != delta:
                    continue
                if not self._same_datum(claim, claim["rva"] + delta):
                    continue
                self.data[claim["rva"]] = (claim["rva"] + delta,
                                           "between its unit's placed neighbours, bytes equal")
                changed = True

    def place_pointees(self, data, owner) -> None:
        """A placed datum's pointer fields name their pointees the way a placed
        body's fields do (`char *gcCDTrackName = gcCDTrackNameText`). The image
        field must be one of its own absolute fields, every pointer to a
        pointee must agree, and the pointee's bytes must equal the game's."""
        by_rva = {c["rva"]: c for c in data}
        pending = dict(self.data)
        while pending:
            votes: dict[int, Counter] = defaultdict(Counter)
            for grva, (erva, _why) in pending.items():
                size = by_rva[grva]["size"]
                i = bisect.bisect_left(self.gsites, grva)
                j = bisect.bisect_left(self.gsites, grva + size)
                for site in self.gsites[i:j]:
                    esite = erva + site - grva
                    if esite not in self.esites:
                        continue
                    gtarget = int.from_bytes(self.game.read(site, 4), "little") \
                        - self.game.image_base
                    etarget = int.from_bytes(self.pe.read(esite, 4), "little") - self.base
                    datum = owner(gtarget)
                    if datum is None or datum["rva"] in self.data:
                        continue
                    votes[datum["rva"]][etarget - (gtarget - datum["rva"])] += 1
            pending = {}
            for grva, counter in votes.items():
                if len(counter) != 1:
                    self.problems.append(f"data {by_rva[grva]['name']} (0x{grva:x}): "
                                         "pointers disagree")
                    continue
                (erva, n), = counter.items()
                if not self._same_datum(by_rva[grva], erva):
                    continue
                self.data[grva] = pending[grva] = (
                    erva, f"{n} pointer field(s) of placed data, bytes equal")

    def run(self) -> None:
        self.place_functions()
        self.resolve_calls()
        self.place_data()

    def rows(self) -> list[dict]:
        claims = {c["rva"]: c for c in self.claims}
        out = []
        for grva, (erva, why) in self.functions.items():
            c = claims[grva]
            out.append({"rva": erva, "size": c["size"],
                        "kind": "func", "name": c["name"], "unit": c["unit"],
                        "game_rva": grva, "evidence": why})
        names = {r["name"] for r in out}
        for erva, (name, why, unit) in self.callees.items():
            if name in names:
                continue        # one name, one address
            # Named for its callers only: the body differs from the game's,
            # so it stays a residual of the image until its unit links here.
            # A runtime or import callee is the same library member or import
            # thunk in both programs and keeps its identification module.
            out.append({"rva": erva, "size": self._image_size(erva), "kind": "func",
                        "name": name,
                        "unit": unit if unit in IDENTIFIED_MODULES else "(unmatched)",
                        "game_rva": 0, "evidence": why})
        for grva, (erva, why) in self.data.items():
            c = claims[grva]
            out.append({"rva": erva, "size": c["size"], "kind": "data", "name": c["name"],
                        "unit": c["unit"], "game_rva": grva, "evidence": why})
        return sorted(out, key=lambda r: (r["rva"], r["kind"]))

    def _image_size(self, rva: int) -> int:
        row = self.inventory.get(rva)
        return int(row["byte_size"]) if row else 0

    def units(self) -> dict[str, dict]:
        claimed = Counter(c["unit"] for c in self.claims if c["kind"] == "func")
        placed = Counter(self.meta[g][0]["unit"] for g in self.functions)
        deltas: dict[str, Counter] = defaultdict(Counter)
        for g, (e, _w) in self.functions.items():
            deltas[self.meta[g][0]["unit"]][e - g] += 1
        return {unit: {"claimed": claimed[unit], "placed": placed[unit],
                       "delta": f"{deltas[unit].most_common(1)[0][0]:+#x}" if deltas[unit] else ""}
                for unit in sorted(claimed) if placed[unit]}


HEADER = ("# Generated by `homm2 --image {image} audit placements --write-config`.\n"
          "# Game identities placed in this image by retail byte correspondence;\n"
          "# game_rva/name/unit join the game claim the shared source spells.\n")
COLUMNS = ("rva", "size", "kind", "name", "unit", "game_rva", "evidence")


def render(placer: Placer) -> str:
    lines = [HEADER.format(image=placer.image).rstrip("\n"), "\t".join(COLUMNS)]
    for r in placer.rows():
        lines.append("\t".join((f"0x{r['rva']:08x}", f"0x{r['size']:x}", r["kind"], r["name"],
                                r["unit"], f"0x{r['game_rva']:08x}", r["evidence"])))
    return "\n".join(lines) + "\n"


from homm2.core.usage import logged


@logged
def main(argv=None) -> int:
    ap = argparse.ArgumentParser(prog="homm2 audit placements", description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--write-config", action="store_true",
                    help="write config/retail/<image>/placements.tsv")
    ap.add_argument("--check", action="store_true",
                    help="fail when the committed table differs from a fresh derivation")
    a = ap.parse_args(argv)
    image = image_key()
    if image == "game":
        ap.error("placements join the game's claims to another image; select --image")
    placer = Placer(image)
    placer.run()
    text = render(placer)
    units = placer.units()
    report = gen_dir() / "placements.json"
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps({"units": units, "problems": placer.problems}, indent=1) + "\n")
    functions = sum(1 for r in placer.rows() if r["kind"] == "func")
    print(f"[placements] {image}: {functions} functions and {len(placer.data)} data placed "
          f"from {len(units)} game units; {len(placer.problems)} problem(s); "
          f"report {report.relative_to(REPO)}")
    for unit, row in units.items():
        print(f"  {unit:28} {row['placed']:4}/{row['claimed']:<4} delta {row['delta']}")
    target = retail_dir(image) / "placements.tsv"
    if a.check:
        if not target.is_file() or target.read_text() != text:
            print(f"[placements] {target.relative_to(REPO)} differs from a fresh derivation")
            return 1
        return 0
    if a.write_config:
        target.write_text(text)
        print(f"[placements] wrote {target.relative_to(REPO)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
