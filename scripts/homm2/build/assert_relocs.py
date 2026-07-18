#!/usr/bin/env python3
"""Relocation audits for objdiff's relocation-masked blind spots.

``homm2 relocs`` remains the broad, opt-in, order-independent target audit.  The
``--fields`` mode is a hard build gate for a narrower invariant: in live near-exact
functions, ordered DIR32 sites that resolve into the same recovered public DATA owner
must use the same owner-relative offset.  This catches a wrong field even when objdiff
masks all four relocation bytes.

For every
near-EXACT function, verify each relocation's TARGET address. objdiff MASKS all relocations when
scoring, so it never checks the target — a "100% match" can silently reference the WRONG
global/const/field/function, or even call a FABRICATED function that is declared, never defined, and
(because we compile-but-don't-link) never caught as an unresolved external. This closes that blind spot.

Order-INDEPENDENT: we resolve each side's relocs to a SET of final RVAs (symbol RVA + instruction
addend; REL32 -> the symbol's own RVA) and flag any address the BASE references that retail never
does. We also flag any base '?'-mangled symbol that resolves to NEITHER CodeView (symbol_names.csv)
NOR a DATA()-pinned definition — i.e. fabricated.

Data offsets come from symbol_names.csv (authoritative CodeView RVAs). Module-private synthetic
storage has no CodeView symbol, so its VA is read from the owning .cpp DATA(0x..) definition.

It is OPT-IN (not in `homm2 build`'s hard gates) because incomplete functions can still carry
legitimate relocation-shape differences. Canonical targets retain real folded-function identities;
synthetic relocation names are errors rather than normalized artifacts.

Review one function (order-independent; usable on <100% walls):
    homm2 relocs 0x<rva>
Review every resolvable same-site target in live exact functions:
    homm2 relocs --resolved
Review relocation-count deficits among incomplete functions:
    homm2 relocs --counts BASE
Compare raw relocation identities and encoded addends in every function:
    homm2 relocs --addends [SOURCE|BASE|EDITOR]
Run from repo root; exits 1 on any wrong/fabricated reloc target.
"""
import sys, os, re, csv, json, glob, struct, subprocess, hashlib
from collections import Counter
from pathlib import Path
from typing import NamedTuple

from homm2.build.reloc_owners import load_owner_ranges, owner_for_rva

IMAGE_BASE = 0x400000
_PARSE_CACHE = {}
# Bump only when parse_obj's serialized relocation representation changes.
_PARSER_FINGERPRINT = "coff-relocs-v2"
FIELD_AUDIT_THRESHOLD = 99.5


class OwnerOffsetMismatch(NamedTuple):
    site: int
    owner_symbol: str
    owner_name: str
    expected: int
    actual: int
    target_symbol: str
    base_symbol: str

    def diagnostic(self):
        return ("WRONG FIELD at +0x%x: %s expected +0x%x, actual +0x%x "
                "(retail %s, base %s)" %
                (self.site, self.owner_name, self.expected, self.actual,
                 self.target_symbol, self.base_symbol))


class OwnerOffsetMultisetMismatch(NamedTuple):
    owner_name: str
    expected: tuple
    actual: tuple

    def diagnostic(self):
        return ("WRONG OWNER OFFSETS: %s expected %s, actual %s" %
                (self.owner_name, self.expected, self.actual))


class RelocAddressMismatch(NamedTuple):
    site: int
    expected: int
    actual: int
    target_symbol: str
    base_symbol: str

    def diagnostic(self):
        return ("WRONG RELOC at +0x%x: expected RVA 0x%x, actual RVA 0x%x "
                "(retail %s, base %s)" %
                (self.site, self.expected, self.actual,
                 self.target_symbol, self.base_symbol))


class UnresolvedCandidateReloc(NamedTuple):
    site: int
    expected: int
    base_symbol: str

    def diagnostic(self):
        return ("UNRESOLVED DATA RELOC at +0x%x: retail RVA 0x%x, candidate %s "
                "has no public, DATA(), or reviewed compiler-local identity" %
                (self.site, self.expected, self.base_symbol))


class LinkedSectionRelocMismatch(NamedTuple):
    site: int
    expected_section: str
    expected_offset: int
    actual_section: str
    actual_offset: int
    target_symbol: str
    base_symbol: str

    def diagnostic(self):
        return ("WRONG LINKED DATA TARGET at +0x%x: retail %s+0x%x, candidate "
                "%s+0x%x (retail %s, base %s)" %
                (self.site, self.expected_section, self.expected_offset,
                 self.actual_section, self.actual_offset,
                 self.target_symbol, self.base_symbol))


def _norm(s):   # llvm-objdump renders each non-printable mangled byte as one '_'; symbol_names.csv
    s = s.replace('\xef\xbf\xbd', '_')          # stores it as the UTF-8 replacement char (3 bytes)
    return ''.join(c if 0x20 <= ord(c) < 0x7f else '_' for c in s)
def _key(s):    # encoding-stable key for ??_C@ strings: the ASCII "??_C@_<len>@<hash>@" prefix
    m = re.match(r'(\?\?_C@_\w+@\w+@)', s); return m.group(1) if m else _norm(s)

def load_symbols():
    sym, dups = {}, {}
    for r in csv.reader(open("build/gen/symbol_names.csv", encoding="latin-1")):
        if len(r) >= 2:
            try:
                v = int(r[0], 16)
            except ValueError:
                continue
            for k in (r[1], _norm(r[1]), _key(r[1])):
                # content-hashed string names (??_C@) can COLLIDE — two distinct addresses share one
                # name (e.g. two "!" literals). Record every address per name so an ambiguous target
                # reloc can match ANY of them (they're value-identical), not just the last one loaded.
                sym.setdefault(k, v); dups.setdefault(k, set()).add(v)
    data = {}                                    # DATA() lives on canonical .cpp definitions;
    for f in glob.glob("src/**/*.cpp", recursive=True) + glob.glob("include/**/*.h", recursive=True):
        for ln in open(f, encoding="latin-1"):   # headers are scanned to diagnose stale violations
            m = re.search(r'DATA\(0x([0-9a-fA-F]+)\).*?\b([A-Za-z_]\w*)\s*(?:\[|;|=)', ln)
            if m:
                data[m.group(2)] = int(m.group(1), 16) - IMAGE_BASE
    return sym, data, dups

def resolve(sym, data, typ, s, add):
    mc = re.match(r'const_([0-9a-fA-F]+)$', s)  # delinker names unlabeled data addrs const_<RVA>
    if mc:
        b = int(mc.group(1), 16)
    else:
        b = sym.get(s) or sym.get(_norm(s)) or sym.get(_key(s))
        if b is None:
            mm = re.match(r'\?(\w+)@@', s)      # ?<name>@@... -> a DATA()-pinned global
            if mm:
                b = data.get(mm.group(1))
    if b is None:
        return None
    return b if typ == 'REL32' else (b + add) & 0xffffffff

def is_fake(sym, data, s):
    if s.startswith('_') or s.startswith('??_C@') or s.startswith('$SG') or not s.startswith('?'):
        return False                             # CRT/import/string/local -> outside CodeView, fine
    return resolve(sym, data, 'DIR32', s, 0) is None

def _addend(insn):
    insn = insn.split('#')[0]                    # drop llvm-objdump's "# imm = 0xNN" annotation
    m = re.search(r'\[([^\]]+)\]', insn)         # Intel memory operand: use its displacement,
    if m:                                        # never a following store immediate
        disps = re.findall(r'(?<![\w\]])(-?0x[0-9a-f]+)', m.group(1))
        if disps:
            return int(disps[-1], 16) & 0xffffffff
        return 0
    # llvm-objdump -dr defaults to AT&T syntax. An indexed memory displacement precedes `(`;
    # it is the relocation addend even when the instruction also stores an immediate afterward.
    m = re.search(r'(?:(?<![\w$])(-?0x[0-9a-f]+))?\([^)]*\)', insn)
    if m:
        return int(m.group(1), 16) & 0xffffffff if m.group(1) else 0
    # Absolute AT&T memory operands have no parentheses and no `$`. Prefer one over an immediate.
    disps = re.findall(r'(?<![\w$])(-?0x[0-9a-f]+)', insn)
    if disps:
        return int(disps[-1], 16) & 0xffffffff
    imms = re.findall(r'\$(-?0x[0-9a-f]+)', insn)
    return int(imms[-1], 16) & 0xffffffff if imms else 0     # -0x4c disp -> 0xffffffb4 addend

