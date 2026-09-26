// REQUEST empty-cell ownership: initialized block-scope arrays, 2026-09-26.
// Parent: 5b49bff9a. Production VC6 SP5 flags; no compiler-state probes.
//
// Retail establishes six distinct zero-initialized writable cells and their
// use sites. Five supply empty names/extensions to strcpy inside three
// fileRequester methods; the sixth is the initializer of cFRDummy. It does
// not reveal the original private identifiers. This reconstruction names the
// five method-owned cells in their narrow use scopes and retains the global
// pointer/literal definition. It introduces no additional program storage.
//
// Complete four-arm structural matrix (all compile, no timeout/truncation):
//   build/bss-local-storage/probe.py
//   build/bss-local-storage/{control,local_initialized,local_zero,namespace_control}.cpp
//   build/bss-local-storage/results.json
//
// Control: existing five bare empty literals; cFRDummy's literal emits first.
// Local initialized: replace each of those five literal operands by an array
// declared immediately before its strcpy in the same lexical scope:
//
//   static char emptyFileName[1] = "";      // InitializeFiles loop
//   strcpy(m_fileNames[indexData].text, emptyFileName);
//   static char emptyExtension[1] = "";     // same loop
//   strcpy(m_extensions[indexData].text, emptyExtension);
//   static char emptyLastFilename[1] = "";  // Open entry
//   strcpy(gLastFilename, emptyLastFilename);
//   static char emptyCycleName[1] = "";     // Main, no selected cycle name
//   strcpy(cycleNameBuffer, emptyCycleName);
//   static char emptyFilteredName[1] = "";  // Main, no selected filtered name
//   strcpy(filteredNameMap, emptyFilteredName);
//
// Local zero: identical scope/use sites, omit each '= ""' initializer.
// Namespace control: declarations in a disposable header, initialized array
// definitions at TU end, uses identical to local initialized. Diagnostic only;
// no namespace storage or disposable header was retained.
//
// Measured native ordinary .bss offsets (anchor retail VA 0x00533d78):
//                         control    local initialized   local zero
//   fGutterMinY             0x00          0x00               0x08
//   fGutterTravelLength     0x04          0x04               0x0c
//   iMaxListSize            0x08          0x08               0x1c
//   emptyFileName           0x10          0x0c               0x00
//   emptyExtension          0x14          0x10               0x18
//   emptyLastFilename       0x18          0x14               0x14
//   emptyCycleName          0x1c          0x18               0x10
//   emptyFilteredName       0x20          0x1c               0x04
//   cFRDummy's literal      0x0c          0x20               0x20
// Namespace control also matches the local-initialized offsets, but is not
// needed to model the method-owned storage. Uninitialized arrays are rejected:
// they disturb the established named globals. Explicit initialization affects
// VC6's allocation phase even when all payload bytes are zero.
//
// Local initialized retains cFRDummy at ordinary .data+4. All .text sections
// retain control sizes and exact relocation-masked bytes. No new initializer,
// guard, destructor, COMDAT, TU, padding, or compiler option is introduced.
// Five DATA annotations now express these real local definitions; annotations
// are byte-neutral and the candidate binder resolves VC6 local-scope names.
//
// Whole-image verification, using only the local_initialized raw probe object
// substituted for raw REQUEST.obj in the otherwise current historical link:
//   build/bss-local-storage/link.py
//   build/bss-local-storage/{link.json,HMM2PL.exe,HMM2PL.map,HMM2PL.rsp}
// All four LINK passes write their own unmodified outputs. The six relocation
// operands now equal retail, including cFRDummy -> 0x00533d98 at RVA 0x116ae0.
// Whole image: 500 differing bytes (previously 506), .text 496/.rdata 3/.data 1;
// the remaining differences concern Audiere/DIMMER, not REQUEST storage.
// SHA256: c0eacb837c0d8bdec918962df5bce11270d05247908f4be34badd4015255a1b8.
//
// Complete native REQUEST byte/relocation review:
//   build/bss-local-storage/review.py
//   build/bss-local-storage/retail-review.json
// All 19 claimed text/pointer spans equal retail at their claimed RVAs;
// all 379 ordered relocation operands equal retail. Every image-address
// DIR32 site belongs to the reviewed relocation manifest. __except_list's
// absolute FS offset is checked byte-for-byte but is not a PE image address.
//
// Disposition: retain initialized block-scope storage. This is a source-level
// explanation for the BSS order, not proof of the original variable names.
// It supersedes the assumption that the five method cells must be literals;
// rejected cFRDummy accessor/PCH/cross-TU experiments remain rejected for the
// specific reasons recorded in the other dossiers.
//
// Annotated production validation:
//   homm2 redelink
//   homm2 build
//   homm2 link --historical
//   homm2 relocs 0x8e836 / 0x8f275 / 0x8f737
//   homm2 sema disasm <each RVA> --blocks --diff --lite
//   git diff --check
// Logs: build/bss-local-storage/{redelink,build,production-link,audit}.log,
//       {initialize,open,main}-{relocs,blocks}.log.
// Build: 1727/1727 exact functions, 291995/291995 data bytes. The final
// production executable has the same SHA256 as the independent raw probe.
// Focused relocation counts are 53/53, 51/51, and 109/109, no missing owners.
// Main's block view interprets trailing jump-table bytes as a final block;
// the complete reviewed function span and ordered native relocations above
// are the byte-level proof, not that diagnostic partition.
// The full-image audit still reports 500 differing bytes and 12 displaced
// project functions. This commit closes REQUEST BSS, not the entire image.
