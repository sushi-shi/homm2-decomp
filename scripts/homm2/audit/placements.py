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
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

from homm2.core.paths import BUILD, REPO, gen_dir, image_key, retail_dir, retail_exe
from homm2.core.pe import Pe

FILL = (0x90, 0xCC)
#: Compiler-private literal names derived from the game's own layout.
LAYOUT_NAMED = re.compile(r"^(?:\$SG|\$T|\?\?_C@|const_|string_|data_|bss_)")


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
        for erva, grvas in at.items():
            if len(grvas) > 1:
                names = ", ".join(meta[g][0]["name"] for g in grvas)
                self.problems.append(f"0x{erva:x}: identical game bodies {names}; unnamed")
                for g in grvas:
                    del self.functions[g]

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
                if insn.mnemonic != "call" or insn.bytes[0] != 0xE8:
                    continue
                gt = insn.address + 5 + struct.unpack_from("<i", bytes(insn.bytes), 1)[0] - 0x400000
                esite = erva + insn.address - 0x400000 - grva + 1
                et = esite + 4 + struct.unpack_from("<i", self.etext, esite - self.eva)[0]
                votes[gt][et] += 1
        return votes

    def resolve_calls(self) -> None:
        """One source call names one function. A callee placed only at its
        unit's block offset moves to the address every placed caller reaches;
        an unplaced game callee is named where the calls land."""
        votes = self.call_votes()
        claims = {c["rva"]: c for c in self.claims if c["kind"] == "func"}
        placed_at = {erva: grva for grva, (erva, _w) in self.functions.items()}
        self.callees: dict[int, tuple[str, str]] = {}
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
                                                   f"callee 0x{gt:x} ({claims[gt]['unit']})")

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
            self.data[grva] = (erva, f"{n} field(s) of placed bodies")

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
        for erva, (name, why) in self.callees.items():
            if name in names:
                continue        # one name, one address
            # Named for its callers only: the body differs from the game's,
            # so it stays a residual of the image until its unit links here.
            out.append({"rva": erva, "size": self._image_size(erva), "kind": "func",
                        "name": name, "unit": "(unmatched)", "game_rva": 0,
                        "evidence": why})
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