def _text_sections(obj):
    """Read every COFF .text payload in section-table order.

    An x86 instruction may contain multiple relocation fields (for example a
    memory displacement and an immediate function pointer). Deriving both from
    the rendered instruction text assigns the first operand's displacement to
    every relocation. The four bytes at the relocation site are the actual COFF
    addend and are unambiguous. MSVC emits one .text COMDAT section per function,
    so retaining every section is essential: their llvm-objdump addresses each
    restart at zero.
    """
    with open(obj, "rb") as stream:
        blob = stream.read()
    if len(blob) < 20:
        return []
    section_count = struct.unpack_from("<H", blob, 2)[0]
    optional_size = struct.unpack_from("<H", blob, 16)[0]
    first = 20 + optional_size
    sections = []
    for index in range(section_count):
        off = first + index * 40
        if blob[off:off + 8].rstrip(b"\0") != b".text":
            continue
        size, raw = struct.unpack_from("<II", blob, off + 16)
        sections.append(blob[raw:raw + size])
    return sections

def parse_obj(obj, with_sites=False, include_imports=False):
    """llvm-objdump -dr -> ordered relocations per function.

    ``with_sites`` prefixes each relocation with its function-relative COFF offset.  Those offsets
    let the empty-COMDAT audit recover the original retail REL32 destination from the PE without
    weakening ordinary target checking. Imports remain excluded for legacy callers unless
    ``include_imports`` requests the complete raw identity set.
    """
    object_path = Path(obj).resolve()
    object_digest = hashlib.sha256(object_path.read_bytes()).hexdigest()
    cache_key = "%s\0%s\0%s\0%s\0%s" % (
        object_path, int(with_sites), int(include_imports), object_digest,
        _PARSER_FINGERPRINT)
    if cache_key in _PARSE_CACHE:
        return _PARSE_CACHE[cache_key]
    cache_name = hashlib.sha256(cache_key.encode("utf-8")).hexdigest() + ".json"
    cache_path = Path("build/cache/relocs") / cache_name
    try:
        cached = json.loads(cache_path.read_text(encoding="utf-8"))
        funcs = {name: [tuple(reloc) for reloc in relocs]
                 for name, relocs in cached.items()}
        _PARSE_CACHE[cache_key] = funcs
        return funcs
    except (OSError, ValueError, TypeError):
        pass

    run = subprocess.run(["llvm-objdump", "-dr", obj], capture_output=True, text=True)
    if run.returncode != 0:
        return {}
    out = run.stdout
    text_sections = _text_sections(obj)
    text_index = -1
    text_bytes = b""
    funcs, cur, cur_start, prev = {}, None, 0, ""
    for ln in out.splitlines():
        if ln == "Disassembly of section .text:":
            text_index += 1
            text_bytes = (text_sections[text_index]
                          if text_index < len(text_sections) else b"")
            cur = None
            prev = ""
            continue
        m = re.match(r'^([0-9a-f]+) <(.+?)>:', ln)
        if m:
            name = m.group(2)
            if name.startswith('$') and cur is not None:
                # MSVC emits block labels as COFF symbols inside a function COMDAT. llvm-objdump
                # prints each as a new title, but its following relocations still belong to the
                # containing function; do not truncate the function at the first local label.
                prev = ""
                continue
            cur = name
            cur_start = int(m.group(1), 16)
            funcs.setdefault(cur, [])
            prev = ""
            continue
        if cur is None:
            continue
        mr = re.search(r'IMAGE_REL_I386_(\w+)\s+(\S+)', ln)
        if mr:
            s = mr.group(2)
            if include_imports or not s.startswith('__imp__'):
                mo = re.match(r'^\s*([0-9a-f]+):', ln)
                site = int(mo.group(1), 16) if mo else -1
                if 0 <= site <= len(text_bytes) - 4:
                    addend = struct.unpack_from("<I", text_bytes, site)[0]
                else:
                    addend = _addend(prev)
                reloc = (mr.group(1), s, addend)
                if with_sites:
                    funcs[cur].append((site - cur_start,) + reloc)
                else:
                    funcs[cur].append(reloc)
            continue
        mi = re.match(r'^\s*[0-9a-f]+:\s+(?:[0-9a-f]{2}\s+)+(.+)$', ln)
        if mi:
            prev = mi.group(1).replace('\t', ' ').strip()
    cache_path.parent.mkdir(parents=True, exist_ok=True)
    temporary = cache_path.with_suffix(".tmp.%d" % os.getpid())
    temporary.write_text(json.dumps(funcs, separators=(",", ":")), encoding="utf-8")
    os.replace(str(temporary), str(cache_path))
    _PARSE_CACHE[cache_key] = funcs
    return funcs


def relocation_addend_map(relocations):
    """Return ``relocation symbol -> sorted encoded COFF addends``.

    Sites and instruction order are deliberately absent. This representation is
    stable for structurally incomplete functions while still proving that both
    objects encode the same displacement set for each relocation identity.
    """
    grouped = {}
    for _typ, symbol, addend in relocations:
        grouped.setdefault(symbol, []).append(addend)
    return {symbol: tuple(sorted(addends))
            for symbol, addends in grouped.items()}


def compare_function_reloc_addends(base_relocs, target_relocs,
                                    function_symbol=None,
                                    canonical_data_names=()):
    """Return raw-symbol addend differences for one function."""
    base = relocation_addend_map(base_relocs)
    target = relocation_addend_map(target_relocs)
    differences = []
    for symbol in sorted(set(base) | set(target)):
        actual = base.get(symbol, ())
        expected = target.get(symbol, ())
        if actual == expected:
            continue
        missing = tuple(sorted((Counter(expected) - Counter(actual)).elements()))
        excess = tuple(sorted((Counter(actual) - Counter(expected)).elements()))
        if symbol == function_symbol:
            classification = "code-local"
        elif not expected or not actual:
            classification = "one-sided"
        elif set(expected) != set(actual):
            classification = "value-set"
        else:
            classification = "count-only"
        differences.append({
            "symbol": symbol,
            "target": expected,
            "base": actual,
            "missing": missing,
            "excess": excess,
            "classification": classification,
            "canonical_data": symbol in canonical_data_names,
        })
    return differences


def load_canonical_data_names(
        symbols_path="build/gen/symbol_names.csv",
        manifest_path="build/gen/delink_data_manifest.tsv"):
    names = set()
    if os.path.isfile(symbols_path):
        with open(symbols_path, encoding="latin-1", newline="") as stream:
            names.update(row["name"] for row in csv.DictReader(stream)
                         if row.get("kind") == "data")
    if os.path.isfile(manifest_path):
        with open(manifest_path, encoding="utf-8", newline="") as stream:
            names.update(
                row["name"] for row in csv.DictReader(
                    (line for line in stream
                     if not line.lstrip().startswith("#")), delimiter="\t"))
    return names


def load_candidate_local_rvas(
        manifests=("config/delink_data_supplemental.tsv",
                   "build/gen/delink_data_from_source.tsv"),
        base_root="build/objdiff/base"):
    """Map current candidate local spellings to canonical retail RVAs.

    Reviewed compiler counters are deliberately stable in the supplemental file,
    while an MSVC rebuild may rename them.  The topology audit proves such a rename
    by exact COFF section ordinal/offset plus canonical payload and relocation
    identity.  Recreate that association here from the current candidate symbol
    table; a discovery-only review-queue payload match is never treated as proof.
    """
    coordinates = {}
    for path in manifests:
        if not os.path.exists(path):
            continue
        with open(path, encoding="utf-8", newline="") as stream:
            rows = csv.DictReader(
                (line for line in stream if not line.lstrip().startswith("#")),
                delimiter="\t")
            for row in rows:
                unit = row["object"].replace("\\", "/")
                if unit.endswith(".c"):
                    unit = unit[:-2]
                try:
                    key = (int(row["section_ordinal"], 0),
                           int(row["section_offset"], 0))
                    rva = int(row["rva"], 0)
                except (KeyError, ValueError):
                    continue
                coordinates.setdefault(unit, {}).setdefault(key, set()).add(rva)

    result = {}
    symbol_pattern = re.compile(
        r"\]\(sec\s+(-?\d+)\).*?\s0x([0-9a-fA-F]+)\s+(\S+)\s*$")
    for unit, unit_coordinates in coordinates.items():
        obj = os.path.join(base_root, unit + ".obj")
        if not os.path.exists(obj):
            continue
        table = subprocess.run(
            ["llvm-objdump", "-t", obj], check=True,
            stdout=subprocess.PIPE, text=True).stdout
        for line in table.splitlines():
            match = symbol_pattern.search(line)
            if match is None:
                continue
            coordinate = (int(match.group(1)), int(match.group(2), 16))
            rvas = unit_coordinates.get(coordinate)
            if rvas:
                result.setdefault((unit, match.group(3)), set()).update(rvas)
    return result


