# `SOURCE/SMACKMGR` data closure

After a forced redelink and full build, all compared SMACKMGR payload sections are
byte-exact:

- `.CRT$XCU`: 4/4 bytes
- `.bss`: 124/124 bytes
- `.data`: 4,621/4,621 bytes
- total data: 4,749/4,749 bytes

The 30 `DATA_COMPGEN` claims cover the complete compiler-string suffix after the
three ordinary initialized objects. The linked relocation audit resolves every
SMACKMGR-owned string and ordinary data target. Its 15 remaining ordered-identity
diagnostics are import-address cells for Miles and Smacker APIs. The linked-placement
report also reflects project-wide section displacement from data objects in other,
unfinished TUs; the SMACKMGR target identities themselves agree.

The raw whole-object command

```sh
homm2 data-relocs --unit SOURCE/SMACKMGR \
  build/objdiff/base/SOURCE/SMACKMGR.obj \
  build/delink/SOURCE/SMACKMGR.c.obj \
  -o build/smackmgr-data-relocs.json
```

retains one `.CRT$XCU` owner/owner-offset classification residual. Candidate and target
each contain one relocation at offset zero to the same compiler initializer body. The
retail object attributes it to the adjacent source function while the candidate object
uses the local CRT initializer owner. The payload, relocation site, type, addend, and
destination are closed; only compiler-metadata ownership normalization remains.
