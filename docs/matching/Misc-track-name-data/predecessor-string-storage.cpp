// Structural recovery: one Misc translation unit with real initialized static
// storage for the nonempty strings owned by its prefix functions.
// Measured 2026-09-27 in matcher-1, branch matcher/bss-misc, with pinned VC6 SP5.
// Current root source/header snapshots are the inputs; stale worker production
// sources were not used. All artifacts below are relative to matcher-1.
//
// Motivation: named-owner-merge.cpp preserved method bodies but moved the track
// name before prefix function literals. REQUEST's initialized local arrays had
// independently demonstrated a different compiler storage-emission phase. The
// new family changes the REAL PREDECESSOR STRINGS, not the track string alone.
// Every new array replaces one actual literal occurrence, is referenced by its
// original expression, and preserves bytes, writability, and static lifetime.
// No unused declarations, synthetic padding, COFF edits, or output rewrites are
// involved. Repeated strings remain separate objects as in the retail image.
//
// Source products and complete scripts:
//   build/misc-prefix-string-storage/{prefix,suffix}.cpp
//   build/misc-prefix-string-storage/include/   (current root header snapshot)
//   build/misc-prefix-string-storage/{run.py,results.json}
//   build/misc-prefix-string-storage/{audit.py,raw-audit.json}
//   build/misc-prefix-string-storage/{link.py,image-audit.json}
//   build/misc-prefix-string-storage/{linked-detail.py,linked-detail.json}
//   build/misc-prefix-string-storage/{order-product.py,order-results.json}
//   build/misc-prefix-string-storage/{order-audit.py,order-raw-audit.json}
//   build/misc-prefix-string-storage/{order-link.py,order-image-audit.json}
//   build/misc-prefix-string-storage/{semantic-names.py,semantic-results.json}
//   build/misc-prefix-string-storage/{semantic-audit.py,semantic-raw-audit.json}
//   build/misc-prefix-string-storage/{semantic-link.py,semantic-image-audit.json}
//   build/misc-prefix-string-storage/{closure-audit.py,closure-audit.json}
//   build/misc-prefix-string-storage/{annotation-check.py,annotation-check.json}
// Every arm retains its full generated source, raw object, native library,
// native EXE/MAP/PDB, compile/link logs, and response files. The result manifests
// record source SHA256, flags, successful return codes, and timeout status.
//
// FIRST COMPLETE PRODUCT: scope(local/global) x track(literal/named), plus one
// clean named merged control. All five compile/link arms completed.
//
// Merge structure is the previously tested lexical parent: prefix Misc.cpp,
// then the full KB header, then MiscRuntime.cpp's body starting at its
// MiscGraphicsConstants include. Duplicate enum/include preamble is removed.
// Both prior TUs have the same current root manifest flags:
//   /Od /MT /Gr /G5 /Ob1 /Gi- /GX /DNO_STRICT /Gy
// Functions retain their source order and bodies. All nonempty string literal
// expressions in the prefix's real function definitions become distinct char
// arrays. Empty strings, existing globals, and suffix literals are unchanged.
// The generator handles adjacent literal tokens as one expression, expands
// MISC_REGISTRY_KEY at each of its two actual uses, and preserves the complete
// localization::Tr expression in its initializer for normal catalog expansion.
// No localization ID is mistakenly emitted as game text. There are 118 arrays.
//
// Local parent, representative recovered source:
//
//   void ShowMemoryStatus(void) {
//       static char memoryStatusFormat[] = "Mem Left %dK";
//       i32 memLeft = MemSize(1);
//       sprintf(gText, memoryStatusFormat, memLeft);
//       AbsAiPrint(gText);
//   }
//
// Global parent places the same distinct initialized definitions immediately
// before each owning function. Initial probe names are disposable identifiers.
// In the literal track arm the shared pointer is restored to
//   H2_CONST char* gcCDTrackName = "\\Tracks2\\02-AudioTrack 02.ogg";
// and the old named backing definition is removed. In the named track arm the
// pointer keeps referring to gcCDTrackNameText and the real array is still
// declared at the prefix/suffix boundary. There is no duplicate backing string.
//
// Native linking replaces Misc.lib with the single merged-object archive and
// omits MiscRuntime.lib, using all other root resource-link inputs unchanged.
// All output paths, PDBs, and Wine work are local to this worktree. These links
// test placement and bytes; they do not replay the four historical link dates.
//
// First product results:
//   Parent                 Raw bodies   Linked bodies   Misc data diff   Track
//   control-named               47/47           35/47        2742/3928  51e5f8
//   local-literal               47/47           45/47          30/3928  51f120
//   local-named                 47/47           45/47          30/3928  51f120
//   global-literal              47/47           45/47          30/3928  51f120
//   global-named                47/47           45/47          30/3928  51f120
// All 47 linked function addresses remain their retail VAs. The four structural
// parents give the same clue: the track array is now .data +0xb44, exactly its
// required placement relative to the pointer at 0x51e5dc. The initialized data
// section containing the prefix storage is 2914 bytes. Full PE section RVAs,
// virtual sizes, raw sizes, and raw offsets agree with retail for every arm.
//
// The remaining 30 data bytes are only two filename/format pairs, in
// ReadPrefsFromFile and WritePrefsToFile. Original expressions are both
//   sprintf(gText, "%s", "HEROES2.CFG");
// The compiler emits the original argument literals in evaluated argument order
// (filename then format), but the first product declared arrays in lexical
// source order (format then filename). Four linked DIR32 operand bytes differ;
// all are within these two calls. No other body byte differs in those parents.
//
// SECOND COMPLETE PRODUCT: two independent declaration-order choices across
// all four structural parents, 4 x 2 x 2 = 16 compile/link arms. No truncation.
// Each choice keeps the two real declarations and their original uses, changing
// only which declaration appears first. Results for each of the four parents:
//   read pair/write pair swapped       Misc data diff       Linked bodies
//   no/no                                      30/3928               45/47
//   no/yes                                     15/3928               46/47
//   yes/no                                     15/3928               46/47
//   yes/yes                                     0/3928               47/47
// Every one of the 16 arms retains 47/47 raw function byte spans and addresses.
// The exact branch declares configFileName before fileNameFormat in both owners.
// This follows measured storage evidence, not arbitrary source-state probing.
//
// SEMANTIC-NAME RECOVERY: both local parents with the successful pair order
// were recompiled and linked with real function-owned names. Both remain exact:
//   local-literal-semantic/{Misc.cpp,Misc.obj,HMM2PL.exe,HMM2PL.map}
//   local-named-semantic/{Misc.cpp,Misc.obj,HMM2PL.exe,HMM2PL.map}
// No miscPrefixText probe identifiers remain in these sources. Registry value
// names describe the actual value domain; both AUTO strings remain distinct
// autoLoadName/autoSaveName storage; the two music-volume query sites remain
// distinct musicVolumeProbeValue/musicVolumeValue storage. RMT format labels
// in this snapshot say receiveRestart*/sendRestart*: their precise protocol role
// was not independently audited and integration may prefer existing field names.
//
// COMPLETE LOCAL CLOSURE AUDIT (both semantic-name parents):
//   * All 47 function sizes and raw bytes match the current split baseline.
//   * All 47 FULL LINKED spans match retail at their actual retail VAs.
//   * All 884 ordered relocation sites, types, and raw owner-relative addends
//     match the baseline sequence. Every linked operand resolves to the same
//     retail destination, including the new array owners and unchanged EH refs.
//   * Every one of the 118 named arrays matches its actual retail bytes at its
//     linked address, with no additional copies or overlap introduced.
//   * All 3928 initialized bytes [0x51e5dc,0x51f534) match retail, including the
//     shared pointer value 0x51f120. This is an unmasked whole-interval check.
//   * The .bss contribution is 73 bytes at 0x536088. All 12 globals retain their
//     addresses through giTotalMemAllocated at 0x5360b4. Seven empty-string cells
//     remain at 0x5360b8, 0x5360bc, 0x5360c0, 0x5360c4, 0x5360c8, 0x5360cc,
//     and 0x5360d0. Ordered linked references independently verify these cells.
//   * One .CRT$XCU cell and the normal initializer pair remain. The native
//     0x000b2306 producer count is 95 instead of the unmerged root/retail 96.
//
// raw-audit's strict private-name comparator alone reports 32/47 identical
// relocation descriptions: the replaced anonymous literals now have real
// names, EH counters renumber, and empty literals occupy a merged .bss section.
// closure-audit.json records every ordered site and resolved destination, plus
// all array payloads and BSS symbols, rather than treating those names as proof.
// Exact raw and linked bytes also preserve every instruction/CFG here; no fuzzy
// or block-topology proxy is being used to call a function exact.
//
// For canonical source identity, closure-audit.py also creates
//   local-{literal,named}-semantic/Misc.annotated.cpp
// with DATA markers on all 118 arrays, derived from native COFF/MAP addresses
// and checked against retail payloads. Both annotation-only snapshots were then
// recompiled successfully as Misc.annotated.obj: all 67 section names, sizes,
// payloads, and characteristics and all 909 complete-object ordered relocations
// remain identical to their respective previously linked objects. Thus the DATA
// markers are measured byte-neutral; the orchestrator owns production gates.
//
// Disposition: retain this independently byte-exact merged owner as an active
// integration candidate. It recovers one native compiler input while preserving
// its real code and storage, unlike the failed plain merge. The root must still
// combine it with the verified DIMMER producer, validate the historical header,
// regenerate canonical annotations, run build gates, and audit the full image.
// Audiere destructor placement is outside this local result and remains open.
// No production edits or commits were made by this experiment lane.