def candidate_reloc_rvas(sym, data, dups, local_rvas, unit, reloc):
    """Resolve a candidate relocation, including reviewed TU-local storage."""
    typ, symbol, addend = reloc
    candidates = _resolved_reloc_candidates(sym, data, dups, reloc)
    if typ == "DIR32":
        candidates.update(
            (rva + addend) & 0xFFFFFFFF
            for rva in local_rvas.get((unit, symbol), ()))
    return candidates


_PE_SECTIONS = None


def _pe_sections():
    """Return the authoritative retail image and its RVA-to-file mappings."""
    global _PE_SECTIONS
    if _PE_SECTIONS is not None:
        return _PE_SECTIONS

    data = open("build/orig/HEROES2W.EXE", "rb").read()
    pe = struct.unpack_from("<L", data, 0x3c)[0]
    section_count = struct.unpack_from("<H", data, pe + 6)[0]
    optional_header_size = struct.unpack_from("<H", data, pe + 20)[0]
    section_table = pe + 24 + optional_header_size
    sections = []
    for index in range(section_count):
        section_header = section_table + index * 40
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
            "<LLLL", data, section_header + 8
        )
        sections.append((rva, max(virtual_size, raw_size), raw_offset))

    _PE_SECTIONS = data, sections
    return _PE_SECTIONS


def _pe_read(rva, size):
    data, sections = _pe_sections()
    for start, span, raw_offset in sections:
        if start <= rva and rva + size <= start + span:
            offset = raw_offset + rva - start
            return data[offset:offset + size]
    return None


def _pe_named_section_ranges(names=(".rdata", ".data")):
    """Return authoritative retail RVA ranges for selected PE sections."""
    data, _sections = _pe_sections()
    pe = struct.unpack_from("<L", data, 0x3c)[0]
    section_count = struct.unpack_from("<H", data, pe + 6)[0]
    optional_header_size = struct.unpack_from("<H", data, pe + 20)[0]
    section_table = pe + 24 + optional_header_size
    ranges = []
    for index in range(section_count):
        header = section_table + index * 40
        name = data[header:header + 8].rstrip(b"\0").decode("latin-1")
        if name not in names:
            continue
        virtual_size, rva, raw_size = struct.unpack_from("<LLL", data, header + 8)
        ranges.append((name, rva, rva + max(virtual_size, raw_size)))
    return ranges


def _load_pe_image(path):
    payload = Path(path).read_bytes()
    pe = struct.unpack_from("<L", payload, 0x3c)[0]
    section_count = struct.unpack_from("<H", payload, pe + 6)[0]
    optional_size = struct.unpack_from("<H", payload, pe + 20)[0]
    optional = pe + 24
    image_base = struct.unpack_from("<I", payload, optional + 28)[0]
    section_table = optional + optional_size
    sections = []
    for index in range(section_count):
        header = section_table + index * 40
        name = payload[header:header + 8].rstrip(b"\0").decode("latin-1")
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
            "<LLLL", payload, header + 8)
        sections.append((name, rva, rva + max(virtual_size, raw_size),
                         raw_offset, raw_size))
    return payload, image_base, sections


def _image_read_rva(image, rva, size):
    payload, _image_base, sections = image
    for _name, start, end, raw_offset, raw_size in sections:
        if start <= rva and rva + size <= min(end, start + raw_size):
            offset = raw_offset + rva - start
            return payload[offset:offset + size]
    return None


def _image_section_offset(image, rva):
    for name, start, end, _raw_offset, _raw_size in image[2]:
        if start <= rva < end:
            return name, rva - start
    return None


def _matching_linked_site_context(base_sites, target_sites, site,
                                  retail_function_rva,
                                  candidate_function_rva,
                                  retail_image, candidate_image,
                                  radius=8):
    """Require the relocation to belong to the same linked instruction context.

    Equal function-relative offsets are insufficient when an earlier code-shape
    difference shifts instructions: an unrelated relocation can then occupy the
    same numeric site. Compare a small raw-code window and mask only relocation
    fields which exist with the same type at the same site on both sides.
    """
    start = max(0, site - radius)
    end = site + 4 + radius
    retail = _image_read_rva(
        retail_image, retail_function_rva + start, end - start)
    candidate = _image_read_rva(
        candidate_image, candidate_function_rva + start, end - start)
    if retail is None or candidate is None:
        return False
    retail = bytearray(retail)
    candidate = bytearray(candidate)
    base_types = {reloc[0]: reloc[1] for reloc in base_sites}
    target_types = {reloc[0]: reloc[1] for reloc in target_sites}
    for reloc_site, reloc_type in base_types.items():
        if target_types.get(reloc_site) != reloc_type:
            continue
        overlap_start = max(reloc_site, start)
        overlap_end = min(reloc_site + 4, end)
        for function_offset in range(overlap_start, overlap_end):
            retail[function_offset - start] = 0
            candidate[function_offset - start] = 0
    return retail == candidate


def check_linked_pe_data_targets(base_sites, target_sites, retail_function_rva,
                                 candidate_function_rva, retail_image,
                                 candidate_image, names=(".rdata", ".data"),
                                 stats=None):
    """Compare final linked destinations normalized by each PE section base."""
    problems = []
    target_by_site = {reloc[0]: reloc[1:] for reloc in target_sites}
    for site, base_type, base_symbol, _base_addend in base_sites:
        target = target_by_site.get(site)
        if target is None or base_type != "DIR32" or target[0] != "DIR32":
            continue
        retail_raw = _image_read_rva(retail_image, retail_function_rva + site, 4)
        candidate_raw = _image_read_rva(
            candidate_image, candidate_function_rva + site, 4)
        if retail_raw is None or candidate_raw is None:
            continue
        retail_target = (struct.unpack("<I", retail_raw)[0] -
                         retail_image[1]) & 0xFFFFFFFF
        candidate_target = (struct.unpack("<I", candidate_raw)[0] -
                            candidate_image[1]) & 0xFFFFFFFF
        expected = _image_section_offset(retail_image, retail_target)
        if expected is None or expected[0] not in names:
            continue
        if not _matching_linked_site_context(
                base_sites, target_sites, site, retail_function_rva,
                candidate_function_rva, retail_image, candidate_image):
            if stats is not None:
                stats["context_mismatch"] += 1
            continue
        if stats is not None:
            stats["compared_section_sites"] += 1
            stats["compared_%s_sites" %
                  ("rdata" if expected[0] == ".rdata" else "writable_data")] += 1
        actual = _image_section_offset(candidate_image, candidate_target)
        if actual == expected:
            continue
        actual = actual or ("outside-image", candidate_target)
        problems.append(LinkedSectionRelocMismatch(
            site, expected[0], expected[1], actual[0], actual[1],
            target[1], base_symbol))
    return problems


def check_pe_data_targets(sym, data, dups, local_rvas, unit, function_rva,
                          base_sites, target_sites, section_ranges, pe_read=_pe_read):
    """Compare aligned candidate DIR32 destinations with raw retail PE operands.

    Delinked target symbols are useful topology, but the linked retail operand is
    the authority.  Reading it directly also avoids treating a payload-equivalent
    compiler local as proof that the function references that allocation.
    """
    problems = []
    target_by_site = {reloc[0]: reloc[1:] for reloc in target_sites}
    for site, base_type, base_symbol, base_addend in base_sites:
        target = target_by_site.get(site)
        if target is None or base_type != "DIR32" or target[0] != "DIR32":
            continue
        raw = pe_read(function_rva + site, 4)
        if raw is None or len(raw) != 4:
            continue
        absolute = struct.unpack("<I", raw)[0]
        expected = (absolute - IMAGE_BASE) & 0xFFFFFFFF
        if not any(start <= expected < end
                   for _name, start, end in section_ranges):
            continue
        actual = candidate_reloc_rvas(
            sym, data, dups, local_rvas, unit,
            (base_type, base_symbol, base_addend))
        if not actual:
            problems.append(UnresolvedCandidateReloc(
                site, expected, base_symbol))
        elif expected not in actual:
            problems.append(RelocAddressMismatch(
                site, expected, min(actual), target[1], base_symbol))
    return problems


