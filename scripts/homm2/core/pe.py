"""homm2.core.pe - the retail image, parsed once.

The PE section table is the authority for every address-space edge; nothing
in the tree hardcodes an image constant. Parsed lazily and cached per
process (the image never changes).
"""

from __future__ import annotations

import struct
import csv
import hashlib
from functools import lru_cache
from pathlib import Path

from homm2.core.paths import retail_exe


class Pe:
    def __init__(self, path: Path | str | None = None):
        self.path = Path(path or retail_exe())
        self.data = d = self.path.read_bytes()
        pe = struct.unpack_from("<I", d, 0x3C)[0]
        nsec = struct.unpack_from("<H", d, pe + 6)[0]
        optsz = struct.unpack_from("<H", d, pe + 20)[0]
        magic = struct.unpack_from("<H", d, pe + 24)[0]
        if magic != 0x10B:
            raise ValueError(f"{self.path}: not a PE32 image (magic 0x{magic:x})")
        self.directories = [struct.unpack_from("<II", d, pe + 24 + 96 + i * 8)
                            for i in range(min(16, struct.unpack_from("<I", d, pe + 24 + 92)[0]))]
        self.image_base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
        self.sections: list[dict] = []
        for i in range(nsec):
            base = pe + 24 + optsz + i * 40
            name = d[base:base + 8].rstrip(b"\0").decode("latin-1")
            vsize, va, rsize, rptr = struct.unpack_from("<IIII", d, base + 8)
            self.sections.append({"name": name, "va": va, "vsize": vsize,
                                  "rsize": rsize, "rptr": rptr})

    def section(self, name: str) -> dict:
        s = next((s for s in self.sections if s["name"] == name), None)
        if s is None:
            raise KeyError(f"{self.path}: no section {name}")
        return s

    def text_span(self) -> tuple[int, int]:
        """[lo, hi) of .text's VIRTUAL extent (what function extents cap at)."""
        t = self.section(".text")
        return t["va"], t["va"] + t["vsize"]

    def data_regions(self) -> dict[str, tuple[int, int]]:
        """The four data regions: .rdata's raw bytes, .data's raw
        (initialized) bytes, .data's loader-zero virtual tail (this image has
        no separate .bss header), and .idata - whose virtual tail the retail
        linker reused for late zero-fill globals."""
        rd, da = self.section(".rdata"), self.section(".data")
        it = next((s for s in self.sections if s["name"] == ".idata"), None)
        return {"rdata": (rd["va"], rd["va"] + rd["rsize"]),
                "data": (da["va"], da["va"] + da["rsize"]),
                "bss": (da["va"] + da["rsize"], da["va"] + da["vsize"]),
                "idata": ((it["va"], it["va"] + max(it["vsize"], it["rsize"]))
                          if it else (0, 0))}

    def highlow_sites(self, manifest: Path | None = None) -> list[int]:
        """PE HIGHLOW fields, or hash-bound reviewed fields for a /FIXED image.

        Missing relocation records are not permission to guess address-sized
        integers. The explicit manifest is shared by sema and delink readers.
        """
        rva, size = self.directories[5] if len(self.directories) > 5 else (0, 0)
        if rva and size:
            blob = self.read(rva, size)
            if blob is None:
                raise ValueError("base relocation directory extends outside the image")
            sites, pos = [], 0
            while pos + 8 <= len(blob):
                page, block = struct.unpack_from("<II", blob, pos)
                if not block:
                    break
                if block < 8 or block % 2 or pos + block > len(blob):
                    raise ValueError("invalid base relocation block")
                for offset in range(pos + 8, pos + block, 2):
                    entry, = struct.unpack_from("<H", blob, offset)
                    if entry >> 12 == 3:
                        sites.append(page + (entry & 0xfff))
                pos += block
            return sorted(set(sites))
        if manifest is None:
            from homm2.core.paths import RETAIL
            manifest = RETAIL / "absolute_relocations.tsv"
        lines = manifest.read_text(encoding="utf-8").splitlines()
        expected = "# image-sha256: " + hashlib.sha256(self.data).hexdigest()
        if expected not in lines:
            raise ValueError(f"{manifest}: relocation manifest is not pinned to {self.path}")
        sites, seen = [], set()
        for row in csv.DictReader((line for line in lines if not line.startswith("#")), delimiter="\t"):
            site = int(row["site_rva"], 16)
            if row["kind"] != "dir32" or self.read(site, 4) is None:
                raise ValueError(f"{manifest}: invalid absolute relocation at {site:#x}")
            if site in seen:
                raise ValueError(f"{manifest}: duplicate relocation at {site:#x}")
            sites.append(site)
            seen.add(site)
        return sorted(sites)

    def read(self, rva: int, size: int) -> bytes | None:
        """Bytes at rva; loader zero-fill (past a section's raw size) reads as
        ZEROS, never as the next section's file bytes; short reads are None."""
        for s in self.sections:
            if s["va"] <= rva and rva + size <= s["va"] + max(s["vsize"], s["rsize"]):
                raw_end = s["va"] + s["rsize"]
                if rva >= raw_end:
                    return bytes(size)
                stored = min(size, raw_end - rva)
                off = s["rptr"] + rva - s["va"]
                chunk = self.data[off:off + stored]
                if len(chunk) != stored:
                    return None
                return chunk + bytes(size - stored)
        return None


@lru_cache(maxsize=1)
def image() -> Pe:
    return Pe()
