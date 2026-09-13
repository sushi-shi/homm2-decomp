# Common-helper verification

The refactor is based on canonical `ca2904a91`, with the completed reading audit
at `77b87cd47` and final applied source at `610e19c1` (documented in `750ce56aa`).
The [final decision table](findings.md) records all retained subsets and exclusions.
Generated/portable-source propagation and gameplay repairs are outside this PR.

## Acceptance and measured alternatives

Retained changes preserve types, evaluation, narrowing, mutation, ownership,
callback order and raw compiler output. Before/after equivalence supplements the
retail comparison; it does not establish original developer spelling or turn a
pre-existing retail residual into an exact closure.

The eight source dossiers retain complete tested alternatives and measured
results. They are documentation, not game build input. Disposable matrix axes,
manifests, results and objects stay under `build/readability/`.

| Evidence | Scope |
| --- | --- |
| [Widget message](../matching/SetWidgetMessage/shared-macro.cpp) | Ordered header writes; the comma expression is retained, `do/while(0)` adds VC6 `/Od` instructions. |
| [Accessors and byte rules](../matching/ReadabilityAccessors/reuse.cpp) | Existing accessors and CP1251 folds; staged-byte calendar/filename alternatives remain explicit. |
| [Widgets and resources](../matching/ReadabilityWidgets/shared-operations.cpp) | Hit tests, geometry reads, allocation/copy, icon bounds and display/configuration operations. |
| [Domain predicates](../matching/ReadabilityDomains/shared-operations.cpp) | Precise creature, map, building, spell and stack queries. |
| [Ordered operations](../matching/ReadabilityOrdered/shared-operations.cpp) | Dialog, extent, deadline, creature-name and combat-state sequences. |
| [Formulas](../matching/ReadabilityFormulas/shared-operations.cpp) | Combat-grid, distance, gate, scouting, building, calendar and packed-count expressions. |
| [Protocols](../matching/ReadabilityProtocols/shared-operations.cpp) | UI/campaign/resource operations and measured rejected wrappers. |
| [Storage](../matching/ReadabilityStorage/shared-operations.cpp) | Adventure drawing, exact-size file values and separate transport-storage primitives. |

## Reproducible inputs and baseline prerequisites

- Retail image: Buka `HMM2PL.exe`, SHA-256
  `bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a`.
  The audit extracted it from `homm2g-buka.iso`, under
  `Autorun/Launch/Setup2/data1.cab` / `data1.hdr` / `data2.cab`, entry
  `Program Executable Files\HMM2PL.exe`. Retail files remain ignored.
- VC6 SP5 archive: SHA-256
  `accfbb0ab1a63d4b698d0d205950611386c39b81b1868b336dbfdab413bb84c4`.
  At setup, the published archive failed the pin. Rebuilding with
  `nix-shell scripts/toolchain/create-toolchain-release.nix` verified all 12
  pinned tools/runtime artifacts and reproduced the expected archive. No pin,
  vendor-header patch or compiler-flag change was retained.
- Fresh link graphs now derive BASE archive membership from the unit tier, not
  an address-sorted split at data-only X_GLOBAL. This prevents BASE/Midi from
  entering the direct-link group when generated symbols are not yet available.
  Tests cover fresh inventories, missing anchors, ordering and OMF members.
- OldMain's legacy mutable-char score API receives an explicit `const_cast<char*>`
  for its literal ternary. This fixes annotation parsing without changing the API
  or authorizing mutation. All 11 KB non-debug sections and 5,085 ordered
  relocations were unchanged before/after this prerequisite.
- Initialize the lowercase SDK mirror once with `python3 -m homm2.init.clangd`
  before parallel annotation jobs on a fresh setup.

## Verification

Enter `nix develop .#build` from the intended worktree and confirm `HOMM2_DIR`
resolves there. Save separately compiled pre-edit objects before rebuilding:

```sh
homm2 build
python3 -m homm2.audit.object_equivalence \
  /path/to/pre-edit-objects build/objdiff/base
homm2 relocs --fields
python3 -m homm2.audit.readability_contracts
homm2 selftest
python3 -m homm2.audit.readability --write
python3 -m homm2.audit.readability --check
```

At the applied snapshot, all 98 raw objects compare unchanged: 1,516 non-debug
sections and 42,433 ordered relocations, including sites, types, destinations and
addends. Named symbols retain identity, coordinates and storage class; only actual
compiler counter suffixes may differ at identical destinations. The comparison
rejects empty/missing/added objects and changed bytes, flags, labels or relocations.

The report retains 1,727/1,727 compared functions and 291,995/291,995 data bytes.
The separate field audit scans 1,727 functions and 38,307 ordered sites with zero
structural review items. Rounded report totals do not supersede the local retail
residuals explicitly retained in the source dossiers.

The applied snapshot passed 905 selftests with Universal Ctags 6.2.1 and no skips.
The documentation/output-path cleanup passes 909 tests with no skips, including
four new inventory-path/read-credit regressions. All four generated inventories
are byte-identical to the former tracked products. The build, 98-object comparison
and field-relocation review were rerun successfully; game source and the eight
matching dossiers are unchanged by this cleanup.

The real-header VC6 contract executable checks all 256 uppercase and lowercase
CP1251 inputs, creature/widget/map predicates, signed formulas, exact-size scalar
and record I/O, single operand evaluation, short reads and EOF. It sends no network
packets and does not validate or repair the retained transport defects. Explicit
global-data, global-definition, vtable and fixed-width-integer gates passed.

## Known baseline limitations

`AUDITS = False` remains unchanged: `homm2 build` does not run the older documented
general hard gates. Separately rerun checks reproduced these existing failures:

- Declaration policy: TU-local AudiereEffectsState and SeedPositionState structs.
- Definition ownership: the same five missing-owner/declaration reports.
- Fake-label audit: two existing missing functions and 97 attribution reports.
- Private-function annotation: parsing rejects the old VC6 STL headers.
- Strict Clang: 95/96 TUs fail; unlimited diagnostics reproduce 1,615 errors in
  eight SDK headers, with zero game-source/header errors. This is not a passing
  strict build; no diagnostic suppression is counted as a fix.

The [reading records](reviews.json) remain bound to the original audit hashes.
The [implementation delta records](implementation-review.tsv) record the separate
caller/contract review for 96 changed files, not a new whole-file reading pass.
Current inventories are generated locally as described in [README.md](README.md).