def _direct_image_data_sites(image, function_rva, relocations,
                             names=(".rdata", ".data")):
    """Read every DIR32 destination in selected sections from a linked image.

    The relocation sites come from the corresponding COFF function, but the
    destination and section identity come only from the linked PE operand.  This
    keeps the retail side independent of delinker symbol names and lets callers
    compare relocation multisets even when code-shape differences move sites.
    """
    result = []
    for site, relocation_type, symbol, addend in relocations:
        if relocation_type != "DIR32":
            continue
        raw = _image_read_rva(image, function_rva + site, 4)
        if raw is None or len(raw) != 4:
            continue
        rva = (struct.unpack("<I", raw)[0] - image[1]) & 0xFFFFFFFF
        section = _image_section_offset(image, rva)
        if section is None or section[0] not in names:
            continue
        result.append({
            "site": site,
            "symbol": symbol,
            "addend": addend,
            "rva": rva,
            "section": section[0],
            "section_offset": section[1],
        })
    return result


def _maximum_data_identity_matching(expected_sites, candidate_sites):
    """Maximum-match candidate identity sets against retail RVA occurrences.

    A content-hashed literal name can legitimately denote several identical
    retail allocations, so one candidate site may have more than one reviewed
    identity.  Treat retail occurrences as distinct right-hand nodes and use an
    augmenting-path matching instead of a greedy choice.  The returned indexes
    make every unmatched retail and candidate occurrence explicit.
    """
    matched_expected = {}

    def augment(candidate_index, seen):
        identities = candidate_sites[candidate_index].get("identities", ())
        for expected_index, expected in enumerate(expected_sites):
            if (expected_index in seen or
                    expected["rva"] not in identities):
                continue
            seen.add(expected_index)
            previous = matched_expected.get(expected_index)
            if previous is None or augment(previous, seen):
                matched_expected[expected_index] = candidate_index
                return True
        return False

    order = sorted(
        range(len(candidate_sites)),
        key=lambda index: (len(candidate_sites[index].get("identities", ())),
                           candidate_sites[index].get("site", -1)))
    matched_candidates = set()
    for candidate_index in order:
        if augment(candidate_index, set()):
            matched_candidates.add(candidate_index)
    return (
        sorted(set(range(len(expected_sites))) - set(matched_expected)),
        sorted(set(range(len(candidate_sites))) - matched_candidates),
    )


def _classify_candidate_excess(expected_sites, excess_sites):
    """Mark excess candidate occurrences whose identity is novel to the function.

    Unequal relocation counts prove a code-shape residual, but they must not hide
    a candidate edge to storage which retail never references in that function.
    An over-publication of an otherwise retail-used identity remains visible as a
    multiplicity residual; a disjoint identity is the stronger review class.
    """
    retail_rvas = {record["rva"] for record in expected_sites}
    annotated = []
    novel = []
    for record in excess_sites:
        record = dict(record)
        overlap = sorted(retail_rvas.intersection(record.get("identities", ())))
        record["retail_function_identity_overlap"] = overlap
        annotated.append(record)
        if not overlap:
            novel.append(record)
    return annotated, novel


def _unique_report_functions(functions):
    """Select one objdiff record per decorated COFF function name.

    Objdiff can expose the same COMDAT twice when an alias/null comparison record
    accompanies the live comparison.  The COFF parser and linked MAP still have
    one function, so auditing both report rows would count every relocation twice.
    Prefer a scored record, then the highest score, while retaining the duplicate
    count for the generated audit report.
    """
    selected = {}
    record_counts = Counter()
    for function in functions:
        name = function["name"]
        record_counts[name] += 1
        current = selected.get(name)
        score = function.get("fuzzy_match_percent")
        rank = (score is not None, score if score is not None else -1.0)
        if current is None:
            selected[name] = function
            continue
        current_score = current.get("fuzzy_match_percent")
        current_rank = (
            current_score is not None,
            current_score if current_score is not None else -1.0,
        )
        if rank > current_rank:
            selected[name] = function
    duplicates = {
        name: count for name, count in record_counts.items() if count > 1
    }
    return [selected[name] for name in sorted(selected)], duplicates


def check_pe_data_target_multiset(sym, data, dups, local_rvas, unit,
                                  retail_function_rva,
                                  candidate_function_rva,
                                  base_sites, target_sites,
                                  retail_image, candidate_image,
                                  retail_section_ranges):
    """Compare every linked data-target identity without assuming site alignment.

    This is the exhaustive complement to the ordered context pass.  Retail
    identities are read directly from the shipping PE.  Candidate identities
    are resolved from public/DATA symbols or reviewed compiler-local COFF
    coordinates, while the candidate PE independently proves that the operand
    actually targets `.rdata`/`.data` in the current link.
    """
    expected = _direct_image_data_sites(
        retail_image, retail_function_rva, target_sites)
    candidate = _direct_image_data_sites(
        candidate_image, candidate_function_rva, base_sites)
    base_by_site = {relocation[0]: relocation for relocation in base_sites}
    for record in candidate:
        relocation = base_by_site[record["site"]]
        identities = candidate_reloc_rvas(
            sym, data, dups, local_rvas, unit, relocation[1:])
        record["identities"] = sorted(
            rva for rva in identities
            if any(start <= rva < end
                   for _name, start, end in retail_section_ranges))

    missing_indexes, excess_indexes = _maximum_data_identity_matching(
        expected, candidate)
    missing = [expected[index] for index in missing_indexes]
    excess, novel_excess = _classify_candidate_excess(
        expected, [candidate[index] for index in excess_indexes])
    unresolved = [record for record in candidate if not record["identities"]]
    missing_index_set = set(missing_indexes)
    matched_expected = [
        record for index, record in enumerate(expected)
        if index not in missing_index_set
    ]
    return {
        "expected_count": len(expected),
        "candidate_count": len(candidate),
        "matched_count": len(expected) - len(missing),
        "expected_section_counts": dict(Counter(
            record["section"] for record in expected)),
        "candidate_section_counts": dict(Counter(
            record["section"] for record in candidate)),
        "matched_section_counts": dict(Counter(
            record["section"] for record in matched_expected)),
        "missing_retail_targets": missing,
        "excess_candidate_targets": excess,
        "novel_candidate_targets": novel_excess,
        "unresolved_candidate_targets": unresolved,
    }


# Pre-existing reloc-target discrepancies surfaced when this gate was introduced (objdiff masked
# them, so they scored ~100%). Two kinds: (A) SUSPECTED wrong VA — a base symbol that disagrees with
# where retail reads (likely a wrong synthetic-global DATA(0x..) guess, or a wrong array base — the
# gDwelling*/gHierarchyMask/gMonsterInfo cases are the same shape: base is 0x4c off retail). (B)
# factoring/codegen differences the multiset can't distinguish (base uses a static field+offset or a
# direct call where retail computes the offset at runtime / dispatches virtually — same effect).
# TODO: confirm each and fix (correct the VA / restructure the source), then delete the entry.
# Keyed (unit, function, base-symbol).
ALLOWLIST = set()

def _cands(dups, s, add):
    """Every address a target reloc name could denote, +addend. Content-hashed string names (??_C@)
    COLLIDE — one name, several value-identical addresses; the reloc may mean ANY, so accept all."""
    c = dups.get(s) or dups.get(_norm(s)) or dups.get(_key(s)) or set()
    return {(a + add) & 0xffffffff for a in c}

def _tvas(sym, data, dups, tgt_relocs):
    """Multiset of every RVA retail references. Order-independent (robust to near-100%-wall reorder).
    An ambiguous (dup) name contributes ALL its candidate addrs per occurrence, so a base ref to any
    one of them clears; a unique name contributes its single resolved addr."""
    tvas = Counter()
    for r in tgt_relocs:
        v = resolve(sym, data, *r)
        if v is None:
            continue
        cs = _cands(dups, r[1], r[2]) if r[0] != 'REL32' else set()
        if len(cs) > 1:
            for c in cs:
                tvas[c] += 1
        else:
            tvas[v] += 1
    return tvas

