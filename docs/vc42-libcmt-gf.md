# VC4.2 writable CRT literal recovery

The shipping executable places 102 decorated VC4.2 CRT string literals in
initialized writable `.data`. The provisioned `LIBCMT.LIB` instead places them
in read-only `.rdata` because its objects were compiled with uppercase `/GFy`.
VC4.2 treats the case as significant:

- `/GFy` is read-only string pooling (`/GF`) plus function COMDATs (`/Gy`).
- `/Gfy` is writable string pooling (`/Gf`) plus function COMDATs (`/Gy`).

The original CRT `MAKEFILE` contains
`CC_OPTS_BASE=-c -nologo -Zelp8 -W3 -WX -GFy -DWIN32`. Its SHA-256 is pinned by
`build_libcmt_gfy.py`, along with the compiler, archive, and available source
files. The verified VC4.2 Enterprise Disc 1 image used for the source audit is:

```text
fdf6c4f696f80759a9a1d7d1aa0b6b6d455debd4fa12b6d1711799a2ca1320a3
```

## Derived archive

`ninja link` deterministically derives `build/link/crt/LIBCMT.LIB` from the
pinned original archive before invoking LINK. It transforms 107 decorated
literal COMDAT sections across ten original members. For each section it changes
only:

- the section header name from `.rdata` to `.data`;
- the `IMAGE_SCN_MEM_WRITE` characteristic;
- the associated static COFF section-symbol name from `.rdata` to `.data`.

That is exactly 11 changed bytes per section and 1,177 bytes total. Member sizes,
archive order and offsets, linker indexes, timestamps, public symbols, literal
data, code, and relocations are unchanged. Both input and derived archives are
hash-gated:

```text
original  1b42eb48c0824798e2585573069f85c9c0b08df45db62ff2cc7a7aa5dd1b5ba2
derived   3b6520118834f596ca1266d8418a234d8fa053a1c895a85acda904fa0dfb8208
```

This is a pre-link archive derivation, not an executable patch. The linker sees
the same original object identities and contents with the historically correct
writable literal-section attributes.

## Authenticity proof

Microsoft ships source for `aw_map`, `crt0msg`, `crtmbox`, `output`, `perror`,
and `syserr`. Recompiling those six files with the official uppercase and
lowercase flags proves the transformation for 73 sections. The uppercase build
also reproduces the archive objects after timestamp and `.file` case
normalization.

`OUTPUT.obj` has one compiler-local wide-literal decoration whose checksum tail
is unstable across rebuild locations. It differs from the archive in exactly
three string-table bytes at offsets 109, 112, and 113 of that decoration; all
other object bytes match. The gate accepts only that precise shape. The final
derivation starts from the archive object, so it preserves the archive's original
decoration rather than retaining a rebuilt identity.

The disc contains `cvt`, `fpexcept`, `testfdiv`, and `x10fout` only as binary
MT_LIB members. Their 34 sections receive the same proven transformation directly
in the pinned original archive.

Run the optional source audit with:

```sh
python3 -m homm2.build.build_libcmt_gfy \
  --prove-source /path/to/MSDEV/CRT/SRC
```

## Link regression gate

The normal final-link path requires the exact 102 retail symbols, including their
expected `LIBCMT:<unit>.obj` owners, to resolve into initialized `.data`. An A/B
audit between the original and derived archives additionally proves:

- exactly those 102 storage-class mismatches are fixed;
- no new storage-class mismatch is introduced;
- `.text` has the same bytes after zeroing HIGHLOW relocation words;
- both `.text` images contain the same 26,438 HIGHLOW relocations.

The captured normalized `.text` SHA-256 is:

```text
779563aff9ad5ba4074ae0bad6493218b4c8a514857b67cc4308b4acbcdfb536
```

The archive contains 107 transformed sections while the executable gate covers
102 symbols because only the latter exact decorated-name/owner set is retained
as public literal storage in the linked retail image.