def check_fn(sym, data, dups, unit, name, base_relocs, tgt_relocs):
    """Order-INDEPENDENT MULTISET reloc-target check. We flag `only_base`: an address the BASE
    references MORE times than retail does — a wrong global/const/field/fn (multiset, not set, so an
    injected wrong ref is caught even when that address is ALSO referenced elsewhere in the same fn).
    Plus a FAKE check for unresolvable base '?'-symbols. Factoring/near-wall reorder no longer false-
    positive here: they're fixed at the resolve layer (signed addend, const_<rva>, dup names)."""
    probs = []
    for r in base_relocs:                        # (1) fabricated function/data symbol
        if is_fake(sym, data, r[1]) and (unit, name, r[1]) not in ALLOWLIST:
            probs.append("FAKE call/ref '%s' — exists in neither CodeView nor a DATA() global" % r[1])
    tvas = _tvas(sym, data, dups, tgt_relocs)
    bvas, va_sym = Counter(), {}
    for r in base_relocs:
        v = resolve(sym, data, *r)
        if v is not None:
            bvas[v] += 1; va_sym.setdefault(v, r[1])
    for v, n in (bvas - tvas).items():            # (2) base references an addr retail never does (or fewer)
        bs = va_sym[v]
        if '_00A@' in bs or (unit, name, bs) in ALLOWLIST:
            continue
        probs.append("WRONG: base references 0x%x (%s) x%d — retail never references it (or fewer)" % (v, bs, n))
    return probs


def check_ordered_owner_offsets(sym, data, owners, base_sites, target_sites):
    """Compare aligned absolute relocations as ``(public owner, field offset)``.

    This deliberately ignores unaligned sites, relative calls, unknown owners, and
    different owners.  Those cases need the broader opt-in audit.  For a function
    whose ordinary bytes are already exact, equal function-relative sites identify
    the same instruction operand without relying on relocation symbol names.
    """
    problems = []
    target_by_site = {reloc[0]: reloc[1:] for reloc in target_sites}
    for base_reloc in base_sites:
        site, base_type, base_symbol, base_addend = base_reloc
        target = target_by_site.get(site)
        if target is None:
            continue
        target_type, target_symbol, target_addend = target
        if base_type != "DIR32" or target_type != "DIR32":
            continue
        base_rva = resolve(sym, data, base_type, base_symbol, base_addend)
        target_rva = resolve(sym, data, target_type, target_symbol, target_addend)
        if base_rva is None or target_rva is None:
            continue
        base_owner = owner_for_rva(owners, base_rva)
        target_owner = owner_for_rva(owners, target_rva)
        if (base_owner is None or target_owner is None or
                base_owner.rva != target_owner.rva or
                base_owner.symbol != target_owner.symbol):
            continue
        # A provisional public-data extent can overlap an unrelated string or
        # local constant.  Normalize only explicit references to the owner or a
        # delinker const_<RVA> alias, never an arbitrary symbol that merely falls
        # numerically inside that extent.
        if not ((base_symbol == base_owner.symbol or base_symbol.startswith("const_")) and
                (target_symbol == target_owner.symbol or target_symbol.startswith("const_"))):
            continue
        base_offset = base_rva - base_owner.rva
        target_offset = target_rva - target_owner.rva
        if base_offset == target_offset:
            continue
        problems.append(OwnerOffsetMismatch(
            site, base_owner.symbol, base_owner.source_name,
            target_offset, base_offset, target_symbol, base_symbol))
    return problems


def _owner_offset_counters(sym, data, owners, sites):
    """Group explicit owner/const DIR32 references by recovered DATA owner."""
    grouped = {}
    for _site, typ, symbol, addend in sites:
        if typ != "DIR32":
            continue
        rva = resolve(sym, data, typ, symbol, addend)
        if rva is None:
            continue
        owner = owner_for_rva(owners, rva)
        if owner is None:
            continue
        if symbol != owner.symbol and not symbol.startswith("const_"):
            continue
        grouped.setdefault(owner.symbol, (owner, Counter()))[1][rva - owner.rva] += 1
    return grouped


def check_owner_offset_multisets(sym, data, owners, base_sites, target_sites):
    """Compare owner-relative fields without depending on instruction order.

    This is valid at any fuzzy percentage when both sides contain the same number
    of references to an owner. Differing counts are left to structural matching;
    equal counts with different offsets prove at least one wrong field access.
    """
    base = _owner_offset_counters(sym, data, owners, base_sites)
    target = _owner_offset_counters(sym, data, owners, target_sites)
    problems = []
    for symbol in sorted(set(base) & set(target)):
        owner, base_offsets = base[symbol]
        _target_owner, target_offsets = target[symbol]
        if (sum(base_offsets.values()) != sum(target_offsets.values()) or
                base_offsets == target_offsets):
            continue
        problems.append(OwnerOffsetMultisetMismatch(
            owner.source_name,
            tuple(sorted(target_offsets.items())),
            tuple(sorted(base_offsets.items()))))
    return problems


def _resolved_reloc_candidates(sym, data, dups, reloc):
    """Return every final RVA a relocation can denote.

    Duplicate content-hashed string symbols are value-equivalent and may legitimately
    resolve to more than one retail address. Other resolvable relocations have one final
    address, regardless of whether their COFF spelling is ``owner+addend`` or
    ``const_<RVA>+0``.
    """
    typ, symbol, addend = reloc
    resolved = resolve(sym, data, typ, symbol, addend)
    if resolved is None:
        return set()
    candidates = {resolved}
    if typ != "REL32":
        candidates.update(_cands(dups, symbol, addend))
    return candidates


def check_ordered_reloc_addresses(sym, data, dups, owners, base_sites, target_sites):
    """Compare final RVAs at aligned relocation sites in an exact function."""
    problems = []
    target_by_site = {reloc[0]: reloc[1:] for reloc in target_sites}
    for site, base_type, base_symbol, base_addend in base_sites:
        target = target_by_site.get(site)
        if target is None or target[0] != base_type:
            continue
        base = (base_type, base_symbol, base_addend)
        base_rvas = _resolved_reloc_candidates(sym, data, dups, base)
        target_rvas = _resolved_reloc_candidates(sym, data, dups, target)
        if not base_rvas or not target_rvas or base_rvas & target_rvas:
            continue

        base_rva = min(base_rvas)
        target_rva = min(target_rvas)
        base_owner = owner_for_rva(owners, base_rva)
        target_owner = owner_for_rva(owners, target_rva)
        if (base_owner is not None and target_owner is not None and
                base_owner.rva == target_owner.rva and
                base_owner.symbol == target_owner.symbol):
            problems.append(OwnerOffsetMismatch(
                site, base_owner.symbol, base_owner.source_name,
                target_rva - target_owner.rva, base_rva - base_owner.rva,
                target[1], base_symbol))
        else:
            problems.append(RelocAddressMismatch(
                site, target_rva, base_rva, target[1], base_symbol))
    return problems


def review_fields(resolved_addresses=False):
    """Audit ordered relocations in live near-exact functions.

    The existing build gate checks proven public-owner offsets. ``--resolved``
    widens this to every relocation whose final retail RVA can be resolved; it is
    promoted to the hard gate after its initial findings are repaired.
    """
    sym, data, dups = load_symbols()
    owners = load_owner_ranges()
    report = json.load(open("build/objdiff/report.json"))
    bad = []
    review = []
    checked_functions = 0
    checked_sites = 0
    for unit_record in report["units"]:
        unit = unit_record["name"]
        functions = {function["name"]: function.get("fuzzy_match_percent", 0)
                     for function in unit_record.get("functions", [])}
        if not functions:
            continue
        base_obj = "build/objdiff/base/%s.obj" % unit
        target_obj = "build/delink/%s.c.obj" % unit
        if not (os.path.exists(base_obj) and os.path.exists(target_obj)):
            continue
        base_functions = parse_obj(base_obj, with_sites=True)
        target_functions = parse_obj(target_obj, with_sites=True)
        for name in sorted(functions):
            if name not in base_functions or name not in target_functions:
                continue
            checked_functions += 1
            checked_sites += len(base_functions[name])
            problems = check_owner_offset_multisets(
                sym, data, owners,
                base_functions[name], target_functions[name])
            if resolved_addresses:
                if functions[name] >= FIELD_AUDIT_THRESHOLD:
                    problems.extend(check_ordered_reloc_addresses(
                        sym, data, dups, owners,
                        base_functions[name], target_functions[name]))
            for problem in problems:
                destination = (bad if functions[name] >= FIELD_AUDIT_THRESHOLD
                               else review)
                destination.append((unit, name, problem))
    for unit, name, problem in review:
        print("  REVIEW %s  %s: %s" % (unit, name, problem.diagnostic()))
    for unit, name, problem in bad:
        print("  %s  %s: %s" % (unit, name, problem.diagnostic()))
    if bad:
        label = "ADDRESSES" if resolved_addresses else "FIELDS"
        print("\nRELOC %s FAIL: %d near-exact audited function(s) use a wrong resolved target."
              % (label, len({(unit, name) for unit, name, _problem in bad})))
        return 1
    label = "addresses" if resolved_addresses else "fields"
    print("reloc %s OK: %d functions, %d ordered relocation sites scanned, "
          "%d structural review item(s)."
          % (label, checked_functions, checked_sites, len(review)))
    return 0


def review_pe_data_targets():
    """Audit every configured linked reference into `.rdata` and `.data`.

    Both operands come from final linked images. Only the section base is
    normalized, so no delinker symbol, payload pairing, or reviewed private-name
    mapping can hide a different destination.
    """
    from homm2.build.link_exe import parse_map_symbol_records

    retail_image = _load_pe_image("build/orig/HEROES2W.EXE")
    candidate_image = _load_pe_image("build/link/HEROES2W.EXE")
    sym, data, dups = load_symbols()
    local_rvas = load_candidate_local_rvas()
    section_ranges = _pe_named_section_ranges()
    map_records = parse_map_symbol_records("build/link/HEROES2W.map")
    candidate_functions = {}
    for record in map_records:
        candidate_functions.setdefault(record["name"], []).append(record)
    function_rvas = {}
    with open("build/gen/symbol_names.csv", encoding="latin-1", newline="") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") != "func":
                continue
            try:
                function_rvas[(row["unit"], row["name"])] = int(row["rva"], 0)
            except (KeyError, ValueError):
                continue

    report = json.load(open("build/objdiff/report.json"))
    bad = []
    identity_bad = []
    multiset_identity_bad = []
    relocation_shape_bad = []
    shape_candidate_excess_bad = []
    shape_novel_identity_bad = []
    unresolved_identity_bad = []
    unavailable_functions = []
    duplicate_report_records = []
    configured_functions = 0
    checked_functions = 0
    checked_sites = 0
    skipped_functions = 0
    audit_stats = Counter()
    for unit_record in report["units"]:
        unit = unit_record["name"]
        base_obj = "build/objdiff/base/%s.obj" % unit
        target_obj = "build/delink/%s.c.obj" % unit
        if not (os.path.exists(base_obj) and os.path.exists(target_obj)):
            continue
        base_functions = parse_obj(
            base_obj, with_sites=True, include_imports=True)
        target_functions = parse_obj(
            target_obj, with_sites=True, include_imports=True)
        functions, duplicates = _unique_report_functions(
            unit_record.get("functions", []))
        duplicate_report_records.extend({
            "unit": unit,
            "function": name,
            "records": count,
        } for name, count in sorted(duplicates.items()))
        for function in functions:
            configured_functions += 1
            name = function["name"]
            function_rva = function_rvas.get((unit, name))
            owner = Path(unit).name.lower() + ".obj"
            candidates = [
                record for record in candidate_functions.get(name, [])
                if (record.get("object") or "").lower() == owner
            ]
            missing_inputs = []
            if function_rva is None:
                missing_inputs.append("retail-function-rva")
            if name not in base_functions:
                missing_inputs.append("candidate-coff-function")
            if name not in target_functions:
                missing_inputs.append("target-coff-function")
            if missing_inputs:
                unavailable_functions.append({
                    "unit": unit,
                    "function": name,
                    "reasons": missing_inputs,
                })
                continue
            if len(candidates) != 1:
                skipped_functions += 1
                unavailable_functions.append({
                    "unit": unit,
                    "function": name,
                    "reasons": ["candidate-map-owner-count-%d" % len(candidates)],
                })
                continue
            checked_functions += 1
            candidate_function_rva = candidates[0]["va"] - candidate_image[1]
            multiset = check_pe_data_target_multiset(
                sym, data, dups, local_rvas, unit, function_rva,
                candidate_function_rva, base_functions[name],
                target_functions[name], retail_image, candidate_image,
                section_ranges)
            audit_stats["retail_multiset_sites"] += multiset["expected_count"]
            audit_stats["candidate_multiset_sites"] += multiset["candidate_count"]
            audit_stats["multiset_matched_sites"] += multiset["matched_count"]
            for section, count in multiset["expected_section_counts"].items():
                audit_stats["retail_multiset_%s_sites" % section.lstrip(".")] += count
            for section, count in multiset["candidate_section_counts"].items():
                audit_stats["candidate_multiset_%s_sites" % section.lstrip(".")] += count
            for section, count in multiset["matched_section_counts"].items():
                audit_stats["multiset_matched_%s_sites" % section.lstrip(".")] += count
            audit_stats["multiset_unresolved_sites"] += len(
                multiset["unresolved_candidate_targets"])
            multiset_record = {
                "unit": unit,
                "function": name,
                "fuzzy_match_percent": function.get("fuzzy_match_percent"),
                **multiset,
            }
            if multiset["unresolved_candidate_targets"]:
                unresolved_identity_bad.append(multiset_record)
            if multiset["expected_count"] != multiset["candidate_count"]:
                relocation_shape_bad.append(multiset_record)
                if multiset["excess_candidate_targets"]:
                    shape_candidate_excess_bad.append(multiset_record)
                if multiset["novel_candidate_targets"]:
                    shape_novel_identity_bad.append(multiset_record)
            elif (not multiset["unresolved_candidate_targets"] and
                  (multiset["missing_retail_targets"] or
                   multiset["excess_candidate_targets"])):
                multiset_identity_bad.append(multiset_record)
            elif (not multiset["missing_retail_targets"] and
                  not multiset["excess_candidate_targets"]):
                audit_stats["multiset_exact_functions"] += 1
            problems = check_linked_pe_data_targets(
                base_functions[name], target_functions[name], function_rva,
                candidate_function_rva, retail_image, candidate_image,
                stats=audit_stats)
            target_types = {
                relocation[0]: relocation[1]
                for relocation in target_functions[name]
            }
            aligned_base = [
                relocation for relocation in base_functions[name]
                if (relocation[1] == "DIR32" and
                    target_types.get(relocation[0]) == "DIR32" and
                    _matching_linked_site_context(
                        base_functions[name], target_functions[name],
                        relocation[0], function_rva, candidate_function_rva,
                        retail_image, candidate_image))
            ]
            # Equal numeric sites are not a reliable instruction pairing after a
            # function gains or loses data operands.  The order-independent
            # multiset above is authoritative for those shape-divergent functions.
            identity_problems = []
            if multiset["expected_count"] == multiset["candidate_count"]:
                identity_problems = check_pe_data_targets(
                    sym, data, dups, local_rvas, unit, function_rva,
                    aligned_base, target_functions[name], section_ranges)
            checked_sites += sum(
                relocation[1] == "DIR32" for relocation in base_functions[name])
            for problem in problems:
                bad.append((unit, name, problem))
            for problem in identity_problems:
                identity_bad.append((unit, name, problem))
    identity_transpositions = []
    unmatched_identity = set(range(len(identity_bad)))
    for first_index, (first_unit, first_name, first_problem) in enumerate(identity_bad):
        if first_index not in unmatched_identity:
            continue
        for second_index in sorted(unmatched_identity):
            if second_index <= first_index:
                continue
            second_unit, second_name, second_problem = identity_bad[second_index]
            if (first_unit == second_unit and first_name == second_name and
                    first_problem.expected == second_problem.actual and
                    first_problem.actual == second_problem.expected):
                identity_transpositions.append({
                    "unit": first_unit,
                    "function": first_name,
                    "sites": [first_problem.site, second_problem.site],
                    "retail_rvas": [first_problem.expected, second_problem.expected],
                })
                unmatched_identity.remove(first_index)
                unmatched_identity.remove(second_index)
                break

    output = {
        "schema": 5,
        "scope": "all configured functions with retail and candidate linked owners",
        "ordered_identity_scope": (
            "equal data-target count and matching linked instruction context"),
        "legacy_field_threshold": FIELD_AUDIT_THRESHOLD,
        "configured_functions": configured_functions,
        "checked_functions": checked_functions,
        "checked_dir32_sites": checked_sites,
        "skipped_functions": skipped_functions,
        "unavailable_functions": unavailable_functions,
        "duplicate_report_records": duplicate_report_records,
        "compared_data_sites": audit_stats["compared_section_sites"],
        "compared_rdata_sites": audit_stats["compared_rdata_sites"],
        "compared_writable_data_sites": audit_stats["compared_writable_data_sites"],
        "context_mismatch_sites": audit_stats["context_mismatch"],
        "retail_multiset_sites": audit_stats["retail_multiset_sites"],
        "candidate_multiset_sites": audit_stats["candidate_multiset_sites"],
        "multiset_matched_sites": audit_stats["multiset_matched_sites"],
        "retail_multiset_rdata_sites": audit_stats["retail_multiset_rdata_sites"],
        "retail_multiset_data_sites": audit_stats["retail_multiset_data_sites"],
        "candidate_multiset_rdata_sites": audit_stats["candidate_multiset_rdata_sites"],
        "candidate_multiset_data_sites": audit_stats["candidate_multiset_data_sites"],
        "multiset_matched_rdata_sites": audit_stats["multiset_matched_rdata_sites"],
        "multiset_matched_data_sites": audit_stats["multiset_matched_data_sites"],
        "multiset_exact_functions": audit_stats["multiset_exact_functions"],
        "multiset_identity_divergences": multiset_identity_bad,
        "relocation_shape_divergences": relocation_shape_bad,
        "shape_candidate_excess_divergences": shape_candidate_excess_bad,
        "shape_novel_identity_divergences": shape_novel_identity_bad,
        "unresolved_identity_divergences": unresolved_identity_bad,
        "divergences": [dict(
            unit=unit, function=name, **problem._asdict())
            for unit, name, problem in bad],
        "identity_divergences": [dict(
            unit=unit, function=name, diagnostic=problem.diagnostic(),
            **problem._asdict())
            for unit, name, problem in identity_bad],
        "identity_transpositions": identity_transpositions,
        "unpaired_identity_divergences": [
            dict(unit=identity_bad[index][0], function=identity_bad[index][1],
                 diagnostic=identity_bad[index][2].diagnostic(),
                 **identity_bad[index][2]._asdict())
            for index in sorted(unmatched_identity)
        ],
    }
    output["summary"] = {
        "divergences": len(bad),
        "functions": len({(unit, name) for unit, name, _problem in bad}),
        "rdata": sum(problem.expected_section == ".rdata"
                     for _unit, _name, problem in bad),
        "data": sum(problem.expected_section == ".data"
                    for _unit, _name, problem in bad),
        "identity_divergences": len(identity_bad),
        "identity_functions": len({
            (unit, name) for unit, name, _problem in identity_bad}),
        "identity_transpositions": len(identity_transpositions),
        "unpaired_identity_divergences": len(unmatched_identity),
        "multiset_identity_divergence_functions": len(multiset_identity_bad),
        "relocation_shape_divergence_functions": len(relocation_shape_bad),
        "shape_candidate_excess_functions": len(shape_candidate_excess_bad),
        "shape_candidate_excess_sites": sum(
            len(record["excess_candidate_targets"])
            for record in shape_candidate_excess_bad),
        "shape_novel_identity_functions": len(shape_novel_identity_bad),
        "shape_novel_identity_sites": sum(
            len(record["novel_candidate_targets"])
            for record in shape_novel_identity_bad),
        "unresolved_identity_functions": len(unresolved_identity_bad),
        "unresolved_identity_sites": audit_stats["multiset_unresolved_sites"],
        "unavailable_functions": len(unavailable_functions),
        "duplicate_report_records": len(duplicate_report_records),
    }
    output_path = Path("build/gen/linked_data_relocs.json")
    temporary = output_path.with_suffix(".tmp.%d" % os.getpid())
    temporary.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    os.replace(str(temporary), str(output_path))
    grouped = Counter(
        (problem.expected_section, problem.expected_offset,
         problem.actual_section, problem.actual_offset,
         problem.target_symbol, problem.base_symbol)
        for _unit, _name, problem in bad)
    for group, count in sorted(
            grouped.items(), key=lambda item: (-item[1], item[0]))[:80]:
        expected_section, expected_offset, actual_section, actual_offset, target, base = group
        print("  x%d retail %s+0x%x -> candidate %s+0x%x (retail %s, base %s)" %
              (count, expected_section, expected_offset, actual_section,
               actual_offset, target, base))
    for unit, name, problem in identity_bad:
        print("  IDENTITY %s  %s: %s" %
              (unit, name, problem.diagnostic()))
    for record in multiset_identity_bad:
        print("  MULTISET IDENTITY %s  %s: %d missing, %d excess" %
              (record["unit"], record["function"],
               len(record["missing_retail_targets"]),
               len(record["excess_candidate_targets"])))
    if bad:
        print("\nPE DATA RELOCS FAIL: %d linked-placement divergence(s) in %d function(s); "
              "%d unique destination mapping(s); %d ordered identity divergence(s), "
              "%d balanced transposition(s), %d unpaired; %d all-function "
              "multiset identity divergence(s), %d relocation-shape divergence(s), "
              "%d shape function(s) with candidate excess, %d novel candidate "
              "identity function(s), %d unresolved identity function(s), "
              "%d unavailable function(s); "
              "report=%s" %
              (len(bad), len({(unit, name) for unit, name, _problem in bad}),
               len(grouped), len(identity_bad), len(identity_transpositions),
               len(unmatched_identity), len(multiset_identity_bad),
               len(relocation_shape_bad), len(shape_candidate_excess_bad),
               len(shape_novel_identity_bad), len(unresolved_identity_bad),
               len(unavailable_functions), output_path))
        return 1
    print("PE data relocs OK: %d functions and %d DIR32 sites checked "
          "between final linked images; %d function(s) lacked a unique MAP owner." %
          (checked_functions, checked_sites, skipped_functions))
    return 0

def _function_for_arg(arg):
    """Resolve an RVA or decorated name to the generated (unit, function) identity."""
    try:
        wanted_rva = int(arg, 0)
    except ValueError:
        wanted_rva = None
    with open("build/gen/symbol_names.csv", encoding="latin-1") as f:
        for row in csv.DictReader(f):
            if row.get("kind") != "func":
                continue
            try:
                row_rva = int(row["rva"], 0)
            except (KeyError, ValueError):
                continue
            if row["name"] == arg or (wanted_rva is not None and row_rva == wanted_rva):
                return row["unit"], row["name"], row_rva
    raise SystemExit("function not found in build/gen/symbol_names.csv: %s" % arg)

def review(rva):
    """Single-function multiset review (order-independent; usable on <100% walls)."""
    sym, data, dups = load_symbols()
    unit, name, _function_rva = _function_for_arg(rva)
    base_obj = "build/objdiff/base/%s.obj" % unit
    target_obj = "build/delink/%s.c.obj" % unit
    B = parse_obj(base_obj).get(name, [])
    T = parse_obj(target_obj).get(name, [])
    for s in sorted({r[1] for r in B if is_fake(sym, data, r[1])}):
        print("  !! FAKE base references '%s'" % s)
    def bvas(rs):
        c, sy = Counter(), {}
        for r in rs:
            if '_00A@' in r[1]:
                continue
            v = resolve(sym, data, *r)
            if v is not None:
                c[v] += 1; sy.setdefault(v, r[1])
        return c, sy
    bc, va_sym = bvas(B)
    tvas = _tvas(sym, data, dups, [r for r in T if '_00A@' not in r[1]])
    diff = bc - tvas
    for v, n in sorted(diff.items()):
        print("  base references 0x%x (%s) x%d that retail never does (wrong/extra global/const/fn)"
              % (v, va_sym[v], n))
    print("base relocs=%d target relocs=%d  only-base=%d" % (len(B), len(T), sum(diff.values())))

def review_counts(scope="BASE"):
    """List incomplete functions whose relocation occurrence counts still differ.

    This is a structural-work queue, not a wrong-target report: missing/excess counts usually mean
    the source has not yet recovered retail's control flow or publication lifetimes. Retained fuzzy
    maxima select incomplete functions so transient TU-state dips do not reopen finished work.
    """
    prefix = scope.rstrip("/") + "/"
    maxima = {}
    with open("config/match_baseline.tsv", encoding="utf-8") as f:
        for row in csv.reader(f, delimiter="\t"):
            if len(row) < 3 or row[0].startswith("#"):
                continue
            try:
                maxima[(row[0], row[1])] = float(row[2])
            except ValueError:
                continue

    report = json.load(open("build/objdiff/report.json"))
    rows = []
    for unit in report["units"]:
        unit_name = unit["name"]
        if not unit_name.startswith(prefix):
            continue
        base_obj = "build/objdiff/base/%s.obj" % unit_name
        target_obj = "build/delink/%s.c.obj" % unit_name
        if not (os.path.exists(base_obj) and os.path.exists(target_obj)):
            continue
        base_functions, target_functions = parse_obj(base_obj), parse_obj(target_obj)
        for fn in unit.get("functions", []):
            name = fn["name"]
            retained = maxima.get((unit_name, name), fn.get("fuzzy_match_percent", 0.0))
            if retained >= 100.0 or name not in base_functions or name not in target_functions:
                continue
            base_count = len(base_functions[name])
            target_count = len(target_functions[name])
            if base_count == target_count:
                continue
            missing = max(target_count - base_count, 0)
            excess = max(base_count - target_count, 0)
            size = int(fn.get("size", 0))
            unmatched = size * (100.0 - retained) / 100.0
            demangled = fn.get("metadata", {}).get("demangled_name", name)
            rows.append((missing + excess, unmatched, unit_name, retained, size,
                         base_count, target_count, missing, excess, demangled))

    # This is an input to the normal hardest-first campaign, so rank recovered byte potential
    # before raw relocation-count delta.
    rows.sort(key=lambda row: (-row[1], -row[0], row[2], row[9]))
    print("unit\tretained_pct\tsize\tunmatched_bytes\tbase_relocs\tretail_relocs\tmissing\texcess\tfunction")
    for _, unmatched, unit, retained, size, base_count, target_count, missing, excess, name in rows:
        print("%s\t%.4f\t%d\t%.2f\t%d\t%d\t%d\t%d\t%s" %
              (unit, retained, size, unmatched, base_count, target_count, missing, excess, name))
    print("summary\tfunctions=%d\tmissing=%d\texcess=%d" %
          (len(rows), sum(row[7] for row in rows), sum(row[8] for row in rows)))
    return 0


def review_addends(scope=None):
    """Compare raw relocation-name/addend multisets for every report function.

    Unlike the ordered field gate, this audit is valid at any fuzzy percentage:
    code movement cannot change an encoded owner-relative addend. A relocation
    that exists on only one side is reported rather than silently skipped.
    """
    prefix = scope.rstrip("/") + "/" if scope else None
    canonical_data_names = load_canonical_data_names()
    report = json.load(open("build/objdiff/report.json"))
    output = {
        "schema": 3,
        "scope": scope or "all",
        "compared_functions": 0,
        "mismatched_functions": 0,
        "missing_objects": [],
        "functions": [],
    }
    for unit_record in report["units"]:
        unit = unit_record["name"]
        if prefix and not unit.startswith(prefix):
            continue
        base_obj = "build/objdiff/base/%s.obj" % unit
        target_obj = "build/delink/%s.c.obj" % unit
        missing = [path for path in (base_obj, target_obj) if not os.path.exists(path)]
        if missing:
            output["missing_objects"].extend(missing)
            continue
        base_functions = parse_obj(base_obj, include_imports=True)
        target_functions = parse_obj(target_obj, include_imports=True)
        for function in unit_record.get("functions", []):
            name = function["name"]
            base_present = name in base_functions
            target_present = name in target_functions
            output["compared_functions"] += 1
            if not base_present or not target_present:
                output["functions"].append({
                    "unit": unit,
                    "function": name,
                    "base_present": base_present,
                    "target_present": target_present,
                    "differences": [],
                })
                continue
            differences = compare_function_reloc_addends(
                base_functions[name], target_functions[name], name,
                canonical_data_names)
            if differences:
                output["functions"].append({
                    "unit": unit,
                    "function": name,
                    "base_present": True,
                    "target_present": True,
                    "differences": differences,
                })

    output["mismatched_functions"] = len(output["functions"])
    difference_counts = Counter(
        difference["classification"]
        for function in output["functions"]
        for difference in function["differences"])
    value_mismatch_functions = {
        (function["unit"], function["function"])
        for function in output["functions"]
        for difference in function["differences"]
        if difference["classification"] == "value-set"
    }
    canonical_one_sided_functions = {
        (function["unit"], function["function"])
        for function in output["functions"]
        for difference in function["differences"]
        if difference["classification"] == "one-sided" and
        difference["canonical_data"]
    }
    output["difference_rows"] = {
        "value_set": difference_counts["value-set"],
        "count_only": difference_counts["count-only"],
        "one_sided": difference_counts["one-sided"],
        "code_local": difference_counts["code-local"],
    }
    output["value_mismatch_functions"] = len(value_mismatch_functions)
    output["canonical_data_one_sided_rows"] = sum(
        difference["classification"] == "one-sided" and
        difference["canonical_data"]
        for function in output["functions"]
        for difference in function["differences"])
    output["canonical_data_one_sided_functions"] = len(
        canonical_one_sided_functions)
    output_path = Path("build/gen/function_reloc_addends.json")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    temporary = output_path.with_suffix(".tmp.%d" % os.getpid())
    temporary.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    os.replace(str(temporary), str(output_path))

    for function in output["functions"]:
        if not function["base_present"] or not function["target_present"]:
            print("  %s  %s: base_present=%s target_present=%s" % (
                function["unit"], function["function"],
                function["base_present"], function["target_present"]))
            continue
        for difference in function["differences"]:
            if (difference["classification"] != "value-set" and not
                    (difference["classification"] == "one-sided" and
                     difference["canonical_data"])):
                continue
            print("  %s  %s: %s target=%s base=%s missing=%s excess=%s" % (
                function["unit"], function["function"], difference["symbol"],
                difference["target"], difference["base"],
                difference["missing"], difference["excess"]))
    print("reloc addends: %d function(s) compared, %d value-set row(s) in "
          "%d function(s), %d count-only row(s), %d one-sided row(s), "
          "%d code-local row(s), %d canonical-data one-sided row(s) in "
          "%d function(s), "
          "%d total mismatched function(s), %d missing object(s); report=%s" % (
              output["compared_functions"],
              output["difference_rows"]["value_set"],
              output["value_mismatch_functions"],
              output["difference_rows"]["count_only"],
              output["difference_rows"]["one_sided"],
              output["difference_rows"]["code_local"],
              output["canonical_data_one_sided_rows"],
              output["canonical_data_one_sided_functions"],
              output["mismatched_functions"], len(output["missing_objects"]),
              output_path))
    return 1 if output["functions"] or output["missing_objects"] else 0

def main():
    if len(sys.argv) > 1 and sys.argv[1] == "--pe-data":
        return review_pe_data_targets()
    if len(sys.argv) > 1 and sys.argv[1] == "--resolved":
        return review_fields(resolved_addresses=True)
    if len(sys.argv) > 1 and sys.argv[1] == "--fields":
        return review_fields()
    if len(sys.argv) > 1 and sys.argv[1] == "--counts":
        return review_counts(sys.argv[2] if len(sys.argv) > 2 else "BASE")
    if len(sys.argv) > 1 and sys.argv[1] == "--addends":
        return review_addends(sys.argv[2] if len(sys.argv) > 2 else None)
    if len(sys.argv) > 1:                        # single-function review mode
        review(sys.argv[1]); return 0
    sym, data, dups = load_symbols()
    report = json.load(open("build/objdiff/report.json"))
    bad = []
    # A WRONG reloc doesn't drop objdiff to a low %, it costs a TINY fraction (~0.005%/reloc) that
    # rounds to "100.00%" in the display — so we must audit near-exact fns, not just == 100. The
    # multiset check (check_fn) is order-independent, so 99.5% (with its minor reordering) is safe.
    THRESHOLD = 99.5
    for u in report["units"]:
        unit = u["name"]
        near_exact_audited = {
            f["name"] for f in u.get("functions", [])
            if f.get("fuzzy_match_percent", 0) >= THRESHOLD
        }
        if not near_exact_audited:
            continue
        base_obj = "build/objdiff/base/%s.obj" % unit
        tgt_obj = "build/delink/%s.c.obj" % unit
        if not (os.path.exists(base_obj) and os.path.exists(tgt_obj)):
            continue
        bf, tf = parse_obj(base_obj), parse_obj(tgt_obj)
        for name in sorted(near_exact_audited):
            if name not in bf or name not in tf:
                continue
            for p in check_fn(sym, data, dups, unit, name, bf[name], tf[name]):
                bad.append((unit, name, p))
    for unit, name, p in bad:
        print("  %s  %s: %s" % (unit, name, p))
    if bad:
        print("\nRELOCS FAIL: %d near-exact audited function(s) reference a wrong/fabricated target "
              "(objdiff masks relocs, so it can't see this)." % len(bad))
        return 1
    print("relocs OK: every >=99.5% audited function's reloc targets resolve to the retail address.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
